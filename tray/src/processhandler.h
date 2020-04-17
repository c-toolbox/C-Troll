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

#ifndef __TRAY__PROCESSHANDLER_H__
#define __TRAY__PROCESSHANDLER_H__

#include <QObject>

#include "exitcommandmessage.h"
#include "startcommandmessage.h"
#include <QProcess>
#include <json/json.hpp>
#include <map>
#include <string>

class ProcessHandler : public QObject {
Q_OBJECT
public:
    struct ProcessInfo {
        int processId;
        QProcess* process;

        std::string executable;

        // Additional information that was sent to us from the C-Troll application so that
        // we can mirror it back if a new instance connects. We don't do anything with
        // this information directly
        int programId;
        int configurationId;
        int clusterId;
        int nodeId;
        std::size_t dataHash;
    };

public slots:
    void newConnection();
    void handleSocketMessage(const nlohmann::json& message);

    void handlerErrorOccurred(QProcess::ProcessError error);
    void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleReadyReadStandardError();
    void handleReadyReadStandardOutput();
    void handleStarted();

signals:
    void sendSocketMessage(const nlohmann::json& message);

    void startedProcess(ProcessInfo process);
    void closedProcess(ProcessInfo process);

private:
    void executeProcessWithCommandMessage(QProcess* process,
        const common::StartCommandMessage& command);

    void createAndRunProcessFromCommandMessage(
        const common::StartCommandMessage& command);
    
    std::vector<ProcessInfo>::const_iterator processIt(QProcess* process);
    std::vector<ProcessInfo>::const_iterator processIt(int id);

    // The key of this map is a unique id (received from core)
    // The value is the process which is running
    std::vector<ProcessInfo> _processes;

    std::size_t _controllerDataHash = 0;
};

#endif // __TRAY__PROCESSHANDLER_H__
