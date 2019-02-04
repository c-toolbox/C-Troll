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

#include "coreprocess.h"

#include "cluster.h"
#include "jsonsupport.h"
#include "program.h"
#include "guiprocessstatus.h"
#include <logging.h>
#include <QDebug>
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonParseError>
#include <cassert>

int CoreProcess::_nextId = 0;

CoreProcess::CoreProcess(Program* application, const QString& configurationId, Cluster* cluster)
    : _id(_nextId++)
    , _application(application)
    , _configurationId(configurationId)
    , _cluster(cluster)
    , _clusterStatus({ CoreProcess::ClusterStatus::Status::Starting })
{}

int CoreProcess::id() const {
    return _id;
}

Program* CoreProcess::application() const {
    return _application;
}

QString CoreProcess::configurationId() const {
    return _configurationId;
}

Cluster* CoreProcess::cluster() const {
    return _cluster;
}

bool CoreProcess::allNodesHasStatus(CoreProcess::NodeStatus::Status status) {
    bool allHasStatus = true;
    if (_nodeLogs.size() == 0) return false;

    for (auto iNode = _nodeLogs.begin(); iNode != _nodeLogs.end(); iNode++) {
        if (iNode->statuses.size() == 0) {
            allHasStatus = false;
            break;
        }
        if (iNode->statuses.back().status != status) {
            allHasStatus = false;
            break;
        }
    }
    return allHasStatus;
}

bool CoreProcess::anyNodeHasStatus(CoreProcess::NodeStatus::Status status) {
    for (auto iNode = _nodeLogs.begin(); iNode != _nodeLogs.end(); iNode++) {
        if (iNode->statuses.empty()) {
            continue;
        }
        if (iNode->statuses.back().status == status) {
            return true;
        }
    }
    return false;
}

void CoreProcess::pushNodeStatus(QString nodeId,
                                 CoreProcess::NodeStatus::Status nodeStatus)
{
    CoreProcess::ClusterStatus::Status clusterStatus = _clusterStatus.status;
    _nodeLogs[nodeId].statuses.append(
        { nodeStatus, std::chrono::system_clock::now(), _nextNodeStatusId++ }
    );

    switch (nodeStatus) {
        case CoreProcess::NodeStatus::Status::Starting:
            clusterStatus = CoreProcess::ClusterStatus::Status::Starting;
            break;
        case CoreProcess::NodeStatus::Status::Running:
            if (allNodesHasStatus(CoreProcess::NodeStatus::Status::Running)) {
                clusterStatus = CoreProcess::ClusterStatus::Status::Running;
            }
            break;
        case CoreProcess::NodeStatus::Status::NormalExit:
            if (allNodesHasStatus(CoreProcess::NodeStatus::Status::NormalExit)) {
                clusterStatus = CoreProcess::ClusterStatus::Status::Exit;
            }
            else if (!anyNodeHasStatus(CoreProcess::NodeStatus::Status::CrashExit)) {
                clusterStatus = CoreProcess::ClusterStatus::Status::PartialExit;
            }
            break;
        case CoreProcess::NodeStatus::Status::CrashExit:
            clusterStatus = CoreProcess::ClusterStatus::Status::CrashExit;
            break;
        case CoreProcess::NodeStatus::Status::FailedToStart:
            clusterStatus = CoreProcess::ClusterStatus::Status::FailedToStart;
            break;
    }

    if (_clusterStatus.status != clusterStatus) {
        _clusterStatus = { clusterStatus, std::chrono::system_clock::now() };
    }
}

CoreProcess::NodeStatus CoreProcess::latestNodeStatus(QString nodeId) const {
    auto iNodeLog = _nodeLogs.find(nodeId);
    if (iNodeLog == _nodeLogs.end()) {
        return { NodeStatus::Status::Unknown, std::chrono::system_clock::now() };
    }
    return iNodeLog->statuses.back();
}

CoreProcess::NodeLogMessage CoreProcess::latestLogMessage(QString nodeId) const {
    auto iNodeLog = _nodeLogs.find(nodeId);
    if (iNodeLog == _nodeLogs.end()) {
        return CoreProcess::NodeLogMessage();
    }
    return iNodeLog->messages.back();
}

void CoreProcess::pushNodeError(QString nodeId, CoreProcess::NodeError::Error nodeError) {
    _nodeLogs[nodeId].errors.append(
        { nodeError, std::chrono::system_clock::now(), _nextNodeErrorId++ }
    );
}

void CoreProcess::pushNodeStdOut(QString nodeId, QString message) {
    _nodeLogs[nodeId].messages.append({
        message,
        std::chrono::system_clock::now(),
        CoreProcess::NodeLogMessage::OutputType::StdOut,
        _nextLogMessageId++
    });
}

void CoreProcess::pushNodeStdError(QString nodeId, QString message) {
    _nodeLogs[nodeId].messages.append({
        message,
        std::chrono::system_clock::now(),
        CoreProcess::NodeLogMessage::OutputType::StdError,
        _nextLogMessageId++
    });
}

common::GuiInitialization::Process CoreProcess::toGuiInitializationProcess() const {
    common::GuiInitialization::Process p;
    p.id = _id;
    p.applicationId = _application->id().toStdString();
    p.clusterId = _cluster->id().toStdString();
    p.configurationId = _configurationId.toStdString();
    p.clusterStatus = clusterStatusToGuiClusterStatus(_clusterStatus.status).toStdString();
    p.clusterStatusTime = timeToGuiTime(_clusterStatus.time);

    std::vector<common::GuiInitialization::Process::NodeStatus> nodeStatusHistory;
    for (const QString& node : _nodeLogs.keys()) {
        const NodeLog& nodeLog = _nodeLogs[node];
        for (NodeStatus status : nodeLog.statuses) {
            common::GuiInitialization::Process::NodeStatus nodeStatusObject;
            nodeStatusObject.status = nodeStatusToGuiNodeStatus(status.status).toStdString();
            nodeStatusObject.time = timeToGuiTime(status.time);
            nodeStatusObject.node = node.toStdString();
            nodeStatusObject.id = status.id;
            nodeStatusHistory.push_back(nodeStatusObject);
        }
    }

    using NS = common::GuiInitialization::Process::NodeStatus;
    std::sort(
        nodeStatusHistory.begin(),
        nodeStatusHistory.end(),
        [](NS& a, NS& b) { return a.time < b.time; }
    );
    
    p.nodeStatusHistory = std::move(nodeStatusHistory);

    return p;
}

QString CoreProcess::nodeStatusToGuiNodeStatus(CoreProcess::NodeStatus::Status status) {
    switch (status) {
        case CoreProcess::NodeStatus::Status::Starting:
            return "Starting";
        case CoreProcess::NodeStatus::Status::Running:
            return "Running";
        case CoreProcess::NodeStatus::Status::FailedToStart:
            return "FailedToStart";
        case CoreProcess::NodeStatus::Status::NormalExit:
            return "NormalExit";
        case CoreProcess::NodeStatus::Status::CrashExit:
            return "CrashExit";
        case CoreProcess::NodeStatus::Status::Unknown:
        default:
            return "Unknown";
    }
}

QString CoreProcess::clusterStatusToGuiClusterStatus(CoreProcess::ClusterStatus::Status status) {
    switch (status) {
        case CoreProcess::ClusterStatus::Status::Starting:
            return "Starting";
        case CoreProcess::ClusterStatus::Status::Running:
            return "Running";
        case CoreProcess::ClusterStatus::Status::FailedToStart:
            return "FailedToStart";
        case CoreProcess::ClusterStatus::Status::Exit:
            return "Exit";
        case CoreProcess::ClusterStatus::Status::CrashExit:
            return "CrashExit";
        case CoreProcess::ClusterStatus::Status::PartialExit:
            return "PartialExit";
        case CoreProcess::ClusterStatus::Status::Unknown:
        default:
            return "Unknown";
    }
}

double CoreProcess::timeToGuiTime(std::chrono::system_clock::time_point time) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(time.time_since_epoch()).count();
}

common::GuiProcessStatus CoreProcess::toGuiProcessStatus(const QString& nodeId) const {
    common::GuiProcessStatus g;
    g.processId = _id;
    g.applicationId = _application->id().toStdString();
    g.clusterId = _cluster->id().toStdString();
    CoreProcess::NodeStatus nodeStatus = latestNodeStatus(nodeId);
    g.id = nodeStatus.id;
    g.nodeStatus[nodeId.toStdString()] = nodeStatusToGuiNodeStatus(nodeStatus.status).toStdString();
    g.time = timeToGuiTime(nodeStatus.time);
    g.clusterStatus = clusterStatusToGuiClusterStatus(_clusterStatus.status).toStdString();
    return g;
}

common::GuiProcessLogMessage
CoreProcess::latestGuiProcessLogMessage(const QString& nodeId) const
{
    common::GuiProcessLogMessage g;
    g.processId = _id;
    g.applicationId = _application->id().toStdString();
    g.clusterId = _cluster->id().toStdString();
    g.nodeId = nodeId.toStdString();
    CoreProcess::NodeLogMessage logMessage = latestLogMessage(nodeId);
    g.id = logMessage.id;
    g.logMessage = logMessage.message.toStdString();
    g.time = timeToGuiTime(logMessage.time);
    g.outputType = (logMessage.outputType == CoreProcess::NodeLogMessage::OutputType::StdOut ? "stdout" : "stderr");
    return g;
}

common::GuiProcessLogMessageHistory CoreProcess::guiProcessLogMessageHistory() const {
    common::GuiProcessLogMessageHistory h;
    h.processId = _id;
    h.applicationId = _application->id().toStdString();
    h.clusterId = _cluster->id().toStdString();

    std::vector<common::GuiProcessLogMessageHistory::LogMessage> guiLog;
    for (const QString& node : _nodeLogs.keys()) {
        const NodeLog& nodeLog = _nodeLogs[node];
        for (const NodeLogMessage& logMessage : nodeLog.messages) {
            common::GuiProcessLogMessageHistory::LogMessage guiMessage;
            guiMessage.nodeId = node.toStdString();
            guiMessage.id = logMessage.id;
            guiMessage.message = logMessage.message.toStdString();
            guiMessage.time = timeToGuiTime(logMessage.time);
            switch (logMessage.outputType) {
                case CoreProcess::NodeLogMessage::OutputType::StdOut:
                    guiMessage.outputType = "stdout";
                    break;
                case CoreProcess::NodeLogMessage::OutputType::StdError:
                    guiMessage.outputType = "stderr";
                    break;
            }
            guiLog.push_back(guiMessage);
        }
    }

    using LM = common::GuiProcessLogMessageHistory::LogMessage;
    std::sort(
        guiLog.begin(),
        guiLog.end(),
        [](const LM& a, const LM& b) { return a.id < b.id; }
    );

    h.logMessages = std::move(guiLog);

    return h;
}

common::TrayCommand CoreProcess::startProcessCommand() const {
    common::TrayCommand t;
    t.id = _id;
    t.executable = _application->executable().toStdString();
    t.baseDirectory = _application->baseDirectory().toStdString();
    t.currentWorkingDirectory = _application->currentWorkingDirectory().toStdString();
    t.command = "Start";

    t.commandlineParameters = "";

    if (!_application->commandlineParameters().isEmpty()) {
        t.commandlineParameters = _application->commandlineParameters().toStdString();
    }

    auto iConfiguration = std::find_if(
        _application->configurations().cbegin(),
        _application->configurations().cend(),
        [id =_configurationId](const Program::Configuration& config) {
            return config.id == id;
        });
    
    if (iConfiguration != _application->configurations().cend()) {
        t.commandlineParameters = t.commandlineParameters + " " +
            iConfiguration->clusterCommandlineParameters[_cluster->id()].toStdString();
    }

    return t;
}

common::TrayCommand CoreProcess::exitProcessCommand() const {
    common::TrayCommand t;
    t.id = _id;
    t.command = "Exit";
    return t;
}
