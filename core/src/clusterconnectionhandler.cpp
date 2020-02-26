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
    std::string hash(const Cluster::Node& node) {
        // @TODO (abock, 2020-02-24) Remove the IP address from this part once the
        // uniqueness of cluster names is enforced
        return node.name + ':' + std::to_string(node.port) + ';' + node.ipAddress;
    }
} // namespace

ClusterConnectionHandler::~ClusterConnectionHandler() {
    // We need to do the deletion this way since there will be messages pending for the
    // JsonSocket on the event queue (particuarly the signalling that the connection is
    // closed.  So we keep the actual values around while the destructor is running and
    // only delete the sockets in the next tick (see `deleteLater`)
    for (auto& [key, value] : _nodes) {
        QObject::disconnect(value.socket.get());
        value.socket.release()->deleteLater();
    }
    _nodes.clear();
}

void ClusterConnectionHandler::initialize(const std::vector<Cluster*>& clusters) {
    for (Cluster* c : clusters) {
        for (const std::unique_ptr<Cluster::Node>& node : c->nodes) {
            std::string h = hash(*node);
            const auto it = _nodes.find(h);
            if (it != _nodes.end()) {
                // A socket to this node has already been created, so we just need to
                // register our interest in this
                it->second.nodes.push_back({ c, node.get() });
                continue;
            }

            // This handler keeps the sockets to the tray applications open
            std::unique_ptr<QTcpSocket> socket = std::make_unique<QTcpSocket>();

            connect(
                socket.get(), &QAbstractSocket::stateChanged,
                [this, h](QAbstractSocket::SocketState state) {
                    handleSocketStateChange(h, state);
                }
            );

            SocketData data;
            data.socket = std::make_unique<common::JsonSocket>(std::move(socket));
            data.nodes.push_back({ c, node.get() });

            connect(
                data.socket.get(), &common::JsonSocket::readyRead,
                [this, h]() { readyRead(h); }
            );

            // We need to save the pointer as the connectToHost function will trigger the
            // Qt signal/slot which in turn requires the socket to be on the _nodes list
            common::JsonSocket* s = data.socket.get();
            _nodes[h] = std::move(data);

            s->connectToHost(node->ipAddress, node->port);

        }
    }
    
    QTimer* timer = new QTimer(this);
    connect(
        timer, &QTimer::timeout,
        [this]() {
            for (auto& [key, value] : _nodes) {
                QAbstractSocket::SocketState state = value.socket->state();
                if (state == QAbstractSocket::SocketState::UnconnectedState) {
                    for (NodeInfo& ni : value.nodes) {
                        Log(fmt::format(
                            "Unconnected: {}:{}",
                            ni.node->ipAddress, ni.node->port
                        ));
                        if (ni.node->isConnected) {
                            ni.node->isConnected = false;
                            emit connectedStatusChanged(ni.cluster->id, ni.node->id);
                        }
                    }
                }
            }
        }
    );
    timer->start(2500);
}

void ClusterConnectionHandler::handleSocketStateChange(const std::string& h,
                                                       QAbstractSocket::SocketState state)
{
    const auto it = _nodes.find(h);
    assert(it != _nodes.end());

    const bool isConnected = state == QAbstractSocket::SocketState::ConnectedState;

    for (NodeInfo& ni : it->second.nodes) {
        const std::string thisHash = hash(*ni.node);
        if (thisHash != h) {
            // We are only interested in modifying the node info structs that correspond
            // to the node that has just changed the stocket state
            continue;
        }

        Log(fmt::format("Socket state change: {}:{}  {}",
            ni.node->ipAddress, ni.node->port, state
        ));

        if (ni.node->isConnected != isConnected) {
            ni.node->isConnected = isConnected;
            emit connectedStatusChanged(ni.cluster->id, ni.node->id);
        }
    }
}

void ClusterConnectionHandler::readyRead(const std::string& hash) {
    const auto it = _nodes.find(hash);
    assert(it != _nodes.end());

    nlohmann::json message = it->second.socket->read();

    if (common::isValidMessage<common::ProcessStatusMessage>(message)) {
        common::ProcessStatusMessage status = message;
        emit receivedTrayProcess(status);
    }
    else {
        for (const NodeInfo& ni : it->second.nodes) {
            emit messageReceived(ni.cluster, ni.node, message);
        }
    }
}

void ClusterConnectionHandler::sendMessage(const Cluster& cluster,
                                           const Cluster::Node& node,
                                           nlohmann::json msg) const
{
    assert(!msg.is_null());

    std::string p = std::to_string(node.port);
    Log("Node: " + node.name + '\t' + node.ipAddress + ':' + p);
    std::string h = hash(node);

    const auto it = _nodes.find(h);
    assert(it != _nodes.end());

    it->second.socket->write(msg);
}
