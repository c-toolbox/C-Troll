/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
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

#include <iostream>
#include <QTcpSocket.h>
#include "sockethandler.h"

#include <QDebug>

SocketHandler::SocketHandler() {}

SocketHandler::~SocketHandler() {}

void SocketHandler::initialize() {
    const int port = 5000;
    
    qDebug() << "Listening on port:" << port;
    
    _server.listen(QHostAddress::Any, port);
    QObject::connect(&_server, &QTcpServer::newConnection, this, &SocketHandler::newConnection);
}

void SocketHandler::readyRead(QTcpSocket* socket) {
    QByteArray byteArray = socket->readAll();
    QString message = QString::fromUtf8(byteArray);
    //qDebug() << "Message received: " << message;
    emit messageRecieved(message);
}

void SocketHandler::sendMessage(QString message) {
    qDebug() << "Sending message: " << message;
    for (QTcpSocket* socket : _sockets) {
        qDebug() << socket->localAddress() << " -> " << socket->peerAddress();
        socket->write(message.toUtf8());
    }
}

void SocketHandler::disconnected(QTcpSocket* socket) {
    auto ptr = std::find(_sockets.begin(), _sockets.end(), socket);
    if (ptr != _sockets.end()) {
        _sockets.erase(ptr);
        qDebug() << "Socket disconnected";
    }
}

void SocketHandler::newConnection() {
    while (_server.hasPendingConnections()) {
        QTcpSocket* socket = _server.nextPendingConnection();
        QObject::connect(socket, &QTcpSocket::readyRead, [=]() {
            readyRead(socket);
        });
        QObject::connect(socket, &QTcpSocket::disconnected, [=]() {
            disconnected(socket);
        });
        _sockets.push_back(socket);
        qDebug() << "Socket connected";
    }
}
