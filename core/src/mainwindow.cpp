/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sund�n, Emil Axelsson                                            *
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
#include "configurationwidget.h"
#include "database.h"
#include "jsonload.h"
#include "killallmessage.h"
#include "processwidget.h"
#include "programwidget.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QTabBar>
#include <QVBoxLayout>
#include <filesystem>
#include <set>
#include <thread>

namespace {
    constexpr const char* Title = "C-Troll";

    constexpr const char* ConfigurationFile = "config.json";

    constexpr const float MainWindowWidthRatio = 0.35f;
    constexpr const float MainWindowHeightRatio = 0.25f;

    constexpr const float TabWidthRatio = 0.0333f;
    constexpr const float TabHeightRatio = 0.24f;
} // namespace

MainWindow::MainWindow() {
    // We calculate the size of the window based on the screen resolution to be somewhat
    // safe against high and low DPI monitors
    const int screenWidth = QApplication::desktop()->screenGeometry().width();
    const int screenHeight = QApplication::desktop()->screenGeometry().height();

    const int widgetWidth = screenWidth * MainWindowWidthRatio;
    const int widgetHeight = screenHeight * MainWindowHeightRatio;

    setWindowTitle(Title);
    resize(widgetWidth, widgetHeight);

    //
    // Set up the logging
    common::Log::initialize("core", [this](std::string msg) { log(std::move(msg)); });
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
        Log(
            "Status",
            fmt::format("Creating new configuration at '{}'", ConfigurationFile)
        );

        nlohmann::json obj = Configuration();
        std::ofstream file(ConfigurationFile);
        file << obj.dump(2);
    }
    Log("Status", fmt::format("Loading configuration file '{}'", ConfigurationFile));
    _config = common::loadFromJson<Configuration>(ConfigurationFile);

    //
    // Load the data
    Log("Status", fmt::format("Loading programs from '{}'", _config.applicationPath));
    Log("Status", fmt::format("Loading nodes from '{}'", _config.nodePath));
    Log("Status", fmt::format("Loading clusters from '{}'", _config.clusterPath));
    data::loadData(_config.applicationPath, _config.clusterPath, _config.nodePath);
    data::setTagColors(_config.tagColors);

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
    _processesWidget = new ProcessesWidget(_config.removalTimeout);
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedTrayProcess,
        [this](common::ProcessStatusMessage status) {
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
    );
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedTrayStatus,
        [this](Node::ID, common::TrayStatusMessage status) {
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
                    constexpr const char* Text = "Received information from a tray about "
                        "a running process that was started from a controller with a "
                        "different set of configurations. Depending on what was changed "
                        "this might lead to very strange behavior";
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
    );
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedInvalidAuthStatus,
        [this](Node::ID id, common::InvalidAuthMessage) {
            Node* node = data::findNode(id);
         
            std::string m = fmt::format(
                "Send invalid authentication token to node {}", node->name
            );
            QMessageBox::critical(this, "Error in Connection", QString::fromStdString(m));
        }
    );
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedProcessMessage,
        _processesWidget, &ProcessesWidget::receivedProcessMessage
    );
    connect(
        _processesWidget, &ProcessesWidget::killProcess,
        [this](Process::ID processId) { stopProcess(processId); }
    );
    connect(
        _processesWidget, &ProcessesWidget::killAllProcesses,
        [this]() { killAllProcesses(Cluster::ID(-1)); }
    );

    // Set up the tab widget
    tabWidget->addTab(_programWidget, "Programs");
    tabWidget->addTab(_clustersWidget, "Clusters");
    tabWidget->addTab(_processesWidget, "Processes");
    tabWidget->addTab(&_messageBox, "Log");
    tabWidget->addTab(new ConfigurationWidget(_config, ConfigurationFile), "Settings");

    std::string style = fmt::format(
        "QTabBar::tab {{ height: {}px; width: {}px; }}",
        widgetHeight * TabHeightRatio, widgetWidth * TabWidthRatio
    );
    tabWidget->setStyleSheet(QString::fromStdString(style));

    _clusterConnectionHandler.initialize();
}

void MainWindow::log(std::string msg) {
    _messageBox.append(QString::fromStdString(msg));
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
    Node* node = data::findNode(process->nodeId);

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
    Node* node = data::findNode(process->nodeId);

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
