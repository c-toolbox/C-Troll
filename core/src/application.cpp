/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 * of conditions and the following disclaimer.                                           *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 * list of conditions and the following disclaimer in the documentation and/or other     *
 * materials provided with the distribution.                                             *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 * used to endorse or promote products derived from this software without specific prior *
 * written permission.                                                                   *
 *                                                                                       *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY   *
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES  *
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT   *
 * SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,        *
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED  *
 * TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR    *
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN      *
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN    *
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH   *
 * DAMAGE.                                                                               *
 *                                                                                       *
 ****************************************************************************************/

#include "application.h"

#include <guistartcommand.h>
#include <guiprocesscommand.h>
#include <guiprocessstatus.h>
#include <trayprocessstatus.h>
#include <trayprocesslogmessage.h>
#include <logging.h>

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTcpSocket>
#include <QObject>
#include <QThread>

#include <assert.h>

namespace {
const QString KeyApplicationPath = "applicationPath";
const QString KeyClusterPath = "clusterPath";
const QString KeyListeningPort = "listeningPort";
}

Application::Application(QString configurationFile) {
    _configurationFile = configurationFile;
    initalize();
}

void Application::initalize(bool resetGUIconnection) {
    QFile f(_configurationFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject jsonObject = d.object();

    QString programPath = jsonObject.value("applicationPath").toString();
    QString clusterPath = jsonObject.value("clusterPath").toString();
    int listeningPort = jsonObject.value("listeningPort").toInt();

    // Load all program descriptions from the path provided by the configuration file
    auto programs = Program::loadProgramsFromDirectory(programPath);
    std::transform(programs->begin(), programs->end(), std::back_inserter(_programs), [](std::unique_ptr<Program>& program) {
        return std::move(program);
    });

    // Load all cluster descriptions from the path provided by the configuration file
    auto uniqueClisters = Cluster::loadClustersFromDirectory(clusterPath);
    std::transform(uniqueClisters->begin(), uniqueClisters->end(), std::back_inserter(_clusters), [](std::unique_ptr<Cluster>& cluster) {
        return std::move(cluster);
    });

    if (resetGUIconnection) {
        // The incoming socket handler takes care of messages from the GUI
        _incomingSocketHandler.initialize(listeningPort);
    }

    // The outgoing socket handler takes care of messages to the Tray
    QList<Cluster*> clusters;
    std::transform(_clusters.begin(), _clusters.end(), std::back_inserter(clusters), [](auto& cluster) {
        return cluster.get();
    });
    _outgoingSocketHandler.initialize(clusters);

    if (resetGUIconnection) {
        QObject::connect(
            &_incomingSocketHandler, &IncomingSocketHandler::messageReceived,
            [this](const QJsonDocument& message) { incomingGuiMessage(message); }
        );

        QObject::connect(
            &_outgoingSocketHandler, &OutgoingSocketHandler::messageReceived,
            [this](const Cluster& cluster, const Cluster::Node& node, const QJsonDocument& message) { incomingTrayMessage(cluster, node, message); }
        );

        QObject::connect(
            &_outgoingSocketHandler, &OutgoingSocketHandler::connectedStatusChanged,
            [this](const Cluster& cluster, const Cluster::Node& node) {
            _incomingSocketHandler.sendMessageToAll(initializationInformation().toJson());
        }
        );

        QObject::connect(
            &_incomingSocketHandler, &IncomingSocketHandler::newConnectionEstablished,
            [this](common::JsonSocket* socket) {
            _incomingSocketHandler.sendMessage(socket, initializationInformation().toJson());
            for (auto& process : _processes) {
                _incomingSocketHandler.sendMessage(socket, guiProcessLogMessageHistory(*process).toJson());
            }
        }
        );
    }
}

void Application::deinitalize(bool resetGUIconnection) {
    if (resetGUIconnection) {
        _incomingSocketHandler.deinitialize();
    }
    _outgoingSocketHandler.deinitialize();

    _processes.clear();
    _clusters.clear();
    _programs.clear();
}

void Application::handleTrayProcessStatus(const Cluster& cluster, const Cluster::Node& node, common::TrayProcessStatus status) {
    auto iProcess = std::find_if(_processes.begin(), _processes.end(), [&status](auto& p) {
        return p->id() == status.processId;
    });
    if (iProcess == _processes.end()) {
        return;
    }

    QString nodeId = node.id;

    switch (status.status) {
    case common::TrayProcessStatus::Status::Starting:
        (*iProcess)->pushNodeStatus(nodeId, CoreProcess::NodeStatus::Status::Starting);
        sendGuiProcessStatus(*iProcess->get(), nodeId);
        break;
    case common::TrayProcessStatus::Status::Running:
        (*iProcess)->pushNodeStatus(nodeId, CoreProcess::NodeStatus::Status::Running);
        sendGuiProcessStatus(*iProcess->get(), nodeId);
        break;
    case common::TrayProcessStatus::Status::NormalExit:
        (*iProcess)->pushNodeStatus(nodeId, CoreProcess::NodeStatus::Status::NormalExit);
        sendGuiProcessStatus(*iProcess->get(), nodeId);
        break;
    case common::TrayProcessStatus::Status::FailedToStart:
        (*iProcess)->pushNodeStatus(nodeId, CoreProcess::NodeStatus::Status::FailedToStart);
        sendGuiProcessStatus(*iProcess->get(), nodeId);
        break;
    case common::TrayProcessStatus::Status::CrashExit:
        (*iProcess)->pushNodeStatus(nodeId, CoreProcess::NodeStatus::Status::CrashExit);
        sendGuiProcessStatus(*iProcess->get(), nodeId);
        break;
    case common::TrayProcessStatus::Status::WriteError:
        (*iProcess)->pushNodeError(nodeId, CoreProcess::NodeError::Error::WriteError);
        break;
    case common::TrayProcessStatus::Status::ReadError:
        (*iProcess)->pushNodeError(nodeId, CoreProcess::NodeError::Error::ReadError);
        break;
    case common::TrayProcessStatus::Status::TimedOut:
        (*iProcess)->pushNodeError(nodeId, CoreProcess::NodeError::Error::TimedOut);
        break;
    case common::TrayProcessStatus::Status::UnknownError:
        (*iProcess)->pushNodeError(nodeId, CoreProcess::NodeError::Error::UnknownError);
        break;
    }
   
}

void Application::handleTrayProcessLogMessage(const Cluster& cluster, const Cluster::Node& node, common::TrayProcessLogMessage logMessage) {
    auto iProcess = std::find_if(_processes.begin(), _processes.end(), [&logMessage](auto& p) {
        return p->id() == logMessage.processId;
    });

    if (iProcess == _processes.end()) {
        return;
    }
    
    Log(logMessage.message);

    QString nodeId = node.id;
    if (logMessage.outputType == common::TrayProcessLogMessage::OutputType::StdOut) {
        (*iProcess)->pushNodeStdOut(nodeId, logMessage.message);
    } else if (logMessage.outputType == common::TrayProcessLogMessage::OutputType::StdErr) {
        (*iProcess)->pushNodeStdError(nodeId, logMessage.message);
    } else {
        return;
    }
    
    sendLatestLogMessage(*iProcess->get(), nodeId);
}


void Application::handleIncomingGuiStartCommand(common::GuiStartCommand cmd) {
    Log("Application: " + cmd.applicationId);
    Log("Configuration: " + cmd.configurationId);
    Log("Cluster: " + cmd.clusterId);

    auto iProgram = std::find_if(
        _programs.begin(),
        _programs.end(),
            [&](const auto& p) {
                return p->id() == cmd.applicationId;
            }
    );
        
    if (iProgram == _programs.end()) {
        // We didn't find the program you were looking for
        // TODO(alex): Signal this back to the GUI
        Log("Could not find application id " + cmd.applicationId);
        return;
    }
    
    // Get the correct Cluster
    auto iCluster = std::find_if(
        _clusters.begin(),
        _clusters.end(),
        [&](const auto& c) {
            return c->id() == cmd.clusterId;
        }
    );
            
    if (iCluster == _clusters.end()) {
        // We didn't find the cluster you were looking for
        // TODO(alex): Signal this back to the GUI
        Log("Could not find cluster id " + cmd.clusterId);
        return;
    }
    
    QString configurationId = "";

    if (!cmd.configurationId.isEmpty()) {
        const auto& configurations = (*iProgram)->configurations();

        auto iConfiguration = std::find_if(
            configurations.begin(),
            configurations.end(),
            [&](const Program::Configuration& c) {
            return c.id == cmd.configurationId;
        }
        );

        if (iConfiguration == configurations.end()) {
            // The requested configuration does not exist for the application
            // TODO: Signal this back to the GUI
            Log(
                "The configuration " + cmd.configurationId +
                " does not exist for the application id " + cmd.applicationId
                );
            return;

        }
        configurationId = cmd.configurationId;

        const auto& clusterParamsList = iConfiguration->clusterCommanlineParameters;
        const auto& clusterParams = clusterParamsList.find(cmd.clusterId);

        if (clusterParams == clusterParamsList.end()) {
            // The requested configuration does not exist for the application
            // TODO: Signal this back to the GUI
            Log(
                "The configuration " + cmd.configurationId +
                " is not supported on cluster id " + cmd.clusterId
            );
            return;
        }
    }

    std::unique_ptr<CoreProcess> process = std::make_unique<CoreProcess>(iProgram->get(), configurationId, iCluster->get());
    common::TrayCommand trayCommand = process->startProcessCommand();
    _processes.push_back(std::move(process));

    sendTrayCommand(*(iCluster->get()), trayCommand);
}

void Application::handleIncomingGuiProcessCommand(common::GuiProcessCommand cmd) {
    int processId = cmd.processId;
    QString command = cmd.command;

    auto iProcess = std::find_if(
        _processes.begin(),
        _processes.end(),
        [&](const auto& p) {
            return p->id() == processId;
        }
    );

    if (iProcess == _processes.end()) {
        Log(QString("There is no process with id ") + QString(processId));
        return;
    }

    CoreProcess* process = iProcess->get();
    Cluster* cluster = process->cluster();

    sendTrayCommand(*cluster, process->exitProcessCommand());
    
    if (command == "Restart") {
        sendTrayCommand(*cluster, process->startProcessCommand());
    } else if (command == "Stop") {
        //_processes.erase(iProcess);
    } else {
        Log("Unknown command '" + command + "'");
    }
}

void Application::handleIncomingGuiReloadConfigCommand() {
    deinitalize(false);
    initalize(false);
}

void Application::incomingGuiMessage(const QJsonDocument& message) {
    try {
        // The message contains a JSON object of the GenericMessage
        common::GenericMessage msg = common::GenericMessage(message);
            
        qDebug() << "Received message of type " << msg.type;

        if (msg.type == common::GuiStartCommand::Type) {
            // We have received a message from the GUI to start a new application
            handleIncomingGuiStartCommand(common::GuiStartCommand(QJsonDocument(msg.payload)));
        } else if (msg.type == common::GuiProcessCommand::Type) {
            // We have received a message from the GUI to start a new application
            handleIncomingGuiProcessCommand(common::GuiProcessCommand(QJsonDocument(msg.payload)));
        }
        else if (msg.type == "GuiReloadConfigCommand") {
             // We have received a message from the GUI to reload the configs
            handleIncomingGuiReloadConfigCommand();
         }
    } catch (const std::runtime_error& e) {
        Log(QString("Error with incoming gui message: ") + e.what());
        Log(message.toJson());
    }
}

void Application::incomingTrayMessage(const Cluster& cluster, const Cluster::Node& node, const QJsonDocument& message) {
    try {
        // The message contains a JSON object of the GenericMessage
        common::GenericMessage msg = common::GenericMessage(message);

        qDebug() << "Received message of type " << msg.type;

        if (msg.type == common::TrayProcessStatus::Type) {
            // We have received a message from the GUI to start a new application
            handleTrayProcessStatus(cluster, node, common::TrayProcessStatus(QJsonDocument(msg.payload)));
        } else if (msg.type == common::TrayProcessLogMessage::Type) {
            // We have received a message from the GUI to start a new application
            handleTrayProcessLogMessage(cluster, node, common::TrayProcessLogMessage(QJsonDocument(msg.payload)));
        }
    }
    catch (const std::runtime_error& e) {
        Log(QString("Error with incoming tray message: ") + e.what());
        Log(message.toJson());
    }
}

common::GenericMessage Application::initializationInformation() {
    common::GenericMessage msg;
    msg.type = common::GuiInitialization::Type;

    common::GuiInitialization initMsg;
    for (const auto& p : _programs) {
        initMsg.applications.push_back(p->toGuiInitializationApplication());
    }

    for (const auto& c : _clusters) {
        initMsg.clusters.push_back(c->toGuiInitializationCluster());
    }

    for (const auto& p : _processes) {
        initMsg.processes.push_back(p->toGuiInitializationProcess());
    }

    msg.payload = initMsg.toJson().object();
    return msg;
}

void Application::sendGuiProcessStatus(const CoreProcess& process, const QString& nodeId) {
    common::GuiProcessStatus statusMsg = process.toGuiProcessStatus(nodeId);

    common::GenericMessage msg;
    msg.type = common::GuiProcessStatus::Type;
    msg.payload = statusMsg.toJson().object();

    _incomingSocketHandler.sendMessageToAll(msg.toJson());
}

void Application::sendLatestLogMessage(const CoreProcess& process, const QString& nodeId) {
    common::GuiProcessLogMessage logMsg = process.latestGuiProcessLogMessage(nodeId);

    common::GenericMessage msg;
    msg.type = common::GuiProcessLogMessage::Type;
    msg.payload = logMsg.toJson().object();

    _incomingSocketHandler.sendMessageToAll(msg.toJson());
}

common::GenericMessage Application::guiProcessLogMessageHistory(const CoreProcess& process) {
    common::GuiProcessLogMessageHistory historyMsg = process.guiProcessLogMessageHistory();

    common::GenericMessage msg;
    msg.type = common::GuiProcessLogMessageHistory::Type;
    msg.payload = historyMsg.toJson().object();
    return msg;
}

void Application::sendTrayCommand(const Cluster& cluster, const common::TrayCommand& command) {
    // Generate identifier
    
    qDebug() << "Sending Message: ";
    qDebug() << "Cluster:" << cluster.name() << cluster.id();
    
    qDebug() << "\tCommand: " << command.command;
    qDebug() << "Executable: " << command.executable;
    qDebug() << "\tIdentifier: " << command.id;
    qDebug() << "\t:Base Directory:  " << command.baseDirectory;
    qDebug() << "\tCommandline Parameters: " << command.commandlineParameters;
    qDebug() << "\tCWD: " << command.currentWorkingDirectory;
    
    // We have to wrap the TrayCommand into a GenericMessage first
    common::GenericMessage msg;
    msg.type = common::TrayCommand::Type;
    msg.payload = command.toJson().object();
    
    _outgoingSocketHandler.sendMessage(cluster, msg.toJson());
}
