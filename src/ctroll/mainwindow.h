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

#ifndef __CTROLL__MAINWINDOW_H__
#define __CTROLL__MAINWINDOW_H__

#include <QMainWindow>

#include "clusterconnectionhandler.h"
#include "configuration.h"
#include "logwidget.h"
#include "process.h"
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QTextEdit>
#include <memory>
#include <mutex>
#include <string>

class ClustersWidget;
class ProcessesWidget;
class RestConnectionHandler;

namespace programs { class ProgramsWidget; }

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    MainWindow(bool shouldLogDebug);

private slots:
    void handleTrayProcess(common::ProcessStatusMessage status);
    void handleTrayStatus(Node::ID, common::TrayStatusMessage status);
    void handleInvalidAuth(Node::ID id, common::InvalidAuthMessage message);
    void handleErrorMessage(Node::ID id, common::ErrorOccurredMessage message);

    void stopProcess(Process::ID processId) const;

    void iconActivated(QSystemTrayIcon::ActivationReason reason);

protected:
    void closeEvent(QCloseEvent* event);
    void changeEvent(QEvent* event);

private:
    void startProgram(Cluster::ID clusterId, Program::ID programId,
        Program::Configuration::ID configurationId);
    void startCustomProgram(Node::ID nodeId, std::string executable,
        std::string workingDir, std::string arguments);
    void stopProgram(Cluster::ID clusterId, Program::ID programId,
        Program::Configuration::ID configurationId) const;
    void startProcess(Process::ID processId) const;
    void killAllProcesses(Cluster::ID id) const;
    void killAllProcesses(Node::ID id) const;
    void killTray(Node::ID id) const;
    void killTrays(Cluster::ID id) const;

    void log(std::string msg);

    programs::ProgramsWidget* _programWidget = nullptr;
    ClustersWidget* _clustersWidget = nullptr;
    ProcessesWidget* _processesWidget = nullptr;
    LogWidget _logWidget;

    ClusterConnectionHandler _clusterConnectionHandler;
    RestConnectionHandler* _restLoopbackHandler = nullptr;
    RestConnectionHandler* _restGeneralHandler = nullptr;
    Configuration _config;

    QTextEdit _messageBox;

    bool _shouldShowDataHashMessage = false;
};

#endif // __CTROLL__MAINWINDOW_H__
