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

#ifndef __CORE__PROGRAMWIDGET_H__
#define __CORE__PROGRAMWIDGET_H__

#include <QPushButton>
#include <QWidget>

#include <QMenu>
#include "cluster.h"
#include "node.h"
#include "process.h"
#include "program.h"
#include <vector>

namespace programs {

class ProgramButton : public QPushButton {
Q_OBJECT
public:
    ProgramButton(const Cluster* cluster, const Program::Configuration* configuration);

    void updateStatus();
    void processUpdated(Process::ID processId);

signals:
    void startProgram(int configurationId);
    void stopProgram(int configurationId);

    void restartProcess(Process::ID processId);
    void stopProcess(Process::ID processId);

private:
    void handleButtonPress();

    void updateButton();
    void updateMenu();

    bool isProcessRunning(int nodeId) const;
    bool hasNoProcessRunning() const;
    bool hasAllProcessesRunning() const;

    const Cluster* _cluster;
    const Program::Configuration* _configuration;

    struct ProcessInfo {
        Process::ID processId;
        QAction* menuAction;
    };

    QMenu* _actionMenu;
    std::map<int, ProcessInfo> _processes;
};


//////////////////////////////////////////////////////////////////////////////////////////


class ClusterWidget : public QWidget {
Q_OBJECT
public:
    ClusterWidget(Cluster* cluster,
        const std::vector<Program::Configuration>& configurations);

    void updateStatus();
    void processUpdated(Process::ID processId);

signals:
    void startProgram(int configurationId);
    void stopProgram(int configurationId);

    void restartProcess(Process::ID processId);
    void stopProcess(Process::ID processId);

private:
    std::map<int, ProgramButton*> _startButtons;
};


 /////////////////////////////////////////////////////////////////////////////////////////


class ProgramWidget : public QWidget {
Q_OBJECT
public:
    ProgramWidget(const Program& program);

    void updateStatus(int clusterId);
    void processUpdated(Process::ID processId);

signals:
    void startProgram(int clusterId, int configurationId);
    void stopProgram(int clusterId, int configurationId);

    void restartProcess(Process::ID processId);
    void stopProcess(Process::ID processId);

private:
    std::map<int, ClusterWidget*> _widgets;
};


//////////////////////////////////////////////////////////////////////////////////////////


class ProgramsWidget : public QWidget {
Q_OBJECT
public:
    ProgramsWidget();

    void processUpdated(Process::ID processId);

public slots:
    void connectedStatusChanged(int cluster, int node);

signals:
    void startProgram(int clusterId, int programId, int configurationId);
    void stopProgram(int clusterId, int programId, int configurationId);

    void restartProcess(Process::ID processId);
    void stopProcess(Process::ID processId);

private:
    std::map<int, ProgramWidget*> _widgets;
};

} // namespace programs

#endif // __CORE__PROGRAMWIDGET_H__
