/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2023                                                               *
 * Alexander Bock                                                                        *
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
#include "messages.h"
#include "processwidget.h"
#include "programwidget.h"
#include "restconnectionhandler.h"
#include "settingswidget.h"
#include "version.h"
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QProcess>
#include <QTabBar>
#include <QTimer>
#include <QVBoxLayout>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <set>
#include <string_view>
#include <thread>

MainWindow::MainWindow(std::vector<std::string> defaultTags, Configuration config)
    : _trayIcon(QIcon(":/images/C_transparent.png"), this)
{
    setWindowTitle("C-Troll");

    //
    // Set up the logging
    qInstallMessageHandler(
        // Now that the log is enabled and available, we can pipe all Qt messages to that
        [](QtMsgType, const QMessageLogContext&, const QString& msg) {
            Log("Qt", msg.toLocal8Bit().constData());
        }
    );

    _trayIcon.setToolTip("C-Troll");

    // After that create a context menu of two items
    QMenu* menu = new QMenu(this);

    // The first menu item expands the application from the tray,
    _showAction = new QAction("Show", this);
    connect(_showAction, &QAction::triggered, this, &MainWindow::show);
    menu->addAction(_showAction);

    _hideAction = new QAction("Hide", this);
    connect(_hideAction, &QAction::triggered, this, &MainWindow::hide);
    menu->addAction(_hideAction);

    // The second menu item terminates the application
    QAction* quit = new QAction("Quit", this);
    connect(
        quit, &QAction::triggered,
        [this]() {
            _isClosingApplication = true;
            qApp->quit();
        }
    );
    menu->addAction(quit);

    // Set the context menu on the icon and show the application icon in the system tray
    _trayIcon.setContextMenu(menu);
    _trayIcon.show();

    // Also connect clicking on the icon to the signal processor of this press
    connect(&_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    connect(qApp, &QCoreApplication::aboutToQuit, &_trayIcon, &QSystemTrayIcon::hide);

    //
    // Set up the container widgets
    QWidget* center = new QWidget;
    QBoxLayout* layout = new QVBoxLayout(center);
    layout->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(center);

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setTabPosition(QTabWidget::West);
    layout->addWidget(tabWidget);

    common::Log::ref()->setLoggingFunction(
        [this](std::string msg) { log(std::move(msg)); }
    );

    //
    // Load the data
    Log("Status", std::format("Loading programs from '{}'", config.applicationPath));
    if (!std::filesystem::exists(config.applicationPath)) {
        std::string msg = std::format(
            "Could not find application path '{}'", config.applicationPath
        );
        QMessageBox::critical(
            nullptr,
            "Error loading",
            QString::fromStdString(msg)
        );
        exit(EXIT_FAILURE);
    }

    Log("Status", std::format("Loading nodes from '{}'", config.nodePath));
    if (!std::filesystem::exists(config.nodePath)) {
        std::string msg = std::format("Could not find node path '{}'", config.nodePath);
        QMessageBox::critical(
            nullptr,
            "Error loading",
            QString::fromStdString(msg)
        );
        exit(EXIT_FAILURE);
    }

    Log("Status", std::format("Loading clusters from '{}'", config.clusterPath));
    if (!std::filesystem::exists(config.clusterPath)) {
        std::string msg = std::format(
            "Could not find cluster path '{}'", config.clusterPath
        );
        QMessageBox::critical(
            nullptr,
            "Error loading",
            QString::fromStdString(msg)
        );
        exit(EXIT_FAILURE);
    }

    bool success = data::loadData(
        config.applicationPath,
        config.clusterPath,
        config.nodePath
    );
    if (!success) {
        QMessageBox::critical(
            nullptr,
            "Error loading",
            "Error occured while loading data, inspect the Log for detailed information"
        );
    }
    data::setTagColors(config.tagColors);

    if (config.logRotation.has_value()) {
        const bool keepLog = config.logRotation->keepPrevious;
        const std::chrono::hours freq = config.logRotation->frequency;

        QTimer* timer = new QTimer(this);
        timer->setTimerType(Qt::VeryCoarseTimer);
        connect(
            timer, &QTimer::timeout,
            [keepLog]() { common::Log::ref()->performLogRotation(keepLog); }
        );
        timer->start(std::chrono::duration_cast<std::chrono::milliseconds>(freq));
    }

    //
    // Create the widgets
    // Programs
    _programWidget = new programs::ProgramsWidget;

    bool anyMissingTags = std::any_of(
        defaultTags.cbegin(), defaultTags.cend(),
        [](const std::string& tag) { return !data::tags().contains(tag); }
    );
    if (anyMissingTags) {
        Log(
            "Status",
            "Started C-Troll with default tags, one of which did not exist. Removing the "
            "ones that did not exist"
        );

        defaultTags.erase(
            std::remove_if(
                defaultTags.begin(), defaultTags.end(),
                [](const std::string& tag) { return !data::tags().contains(tag); }
            ),
            defaultTags.end()
        );
    }

    if (!defaultTags.empty()) {
        _programWidget->selectTags(defaultTags);
    }
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
        _programWidget, &programs::ProgramsWidget::startCustomProgram,
        this, &MainWindow::startCustomProgram
    );
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::connectedStatusChanged,
        _programWidget, &programs::ProgramsWidget::connectedStatusChanged
    );


    // Clusters
    _clustersWidget = new ClustersWidget(config.showShutdownButtons);
    connect(
        &_clusterConnectionHandler, &ClusterConnectionHandler::connectedStatusChanged,
        _clustersWidget, &ClustersWidget::connectedStatusChanged
    );
    connect(
        _clustersWidget, QOverload<Node::ID>::of(&ClustersWidget::killProcesses),
        this, QOverload<Node::ID>::of(&MainWindow::killAllProcesses)
    );
    connect(
        _clustersWidget, QOverload<Cluster::ID>::of(&ClustersWidget::killProcesses),
        this, QOverload<Cluster::ID>::of(&MainWindow::killAllProcesses)
    );
    connect(_clustersWidget, &ClustersWidget::killTray, this, &MainWindow::killTray);
    connect(_clustersWidget, &ClustersWidget::killTrays, this, &MainWindow::killTrays);
    connect(
        _clustersWidget, &ClustersWidget::restartNode,
        this, &MainWindow::restartNode
    );
    connect(
        _clustersWidget, &ClustersWidget::restartNodes,
        this, &MainWindow::restartNodes
    );
    connect(
        _clustersWidget, &ClustersWidget::shutdownNode,
        this, &MainWindow::shutdownNode
    );
    connect(
        _clustersWidget, &ClustersWidget::shutdownNodes,
        this, &MainWindow::shutdownNodes
    );

    // Processes
    _processesWidget = new ProcessesWidget(config.removalTimeout);
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
        &_clusterConnectionHandler, &ClusterConnectionHandler::receivedErrorMessage,
        this, &MainWindow::handleErrorMessage
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
    tabWidget->addTab(new SettingsWidget(config, "config.json"), "Settings");

    _clusterConnectionHandler.initialize();


    if (config.restLoopback.has_value()) {
        _restLoopbackHandler = new RestConnectionHandler(
            this,
            config.restLoopback->port,
            true, // only accept local connection
            config.restLoopback->username,
            config.restLoopback->password,
            config.restLoopback->allowCustomPrograms
        );

        connect(
            _restLoopbackHandler, &RestConnectionHandler::startProgram,
            this, &MainWindow::startProgram
        );
        connect(
            _restLoopbackHandler, &RestConnectionHandler::stopProgram,
            this, &MainWindow::stopProgram
        );
        connect(
            _restLoopbackHandler, &RestConnectionHandler::startCustomProgram,
            this, &MainWindow::startCustomProgram
        );
    }

    if (config.restGeneral.has_value()) {
        _restGeneralHandler = new RestConnectionHandler(
            this,
            config.restGeneral->port,
            false, // do not only accept local connection
            config.restGeneral->username,
            config.restGeneral->password,
            config.restGeneral->allowCustomPrograms
        );

        connect(
            _restGeneralHandler, &RestConnectionHandler::startProgram,
            this, &MainWindow::startProgram
        );
        connect(
            _restGeneralHandler, &RestConnectionHandler::stopProgram,
            this, &MainWindow::stopProgram
        );
        connect(
            _restGeneralHandler, &RestConnectionHandler::startCustomProgram,
            this, &MainWindow::startCustomProgram
        );
    }

    auto maybeShowMessages = [this]() {
        if (_shouldShowDifferentDataHashMessage) {
            constexpr const char* Text = "Received information from a tray about a "
                "running process that was started from a controller with a different set "
                "of configurations. Depending on what was changed this might lead to "
                "very strange behavior";
            _trayIcon.showMessage("Different Data", Text, QSystemTrayIcon::Warning);
            Log("Warning", Text);
            _shouldShowDifferentDataHashMessage = false;
        }
    };

    // A permanently running timer that will check every 5 seconds if a new tray has
    // connected and should show a message if the data hash has changed
    QTimer* longTermTimer = new QTimer(this);
    longTermTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(longTermTimer, &QTimer::timeout, maybeShowMessages);
    longTermTimer->start(std::chrono::seconds(5));

    // Don't want to wait 5 seconds for the first message, so we check once after 250ms
    QTimer::singleShot(std::chrono::milliseconds(250), maybeShowMessages);

    // Notify the user if configuration files have changed
    _watcher.addPaths({
        QString::fromStdString(config.applicationPath),
        QString::fromStdString(config.clusterPath),
        QString::fromStdString(config.nodePath)
    });

    auto watchAllFilesInFolder = [this](std::string path) {
        QDirIterator it(
            QString::fromStdString(path),
            QStringList() << "*.json",
            QDir::Files,
            QDirIterator::Subdirectories
        );
        while (it.hasNext()) {
            _watcher.addPath(it.next());
        }
    };
    watchAllFilesInFolder(config.applicationPath);
    watchAllFilesInFolder(config.clusterPath);
    watchAllFilesInFolder(config.nodePath);

    connect(
        &_watcher, &QFileSystemWatcher::directoryChanged,
        [this, watchAllFilesInFolder](const QString& path) {
            constexpr const char* Text = "The data files on disk have changed. This "
                "change is not reflected until C-Troll is restarted.";
            _trayIcon.showMessage("New Data", Text);
            Log("Info", Text);

            watchAllFilesInFolder(path.toStdString());
        }
    );
    connect(
        &_watcher, &QFileSystemWatcher::fileChanged,
        [this](const QString& path) {
            if (!_watcher.files().contains(path) && QFile(path).exists()) {
                // The file was deleted, but has been recreated directly again, in which
                // case we have to continue watching it
                _watcher.addPath(path);
            }

            constexpr const char* Text = "The data files on disk have changed. This "
                "change is not reflected until C-Troll is restarted.";
            Log("Info", Text);
            _trayIcon.showMessage("New Data", Text);
        }
    );
}

void MainWindow::log(std::string msg) {
    _logWidget.appendMessage(std::move(msg));
}

void MainWindow::handleTrayProcess(common::ProcessStatusMessage status) {
    if (status.processId < 0) {
        // This will be the case for custom processes, since we manually assign them
        // negative process ids.  We don't store any process information for them, so
        // there is nothing to update here
        return;
    }

    const Process* process = data::findProcess(Process::ID(status.processId));
    if (!process) {
        // This state might happen if C-Troll was restarted while programs were
        // still running on the trays, if we than issue a killall command, we are
        // handed back a process id that we don't know.
        return;
    }

    data::setProcessStatus(process->id, status.status);

    // The process was already known to us, which should always be the case
    _processesWidget->processUpdated(process->id);
    _programWidget->processUpdated(process->id);
}

void MainWindow::handleTrayStatus(Node::ID, common::TrayStatusMessage status) {
    // We need to remove all negative process ids as these are custom programs that we
    // don't really care about
    status.processes.erase(
        std::remove_if(
            status.processes.begin(), status.processes.end(),
            [](const common::TrayStatusMessage::ProcessInfo& pi) {
                return pi.processId < 0;
            }
        ),
        status.processes.end()
    );

    // We need to check this *after* we remove the negative process IDs as we will
    // otherwise run into issues if the only running process is a custom one
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
        const Process* proc = data::findProcess(pid);
        if (proc) {
            Log(
                "Status",
                std::format("Ignoring process with duplicate id {}", pi.processId)
            );
            continue;
        }

        if (pi.dataHash != data::dataHash()) {
            _shouldShowDifferentDataHashMessage = true;
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
    const Node* node = data::findNode(id);
    assert(node);

    std::string m = std::format("Send invalid auth token to node {}", node->name);
    QMessageBox::critical(this, "Error in Connection", QString::fromStdString(m));
}

void MainWindow::handleErrorMessage(Node::ID id, common::ErrorOccurredMessage message) {
    const Node* node = data::findNode(id);
    assert(node);

    std::string msg = std::accumulate(
        message.lastMessages.begin(), message.lastMessages.end(),
        std::string(""),
        [](const std::string& lhs, const std::string& rhs) { return lhs + "\n\n" + rhs; }
    );
    QMessageBox::critical(
        this,
        "Tray error",
        QString::fromStdString(std::format(
            "Node {} reported a critical error. It said: \n{}\n\nThe last {} messages "
            "that were received on that node were:\n\n{}",
            node->name, message.error, message.lastMessages.size(), msg
        ))
    );
}

void MainWindow::startProgram(Cluster::ID clusterId, Program::ID programId,
                              Program::Configuration::ID configId)
{
    // We don't want to make sure that the program isn't already running as it might be
    // perfectly valid to start the program multiple times

    const Cluster* cluster = data::findCluster(clusterId);
    assert(cluster);

    const Program* p = data::findProgram(programId);
    assert(p);

    // If the program has a preStart script, we need to execute it first and wait until it
    // is finished
    if (!p->preStart.empty()) {
        Log("Program", "Starting pre-start script");
        QProcess proc;
        proc.start(QString::fromStdString(p->preStart));
        proc.waitForFinished(-1);
    }

    for (const std::string& nodeName : cluster->nodes) {
        const Node* node = data::findNode(nodeName);
        auto proc = std::make_unique<Process>(programId, configId, clusterId, node->id);
        Process::ID id = proc->id;
        data::addProcess(std::move(proc));

        startProcess(id);
        _processesWidget->processAdded(id);

        // If the program wants, we need to sleep for a duration before submitting the
        // next process
        if (p->delay.has_value()) {
            std::this_thread::sleep_for(*p->delay);
        }
    }
}

void MainWindow::startCustomProgram(Node::ID nodeId, std::string executable,
                                    std::string workingDir, std::string arguments)
{
    static int CustomCommandId = -1;

    const Node* n = data::findNode(nodeId);
    assert(n);

    common::StartCommandMessage command;
    command.id = CustomCommandId;
    command.executable = std::move(executable);
    command.workingDirectory = std::move(workingDir);
    command.commandlineParameters = std::move(arguments);

    if (!n->secret.empty()) {
        command.secret = n->secret;
    }

    nlohmann::json j = command;
    if (!n->secret.empty()) {
        command.secret = n->secret;
    }
    _clusterConnectionHandler.sendMessage(*n, j);

    // Decrease the ID for the next custom program
    --CustomCommandId;
}

void MainWindow::stopProgram(Cluster::ID clusterId, Program::ID programId,
                             Program::Configuration::ID configurationId) const
{
    // First, collect all the processes that belong to this program combination
    std::vector<const Process*> processes;
    for (const Process* process : data::processes()) {
        assert(process);
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

    for (const Process* process : processes) {
        stopProcess(process->id);
    }
}

void MainWindow::startProcess(Process::ID processId) const {
    const Process* process = data::findProcess(processId);
    assert(process);
    const Node* node = data::findNode(process->nodeId);
    assert(node);

    common::StartCommandMessage command = startProcessCommand(*process);
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }

    _clusterConnectionHandler.sendMessage(*node, command);
}

void MainWindow::stopProcess(Process::ID processId) const {
    const Process* process = data::findProcess(processId);
    assert(process);
    const Node* node = data::findNode(process->nodeId);
    assert(node);

    common::ExitCommandMessage command = exitProcessCommand(*process);
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }

    _clusterConnectionHandler.sendMessage(*node, command);
}

void MainWindow::killAllProcesses(Cluster::ID id) const {
    Log("Sending", "Send message to stop all programs");

    std::vector<const Node*> nodes;
    if (id.v == -1) {
        // Send kill command to all clusters
        for (const Cluster* cluster : data::clusters()) {
            std::vector<const Node*> ns = data::findNodesForCluster(*cluster);
            std::copy(ns.begin(), ns.end(), std::back_inserter(nodes));
        }

        // We have probably picked up a number of duplicates in this process as nodes can
        // be specified in multiple clusters
        std::sort(nodes.begin(), nodes.end());
        nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
    }
    else {
        // We want to send the kill command only to the nodes of a specific cluster
        const Cluster* cluster = data::findCluster(id);
        nodes = data::findNodesForCluster(*cluster);
    }

    for (const Node* node : nodes) {
        if (!node->isConnected) {
            continue;
        }

        common::KillAllMessage command;
        if (!node->secret.empty()) {
            command.secret = node->secret;
        }
        _clusterConnectionHandler.sendMessage(*node, command);
    }
}

void MainWindow::killAllProcesses(Node::ID id) const {
    const Node* node = data::findNode(id);
    assert(node);

    common::KillAllMessage command;
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }
    _clusterConnectionHandler.sendMessage(*node, command);
}

void MainWindow::killTray(Node::ID id) const {
    Log("Sending", std::format("Send message to kill Tray on {}", id.v));
    const Node* node = data::findNode(id);
    assert(node);

    common::KillTrayMessage command;
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }
    _clusterConnectionHandler.sendMessage(*node, command);
}

void MainWindow::killTrays(Cluster::ID id) const {
    Log("Sending", std::format("Send message to kill Trays on {}", id.v));

    std::vector<const Node*> nodes;
    if (id.v == -1) {
        // Send kill command to all clusters
        for (const Cluster* cluster : data::clusters()) {
            std::vector<const Node*> ns = data::findNodesForCluster(*cluster);
            std::copy(ns.begin(), ns.end(), std::back_inserter(nodes));
        }

        // We have probably picked up a number of duplicates in this process as nodes can
        // be specified in multiple clusters
        std::sort(nodes.begin(), nodes.end());
        nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
    }
    else {
        // We want to send the kill command only to the nodes of a specific cluster
        const Cluster* cluster = data::findCluster(id);
        nodes = data::findNodesForCluster(*cluster);
    }

    for (const Node* node : nodes) {
        common::KillTrayMessage command;
        if (!node->secret.empty()) {
            command.secret = node->secret;
        }
        _clusterConnectionHandler.sendMessage(*node, command);
    }
}

void MainWindow::restartNode(Node::ID id) const {
    Log("Sending", std::format("Send message to restart node {}", id.v));
    const Node* node = data::findNode(id);
    assert(node);

    common::RestartNodeMessage command;
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }
    _clusterConnectionHandler.sendMessage(*node, command);
}

void MainWindow::restartNodes(Cluster::ID id) const {
    Log("Sending", std::format("Send message to restart nodes on {}", id.v));

    std::vector<const Node*> nodes;
    if (id.v == -1) {
        // Send kill command to all clusters
        for (const Cluster* cluster : data::clusters()) {
            std::vector<const Node*> ns = data::findNodesForCluster(*cluster);
            std::copy(ns.begin(), ns.end(), std::back_inserter(nodes));
        }

        // We have probably picked up a number of duplicates in this process as nodes can
        // be specified in multiple clusters
        std::sort(nodes.begin(), nodes.end());
        nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
    }
    else {
        // We want to send the kill command only to the nodes of a specific cluster
        const Cluster* cluster = data::findCluster(id);
        nodes = data::findNodesForCluster(*cluster);
    }

    for (const Node* node : nodes) {
        common::RestartNodeMessage command;
        if (!node->secret.empty()) {
            command.secret = node->secret;
        }
        _clusterConnectionHandler.sendMessage(*node, command);
    }
}

void MainWindow::shutdownNode(Node::ID id) const {
    Log("Sending", std::format("Send message to shut down node {}", id.v));
    const Node* node = data::findNode(id);
    assert(node);

    common::ShutdownNodeMessage command;
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }
    _clusterConnectionHandler.sendMessage(*node, command);
}

void MainWindow::shutdownNodes(Cluster::ID id) const {
    Log("Sending", std::format("Send message to shut down nodes on {}", id.v));

    std::vector<const Node*> nodes;
    if (id.v == -1) {
        // Send kill command to all clusters
        for (const Cluster* cluster : data::clusters()) {
            std::vector<const Node*> ns = data::findNodesForCluster(*cluster);
            std::copy(ns.begin(), ns.end(), std::back_inserter(nodes));
        }

        // We have probably picked up a number of duplicates in this process as nodes can
        // be specified in multiple clusters
        std::sort(nodes.begin(), nodes.end());
        nodes.erase(std::unique(nodes.begin(), nodes.end()), nodes.end());
    }
    else {
        // We want to send the kill command only to the nodes of a specific cluster
        const Cluster* cluster = data::findCluster(id);
        nodes = data::findNodesForCluster(*cluster);
    }

    for (const Node* node : nodes) {
        common::ShutdownNodeMessage command;
        if (!node->secret.empty()) {
            command.secret = node->secret;
        }
        _clusterConnectionHandler.sendMessage(*node, command);
    }
}

// The method that handles the closing event of the application window
void MainWindow::closeEvent(QCloseEvent* event) {
    if (_isClosingApplication) {
        // We are closing the appliation via the system tray, so we don't interfere
        return;
    }

    // If the window is visible, and the checkbox is checked, then the completion of the
    // application. Ignored, and the window simply hides that accompanied the
    // corresponding pop-up message
    if (isVisible()) {
        event->ignore();
        hide();

        _trayIcon.showMessage(
            "C-Troll",
            "C-Troll is still running in the background",
            QSystemTrayIcon::Information,
            1000
        );
    }
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::WindowStateChange) {
        // Hide the taskbar icon if the window is minimized
        if (isMinimized()) {
            hide();
        }
        event->ignore();
    }
}

// The method that handles click on the application icon in the system tray
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        // If the window is visible, it is hidden
        // Conversely, if hidden, it unfolds on the screen
        if (isVisible()) {
            hide(); // Hide the taskbar icon

        }
        else {
            show(); // Show the taskbar icon
            showNormal(); // Bring the window to the front

        }
    }
}

void MainWindow::showEvent(QShowEvent*) {
    _showAction->setVisible(false);
    _hideAction->setVisible(true);
}

void MainWindow::hideEvent(QHideEvent*) {
    _showAction->setVisible(true);
    _hideAction->setVisible(false);
}
