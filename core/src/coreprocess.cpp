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

#include "coreprocess.h"
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

int CoreProcess::_nextId = 0;

CoreProcess::CoreProcess(Program* application, const QString& configurationId, Cluster* cluster)
    : _id(_nextId++)
    , _application(application)
    , _configurationId(configurationId)
    , _cluster(cluster)
    , _clusterStatus({ CoreProcess::ClusterStatus::Status::Starting })
{
}

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
        if (iNode->statuses.size() == 0) {
            continue;
        }
        if (iNode->statuses.back().status == status) {
            return true;
        }
    }
    return false;
}

void CoreProcess::pushNodeStatus(QString nodeId, CoreProcess::NodeStatus::Status nodeStatus) {
    CoreProcess::ClusterStatus::Status clusterStatus = _clusterStatus.status;
    _nodeLogs[nodeId].statuses.append({ nodeStatus, std::chrono::system_clock::now() });

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
        return { CoreProcess::NodeStatus::Status::Unknown, std::chrono::system_clock::now() };
    }
    return iNodeLog->statuses.back();
}

void CoreProcess::pushNodeError(QString nodeId, CoreProcess::NodeError::Error nodeError) {
    _nodeLogs[nodeId].errors.append({ nodeError, std::chrono::system_clock::now() });
}

common::GuiInitialization::Process CoreProcess::toGuiInitializationProcess() const {
    common::GuiInitialization::Process p;
    p.id = _id;
    p.applicationId = _application->id();
    p.clusterId = _cluster->id();
    p.configurationId = _configurationId;
    return p;
}

common::GuiProcessStatus CoreProcess::toGuiProcessStatus(const QString& nodeId) const {
    common::GuiProcessStatus g;
    g.processId = _id;
    g.applicationId = _application->id();
    g.clusterId = _cluster->id();
    CoreProcess::NodeStatus nodeStatus = latestNodeStatus(nodeId);
    switch (nodeStatus.status) {
    case CoreProcess::NodeStatus::Status::Starting:
        g.status = "Starting";
        break;
    case CoreProcess::NodeStatus::Status::Running:
        g.status = "Running";
        break;
    case CoreProcess::NodeStatus::Status::FailedToStart:
        g.status = "FailedToStart";
        break;
    }
    return g;
}

common::TrayCommand CoreProcess::startProcessCommand() const {
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

common::TrayCommand CoreProcess::exitProcessCommand() const {
    common::TrayCommand t;
    t.id = _id;
    t.command = "Exit";
    return t;
}
