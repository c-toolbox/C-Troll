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
#include "genericmessage.h"
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

    // Programs
    Log(fmt::format("Loading programs from directory {}", config.applicationPath));
    _programs = loadProgramsFromDirectory(config.applicationPath);
    _programWidget = new ProgramsWidget(_programs);
    connect(
        _programWidget, &ProgramsWidget::startProgram,
        this, &MainWindow::startProgram
    );
    tabWidget->addTab(_programWidget, "Programs");
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::connectedStatusChanged,
        _programWidget, &ProgramsWidget::connectedStatusChanged
    );


    // Clusters
    Log(fmt::format("Loading clusters from directory {}", config.clusterPath));
    _clusters = loadClustersFromDirectory(config.clusterPath);
    _clustersWidget = new ClustersWidget(_clusters);
    tabWidget->addTab(_clustersWidget, "Clusters");
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::connectedStatusChanged,
        _clustersWidget, &ClustersWidget::connectedStatusChanged
    );


    // Processes
    _processesWidget = new ProcessesWidget;
    tabWidget->addTab(_processesWidget, "Processes");
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedTrayProcess,
        [this](common::TrayProcessStatus status) {
        const auto it = std::find_if(
            _processes.begin(), _processes.end(),
            [status](const Process& p) { return p.id == status.processId; }
        );
        if (it != _processes.end()) {
            it->status = status.status;

            // The process was already known to us, which should always be the case
            _processesWidget->processUpdated(it->id);
        }
    }
    );


    // Log messages
    tabWidget->addTab(_messageBox, "Log");



    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::messageReceived,
        [](const Cluster& cluster, const Cluster::Node& node, nlohmann::json message) {
            Log(fmt::format("{} {} {}", cluster.name, node.name, std::string(message)));
        }
    );

    _clusterConnectionHandler.initialize(_clusters);
}

void MainWindow::log(std::string msg) {
    _messageBox->append(QString::fromStdString(msg));
}

void MainWindow::startProgram(const Program& program,
                              const Program::Configuration& configuration,
                              const std::string& clusterId)
{
    // We don't want to make sure that the program isn't already running as it might be
    // perfectly valid to start the program multiple times

    Log("Application: " + program.name);
    Log("Configuration: " + configuration.name);
    Log("Cluster: " + clusterId);

    // @TODO (abock, 2020-01-12) Replace the clusterId with a reference to the actual
    // cluster so that we don't need to look up the cluster itself
    const auto iCluster = std::find_if(
        _clusters.cbegin(),
        _clusters.cend(),
        [clusterId](const Cluster& c) { return c.id == clusterId; }
    );
    assert(iCluster != _clusters.end());
    const Cluster& cluster = *iCluster;

    for (const Cluster::Node& node : cluster.nodes) {
        Process process(program, configuration, cluster);
        common::TrayCommand command = startProcessCommand(process);

        // Generate identifier
        Log("Sending Message:");
        Log(fmt::format("\tCluster: {} {}", cluster.name, cluster.id));
        Log(fmt::format("\tCommand: {}", command.command));
        Log(fmt::format("\tExecutable: {}", command.executable));
        Log(fmt::format("\tIdentifier: {}", command.id));
        Log(fmt::format("\tCommandline Parameters: {}", command.commandlineParameters));
        Log(fmt::format("\tCWD: {}", command.currentWorkingDirectory));

        // We have to wrap the TrayCommand into a GenericMessage first
        common::GenericMessage msg;
        msg.type = common::TrayCommand::Type;
        msg.payload = command;

        nlohmann::json j = msg;
        _clusterConnectionHandler.sendMessage(cluster, node, j);

        _processes.push_back(std::move(process));
        _processesWidget->processAdded(_processes.back());
    }
}
