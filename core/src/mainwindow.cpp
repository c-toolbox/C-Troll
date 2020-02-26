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
#include "message.h"
#include "jsonload.h"
#include "logging.h"
#include "processwidget.h"
#include "programwidget.h"
#include <QPushButton>
#include <QHBoxLayout>
#include <QLabel>
#include <QTabWidget>
#include <QTextEdit>
#include <QVBoxLayout>

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
            assert(process);
            process->status = status.status;

            // The process was already known to us, which should always be the case
            _processesWidget->processUpdated(process->id);
            _programWidget->processUpdated(process->id);
        }
    );
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::messageReceived,
        [](int clusterId, int nodeId, nlohmann::json message) {
            Cluster* cluster = data::findCluster(clusterId);
            Node* node = data::findNode(nodeId);
            Log(fmt::format("{} {} {}", cluster->name, node->name, std::string(message)));
        }
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

void MainWindow::startProgram(int clusterId, int programId, int configurationId) {
    // We don't want to make sure that the program isn't already running as it might be
    // perfectly valid to start the program multiple times

    Cluster* cluster = data::findCluster(clusterId);
    assert(cluster);
    //Program* program = data::findProgram(programId);
    //assert(program);
    //const Program::Configuration& configuration = data::findConfigurationForProgram(
    //    *program,
    //    configurationId
    //);



    //Log("Application: " + program->name);
    //Log("Configuration: " + configuration.name);
    //Log("Cluster: " + cluster->name);

    for (int node : cluster->nodes) {
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

void MainWindow::stopProgram(int clusterId, int programId, int configurationId) {
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
    Log("Sending Message to start application:");
    Log(fmt::format("\tCluster: {} {}", cluster->name, cluster->id));
    Log(fmt::format("\tCommand: {}", command.command));
    Log(fmt::format("\tExecutable: {}", command.executable));
    Log(fmt::format("\tIdentifier: {}", command.id));
    Log(fmt::format("\tCommandline Parameters: {}", command.commandlineParameters));
    Log(fmt::format("\tCWD: {}", command.workingDirectory));
    
    nlohmann::json j = command;
    _clusterConnectionHandler.sendMessage(*cluster, *node, j);
}

void MainWindow::stopProcess(Process::ID processId) {
    Process* process = data::findProcess(processId);
    Cluster* cluster = data::findCluster(process->clusterId);
    Node* node = data::findNode(process->nodeId);

    common::CommandMessage command = exitProcessCommand(*process);

    Log("Sending Message to stop application:");
    Log(fmt::format("\tCluster: {} {}", cluster->name, cluster->id));
    Log(fmt::format("\tCommand: {}", command.command));
    Log(fmt::format("\tExecutable: {}", command.executable));
    Log(fmt::format("\tIdentifier: {}", command.id));
    Log(fmt::format("\tCommandline Parameters: {}", command.commandlineParameters));
    Log(fmt::format("\tCWD: {}", command.workingDirectory));

    nlohmann::json j = command;
    _clusterConnectionHandler.sendMessage(*cluster, *node, j);
}
