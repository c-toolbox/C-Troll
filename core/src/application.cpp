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
#include <logging.h>
#include <trayprocesslogmessage.h>
#include <trayprocessstatus.h>
#include <QFile>
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

Application::Application(std::string configurationFile)
    : _configurationFile(std::move(configurationFile))
{
    initalize();
}

void Application::initalize(bool resetGUIconnection) {
    std::fstream f(_configurationFile);
    nlohmann::json json;
    f >> json;

    std::string programPath = json.at("applicationPath").get<std::string>();
    std::string clusterPath = json.at("clusterPath").get<std::string>();
    int listeningPort = json.at("listeningPort").get<int>();

    if (json.find("services") != json.end()) {
        nlohmann::json services = json.at("services");

        for (const nlohmann::json& s : services) {
            if (!s.is_string()) {
                Log("Error when parsing service command.");
                continue;
            }

            std::string command = s.get<std::string>();
            std::unique_ptr<QProcess> p = std::make_unique<QProcess>();
            p->start(QString::fromStdString(command));
            _services.push_back(std::move(p));
        }
    }

    // Load all program descriptions from the path provided by the configuration file
    std::unique_ptr<std::vector<std::unique_ptr<Program>>> programs =
        Program::loadProgramsFromDirectory(QString::fromStdString(programPath));

    std::transform(
        programs->begin(),
        programs->end(),
        std::back_inserter(_programs),
        [](std::unique_ptr<Program>& program) { return std::move(program); }
    );

    // Load all cluster descriptions from the path provided by the configuration file
    std::unique_ptr<std::vector<std::unique_ptr<Cluster>>> uniqueClisters =
        Cluster::loadClustersFromDirectory(QString::fromStdString(clusterPath));
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
            [this](const nlohmann::json& message) { incomingGuiMessage(message); }
        );

        QObject::connect(
            &_outgoingSocketHandler, &OutgoingSocketHandler::messageReceived,
            [this](const Cluster& cluster, const Cluster::Node& node, const nlohmann::json& message) { incomingTrayMessage(cluster, node, message); }
        );

        QObject::connect(
            &_outgoingSocketHandler, &OutgoingSocketHandler::connectedStatusChanged,
            [this](const Cluster&, const Cluster::Node&) {
                nlohmann::json j = initializationInformation();
                _incomingSocketHandler.sendMessageToAll(j);
            }
        );

        QObject::connect(
            &_incomingSocketHandler, &IncomingSocketHandler::newConnectionEstablished,
            [this](common::JsonSocket* socket) {
                nlohmann::json j = initializationInformation();
                _incomingSocketHandler.sendMessage(socket, j);
                for (std::unique_ptr<CoreProcess>& process : _processes) {
                    nlohmann::json jo = guiProcessLogMessageHistory(*process);
                    _incomingSocketHandler.sendMessage(socket, jo);
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

    switch (status.status) {
        case common::TrayProcessStatus::Status::Starting:
            p.pushNodeStatus(node.id, CoreProcess::NodeStatus::Status::Starting);
            sendGuiProcessStatus(*iProcess->get(), node.id);
            break;
        case common::TrayProcessStatus::Status::Running:
            p.pushNodeStatus(node.id, CoreProcess::NodeStatus::Status::Running);
            sendGuiProcessStatus(*iProcess->get(), node.id);
            break;
        case common::TrayProcessStatus::Status::NormalExit:
            p.pushNodeStatus(node.id, CoreProcess::NodeStatus::Status::NormalExit);
            sendGuiProcessStatus(*iProcess->get(), node.id);
            break;
        case common::TrayProcessStatus::Status::FailedToStart:
            p.pushNodeStatus(node.id, CoreProcess::NodeStatus::Status::FailedToStart);
            sendGuiProcessStatus(*iProcess->get(), node.id);
            break;
        case common::TrayProcessStatus::Status::CrashExit:
            p.pushNodeStatus(node.id, CoreProcess::NodeStatus::Status::CrashExit);
            sendGuiProcessStatus(*iProcess->get(), node.id);
            break;
        case common::TrayProcessStatus::Status::WriteError:
            p.pushNodeError(node.id, CoreProcess::NodeError::Error::WriteError);
            break;
        case common::TrayProcessStatus::Status::ReadError:
            p.pushNodeError(node.id, CoreProcess::NodeError::Error::ReadError);
            break;
        case common::TrayProcessStatus::Status::TimedOut:
            p.pushNodeError(node.id, CoreProcess::NodeError::Error::TimedOut);
            break;
        case common::TrayProcessStatus::Status::UnknownError:
            p.pushNodeError(node.id, CoreProcess::NodeError::Error::UnknownError);
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

    switch (logMessage.outputType) {
        case common::TrayProcessLogMessage::OutputType::StdOut:
            p.pushNodeStdOut(node.id, logMessage.message);
            break;
        case common::TrayProcessLogMessage::OutputType::StdErr:
            p.pushNodeStdError(node.id, logMessage.message);
            break;
    }
    
    sendLatestLogMessage(*iProcess->get(), node.id);
}

void Application::handleIncomingGuiStartCommand(common::GuiStartCommand cmd) {
    Log("Application: " + cmd.applicationId);
    Log("Configuration: " + cmd.configurationId);
    Log("Cluster: " + cmd.clusterId);

    auto iProgram = std::find_if(
        _programs.begin(),
        _programs.end(),
        [&](const std::unique_ptr<Program>& p) { return p->id == cmd.applicationId; }
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
        [&](const std::unique_ptr<Cluster>& c) { return c->id == cmd.clusterId; }
    );
            
    if (iCluster == _clusters.end()) {
        // We didn't find the cluster you were looking for
        // TODO(alex): Signal this back to the GUI
        Log("Could not find cluster id " + cmd.clusterId);
        return;
    }
    
    std::string configurationId;

    if (!cmd.configurationId.empty()) {
        const std::vector<Program::Configuration>& confs = (*iProgram)->configurations;

        auto iConfiguration = std::find_if(
            confs.begin(),
            confs.end(),
            [&](const Program::Configuration& c) { return c.id == cmd.configurationId; }
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

        const std::map<std::string, std::string>& clusterParamsList =
            iConfiguration->clusterCommandlineParameters;
        auto clusterParams = clusterParamsList.find(cmd.clusterId);

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
        configurationId,
        iCluster->get()
    );
    common::TrayCommand trayCommand = process->startProcessCommand();
    _processes.push_back(std::move(process));

    sendTrayCommand(*(iCluster->get()), trayCommand);
}

void Application::handleIncomingGuiProcessCommand(common::GuiProcessCommand cmd) {
    int processId = cmd.processId;

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
    
    if (cmd.command == "Restart") {
        sendTrayCommand(*cluster, process->startProcessCommand());
    } else if (cmd.command == "Stop") {
        //_processes.erase(iProcess);
    } else {
        Log("Unknown command '" + cmd.command + "'");
    }
}

void Application::handleIncomingGuiReloadConfigCommand() {
    deinitalize(false);
    initalize(false);
}

void Application::incomingGuiMessage(const nlohmann::json& message) {
    try {
        // The message contains a JSON object of the GenericMessage
        common::GenericMessage msg = message;
            
        Log("Received message of type " + msg.type);

        if (msg.type == common::GuiStartCommand::Type) {
            // We have received a message from the GUI to start a new application
            handleIncomingGuiStartCommand(msg.payload);
        } else if (msg.type == common::GuiProcessCommand::Type) {
            // We have received a message from the GUI to start a new application
            handleIncomingGuiProcessCommand(msg.payload);
        }
        else if (msg.type == "GuiReloadConfigCommand") {
             // We have received a message from the GUI to reload the configs
            handleIncomingGuiReloadConfigCommand();
         }
    } catch (const std::runtime_error& e) {
        Log(std::string("Error with incoming gui message: ") + e.what());
        Log(message.dump());
    }
}

void Application::incomingTrayMessage(const Cluster& cluster, const Cluster::Node& node,
                                      const nlohmann::json& message)
{
    try {
        // The message contains a JSON object of the GenericMessage
        common::GenericMessage msg = message;

        Log("Received message of type " + msg.type);

        if (msg.type == common::TrayProcessStatus::Type) {
            // We have received a message from the GUI to start a new application
            handleTrayProcessStatus(cluster, node, msg.payload);
        } else if (msg.type == common::TrayProcessLogMessage::Type) {
            // We have received a message from the GUI to start a new application
            handleTrayProcessLogMessage(cluster, node, msg.payload);
        }
    }
    catch (const std::runtime_error& e) {
        Log(std::string("Error with incoming tray message: ") + e.what());
        Log(message.dump());
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

void Application::sendGuiProcessStatus(const CoreProcess& process, const std::string& nodeId)
{
    common::GuiProcessStatus statusMsg = process.toGuiProcessStatus(nodeId);

    common::GenericMessage msg;
    msg.type = common::GuiProcessStatus::Type;
    msg.payload = statusMsg;

    nlohmann::json j = msg;
    _incomingSocketHandler.sendMessageToAll(j);
}

void Application::sendLatestLogMessage(const CoreProcess& process, const std::string& nodeId)
{
    common::GuiProcessLogMessage logMsg = process.latestGuiProcessLogMessage(nodeId);

    common::GenericMessage msg;
    msg.type = common::GuiProcessLogMessage::Type;
    msg.payload = logMsg;

    nlohmann::json j = msg;
    _incomingSocketHandler.sendMessageToAll(j);
}

common::GenericMessage Application::guiProcessLogMessageHistory(const CoreProcess& proc) {
    common::GuiProcessLogMessageHistory historyMsg = proc.guiProcessLogMessageHistory();

    common::GenericMessage msg;
    msg.type = common::GuiProcessLogMessageHistory::Type;
    msg.payload = historyMsg;
    return msg;
}

void Application::sendTrayCommand(const Cluster& cluster,
                                  const common::TrayCommand& command)
{
    // Generate identifier
    
    Log("Sending Message: ");
    Log("Cluster:" + cluster.name + cluster.id);
    Log("\tCommand: " + command.command);
    Log("\tExecutable: " + command.executable);
    Log("\tIdentifier: " + command.id);
    Log("\tBase Directory: " + command.baseDirectory);
    Log("\tCommandline Parameters: " + command.commandlineParameters);
    Log("\tCWD: " + command.currentWorkingDirectory);
    
    // We have to wrap the TrayCommand into a GenericMessage first
    common::GenericMessage msg;
    msg.type = common::TrayCommand::Type;
    msg.payload = command;
    
    nlohmann::json j = msg;
    _outgoingSocketHandler.sendMessage(cluster, j);
}
