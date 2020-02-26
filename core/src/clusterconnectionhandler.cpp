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

#include "database.h"
#include "jsonsocket.h"
#include "logging.h"
#include <QTimer>
#include <assert.h>
#include <fmt/format.h>

ClusterConnectionHandler::~ClusterConnectionHandler() {
    // We need to do the deletion this way since there will be messages pending for the
    // JsonSocket on the event queue (particuarly the signalling that the connection is
    // closed.  So we keep the actual values around while the destructor is running and
    // only delete the sockets in the next tick (see `deleteLater`)
    for (std::pair<const int, std::unique_ptr<common::JsonSocket>>& p : _sockets) {
        QObject::disconnect(p.second.get());
        p.second.release()->deleteLater();
    }
    _sockets.clear();
}

void ClusterConnectionHandler::initialize() {
    for (Node* node : data::nodes()) {
        // This handler keeps the sockets to the tray applications open
        std::unique_ptr<QTcpSocket> socket = std::make_unique<QTcpSocket>();

        connect(
            socket.get(), &QAbstractSocket::stateChanged,
            [this, id = node->id](QAbstractSocket::SocketState state) {
                handleSocketStateChange(id, state);
            }
        );

        auto jsonSocket = std::make_unique<common::JsonSocket>(std::move(socket));
        connect(
            jsonSocket.get(), &common::JsonSocket::readyRead,
            [this, id = node->id]() { readyRead(id); }
        );
        common::JsonSocket* s = jsonSocket.get();
        _sockets[node->id] = std::move(jsonSocket);
        s->connectToHost(node->ipAddress, node->port);
    }


    //for (Cluster* c : data::clusters()) {
    //    std::vector<Node*> nodes = data::findNodesForCluster(*c);
    //    for (Node* node : nodes) {
    //        std::string h = hash(*node);
    //        const auto it = _nodes.find(h);
    //        if (it != _nodes.end()) {
    //            // A socket to this node has already been created, so we just need to
    //            // register our interest in this
    //            it->second.nodes.push_back({ c, node });
    //            continue;
    //        }

    //        // This handler keeps the sockets to the tray applications open
    //        std::unique_ptr<QTcpSocket> socket = std::make_unique<QTcpSocket>();

    //        connect(
    //            socket.get(), &QAbstractSocket::stateChanged,
    //            [this, h](QAbstractSocket::SocketState state) {
    //                handleSocketStateChange(h, state);
    //            }
    //        );

    //        SocketData data;
    //        data.socket = std::make_unique<common::JsonSocket>(std::move(socket));
    //        data.nodes.push_back({ c, node });

    //        connect(
    //            data.socket.get(), &common::JsonSocket::readyRead,
    //            [this, h]() { readyRead(h); }
    //        );

    //        // We need to save the pointer as the connectToHost function will trigger the
    //        // Qt signal/slot which in turn requires the socket to be on the _nodes list
    //        common::JsonSocket* s = data.socket.get();
    //        _nodes[h] = std::move(data);

    //        s->connectToHost(node->ipAddress, node->port);

    //    }
    //}
    
    //QTimer* timer = new QTimer(this);
    //connect(
    //    timer, &QTimer::timeout,
    //    [this]() {
    //        for (auto& [key, value] : _nodes) {
    //            QAbstractSocket::SocketState state = value.socket->state();
    //            if (state == QAbstractSocket::SocketState::UnconnectedState) {
    //                for (NodeInfo& ni : value.nodes) {
    //                    Log(fmt::format(
    //                        "Unconnected: {}:{}",
    //                        ni.node->ipAddress, ni.node->port
    //                    ));
    //                    if (ni.node->isConnected) {
    //                        ni.node->isConnected = false;
    //                        emit connectedStatusChanged(ni.cluster->id, ni.node->id);
    //                    }
    //                }
    //            }
    //        }
    //    }
    //);
    //timer->start(2500);
}

void ClusterConnectionHandler::handleSocketStateChange(int nodeId,
                                                       QAbstractSocket::SocketState state)
{
    Node* node = data::findNode(nodeId);
    assert(node);

    const bool isConnected = state == QAbstractSocket::SocketState::ConnectedState;

    Log(fmt::format("Socket state change: {}:{} {}", node->ipAddress, node->port, state));
    if (node->isConnected != isConnected) {
        node->isConnected = isConnected;
    }

    std::vector<Cluster*> clusters = data::findClusterForNode(*node);
    for (Cluster* cluster : clusters) {
        emit connectedStatusChanged(cluster->id, node->id);
    }
}

void ClusterConnectionHandler::readyRead(int nodeId) {
    const auto it = _sockets.find(nodeId);
    assert(it != _sockets.end());

    nlohmann::json message = it->second->read();

    if (common::isValidMessage<common::ProcessStatusMessage>(message)) {
        common::ProcessStatusMessage status = message;
        emit receivedTrayProcess(status);
    }
    else {
        Node* node = data::findNode(nodeId);
        std::vector<Cluster*> clusters = data::findClusterForNode(*node);

        for (Cluster* cluster : clusters) {
            emit messageReceived(cluster->id, node->id, message);
        }
    }
}

void ClusterConnectionHandler::sendMessage(const Cluster& cluster, const Node& node,
                                           nlohmann::json msg) const
{
    assert(!msg.is_null());

    std::string p = std::to_string(node.port);
    Log("Node: " + node.name + '\t' + node.ipAddress + ':' + p);

    const auto it = _sockets.find(node.id);
    assert(it != _sockets.end());

    it->second->write(msg);
}
