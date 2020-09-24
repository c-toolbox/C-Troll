/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sund�n, Emil Axelsson                                            *
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

#ifndef __CORE__MAINWINDOW_H__
#define __CORE__MAINWINDOW_H__

#include <QMainWindow>

#include "clusterconnectionhandler.h"
#include "clusterwidget.h"
#include "process.h"
#include "processwidget.h"
#include "programwidget.h"
#include <QTextEdit>
#include <memory>

class ClustersWidget;
class ProcessesWidget;

namespace programs { class ProgramsWidget; }

class MainWindow : public QMainWindow {
Q_OBJECT
public:
    explicit MainWindow(const std::string& configurationFile);

private:
    void startProgram(Cluster::ID clusterId, Program::ID programId,
        Program::Configuration::ID configurationId);
    void stopProgram(Cluster::ID clusterId, Program::ID programId,
        Program::Configuration::ID configurationId);
    void startProcess(Process::ID processId);
    void stopProcess(Process::ID processId);
    void killAllProcesses(Cluster::ID id);


    void log(std::string msg);

    std::unique_ptr<programs::ProgramsWidget> _programWidget;
    std::unique_ptr<ClustersWidget> _clustersWidget;
    std::unique_ptr<ProcessesWidget> _processesWidget;

    ClusterConnectionHandler _clusterConnectionHandler;

    QTextEdit _messageBox;
};

#endif // __CORE__MAINWINDOW_H__
