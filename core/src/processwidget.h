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

#ifndef __CORE__PROCESSWIDGET_H__
#define __CORE__PROCESSWIDGET_H__

#include <QWidget>

#include "process.h"
#include "processoutputmessage.h"
#include <chrono>
#include <map>

class QBoxLayout;
class QLabel;
class QPlainTextEdit;
class QPushButton;
class QScrollArea;
class QTimer;

class ProcessWidget : public QWidget {
Q_OBJECT
public:
    ProcessWidget(Process::ID processId, const std::chrono::milliseconds& timeout);
    ~ProcessWidget();

    void updateStatus();
    void addMessage(common::ProcessOutputMessage message);

signals:
    void remove(Process::ID processId);
    void kill(Process::ID processId);

private:
    QWidget* createMessageContainer();

    const Process::ID _processId;
    const std::chrono::milliseconds& _timeout;

    QLabel* _status = nullptr;
    QWidget* _messageContainer = nullptr;
    QPlainTextEdit* _messages = nullptr;
    QPlainTextEdit* _errorMessages = nullptr;
    QPushButton* _remove = nullptr;
    QTimer* _removalTimer = nullptr;
};


//////////////////////////////////////////////////////////////////////////////////////////


class ProcessesWidget : public QWidget {
Q_OBJECT
public:
    ProcessesWidget(const std::chrono::milliseconds& processTimeout);

    void processAdded(Process::ID processId);
    void processUpdated(Process::ID processId);
    void processRemoved(Process::ID processId);

public slots:
    void receivedProcessMessage(Node::ID node, common::ProcessOutputMessage message);

signals:
    void killProcess(Process::ID processId);
    void killAllProcesses();

private:
    QBoxLayout* _contentLayout = nullptr;

    const std::chrono::milliseconds& _processTimeout;
    std::map<Process::ID, ProcessWidget*> _widgets;
};

#endif // __CORE__PROCESSWIDGET_H__
