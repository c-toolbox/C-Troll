/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
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

#include "sockethandler.h"

#include "invalidauthmessage.h"
#include "jsonsocket.h"
#include "logging.h"
#include "message.h"
#include <QTcpSocket>
#include <fmt/format.h>
#include <iostream>
#include <memory>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

namespace {
    std::string currentTime() {
#ifdef WIN32
        SYSTEMTIME t = {};
        GetLocalTime(&t);

        return fmt::format(
            "{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0<3}",
            t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds
        );
#else
        struct timeval t;
        gettimeofday(&t, nullptr);
        tm* m = gmtime(&t.tv_sec);

        return fmt::format(
            "{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0<3}",
            m->tm_year, m->tm_mon, m->tm_mday,
            m->tm_hour, m->tm_min, m->tm_sec, t.tv_usec / 1000
        );
#endif
    }
} // namespace

SocketHandler::SocketHandler(int port, std::string secret)
    : _secret(std::move(secret))
{
    Log("Status", fmt::format("Listening on port: {}", port));
    
    const bool success = _server.listen(QHostAddress::Any, static_cast<quint16>(port));
    if (!success) {
        Log("Error", fmt::format("Creating socket to listen on port {} failed", port));
        return;
    }
    connect(
        &_server, &QTcpServer::newConnection,
        this, &SocketHandler::newConnectionEstablished
    );
}

std::array<SocketHandler::MessageLog, 3> SocketHandler::lastMessages() const {
    return _lastMessages;
}

void SocketHandler::handleMessage(nlohmann::json message, common::JsonSocket* socket) {
#ifdef QT_DEBUG
    Log(fmt::format("(Debug) Received [{}]", socket->peerAddress()), message.dump());
#endif // QT_DEBUG

    common::Message msg = message;
    if (msg.secret == _secret) {
        emit messageReceived(std::move(message), socket->peerAddress());
    }
    else {
        Log(fmt::format("Received [{}]", socket->peerAddress()), "Invalid message");
        common::InvalidAuthMessage invalidAuthMsg;
        nlohmann::json j = invalidAuthMsg;
        socket->write(j);
    }
}

void SocketHandler::sendMessage(const nlohmann::json& message, bool printMessage) {
    for (common::JsonSocket* jsonSocket : _sockets) {
        std::string peer = jsonSocket->peerAddress();
        if (printMessage) {
            Log(fmt::format("Sending [{}]", peer), message.dump());
        }
        jsonSocket->write(message);
    }
}

void SocketHandler::disconnected(common::JsonSocket* socket) {
    auto ptr = std::find(_sockets.begin(), _sockets.end(), socket);
    if (ptr != _sockets.end()) {
        (*ptr)->deleteLater();
        _sockets.erase(ptr);
        Log("Status", fmt::format("Socket from {} disconnected", socket->peerAddress()));

        emit closedConnection(socket->peerAddress());
    }
}

void SocketHandler::newConnectionEstablished() {
    while (_server.hasPendingConnections()) {
        common::JsonSocket* socket = new common::JsonSocket(
            std::unique_ptr<QTcpSocket>(_server.nextPendingConnection()),
            _secret
        );

        QObject::connect(
            socket, &common::JsonSocket::disconnected,
            [this, socket]() { disconnected(socket); }
        );

        QObject::connect(
            socket, &common::JsonSocket::messageReceived,
            [this, socket](nlohmann::json message) {
                try {
                    // We store a copy of the last n messages to be able to print those in
                    // case of a catastrophic error
                    std::rotate(
                        _lastMessages.rbegin(),
                        _lastMessages.rbegin() + 1,
                        _lastMessages.rend()
                    );

                    MessageLog ml;
                    ml.time = currentTime();
                    ml.message = message;
                    ml.peer = socket->peerAddress();
                    _lastMessages.front() = ml;

                    handleMessage(message, socket);
                }
                catch (const std::exception& e) {
                    Log("Message Decode", e.what());
                }
            }
        );

        _sockets.push_back(socket);
        Log("Status", fmt::format("Socket connected from {}", socket->peerAddress()));

        emit newConnection(socket->peerAddress());
    }
}
