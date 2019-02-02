/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
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

#include "cluster.h"
#include "guiinitialization.h"
#include "guiprocesslogmessage.h"
#include "guiprocesslogmessagehistory.h"
#include "program.h"
#include "traycommand.h"
#include "handler/incomingsockethandler.h"
#include "handler/outgoingsockethandler.h"
#include <chrono>

namespace common {
    struct GuiCommand;
    struct GuiProcessStatus;
    struct TrayProcessStatus;
    struct TrayProcessLogMessage;
    class JsonSocket;
} // namespace common

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
            TimedOut,
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
        QString message;
        std::chrono::system_clock::time_point time;
        OutputType outputType;
        int id;
    };

    struct NodeLog {
        QVector<NodeStatus> statuses;
        QVector<NodeError> errors;
        QVector<NodeLogMessage> messages;
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

    CoreProcess(Program* program, const QString& configurationId, Cluster* cluster);
    int id() const;
    Program* application() const;
    QString configurationId() const;
    Cluster* cluster() const;

    common::GuiInitialization::Process toGuiInitializationProcess() const;
    common::GuiProcessStatus toGuiProcessStatus(const QString& nodeId) const;
    common::GuiProcessLogMessage latestGuiProcessLogMessage(const QString& nodeId) const;
    common::GuiProcessLogMessageHistory guiProcessLogMessageHistory() const;
    common::TrayCommand startProcessCommand() const;
    common::TrayCommand exitProcessCommand() const;

    void pushNodeStatus(QString nodeId, NodeStatus::Status status);
    void pushNodeError(QString nodeId, NodeError::Error error);
    void pushNodeStdOut(QString nodeId, QString message);
    void pushNodeStdError(QString nodeId, QString message);

    NodeStatus latestNodeStatus(QString nodeId) const;
    NodeLogMessage latestLogMessage(QString nodeId) const;

private:
    bool allNodesHasStatus(NodeStatus::Status status);
    bool anyNodeHasStatus(NodeStatus::Status status);

    static QString nodeStatusToGuiNodeStatus(NodeStatus::Status status);
    static QString clusterStatusToGuiClusterStatus(ClusterStatus::Status status);
    static double timeToGuiTime(std::chrono::system_clock::time_point time);

    int _id;
    Program* _application;
    QString _configurationId;
    Cluster* _cluster;
    static int _nextId;
    QMap<QString, CoreProcess::NodeLog> _nodeLogs;
    CoreProcess::ClusterStatus _clusterStatus;

    int _nextLogMessageId = 0;
    int _nextNodeErrorId = 0;
    int _nextNodeStatusId = 0;
};

#endif // __COREPROCESS_H__
