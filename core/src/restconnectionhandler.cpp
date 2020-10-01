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

#include "restconnectionhandler.h"

#include "logging.h"
#include <QTcpSocket>
#include <fmt/format.h>

RestConnectionHandler::RestConnectionHandler(QObject* parent, int port,
                                             std::string secret)
    : QObject(parent)
    , _secret(std::move(secret))
{
    Log("Status", fmt::format("REST API listening on port: {}", port));

    const bool success = _server.listen(QHostAddress::Any, static_cast<quint16>(port));
    if (!success) {
        Log("Error", fmt::format("Listening to REST API on port {} failed", port));
        return;
    }

    connect(
        &_server, &QTcpServer::newConnection,
        this, &RestConnectionHandler::newConnectionEstablished
    );
}

void RestConnectionHandler::newConnectionEstablished() {
    while (_server.hasPendingConnections()) {
        QTcpSocket* socket = _server.nextPendingConnection();

        connect(
            socket, &QTcpSocket::disconnected,
            socket, &QTcpSocket::deleteLater
        );

        connect(
            socket, &QTcpSocket::readyRead,
            this, &RestConnectionHandler::handleNewConnection
        );

        _sockets.push_back(socket);
    }
}

void RestConnectionHandler::handleNewConnection() {
    QTcpSocket* socket = dynamic_cast<QTcpSocket*>(QObject::sender());
    assert(socket);
    assert(std::find(_sockets.begin(), _sockets.end(), socket) != _sockets.end());

    //QString a = socket->readAll();
    QStringList tokens = QString(socket->readAll()).split(QRegExp("[ \r\n][ \r\n]*"));
    qDebug() << tokens;


    //char* d = a.data();
    //d = d;

    socket->write("blabla\n");
    socket->close();
}
