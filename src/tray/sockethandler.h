/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2023                                                               *
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

#ifndef __TRAY__SOCKETHANDLER_H__
#define __TRAY__SOCKETHANDLER_H__

#include "jsonsocket.h"
#include <QFloat16>
#include <QObject>
#include <QTcpServer>
#include <nlohmann/json.hpp>
#include <array>

class SocketHandler : public QObject {
Q_OBJECT
public:
    struct MessageLog {
        std::string time;
        nlohmann::json message;
        std::string peer;
    };

    SocketHandler(int port, std::string secret);
    ~SocketHandler();

    std::array<MessageLog, 3> lastMessages() const;

public slots:
    void sendMessage(const nlohmann::json& message, bool printMessage = true);

signals:
    void newConnection(const std::string& peerAddress);
    void closedConnection(const std::string& peerAddress);
    void messageReceived(const nlohmann::json& message, const std::string& peerAddress);

private:
    void newConnectionEstablished();
    void disconnected(common::JsonSocket* socket);
    void handleMessage(nlohmann::json message, common::JsonSocket* socket);

    QTcpServer _server;
    std::vector<common::JsonSocket*> _sockets;
    std::string _secret;

    std::array<MessageLog, 3> _lastMessages;
};

#endif // __TRAY__SOCKETHANDLER_H__
