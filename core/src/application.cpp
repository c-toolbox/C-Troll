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

#include <guicommand.h>
#include <trayprocessstatus.h>
#include <trayprocesslogmessage.h>
#include <logging.h>
#include <genericmessage.h>

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
    QFile f(configurationFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject jsonObject = d.object();

    QString programPath = jsonObject.value("applicationPath").toString();
    QString clusterPath = jsonObject.value("clusterPath").toString();
    int listeningPort = jsonObject.value("listeningPort").toInt();

    // Load all program descriptions from the path provided by the configuration file
    _programs = loadProgramsFromDirectory(programPath);
    // Load all cluster descriptions from the path provided by the configuration file
    _clusters = loadClustersFromDirectory(clusterPath);

    // The incoming socket handler takes care of messages from the GUI
    _incomingSocketHandler.initialize(listeningPort);
    
    // The outgoing socket handler takes care of messages to the Tray
    _outgoingSocketHandler.initialize(_clusters);

    QObject::connect(
        &_incomingSocketHandler, &IncomingSocketHandler::messageReceived,
        [this](const QJsonDocument& message) { incomingGuiMessage(message); }
    );
    
    QObject::connect(
        &_outgoingSocketHandler, &OutgoingSocketHandler::messageReceived,
        [this](const QJsonDocument& message) { incomingTrayMessage(message); }
    );

    QObject::connect(
        &_incomingSocketHandler, &IncomingSocketHandler::newConnectionEstablished,
        [this](common::JsonSocket* socket) { sendInitializationInformation(socket); }
    );
}

void Application::handleTrayProcessStatus(common::TrayProcessStatus status) {
    Log("Status: " + status.status);
}

void Application::handleTrayProcessLogMessage(common::TrayProcessLogMessage logMessage) {
    Log("Std out: " + logMessage.stdOutLog);
    Log("Std err: " + logMessage.stdErrorLog);
}


void Application::handleIncomingGuiCommand(common::GuiCommand cmd) {
    Log("Command: " + cmd.command);
    Log("Application: " + cmd.applicationId);
    Log("Configuration: " + cmd.configurationId);
    Log("Cluster: " + cmd.clusterId);

    auto iProgram = std::find_if(
        _programs.cbegin(),
        _programs.cend(),
            [&](const Program& p) {
                return p.id() == cmd.applicationId;
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
        _clusters.cbegin(),
        _clusters.cend(),
        [&](const Cluster& c) {
            return c.id() == cmd.clusterId;
        }
    );
            
    if (iCluster == _clusters.end()) {
        // We didn't find the cluster you were looking for
        // TODO(alex): Signal this back to the GUI
        Log("Could not find cluster id " + cmd.clusterId);
        return;
    }
        
    auto validCluster = [this](const Program& p, const Cluster& c) -> bool {
        if (p.clusters().empty()) {
            return true;
        }
        else {
            const QStringList& clusters = p.clusters();
            auto it = std::find_if(
                clusters.cbegin(),
                clusters.cend(),
                [&](const QString& s) { return c.id() == s; }
            );
            return it != clusters.end();
        }
    };
        
    if (!validCluster(*iProgram, *iCluster)) {
        // We tried to start an application on a cluster for which the application
        // is not configured
        // TODO(alex): Signal this back to the GUI
        Log(
            "Application id " + cmd.applicationId +
            " cannot be started on cluster id " + cmd.clusterId
        );
        return;
    }
            
    // Get the correct configuration, if it exists
    if (cmd.configurationId.isEmpty()) {
        sendTrayCommand(*iCluster, programToTrayCommand(*iProgram), cmd.command);
    }
    else {
        auto iConfiguration = std::find_if(
            iProgram->configurations().cbegin(),
            iProgram->configurations().cend(),
            [&](const Program::Configuration& c) {
                return c.id == cmd.configurationId;
            }
        );
            
        if (iConfiguration == iProgram->configurations().end()) {
            // The requested configuration does not exist for the application
            // TODO(alex): Signal this back to the GUI
            Log(
                "The configuration " + cmd.configurationId +
                " does not exist for the application id " + cmd.applicationId
            );
            return;
                
        }
        sendTrayCommand(
            *iCluster,
            programToTrayCommand(*iProgram, iConfiguration->commandlineParameters), cmd.command
        );
    }
}

void Application::incomingGuiMessage(const QJsonDocument& message) {
    try {
        // The message contains a JSON object of the GenericMessage
        common::GenericMessage msg = common::GenericMessage(message);
            
        qDebug() << "Received message of type " << msg.type;

        if (msg.type == common::GuiCommand::Type) {
            // We have received a message from the GUI to start a new application
            handleIncomingGuiCommand(common::GuiCommand(QJsonDocument(msg.payload)));
        }
    } catch (const std::runtime_error& e) {
        Log(QString("Error with incoming message: ") + e.what());
    }
}

void Application::incomingTrayMessage(const QJsonDocument& message) {
    try {
        // The message contains a JSON object of the GenericMessage
        common::GenericMessage msg = common::GenericMessage(message);

        qDebug() << "Received message of type " << msg.type;

        if (msg.type == common::TrayProcessStatus::Type) {
            // We have received a message from the GUI to start a new application
            handleTrayProcessStatus(common::TrayProcessStatus(QJsonDocument(msg.payload)));
        } else if (msg.type == common::TrayProcessLogMessage::Type) {
            // We have received a message from the GUI to start a new application
            handleTrayProcessLogMessage(common::TrayProcessLogMessage(QJsonDocument(msg.payload)));
        }
    }
    catch (const std::runtime_error& e) {
        Log(QString("Error with incoming message: ") + e.what());
    }
}

void Application::sendInitializationInformation(common::JsonSocket* socket) {
    common::GenericMessage msg;
    msg.type = common::GuiInitialization::Type;
    
    common::GuiInitialization initMsg;
    for (const Program& p : _programs) {
        initMsg.applications.push_back(programToGuiInitializationApplication(p));
    }
    
    for (const Cluster& c : _clusters) {
        initMsg.clusters.push_back(clusterToGuiInitializationCluster(c));
    }
    
    msg.payload = initMsg.toJson().object();
    _incomingSocketHandler.sendMessage(socket, msg.toJson());
}
int i = 0;
void Application::sendTrayCommand(const Cluster& cluster, common::TrayCommand command, QString cmd) {
    // Generate identifier
    
    qDebug() << "Sending Message: ";
    qDebug() << "Cluster:" << cluster.name() << cluster.id();
    
    command.command = cmd;
    command.id = "Hello" + QString::fromStdString(std::to_string(i));
    i++;

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
