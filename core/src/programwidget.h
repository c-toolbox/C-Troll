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

#include <QGroupBox>
#include <QPushButton>
#include <QWidget>

#include "process.h"
#include "program.h"
#include <map>

struct Cluster;
class QMenu;

namespace programs {

class ProgramButton : public QPushButton {
Q_OBJECT
public:
    ProgramButton(const Cluster* cluster, const Program::Configuration* configuration);

    void updateStatus();
    void processUpdated(Process::ID processId);

signals:
    void startProgram(Program::Configuration::ID configurationId);
    void stopProgram(Program::Configuration::ID configurationId);

    void restartProcess(Process::ID processId);
    void stopProcess(Process::ID processId);

private:
    void handleButtonPress();

    void updateButton();
    void updateMenu();

    bool isProcessRunning(Node::ID nodeId) const;
    bool hasNoProcessRunning() const;
    bool hasAllProcessesRunning() const;

    const Cluster* _cluster;
    const Program::Configuration* _configuration;

    struct ProcessInfo {
        Process::ID processId;
        QAction* menuAction;
    };

    QMenu* _actionMenu;
    std::map<Node::ID, ProcessInfo> _processes;
};


//////////////////////////////////////////////////////////////////////////////////////////


class ClusterWidget : public QGroupBox {
Q_OBJECT
public:
    ClusterWidget(Cluster* cluster,
        const std::vector<Program::Configuration>& configurations);

    void updateStatus();
    void processUpdated(Process::ID processId);

signals:
    void startProgram(Program::Configuration::ID configurationId);
    void stopProgram(Program::Configuration::ID configurationId);

    void restartProcess(Process::ID processId);
    void stopProcess(Process::ID processId);

private:
    std::map<Program::Configuration::ID, ProgramButton*> _startButtons;
};


/////////////////////////////////////////////////////////////////////////////////////////

class TagInfoWidget : public QWidget {
Q_OBJECT
public:
    TagInfoWidget(const std::vector<std::string>& tags);
};


 /////////////////////////////////////////////////////////////////////////////////////////


class ProgramWidget : public QGroupBox {
Q_OBJECT
public:
    ProgramWidget(const Program& program);

    void updateStatus(Cluster::ID clusterId);
    void processUpdated(Process::ID processId);

signals:
    void startProgram(Cluster::ID clusterId, Program::Configuration::ID configurationId);
    void stopProgram(Cluster::ID clusterId, Program::Configuration::ID configurationId);

    void restartProcess(Process::ID processId);
    void stopProcess(Process::ID processId);

private:
    std::map<Cluster::ID, ClusterWidget*> _widgets;
};


//////////////////////////////////////////////////////////////////////////////////////////


class TagsWidget : public QGroupBox {
Q_OBJECT
public:
    TagsWidget();

private slots:
    void buttonPressed();

signals:
    void pickedTags(std::vector<std::string> tags);

private:
    std::map<QPushButton*, std::string> _buttons;
};


//////////////////////////////////////////////////////////////////////////////////////////

class ProgramsWidget : public QWidget {
Q_OBJECT
public:
    ProgramsWidget();

    void processUpdated(Process::ID processId);

public slots:
    void connectedStatusChanged(Cluster::ID cluster, Node::ID node);

private slots:
    void tagsPicked(std::vector<std::string> tags);
    void searchUpdated(std::string text);

signals:
    void startProgram(Cluster::ID clusterId, Program::ID programId,
        Program::Configuration::ID configurationId);
    void stopProgram(Cluster::ID clusterId, Program::ID programId,
        Program::Configuration::ID configurationId);

    void restartProcess(Process::ID processId);
    void stopProcess(Process::ID processId);

private:
    QWidget* createControls();
    QWidget* createPrograms();

    void updatedVisibilityState();

    std::map<Program::ID, ProgramWidget*> _widgets;

    struct VisibilityInfo {
        bool byTag = true;
        bool bySearch = true;
    };
    std::map<Program::ID, VisibilityInfo> _visibilities;
};

} // namespace programs

#endif // __CORE__PROGRAMWIDGET_H__
