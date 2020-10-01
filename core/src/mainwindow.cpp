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
#include "database.h"
#include "jsonload.h"
#include "killallmessage.h"
#include "processwidget.h"
#include "programwidget.h"
#include "restconnectionhandler.h"
#include "settingswidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QTabBar>
#include <QTimer>
#include <QVBoxLayout>
#include <filesystem>
#include <iostream>
#include <set>
#include <thread>

namespace {
    constexpr const char* Title = "C-Troll";
    constexpr const char* ConfigurationFile = "config.json";
} // namespace

MainWindow::MainWindow() {
    setWindowTitle(Title);

    //
    // Set up the logging
    qInstallMessageHandler(
        // Now that the log is enabled and available, we can pipe all Qt messages to that
        [](QtMsgType, const QMessageLogContext&, const QString& msg) {
            Log("Qt", msg.toLocal8Bit().constData());
        }
    );

    //
    // Set up the container widgets
    QWidget* center = new QWidget;
    QBoxLayout* layout = new QVBoxLayout(center);
    layout->setMargin(0);
    setCentralWidget(center);

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setTabPosition(QTabWidget::West);
    layout->addWidget(tabWidget);

    //
    // Load the configuration
    if (!std::filesystem::exists(ConfigurationFile)) {
        std::cout <<
            fmt::format("Creating new configuration at '{}'", ConfigurationFile) << '\n';

        nlohmann::json obj = Configuration();
        std::ofstream file(ConfigurationFile);
        file << obj.dump(2);
    }
    std::cout << fmt::format("Loading configuration '{}'", ConfigurationFile) << '\n';
    _config = common::loadFromJson<Configuration>(ConfigurationFile);
    common::Log::initialize(
        "core",
        _config.logFile,
        [this](std::string msg) { log(std::move(msg)); }
    );


    //
    // Load the data
    Log("Status", fmt::format("Loading programs from '{}'", _config.applicationPath));
    Log("Status", fmt::format("Loading nodes from '{}'", _config.nodePath));
    Log("Status", fmt::format("Loading clusters from '{}'", _config.clusterPath));
    data::loadData(_config.applicationPath, _config.clusterPath, _config.nodePath);
    data::setTagColors(_config.tagColors);

    if (_config.logRotation.has_value()) {
        const bool keepLog = _config.logRotation->keepPrevious;
        const std::chrono::hours freq = _config.logRotation->frequency;

        QTimer* timer = new QTimer(this);
        timer->setTimerType(Qt::VeryCoarseTimer);
        connect(
            timer, &QTimer::timeout,
            [keepLog]() { common::Log::ref().performLogRotation(keepLog); }
        );
        timer->start(std::chrono::duration_cast<std::chrono::milliseconds>(freq));
    }

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
    connect(
        _clustersWidget, QOverload<Node::ID>::of(&ClustersWidget::killProcesses),
        [this](Node::ID id) { killAllProcesses(id); }
    );
    connect(
        _clustersWidget, QOverload<Cluster::ID>::of(&ClustersWidget::killProcesses),
        this, QOverload<Cluster::ID>::of(&MainWindow::killAllProcesses)
    );

    // Processes
    _processesWidget = new ProcessesWidget(_config.removalTimeout);
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedTrayProcess,
        this, &MainWindow::handleTrayProcess
    );
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedTrayStatus,
        this, &MainWindow::handleTrayStatus
    );
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedInvalidAuthStatus,
        this, &MainWindow::handleInvalidAuth
    );
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedProcessMessage,
        _processesWidget, &ProcessesWidget::receivedProcessMessage
    );
    connect(
        _processesWidget, &ProcessesWidget::killProcess,
        this, &MainWindow::stopProcess
    );
    connect(
        _processesWidget, &ProcessesWidget::killAllProcesses,
        [this]() { killAllProcesses(Cluster::ID(-1)); }
    );

    // Set up the tab widget
    tabWidget->addTab(_programWidget, "Programs");
    tabWidget->addTab(_clustersWidget, "Clusters");
    tabWidget->addTab(_processesWidget, "Processes");
    tabWidget->addTab(&_logWidget, "Log");
    tabWidget->addTab(new SettingsWidget(_config, ConfigurationFile), "Settings");

    _clusterConnectionHandler.initialize();

    _restHandler = new RestConnectionHandler(this, 8080, "ff", "gg");
    connect(
        _restHandler, &RestConnectionHandler::startProgram,
        this, &MainWindow::startProgram
    );
    connect(
        _restHandler, &RestConnectionHandler::stopProgram,
        this, &MainWindow::stopProgram
    );
}

void MainWindow::log(std::string msg) {
    _logWidget.appendMessage(std::move(msg));
}

void MainWindow::handleTrayProcess(common::ProcessStatusMessage status) {
    Process* process = data::findProcess(Process::ID(status.processId));
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

void MainWindow::handleTrayStatus(Node::ID, common::TrayStatusMessage status) {
    if (status.processes.empty()) {
        // Nothing to do here
        return;
    }

    // We need to sort the incoming status messages as we need to figure out which
    // id values for new processes we should use
    std::sort(
        status.processes.begin(), status.processes.end(),
        [](const common::TrayStatusMessage::ProcessInfo& lhs,
            const common::TrayStatusMessage::ProcessInfo& rhs)
    {
        return lhs.processId < rhs.processId;
    }
    );
    const int hightestId = status.processes.back().processId;
    Process::setNextIdIfHigher(hightestId + 1);

    for (const common::TrayStatusMessage::ProcessInfo& pi : status.processes) {
        const Process::ID pid = Process::ID(pi.processId);

        // We need to check if a process with this ID already exists as we might
        // have made two connections to the node under two different IP addresses
        Process* proc = data::findProcess(pid);
        if (proc) {
            Log(
                "Status",
                fmt::format("Ignoring process with duplicate id {}", pi.processId)
            );
            continue;
        }

        if (pi.dataHash != data::dataHash()) {
            constexpr const char* Text = "Received information from a tray about a "
                "running process that was started from a controller with a different set "
                "of configurations. Depending on what was changed this might lead to "
                "very strange behavior";
            QMessageBox::warning(this, "Different Data", Text);
            Log("Warning", Text);
        }


        std::unique_ptr<Process> process = std::make_unique<Process>(
            pid,
            Program::ID(pi.programId),
            Program::Configuration::ID(pi.configurationId),
            Cluster::ID(pi.clusterId),
            Node::ID(pi.nodeId)
        );
        process->status = common::ProcessStatusMessage::Status::Running;
        data::addProcess(std::move(process));
        _processesWidget->processAdded(pid);
        _programWidget->processUpdated(pid);
    }
}

void MainWindow::handleInvalidAuth(Node::ID id, common::InvalidAuthMessage) {
    Node* node = data::findNode(id);

    std::string m = fmt::format("Send invalid auth token to node {}", node->name);
    QMessageBox::critical(this, "Error in Connection", QString::fromStdString(m));
}

void MainWindow::startProgram(Cluster::ID clusterId, Program::ID programId,
                              Program::Configuration::ID configId)
{
    // We don't want to make sure that the program isn't already running as it might be
    // perfectly valid to start the program multiple times

    Cluster* cluster = data::findCluster(clusterId);
    assert(cluster);

    Program* p = data::findProgram(programId);
    assert(p);
    for (Node::ID node : cluster->nodes) {
        auto process = std::make_unique<Process>(programId, configId, clusterId, node);
        Process::ID id = process->id;
        data::addProcess(std::move(process));

        startProcess(id);
        _processesWidget->processAdded(id);

        // If the program wants, we need to sleep for a duration before submitting the
        // next process
        if (p->delay.has_value()) {
            std::this_thread::sleep_for(*p->delay);
        }
    }
}

void MainWindow::stopProgram(Cluster::ID clusterId, Program::ID programId,
                             Program::Configuration::ID configurationId) const
{
    // First, collect all the processes that belong to this program combination
    std::vector<Process*> processes;
    for (Process* process : data::processes()) {
        const bool clusterMatch = (process->clusterId == clusterId);
        const bool programMatch = (process->programId == programId);
        const bool configurationMatch = (process->configurationId == configurationId);

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

void MainWindow::startProcess(Process::ID processId) const {
    Process* process = data::findProcess(processId);
    assert(process);
    Node* node = data::findNode(process->nodeId);
    assert(node);

    common::StartCommandMessage command = startProcessCommand(*process);
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }
    
    nlohmann::json j = command;
    Log(
        fmt::format("Sending [{}:{} ({})]", node->ipAddress, node->port, node->name),
        j.dump()
    );
    _clusterConnectionHandler.sendMessage(*node, j);
}

void MainWindow::stopProcess(Process::ID processId) const {
    Process* process = data::findProcess(processId);
    assert(process);
    Node* node = data::findNode(process->nodeId);
    assert(node);

    common::ExitCommandMessage command = exitProcessCommand(*process);
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }

    nlohmann::json j = command;
    Log("Sending", j.dump());
    _clusterConnectionHandler.sendMessage(*node, j);
}

void MainWindow::killAllProcesses(Cluster::ID id) const {
    Log("Sending", "Send message to stop all programs");

    std::vector<Node*> nodes;
    if (id.v == -1) {
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

    for (Node* node : nodes) {
        common::KillAllMessage command;
        if (!node->secret.empty()) {
            command.secret = node->secret;
        }
        nlohmann::json j = command;

        Log(
            fmt::format("Sending [{}:{} ({})]", node->ipAddress, node->port, node->name),
            j.dump()
        );
        _clusterConnectionHandler.sendMessage(*node, j);
    }
}

void MainWindow::killAllProcesses(Node::ID id) const {
    Node* node = data::findNode(id);
    assert(node);

    common::KillAllMessage command;
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }
    nlohmann::json j = command;

    Log(
        fmt::format("Sending [{}:{} ({})]", node->ipAddress, node->port, node->name),
        j.dump()
    );
    _clusterConnectionHandler.sendMessage(*node, j);
}
