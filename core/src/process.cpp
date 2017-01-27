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

#include "process.h"
#include "program.h"
#include "cluster.h"

#include <QDebug>
#include <QDirIterator>
#include <QJsonArray>
#include <QJsonParseError>

#include <cassert>
#include <logging.h>

#include "guiprocessstatus.h"
#include "jsonsupport.h"

namespace {
}

int Process::_nextId = 0;

Process::Process(Program* application, const QString& configurationId, Cluster* cluster)
    : _id(_nextId++)
    , _application(application)
    , _configurationId(configurationId)
    , _cluster(cluster)
    , _clusterStatus({ Process::ClusterStatus::Status::Starting })
{
}


int Process::id() const {
    return _id;
}
Program* Process::application() const {
    return _application;
}

QString Process::configurationId() const {
    return _configurationId;
}

Cluster* Process::cluster() const {
    return _cluster;
}

bool Process::allNodesHasStatus(Process::NodeStatus::Status status) {
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

bool Process::anyNodeHasStatus(Process::NodeStatus::Status status) {
    for (auto iNode = _nodeLogs.begin(); iNode != _nodeLogs.end(); iNode++) {
        if (iNode->statuses.size() == 0) {
            continue;
        }
        if (iNode->statuses.back().status == status) {
            return true;
        }
    }
    return false;
}

void Process::pushNodeStatus(QString nodeId, Process::NodeStatus::Status nodeStatus) {
    Process::ClusterStatus::Status clusterStatus = _clusterStatus.status;
    _nodeLogs[nodeId].statuses.append({ nodeStatus, std::chrono::system_clock::now() });

    switch (nodeStatus) {
    case Process::NodeStatus::Status::Starting:
        clusterStatus = Process::ClusterStatus::Status::Starting;
        break;
    case Process::NodeStatus::Status::Running:
        if (allNodesHasStatus(Process::NodeStatus::Status::Running)) {
            clusterStatus = Process::ClusterStatus::Status::Running;
        }
        break;
    case Process::NodeStatus::Status::NormalExit:
        if (allNodesHasStatus(Process::NodeStatus::Status::NormalExit)) {
            clusterStatus = Process::ClusterStatus::Status::Exit;
        }
        else if (!anyNodeHasStatus(Process::NodeStatus::Status::CrashExit)) {
            clusterStatus = Process::ClusterStatus::Status::PartialExit;
        }
        break;
    case Process::NodeStatus::Status::CrashExit:
        clusterStatus = Process::ClusterStatus::Status::CrashExit;
        break;
    case Process::NodeStatus::Status::FailedToStart:
        clusterStatus = Process::ClusterStatus::Status::FailedToStart;
        break;
    }

    if (_clusterStatus.status != clusterStatus) {
        _clusterStatus = { clusterStatus, std::chrono::system_clock::now() };
    }
}

Process::NodeStatus Process::latestNodeStatus(QString nodeId) const {
    auto iNodeLog = _nodeLogs.find(nodeId);
    if (iNodeLog == _nodeLogs.end()) {
        return { Process::NodeStatus::Status::Unknown, std::chrono::system_clock::now() };
    }
    return iNodeLog->statuses.back();
}

void Process::pushNodeError(QString nodeId, Process::NodeError::Error nodeError) {
    _nodeLogs[nodeId].errors.append({ nodeError, std::chrono::system_clock::now() });
}

common::GuiInitialization::Process Process::toGuiInitializationProcess() const {
    return common::GuiInitialization::Process();
}

common::GuiProcessStatus Process::toGuiProcessStatus(const QString& nodeId) const {
    common::GuiProcessStatus g;
    g.processId = _id;
    Process::NodeStatus nodeStatus = latestNodeStatus(nodeId);
    switch (nodeStatus.status) {
    case Process::NodeStatus::Status::Starting:
        g.status = "Starting";
        break;
    case Process::NodeStatus::Status::Running:
        g.status = "Running";
        break;
    case Process::NodeStatus::Status::FailedToStart:
        g.status = "FailedToStart";
        break;
    }
    return g;
}

common::TrayCommand Process::startProcessCommand() const {
    common::TrayCommand t;
    t.id = _id;
    t.executable = _application->executable();
    t.baseDirectory = _application->baseDirectory();
    t.currentWorkingDirectory = _application->currentWorkingDirectory();
    t.command = "Start";

    t.commandlineParameters = "";

    if (!_application->commandlineParameters().isEmpty()) {
        t.commandlineParameters = _application->commandlineParameters();
    }

    auto iConfiguration = std::find_if(
        _application->configurations().cbegin(),
        _application->configurations().cend(),
        [this](const Program::Configuration& config) {
            return config.id == _configurationId;
        });
    
    if (iConfiguration != _application->configurations().cend()) {
        t.commandlineParameters = t.commandlineParameters + " " + iConfiguration->commandlineParameters;
    }

    return t;
}

common::TrayCommand Process::exitProcessCommand() const {
    common::TrayCommand t;
    t.id = _id;
    t.command = "Exit";
    return t;
}
