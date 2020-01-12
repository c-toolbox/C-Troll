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

#include "coreprocess.h"

#include "cluster.h"
#include "program.h"
#include "logging.h"
#include <assert.h>

namespace {

bool hasStatus(const CoreProcess::NodeLog& l, CoreProcess::NodeStatus::Status status) {
    return !l.statuses.empty() && l.statuses.back().status == status;
}

bool allNodesHaveStatus(const CoreProcess& proc, CoreProcess::NodeStatus::Status status) {
    return !proc.nodeLogs.empty() &&
        std::all_of(
            proc.nodeLogs.begin(),
            proc.nodeLogs.end(),
            [status](const std::pair<std::string, CoreProcess::NodeLog>& p) {
                return hasStatus(p.second, status);
            }
        );
}

bool anyNodeHasStatus(const CoreProcess& proc, CoreProcess::NodeStatus::Status status) {
    return std::any_of(
        proc.nodeLogs.begin(),
        proc.nodeLogs.end(),
        [status](const std::pair<std::string, CoreProcess::NodeLog>& p) {
            return hasStatus(p.second, status);
        }
    );
}

std::string nodeStatusToGuiNodeStatus(CoreProcess::NodeStatus::Status status) {
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

std::string clusterStatusToGuiClusterStatus(CoreProcess::ClusterStatus::Status status) {
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

double timeToGuiTime(std::chrono::system_clock::time_point time) {
    using namespace std::chrono;
    return duration_cast<milliseconds>(time.time_since_epoch()).count();
}


} // namespace

common::TrayCommand startProcessCommand(const CoreProcess& proc) {
    common::TrayCommand t;
    t.id = proc.id;
    t.executable = proc.application.executable;
    t.currentWorkingDirectory = proc.application.currentWorkingDirectory;
    t.command = common::TrayCommand::Command::Start;

    t.commandlineParameters = proc.application.commandlineParameters;

    t.commandlineParameters = t.commandlineParameters + ' ' +
        proc.configuration.clusterCommandlineParameters.at(proc.cluster.id);

    return t;
}

common::TrayCommand exitProcessCommand(const CoreProcess& proc) {
    common::TrayCommand t;
    t.id = proc.id;
    t.command = common::TrayCommand::Command::Exit;
    return t;
}

int CoreProcess::nextId = 0;

CoreProcess::CoreProcess(const Program& application,
                         const Program::Configuration& configuration,
                         const Cluster& cluster)
    : id(nextId++)
    , application(application)
    , configuration(configuration)
    , cluster(cluster)
{}


void CoreProcess::pushNodeStatus(std::string nodeId, NodeStatus::Status nodeStatus) {
    ClusterStatus::Status status = clusterStatus.status;
    nodeLogs[nodeId].statuses.push_back(
        { nodeStatus, std::chrono::system_clock::now(), nextNodeStatusId++ }
    );

    switch (nodeStatus) {
        case NodeStatus::Status::Starting:
            status = ClusterStatus::Status::Starting;
            break;
        case NodeStatus::Status::Running:
            if (allNodesHaveStatus(*this, NodeStatus::Status::Running)) {
                status = ClusterStatus::Status::Running;
            }
            break;
        case NodeStatus::Status::NormalExit:
            if (allNodesHaveStatus(*this, NodeStatus::Status::NormalExit)) {
                status = ClusterStatus::Status::Exit;
            }
            else if (!anyNodeHasStatus(*this, NodeStatus::Status::CrashExit)) {
                status = ClusterStatus::Status::PartialExit;
            }
            break;
        case NodeStatus::Status::CrashExit:
            status = ClusterStatus::Status::CrashExit;
            break;
        case NodeStatus::Status::FailedToStart:
            status = ClusterStatus::Status::FailedToStart;
            break;
    }

    if (clusterStatus.status != status) {
        clusterStatus = { status, std::chrono::system_clock::now() };
    }
}

void CoreProcess::pushNodeError(std::string nodeId, NodeError::Error nodeError) {
    nodeLogs[nodeId].errors.push_back(
        { nodeError, std::chrono::system_clock::now(), nextNodeErrorId++ }
    );
}

void CoreProcess::pushNodeMessage(const std::string& nodeId,
                                  CoreProcess::NodeLogMessage::OutputType type,
                                  std::string message)
{
    nodeLogs[nodeId].messages.push_back(
        { std::move(message), std::chrono::system_clock::now(), type, nextLogMessageId++ }
    );
}
