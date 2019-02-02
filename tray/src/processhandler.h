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

#ifndef __PROCESSHANDLER_H__
#define __PROCESSHANDLER_H__

#include <QObject>

#include <QJsonDocument>
#include <QProcess>
#include <traycommand.h>
#include <map>
#include <string>

class ProcessHandler : public QObject {
Q_OBJECT
public:
    ProcessHandler();
    ~ProcessHandler();

public slots:
    void handleSocketMessage(const QJsonDocument& message);
    // Process slots
    void handlerErrorOccurred(QProcess::ProcessError error);
    void handleFinished(int exitCode);
    void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleReadyReadStandardError();
    void handleReadyReadStandardOutput();
    void handleStarted();

signals:
    void sendSocketMessage(const QJsonDocument& message);

private:
    void executeProcessWithTrayCommand(QProcess* process,
        const common::TrayCommand& command);
    void createAndRunProcessFromTrayCommand(const common::TrayCommand& command);
    
    // The key of this map is a unique id (recieved from core)
    // The value is the process which is running
    using ProcessMap = std::map<int, QProcess*>;
    ProcessMap _processes;
};

#endif
