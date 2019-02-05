/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2019                                                             *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
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

#include "incomingsockethandler.h"

#include <jsonsocket.h>
#include <QTcpSocket>
#include <assert.h>
#include <memory>

void IncomingSocketHandler::initialize(quint16 port) {
    _server.listen(QHostAddress::Any, port);

    QObject::connect(&_server, &QTcpServer::newConnection, [this]() { newConnection(); });
}

void IncomingSocketHandler::deinitialize() {
    disconnect();
    _sockets.clear();
}

void IncomingSocketHandler::newConnection() {
    while (_server.hasPendingConnections()) {
        QTcpSocket* socket = _server.nextPendingConnection();
        common::JsonSocket* jsonSocket = new common::JsonSocket(
            std::unique_ptr<QTcpSocket>(socket)
        );

        _sockets.push_back(jsonSocket);

        QObject::connect(
            jsonSocket, &common::JsonSocket::disconnected,
            [this, jsonSocket]() { disconnectedConnection(jsonSocket); }
        );

        QObject::connect(
            jsonSocket, &common::JsonSocket::readyRead,
            [this, jsonSocket]() { readyRead(jsonSocket); }
        );

        emit newConnectionEstablished(jsonSocket);
    }
}

void IncomingSocketHandler::readyRead(common::JsonSocket* jsonSocket) {
    assert(jsonSocket);
    nlohmann::json message = jsonSocket->read();
    emit messageReceived(message);
}

void IncomingSocketHandler::disconnectedConnection(common::JsonSocket* socket) {
    socket->disconnect();
    assert(socket);
    auto it = std::find(_sockets.begin(), _sockets.end(), socket);
    assert(it != _sockets.end());
    (*it)->deleteLater();
    _sockets.erase(it);
}

void IncomingSocketHandler::sendMessage(common::JsonSocket* socket, nlohmann::json message) {
    assert(socket);
    assert(std::find(_sockets.begin(), _sockets.end(), socket) != _sockets.end());
    assert(!message.is_null());

    socket->write(message);
}

void IncomingSocketHandler::sendMessageToAll(nlohmann::json message) {
    assert(!message.is_null());
    for (common::JsonSocket* socket : _sockets) {
        socket->write(message);
    }
}
