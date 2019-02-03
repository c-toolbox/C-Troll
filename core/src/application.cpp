/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2019                                                             *
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
#include <jsonsupport.h>
#include <logging.h>
#include <trayprocesslogmessage.h>
#include <trayprocessstatus.h>
#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QObject>
#include <QTcpSocket>
#include <QThread>
#include <assert.h>
#include <json/json.hpp>

namespace {
    const QString KeyApplicationPath = "applicationPath";
    const QString KeyClusterPath = "clusterPath";
    const QString KeyListeningPort = "listeningPort";
} // namespace

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

    QJsonArray services = common::testAndReturnArray(
        jsonObject,
        "services",
        Optional::Yes
    );
    for (const QJsonValueRef& s : services) {
        if (!s.isString()) {
            Log("Error when parsing service command.");
            continue;
        }
        QString command = s.toString();
        std::unique_ptr<QProcess> p = std::make_unique<QProcess>();
        p->start(command);
        _services.push_back(std::move(p));
    }

    // Load all program descriptions from the path provided by the configuration file
    std::unique_ptr<std::vector<std::unique_ptr<Program>>> programs =
        Program::loadProgramsFromDirectory(programPath);

    std::transform(
        programs->begin(),
        programs->end(),
        std::back_inserter(_programs),
        [](std::unique_ptr<Program>& program) { return std::move(program); }
    );

    // Load all cluster descriptions from the path provided by the configuration file
    std::unique_ptr<std::vector<std::unique_ptr<Cluster>>> uniqueClisters =
        Cluster::loadClustersFromDirectory(clusterPath);
    std::transform(
        uniqueClisters->begin(),
        uniqueClisters->end(),
        std::back_inserter(_clusters),
        [](std::unique_ptr<Cluster>& cluster) { return std::move(cluster); }
    );

    if (resetGUIconnection) {
        // The incoming socket handler takes care of messages from the GUI
        _incomingSocketHandler.initialize(listeningPort);
    }

    // The outgoing socket handler takes care of messages to the Tray
    QList<Cluster*> clusters;
    std::transform(
        _clusters.begin(),
        _clusters.end(),
        std::back_inserter(clusters),
        [](std::unique_ptr<Cluster>& cluster) { return cluster.get(); }
    );
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
            [this](const Cluster&, const Cluster::Node&) {
                nlohmann::json j = initializationInformation();
                _incomingSocketHandler.sendMessageToAll(common::conv::to_qtjsondoc(j));
            }
        );

        QObject::connect(
            &_incomingSocketHandler, &IncomingSocketHandler::newConnectionEstablished,
            [this](common::JsonSocket* socket) {
                nlohmann::json j = initializationInformation();
                _incomingSocketHandler.sendMessage(socket, common::conv::to_qtjsondoc(j));
                for (std::unique_ptr<CoreProcess>& process : _processes) {
                    nlohmann::json jo = guiProcessLogMessageHistory(*process);
                    _incomingSocketHandler.sendMessage(socket, common::conv::to_qtjsondoc(jo));
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

void Application::handleTrayProcessStatus(const Cluster&,
                                          const Cluster::Node& node,
                                          common::TrayProcessStatus status)
{
    auto iProcess = std::find_if(
        _processes.begin(),
        _processes.end(),
        [&status](std::unique_ptr<CoreProcess>& p) { return p->id() == status.processId; }
    );
    if (iProcess == _processes.end()) {
        return;
    }

    CoreProcess& p = *(*iProcess);

    QString nodeId = node.id;

    switch (status.status) {
        case common::TrayProcessStatus::Status::Starting:
            p.pushNodeStatus(nodeId, CoreProcess::NodeStatus::Status::Starting);
            sendGuiProcessStatus(*iProcess->get(), nodeId);
            break;
        case common::TrayProcessStatus::Status::Running:
            p.pushNodeStatus(nodeId, CoreProcess::NodeStatus::Status::Running);
            sendGuiProcessStatus(*iProcess->get(), nodeId);
            break;
        case common::TrayProcessStatus::Status::NormalExit:
            p.pushNodeStatus(nodeId, CoreProcess::NodeStatus::Status::NormalExit);
            sendGuiProcessStatus(*iProcess->get(), nodeId);
            break;
        case common::TrayProcessStatus::Status::FailedToStart:
            p.pushNodeStatus(nodeId, CoreProcess::NodeStatus::Status::FailedToStart);
            sendGuiProcessStatus(*iProcess->get(), nodeId);
            break;
        case common::TrayProcessStatus::Status::CrashExit:
            p.pushNodeStatus(nodeId, CoreProcess::NodeStatus::Status::CrashExit);
            sendGuiProcessStatus(*iProcess->get(), nodeId);
            break;
        case common::TrayProcessStatus::Status::WriteError:
            p.pushNodeError(nodeId, CoreProcess::NodeError::Error::WriteError);
            break;
        case common::TrayProcessStatus::Status::ReadError:
            p.pushNodeError(nodeId, CoreProcess::NodeError::Error::ReadError);
            break;
        case common::TrayProcessStatus::Status::TimedOut:
            p.pushNodeError(nodeId, CoreProcess::NodeError::Error::TimedOut);
            break;
        case common::TrayProcessStatus::Status::UnknownError:
            p.pushNodeError(nodeId, CoreProcess::NodeError::Error::UnknownError);
            break;
        }
}

void Application::handleTrayProcessLogMessage(const Cluster&,
                                              const Cluster::Node& node,
                                              common::TrayProcessLogMessage logMessage)
{
    auto iProcess = std::find_if(
        _processes.begin(),
        _processes.end(),
        [&logMessage](std::unique_ptr<CoreProcess>& p) {
            return p->id() == logMessage.processId;
        }
    );

    if (iProcess == _processes.end()) {
        return;
    }

    CoreProcess& p = **iProcess;
    
    Log(logMessage.message);

    QString nodeId = node.id;
    switch (logMessage.outputType) {
        case common::TrayProcessLogMessage::OutputType::StdOut:
            p.pushNodeStdOut(nodeId, QString::fromStdString(logMessage.message));
            break;
        case common::TrayProcessLogMessage::OutputType::StdErr:
            p.pushNodeStdError(nodeId, QString::fromStdString(logMessage.message));
            break;
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
        [&](const std::unique_ptr<Program>& p) {
            return p->id().toStdString() == cmd.applicationId;
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
        [&](const std::unique_ptr<Cluster>& c) {
            return c->id().toStdString() == cmd.clusterId;
        }
    );
            
    if (iCluster == _clusters.end()) {
        // We didn't find the cluster you were looking for
        // TODO(alex): Signal this back to the GUI
        Log("Could not find cluster id " + cmd.clusterId);
        return;
    }
    
    std::string configurationId;

    if (!cmd.configurationId.empty()) {
        const QList<Program::Configuration>& confs = (*iProgram)->configurations();

        auto iConfiguration = std::find_if(
            confs.begin(),
            confs.end(),
            [&](const Program::Configuration& c) {
                return c.id.toStdString() == cmd.configurationId;
            }
        );

        if (iConfiguration == confs.end()) {
            // The requested configuration does not exist for the application
            // TODO: Signal this back to the GUI
            Log("The configuration " + cmd.configurationId +
                " does not exist for the application id " + cmd.applicationId
            );
            return;

        }
        configurationId = cmd.configurationId;

        const QMap<QString, QString>& clusterParamsList =
            iConfiguration->clusterCommandlineParameters;
        auto clusterParams = clusterParamsList.find(QString::fromStdString(cmd.clusterId));

        if (clusterParams == clusterParamsList.end()) {
            // The requested configuration does not exist for the application
            // TODO: Signal this back to the GUI
            Log("The configuration " + cmd.configurationId +
                " is not supported on cluster id " + cmd.clusterId
            );
            return;
        }
    }

    std::unique_ptr<CoreProcess> process = std::make_unique<CoreProcess>(
        iProgram->get(),
        QString::fromStdString(configurationId),
        iCluster->get()
    );
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
        [&](const std::unique_ptr<CoreProcess>& p) { return p->id() == processId; }
    );

    if (iProcess == _processes.end()) {
        Log("There is no process with id " + std::to_string(processId));
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
        Log("Unknown command '" + command.toStdString() + "'");
    }
}

void Application::handleIncomingGuiReloadConfigCommand() {
    deinitalize(false);
    initalize(false);
}

void Application::incomingGuiMessage(const QJsonDocument& message) {
    try {
        // The message contains a JSON object of the GenericMessage
        common::GenericMessage msg = common::conv::from_qtjsondoc(message);
            
        qDebug() << "Received message of type " << QString::fromStdString(msg.type);

        if (msg.type == common::GuiStartCommand::Type) {
            // We have received a message from the GUI to start a new application
            handleIncomingGuiStartCommand(
                common::GuiStartCommand(common::conv::to_qtjsondoc(msg.payload))
            );
        } else if (msg.type == common::GuiProcessCommand::Type) {
            // We have received a message from the GUI to start a new application
            handleIncomingGuiProcessCommand(
                common::GuiProcessCommand(common::conv::to_qtjsondoc(msg.payload))
            );
        }
        else if (msg.type == "GuiReloadConfigCommand") {
             // We have received a message from the GUI to reload the configs
            handleIncomingGuiReloadConfigCommand();
         }
    } catch (const std::runtime_error& e) {
        Log(std::string("Error with incoming gui message: ") + e.what());
        Log(QString(message.toJson()).toStdString());
    }
}

void Application::incomingTrayMessage(const Cluster& cluster, const Cluster::Node& node,
                                      const QJsonDocument& message)
{
    try {
        // The message contains a JSON object of the GenericMessage
        common::GenericMessage msg = common::conv::from_qtjsondoc(message);

        qDebug() << "Received message of type " << QString::fromStdString(msg.type);

        if (msg.type == common::TrayProcessStatus::Type) {
            // We have received a message from the GUI to start a new application
            handleTrayProcessStatus(
                cluster,
                node,
                common::TrayProcessStatus(common::conv::to_qtjsondoc(msg.payload))
            );
        } else if (msg.type == common::TrayProcessLogMessage::Type) {
            // We have received a message from the GUI to start a new application
            handleTrayProcessLogMessage(
                cluster,
                node,
                common::TrayProcessLogMessage(common::conv::to_qtjsondoc(msg.payload))
            );
        }
    }
    catch (const std::runtime_error& e) {
        Log(std::string("Error with incoming tray message: ") + e.what());
        Log(QString(message.toJson()).toStdString());
    }
}

common::GenericMessage Application::initializationInformation() {
    common::GenericMessage msg;
    msg.type = common::GuiInitialization::Type;

    common::GuiInitialization initMsg;
    for (const std::unique_ptr<Program>& p : _programs) {
        initMsg.applications.push_back(p->toGuiInitializationApplication());
    }

    for (const std::unique_ptr<Cluster>& c : _clusters) {
        initMsg.clusters.push_back(c->toGuiInitializationCluster());
    }

    for (const std::unique_ptr<CoreProcess>& p : _processes) {
        initMsg.processes.push_back(p->toGuiInitializationProcess());
    }

    msg.payload = initMsg;
    return msg;
}

void Application::sendGuiProcessStatus(const CoreProcess& process, const QString& nodeId)
{
    common::GuiProcessStatus statusMsg = process.toGuiProcessStatus(nodeId);

    common::GenericMessage msg;
    msg.type = common::GuiProcessStatus::Type;
    msg.payload = common::conv::from_qtjsondoc(statusMsg.toJson());

    nlohmann::json j = msg;
    _incomingSocketHandler.sendMessageToAll(common::conv::to_qtjsondoc(j));
}

void Application::sendLatestLogMessage(const CoreProcess& process, const QString& nodeId)
{
    common::GuiProcessLogMessage logMsg = process.latestGuiProcessLogMessage(nodeId);

    common::GenericMessage msg;
    msg.type = common::GuiProcessLogMessage::Type;
    msg.payload = common::conv::from_qtjsondoc(logMsg.toJson());

    nlohmann::json j = msg;
    _incomingSocketHandler.sendMessageToAll(common::conv::to_qtjsondoc(j));
}

common::GenericMessage Application::guiProcessLogMessageHistory(const CoreProcess& proc) {
    common::GuiProcessLogMessageHistory historyMsg = proc.guiProcessLogMessageHistory();

    common::GenericMessage msg;
    msg.type = common::GuiProcessLogMessageHistory::Type;
    msg.payload = common::conv::from_qtjsondoc(historyMsg.toJson());
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
    msg.payload = common::conv::from_qtjsondoc(command.toJson());
    
    nlohmann::json j = msg;
    _outgoingSocketHandler.sendMessage(cluster, common::conv::to_qtjsondoc(j));
}
