/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2025                                                               *
 * Alexander Bock                                                                        *
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

#ifndef __TRAY__CENTRALWIDGET_H__
#define __TRAY__CENTRALWIDGET_H__

#include <QWidget>

#include "processhandler.h"
#include <map>
#include <string>

class QLabel;
class QTextEdit;

class CentralWidget : public QWidget {
Q_OBJECT
public:
    CentralWidget();

    void setPort(int port);
    void log(std::string msg);

    bool hasConnections() const;

public slots:
    void newConnection(const std::string& peerAddress);
    void closedConnection(const std::string& peerAddress);

    void newProcess(ProcessHandler::ProcessInfo process);
    void endedProcess(ProcessHandler::ProcessInfo process);

private:
    struct ConnectionInfo {
        QLabel* label = nullptr;
        std::string peerAddress;
        int nConnections = 0;
    };

    QWidget* createInfoWidget();
    void updateLabel(ConnectionInfo& ci);


    QTextEdit* _messageBox = nullptr;
    QLabel* _portLabel = nullptr;

    QLayout* _connectionsLayout = nullptr;
    std::map<std::string, ConnectionInfo> _connections;
    QLayout* _processesLayout = nullptr;
    std::map<int, QLabel*> _processes;
};

#endif // __TRAY__CENTRALWIDGET_H__
