/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
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

#include "outgoingsockethandler.h"

#include <jsonsocket.h>
#include <QTimer>

#include <assert.h>

// This is just here so that we can use a forward declaration in the header file
OutgoingSocketHandler::~OutgoingSocketHandler() = default;

OutgoingSocketHandler::HashValue OutgoingSocketHandler::hash(
    const Cluster& cluster, const Cluster::Node& node) const
{
    return cluster.name() + "::" + node.name;
}

void OutgoingSocketHandler::initialize(const QList<Cluster*>& clusters) {
    std::copy_if(
        clusters.cbegin(),
        clusters.cend(),
        std::back_inserter(_clusters),
        [](const Cluster* cluster) {
            return cluster->enabled();
    });

    for (Cluster* c : _clusters) {
        for (Cluster::Node& node : c->nodes()) {
            HashValue h = hash(*c, node);

            // This handler keeps the sockets to the tray applications open
            std::unique_ptr<QTcpSocket> socket = std::make_unique<QTcpSocket>();
            connect(
                socket.get(), &QAbstractSocket::stateChanged,
                [this, c, &node](QAbstractSocket::SocketState state) {
                    if (state == QAbstractSocket::SocketState::ConnectedState) {
                        qDebug() << "Socket state change: "
                            << node.ipAddress << node.port << state;
                        if (!node.connected) {
                            node.connected = true;
                            emit connectedStatusChanged(*c, node);
                        }
                    }
                    else if (state == QAbstractSocket::SocketState::ClosingState) {
                        qDebug() << "Socket state change: "
                            << node.ipAddress << node.port << state;
                        if (node.connected) {
                            node.connected = false;
                            emit connectedStatusChanged(*c, node);
                        }
                    }
                }
            );

            std::unique_ptr<common::JsonSocket> jsonSocket = std::make_unique<common::JsonSocket>(std::move(socket));
            common::JsonSocket* s = jsonSocket.get();

            connect(
                jsonSocket.get(), &common::JsonSocket::readyRead,
                [this, c, node]() { readyRead(*c, node); }
            );

            jsonSocket->socket()->connectToHost(node.ipAddress, node.port);
            _sockets[h] = std::move(jsonSocket);
        }
    }
    
    QTimer* timer = new QTimer(this);
    connect(
        timer, &QTimer::timeout,
        [this](){
            for (Cluster* c : _clusters) {
                for (Cluster::Node& node : c->nodes()) {
                    HashValue h = hash(*c, node);

                    auto it = _sockets.find(h);
                    assert(it != _sockets.end());

                    auto socket = it->second->socket();
                               
                    if (socket->state() == QAbstractSocket::SocketState::UnconnectedState) {
                        qDebug() << "Unconnected: " << node.ipAddress << node.port;
                        if (node.connected) {
                            node.connected = false;
                            emit connectedStatusChanged(*c, node);
                        }
                        socket->connectToHost(node.ipAddress, node.port);
                    }
                }
            }
            
        }
    );
    timer->start(2500);
}

void OutgoingSocketHandler::deinitialize() {
	_sockets.clear();
	_clusters.clear();
}

void OutgoingSocketHandler::readyRead(const Cluster& cluster, const Cluster::Node& node) {
    QJsonDocument message = _sockets[hash(cluster, node)]->read();
    emit messageReceived(cluster, node, message);
}


void OutgoingSocketHandler::sendMessage(const Cluster& cluster, QJsonDocument msg) const {
    assert(!msg.isEmpty());

    for (const Cluster::Node& node : cluster.nodes()) {
        qDebug() << "Node: " << node.name << node.ipAddress << node.port;
        HashValue h = hash(cluster, node);

        auto it = _sockets.find(h);
        assert(it != _sockets.end());

        it->second->socket()->dumpObjectInfo();

        it->second->write(msg);
    }
}

