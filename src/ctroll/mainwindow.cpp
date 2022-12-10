/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2022                                                             *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
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

#include "apiversion.h"
#include "clusterwidget.h"
#include "database.h"
#include "jsonload.h"
#include "killallmessage.h"
#include "killtraymessage.h"
#include "processwidget.h"
#include "programwidget.h"
#include "restconnectionhandler.h"
#include "settingswidget.h"
#include "version.h"
#include <QApplication>
#include <QMenu>
#include <QMessageBox>
#include <QTabBar>
#include <QTimer>
#include <QVBoxLayout>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <set>
#include <string_view>
#include <thread>

MainWindow::MainWindow(bool shouldLogDebug) {
    setWindowTitle("C-Troll");

    //
    // Set up the logging
    qInstallMessageHandler(
        // Now that the log is enabled and available, we can pipe all Qt messages to that
        [](QtMsgType, const QMessageLogContext&, const QString& msg) {
            Log("Qt", msg.toLocal8Bit().constData());
        }
    );

    // Initialize the tray icon, set the icon of a set of system icons,
    // as well as set a tooltip
    QSystemTrayIcon* trayIcon = new QSystemTrayIcon(
        QIcon(":/images/C_transparent.png"),
        this
    );
    trayIcon->setToolTip("C-Troll");

    // After that create a context menu of two items
    QMenu* menu = new QMenu(this);
    // The first menu item expands the application from the tray,
    QAction* viewWindow = new QAction("Show", this);
    connect(viewWindow, &QAction::triggered, this, &MainWindow::show);
    menu->addAction(viewWindow);

    // The second menu item terminates the application
    QAction* quit = new QAction("Quit", this);
    connect(quit, &QAction::triggered, qApp, &QApplication::quit);
    menu->addAction(quit);

    // Set the context menu on the icon and show the application icon in the system tray
    trayIcon->setContextMenu(menu);
    trayIcon->show();

    // Also connect clicking on the icon to the signal processor of this press
    connect(trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    connect(qApp, &QCoreApplication::aboutToQuit, trayIcon, &QSystemTrayIcon::hide);

    //
    // Set up the container widgets
    QWidget* center = new QWidget;
    QBoxLayout* layout = new QVBoxLayout(center);
    layout->setContentsMargins(0, 0, 0, 0);
    setCentralWidget(center);

    QTabWidget* tabWidget = new QTabWidget;
    tabWidget->setTabPosition(QTabWidget::West);
    layout->addWidget(tabWidget);

    //
    // Load the configuration
    if (!std::filesystem::exists(BaseConfiguration::ConfigurationFile)) {
        std::cout << fmt::format(
            "Creating new configuration at '{}'\n",
            BaseConfiguration::ConfigurationFile
        );

        nlohmann::json obj = Configuration();
        std::ofstream file = std::ofstream(BaseConfiguration::ConfigurationFile);
        file << obj.dump(2);
    }
    std::cout << fmt::format(
        "Loading configuration '{}'\n", BaseConfiguration::ConfigurationFile
    );
    _config = common::loadFromJson<Configuration>(BaseConfiguration::ConfigurationFile);
    common::Log::initialize(
        "ctroll",
        _config.logFile,
        shouldLogDebug,
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
            [keepLog]() { common::Log::ref()->performLogRotation(keepLog); }
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
        _programWidget, &programs::ProgramsWidget::startCustomProgram,
        this, &MainWindow::startCustomProgram
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
        this, QOverload<Node::ID>::of(&MainWindow::killAllProcesses)
    );
    connect(
        _clustersWidget, QOverload<Cluster::ID>::of(&ClustersWidget::killProcesses),
        this, QOverload<Cluster::ID>::of(&MainWindow::killAllProcesses)
    );
    connect(_clustersWidget, &ClustersWidget::killTray, this, &MainWindow::killTray);
    connect(_clustersWidget, &ClustersWidget::killTrays, this, &MainWindow::killTrays);

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
    tabWidget->addTab(
        new SettingsWidget(_config, BaseConfiguration::ConfigurationFile),
        "Settings"
    );

    _clusterConnectionHandler.initialize();


    if (_config.restLoopback.has_value()) {
        _restLoopbackHandler = new RestConnectionHandler(
            this,
            _config.restLoopback->port,
            true, // only accept local connection
            _config.restLoopback->username,
            _config.restLoopback->password,
            _config.restLoopback->allowCustomPrograms
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

    if (_config.restGeneral.has_value()) {
        _restGeneralHandler = new RestConnectionHandler(
            this,
            _config.restGeneral->port,
            false, // do not only accept local connection
            _config.restGeneral->username,
            _config.restGeneral->password,
            _config.restGeneral->allowCustomPrograms
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

    auto maybeShowDataHashMessage = [this]() {
        if (_shouldShowDataHashMessage) {
            constexpr const char* Text = "Received information from a tray about a "
                "running process that was started from a controller with a different set "
                "of configurations. Depending on what was changed this might lead to "
                "very strange behavior";
            QMessageBox::warning(this, "Different Data", Text);
            Log("Warning", Text);
            _shouldShowDataHashMessage = false;
        }
    };

    // A permanently running timer that will check every 5 seconds if a new tray has
    // connected and should show a message if the data hash has changed
    QTimer* longTermTimer = new QTimer(this);
    longTermTimer->setTimerType(Qt::VeryCoarseTimer);
    connect(longTermTimer, &QTimer::timeout, maybeShowDataHashMessage);
    longTermTimer->start(std::chrono::seconds(5));

    // Don't want to wait 5 seconds for the first message, so we check once after 250ms
    QTimer::singleShot(std::chrono::milliseconds(250), maybeShowDataHashMessage);
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
                fmt::format("Ignoring process with duplicate id {}", pi.processId)
            );
            continue;
        }

        if (pi.dataHash != data::dataHash()) {
            _shouldShowDataHashMessage = true;
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

    std::string m = fmt::format("Send invalid auth token to node {}", node->name);
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
        QString::fromStdString(fmt::format(
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
    for (const std::string& nodeName : cluster->nodes) {
        const Node* node = data::findNode(nodeName);
        auto process = std::make_unique<Process>(
            programId,
            configId,
            clusterId,
            node->id
        );
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

    nlohmann::json j = command;
    _clusterConnectionHandler.sendMessage(*node, j);
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

    nlohmann::json j = command;
    _clusterConnectionHandler.sendMessage(*node, j);
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
        common::KillAllMessage command;
        if (!node->secret.empty()) {
            command.secret = node->secret;
        }
        nlohmann::json j = command;
        _clusterConnectionHandler.sendMessage(*node, j);
    }
}

void MainWindow::killAllProcesses(Node::ID id) const {
    const Node* node = data::findNode(id);
    assert(node);

    common::KillAllMessage command;
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }
    nlohmann::json j = command;
    _clusterConnectionHandler.sendMessage(*node, j);
}

void MainWindow::killTray(Node::ID id) const {
    Log("Sending", fmt::format("Send message to kill Tray on {}", id.v));
    const Node* node = data::findNode(id);
    assert(node);

    common::KillTrayMessage command;
    if (!node->secret.empty()) {
        command.secret = node->secret;
    }
    nlohmann::json j = command;
    _clusterConnectionHandler.sendMessage(*node, j);
}

void MainWindow::killTrays(Cluster::ID id) const {
    Log("Sending", fmt::format("Send message to kill Trays on {}", id.v));

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
        nlohmann::json j = command;
        _clusterConnectionHandler.sendMessage(*node, j);
    }
}

// The method that handles the closing event of the application window
void MainWindow::closeEvent(QCloseEvent* event) {
    // If the window is visible, and the checkbox is checked, then the completion of the
    // application. Ignored, and the window simply hides that accompanied the
    // corresponding pop-up message
    if (isVisible()) {
        event->ignore();
        hide();
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
