/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2019                                                             *
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

#include "sockethandler.h"

#include "jsonsocket.h"
#include <QTcpSocket.h>
#include <iostream>
#include <memory>
#include <logging.h>

SocketHandler::SocketHandler() {}

SocketHandler::~SocketHandler() {}

void SocketHandler::initialize() {
    const int port = 5000;
    
    Log("Listening on port: " + std::to_string(port));
    
    _server.listen(QHostAddress::Any, port);
    QObject::connect(
        &_server, &QTcpServer::newConnection,
        this, &SocketHandler::newConnection
    );
}

void SocketHandler::readyRead(common::JsonSocket* socket) {
    nlohmann::json message = socket->read();
    emit messageRecieved(std::move(message));
}

void SocketHandler::sendMessage(const nlohmann::json& message) {
    Log("Sending message:\n" + message.dump());
    for (common::JsonSocket* jsonSocket : _sockets) {
        std::string local = jsonSocket->socket()->localAddress().toString().toStdString();
        std::string peer = jsonSocket->socket()->peerAddress().toString().toStdString();
        Log(local + " -> " + peer);
        jsonSocket->write(message);
    }
}

void SocketHandler::disconnected(common::JsonSocket* socket) {
    auto ptr = std::find(_sockets.begin(), _sockets.end(), socket);
    if (ptr != _sockets.end()) {
        (*ptr)->deleteLater();
        _sockets.erase(ptr);
        Log("Socket disconnected");
    }
}

void SocketHandler::newConnection() {
    while (_server.hasPendingConnections()) {
        common::JsonSocket* jsonSocket = new common::JsonSocket(
            std::unique_ptr<QTcpSocket>(_server.nextPendingConnection()));
        
        QTcpSocket* rawSocket = jsonSocket->socket();

        QObject::connect(
            rawSocket, &QTcpSocket::disconnected,
            [this, jsonSocket]() { disconnected(jsonSocket); }
        );

        QObject::connect(
            jsonSocket, &common::JsonSocket::readyRead,
            [this, jsonSocket]() { readyRead(jsonSocket); }
        );

        _sockets.push_back(jsonSocket);
        Log("Socket connected");
    }
}
