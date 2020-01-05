/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
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

#ifndef __COREPROCESS_H__
#define __COREPROCESS_H__

#include "guiinitialization.h"
#include "guiprocesslogmessage.h"
#include "guiprocesslogmessagehistory.h"
#include "guiprocessstatus.h"
#include "traycommand.h"
#include <chrono>

class Cluster;
class Program;

class CoreProcess {
public:
    struct NodeStatus {
        enum class Status {
            Starting = 0,
            FailedToStart,
            Running,
            NormalExit,
            CrashExit,
            Unknown
        } status;
        std::chrono::system_clock::time_point time;
        int id;
    };

    struct NodeError {
        enum class Error {
            TimedOut = 0,
            WriteError,
            ReadError,
            UnknownError
        } error;
        std::chrono::system_clock::time_point time;
        int id;
    };

    struct NodeLogMessage {
        enum class OutputType {
            StdOut,
            StdError
        };
        std::string message;
        std::chrono::system_clock::time_point time;
        OutputType outputType;
        int id;
    };

    struct NodeLog {
        std::vector<NodeStatus> statuses;
        std::vector<NodeError> errors;
        std::vector<NodeLogMessage> messages;
    };

    struct ClusterStatus {
        enum class Status {
            Starting = 0,
            FailedToStart,
            Running,
            Exit,
            PartialExit,
            CrashExit,
            Unknown
        } status;
        std::chrono::system_clock::time_point time;
    };

    CoreProcess(Program& program, const std::string& configurationId, Cluster& cluster);

    void pushNodeStatus(std::string nodeId, NodeStatus::Status status);
    void pushNodeError(std::string nodeId, NodeError::Error error);
    void pushNodeMessage(const std::string& nodeId, NodeLogMessage::OutputType type,
        std::string message);

    int id;
    Program& application;
    std::string configurationId;
    Cluster& cluster;
    std::map<std::string, NodeLog> nodeLogs;
    ClusterStatus clusterStatus = { CoreProcess::ClusterStatus::Status::Starting };

private:
    static int nextId;

    int nextLogMessageId = 0;
    int nextNodeErrorId = 0;
    int nextNodeStatusId = 0;
};

common::GuiInitialization::Process coreProcessToGuiProcess(const CoreProcess& process);
common::GuiProcessStatus coreProcessToProcessStatus(const CoreProcess& process,
    const std::string& nodeId);
common::GuiProcessLogMessage latestGuiProcessLogMessage(const CoreProcess& process,
    const std::string& nodeId);
common::GuiProcessLogMessageHistory logMessageHistory(const CoreProcess& proc);

common::TrayCommand startProcessCommand(const CoreProcess& proc);
common::TrayCommand exitProcessCommand(const CoreProcess& proc);

#endif // __COREPROCESS_H__
