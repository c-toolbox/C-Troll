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

#include "clusterconnectionhandler.h"

#include "jsonsocket.h"
#include "logging.h"
#include <QTimer>
#include <assert.h>
#include <fmt/format.h>

namespace {
    std::string hash(const Cluster& cluster, const Cluster::Node& node) {
        return cluster.name + "::" + node.name;
    }
} // namespace

void ClusterConnectionHandler::initialize(std::vector<Cluster>& clusters) {
    for (Cluster& c : clusters) {
        for (Cluster::Node& node : c.nodes) {
            // This handler keeps the sockets to the tray applications open
            std::unique_ptr<QTcpSocket> socket = std::make_unique<QTcpSocket>();
            connect(
                socket.get(), &QAbstractSocket::stateChanged,
                [this, &c, &node](QAbstractSocket::SocketState state) {
                    if (state == QAbstractSocket::SocketState::ConnectedState) {
                        Log(fmt::format("Socket state change: {}:{}  {}",
                            node.ipAddress, node.port, state 
                        ));
                        if (!node.isConnected) {
                            node.isConnected = true;
                            emit connectedStatusChanged(c, node);
                        }
                    }
                    else if (state == QAbstractSocket::SocketState::ClosingState) {
                        Log(fmt::format("Socket state change: {}:{}  {}",
                            node.ipAddress, node.port, state
                        ));
                        if (node.isConnected) {
                            node.isConnected = false;
                            emit connectedStatusChanged(c, node);
                        }
                    }
                }
            );

            auto jsonSocket = std::make_unique<common::JsonSocket>(std::move(socket));

            connect(
                jsonSocket.get(), &common::JsonSocket::readyRead,
                [this, c, node]() { readyRead(c, node); }
            );

            jsonSocket->connectToHost(node.ipAddress, node.port);
            std::string h = hash(c, node);
            _sockets[h] = std::move(jsonSocket);
        }
    }
    
    QTimer* timer = new QTimer(this);
    connect(
        timer, &QTimer::timeout,
        [this, &clusters]() {
            for (Cluster& c : clusters) {
                for (Cluster::Node& node : c.nodes) {
                    std::string h = hash(c, node);
                    auto it = _sockets.find(h);
                    assert(it != _sockets.end());

                    QAbstractSocket::SocketState state = it->second->state();
                    if (state == QAbstractSocket::SocketState::UnconnectedState) {
                        Log(fmt::format("Unconnected: {}:{}", node.ipAddress, node.port));
                        if (node.isConnected) {
                            node.isConnected = false;
                            emit connectedStatusChanged(c, node);
                        }
                        it->second->connectToHost(node.ipAddress, node.port);
                    }
                }
            }
            
        }
    );
    timer->start(2500);
}

void ClusterConnectionHandler::readyRead(const Cluster& cluster,
                                         const Cluster::Node& node)
{
    nlohmann::json message = _sockets[hash(cluster, node)]->read();

    std::string type = message["type"];
    if (type == common::TrayProcessStatus::Type) {
        common::TrayProcessStatus status = message["payload"];
        emit receivedTrayProcess(status);
    }
    else {
        emit messageReceived(cluster, node, message);
    }
}

void ClusterConnectionHandler::sendMessage(const Cluster& cluster,
                                           nlohmann::json msg) const
{
    assert(!msg.is_null());

    for (const Cluster::Node& node : cluster.nodes) {
        std::string p = std::to_string(node.port);
        Log("Node: " + node.name + '\t' + node.ipAddress + ':' + p);
        std::string h = hash(cluster, node);

        const auto it = _sockets.find(h);
        assert(it != _sockets.end());

        it->second->write(msg);
    }
}