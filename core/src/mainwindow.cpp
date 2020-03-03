/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 *    of conditions and the following disclaimer.                                        *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 *    list of conditions and the following disclaimer in the documentation and/or other  *
 *    materials provided with the distribution.                                          *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 *    used to endorse or promote products derived from this software without specific    *
 *    prior written permission.                                                          *
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

#include "mainwindow.h"

#include "clusterwidget.h"
#include "configuration.h"
#include "database.h"
#include "jsonload.h"
#include "killallmessage.h"
#include "processwidget.h"
#include "programwidget.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include <set>

MainWindow::MainWindow(QString title, const std::string& configurationFile) {
    setWindowTitle(title);

    //
    // Set up the logging
    common::Log::initialize("core", [this](std::string msg) { log(msg); });
    qInstallMessageHandler(
        // Now that the log is enabled and available, we can pipe all Qt messages to that
        [](QtMsgType, const QMessageLogContext&, const QString& msg) {
            Log(msg.toStdString());
        }
    );
    _messageBox = new QTextEdit;

    //
    // Set up the container widgets
    QWidget* center = new QWidget;
    QBoxLayout* layout = new QVBoxLayout;
    center->setLayout(layout);
    setCentralWidget(center);

    QTabWidget* tabWidget = new QTabWidget;
    layout->addWidget(tabWidget);

    //
    // Load the configuration
    Log(fmt::format("Loading configuration file {}", configurationFile));
    Configuration config = common::loadFromJson<Configuration>(configurationFile, "");

    //
    // Load the data
    Log(fmt::format("Loading programs from directory {}", config.applicationPath));
    Log(fmt::format("Loading nodes from directory {}", config.nodePath));
    Log(fmt::format("Loading clusters from directory {}", config.clusterPath));
    data::loadData(config.applicationPath, config.clusterPath, config.nodePath);

    //
    // Create the widgets
    // Programs
    _programWidget = new programs::ProgramsWidget;
    connect(
        _programWidget, &programs::ProgramsWidget::startProgram,
        this, &MainWindow::startProgram
    );
    connect(
        _programWidget, &programs::ProgramsWidget::stopProgram,
        this, &MainWindow::stopProgram
    );
    connect(
        _programWidget, &programs::ProgramsWidget::restartProcess,
        this, &MainWindow::startProcess
    );
    connect(
        _programWidget, &programs::ProgramsWidget::stopProcess,
        this, &MainWindow::stopProcess
    );

    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::connectedStatusChanged,
        _programWidget, &programs::ProgramsWidget::connectedStatusChanged
    );


    // Clusters
    _clustersWidget = new ClustersWidget;
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::connectedStatusChanged,
        _clustersWidget, &ClustersWidget::connectedStatusChanged
    );


    // Processes
    _processesWidget = new ProcessesWidget;
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedTrayProcess,
        [this](common::ProcessStatusMessage status) {
            Process* process = data::findProcess(Process::ID{ status.processId });
            if (!process) {
                // This state might happen if C-Troll was restarted while programs were
                // still running on the trays, if we than issue a killall command, we are
                // handed back a process id that we don't know
                return;
            }

            process->status = status.status;

            // The process was already known to us, which should always be the case
            _processesWidget->processUpdated(process->id);
            _programWidget->processUpdated(process->id);
        }
    );
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedTrayStatus,
        [this](Node::ID id, common::TrayStatusMessage status) {
            if (!status.runningProcesses.empty()) {
                Node* node = data::findNode(id);
                Log(fmt::format(
                    "Received status update with {} running processes on node {} @ {}",
                    static_cast<int>(status.runningProcesses.size()),
                    node->name,
                    node->ipAddress
                ));
                // @TODO (abock, 2020-02-27) Update the processeswidget with information
                // about these zombie projects
            }
        }
    );
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::messageReceived,
        [](Cluster::ID clusterId, Node::ID nodeId, nlohmann::json message) {
            Cluster* cluster = data::findCluster(clusterId);
            Node* node = data::findNode(nodeId);
            Log(fmt::format("{} {} {}", cluster->name, node->name, std::string(message)));
        }
    );
    connect(
        _processesWidget, &ProcessesWidget::killAllProcesses,
        [this]() { killAllProcesses(Cluster::ID{ -1 }); }
    );

    // Set up the tab widget
    tabWidget->addTab(_programWidget, "Programs");
    tabWidget->addTab(_clustersWidget, "Clusters");
    tabWidget->addTab(_processesWidget, "Processes");
    tabWidget->addTab(_messageBox, "Log");

    _clusterConnectionHandler.initialize();
}

void MainWindow::log(std::string msg) {
    _messageBox->append(QString::fromStdString(msg));
}

void MainWindow::startProgram(Cluster::ID clusterId, Program::ID programId,
                              Program::Configuration::ID configurationId)
{
    // We don't want to make sure that the program isn't already running as it might be
    // perfectly valid to start the program multiple times

    Cluster* cluster = data::findCluster(clusterId);
    assert(cluster);

    for (Node::ID node : cluster->nodes) {
        std::unique_ptr<Process> process = std::make_unique<Process>(
            programId,
            configurationId,
            clusterId,
            node
        );
        Process::ID id = process->id;
        data::addProcess(std::move(process));

        startProcess(id);
        _processesWidget->processAdded(id);
    }
}

void MainWindow::stopProgram(Cluster::ID clusterId, Program::ID programId,
                             Program::Configuration::ID configurationId)
{
    // First, collect all the processes that belong to this program combination
    std::vector<Process*> processes;
    for (Process* process : data::processes()) {
        const bool clusterMatch = process->clusterId == clusterId;
        const bool programMatch = process->programId == programId;
        const bool configurationMatch = process->configurationId == configurationId;

        if (clusterMatch && programMatch && configurationMatch) {
            processes.push_back(process);
        }
    }

    // Something strange must have happened if we get to the stop command without actually
    // having a single process that corresponds to the command.  The question is, how did
    // we manage to set off the stopProgram function in the first place?
    assert(!processes.empty());

    for (Process* process : processes) {
        stopProcess(process->id);
    }
}

void MainWindow::startProcess(Process::ID processId) {
    Process* process = data::findProcess(processId);
    Cluster* cluster = data::findCluster(process->clusterId);
    Node* node = data::findNode(process->nodeId);

    common::CommandMessage command = startProcessCommand(*process);

    // Generate identifier
    Log("Sending message to start program:");
    Log(fmt::format("\tCluster: {} {}", cluster->name, cluster->id.v));
    Log(fmt::format("\tCommand: {}", command.command));
    Log(fmt::format("\tExecutable: {}", command.executable));
    Log(fmt::format("\tIdentifier: {}", command.id));
    Log(fmt::format("\tCommandline Parameters: {}", command.commandlineParameters));
    Log(fmt::format("\tCWD: {}", command.workingDirectory));
    
    nlohmann::json j = command;
    _clusterConnectionHandler.sendMessage(*node, j);
}

void MainWindow::stopProcess(Process::ID processId) {
    Process* process = data::findProcess(processId);
    Cluster* cluster = data::findCluster(process->clusterId);
    Node* node = data::findNode(process->nodeId);

    common::CommandMessage command = exitProcessCommand(*process);

    Log("Sending message to stop program:");
    Log(fmt::format("\tCluster: {} {}", cluster->name, cluster->id.v));
    Log(fmt::format("\tCommand: {}", command.command));
    Log(fmt::format("\tExecutable: {}", command.executable));
    Log(fmt::format("\tIdentifier: {}", command.id));
    Log(fmt::format("\tCommandline Parameters: {}", command.commandlineParameters));
    Log(fmt::format("\tCWD: {}", command.workingDirectory));

    nlohmann::json j = command;
    _clusterConnectionHandler.sendMessage(*node, j);
}

void MainWindow::killAllProcesses(Cluster::ID id) {
    Log("Send message to stop all programs");

    std::vector< Node*> nodes;
    if (id.v == -1 ) {
        // Send kill command to all clusters
        for (Cluster* cluster : data::clusters()) {
            std::vector<Node*> ns = data::findNodesForCluster(*cluster);
            std::copy(ns.begin(), ns.end(), std::back_inserter(nodes));
        }

        // We have probably picked up a number of duplicates in this process as nodes can
        // be specified in multiple clusters
        std::sort(nodes.begin(), nodes.end());
        nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
    }
    else {
        // We want to send the kill command only to the nodes of a specific cluster
        Cluster* cluster = data::findCluster(id);
        nodes = data::findNodesForCluster(*cluster);
    }

    common::KillAllMessage command;
    nlohmann::json j = command;
    for (Node* node : nodes) {
        Log("\t" + node->name);
        _clusterConnectionHandler.sendMessage(*node, j);
    }
}
