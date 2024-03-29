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

#include "clusterconnectionhandler.h"

#include "database.h"
#include "logging.h"
#include "messages.h"
#include "node.h"
#include <QTimer>
#include <fmt/format.h>
#include <assert.h>

namespace {
    std::string_view stateToString(QAbstractSocket::SocketState state) {
        switch (state) {
            case QAbstractSocket::SocketState::UnconnectedState: return "Unconnected";
            case QAbstractSocket::SocketState::HostLookupState: return "Host Lookup";
            case QAbstractSocket::SocketState::ConnectingState: return "Connecting";
            case QAbstractSocket::SocketState::ConnectedState: return "Connected";
            case QAbstractSocket::SocketState::BoundState: return "Bound";
            case QAbstractSocket::SocketState::ListeningState: return "Listening";
            case QAbstractSocket::SocketState::ClosingState: return "Closing";
        }
        throw std::logic_error("Missing case label");
    }
} // namespace

ClusterConnectionHandler::~ClusterConnectionHandler() {
    // We need to do the deletion this way since there will be messages pending for the
    // JsonSocket on the event queue (particuarly the signalling that the connection is
    // closed.  So we keep the actual values around while the destructor is running and
    // only delete the sockets in the next tick (see `deleteLater`)
    for (std::pair<const Node::ID, std::unique_ptr<common::JsonSocket>>& p : _sockets) {
        QObject::disconnect(p.second.get());
        p.second.release()->deleteLater();
    }
    _sockets.clear();
}

void ClusterConnectionHandler::initialize() {
    for (const Node* node : data::nodes()) {
        // This handler keeps the sockets to the tray applications open
        std::unique_ptr<QTcpSocket> socket = std::make_unique<QTcpSocket>();

        connect(
            socket.get(), &QAbstractSocket::stateChanged,
            [this, id = node->id](QAbstractSocket::SocketState state) {
                handleSocketStateChange(id, state);
            }
        );

        std::unique_ptr<common::JsonSocket> jsonSocket =
            std::make_unique<common::JsonSocket>(std::move(socket), node->secret);

        connect(
            jsonSocket.get(), &common::JsonSocket::messageReceived,
            [this, id = node->id](nlohmann::json message) {
                try {
                    handleMessage(message, id);
                }
                catch (const std::exception& e) {
                    Log(
                        "ClusterConnectionHandler::initialize",
                        fmt::format(
                            "Caught exception {} when receiving message {}",
                            e.what(), message.dump()
                        )
                    );
                }
            }
        );
        common::JsonSocket* s = jsonSocket.get();
        _sockets[node->id] = std::move(jsonSocket);
        s->connectToHost(node->ipAddress, node->port);
    }
    QTimer* timer = new QTimer(this);
    connect(
        timer, &QTimer::timeout,
        [this]() {
            using K = Node::ID;
            using V = std::unique_ptr<common::JsonSocket>;
            for (const std::pair<const K, V>& p : _sockets) {
                // Try to reconnect all sockets that are currently unconnected
                if (p.second->state() == QAbstractSocket::SocketState::UnconnectedState) {
                    const Node* node = data::findNode(p.first);
                    p.second->connectToHost(node->ipAddress, node->port);
                }
            }
        }
    );
    timer->start(std::chrono::milliseconds(2500));
}

void ClusterConnectionHandler::handleSocketStateChange(Node::ID nodeId,
                                                       QAbstractSocket::SocketState state)
{
    if (state != QAbstractSocket::SocketState::ConnectedState &&
        state != QAbstractSocket::SocketState::ClosingState)
    {
        // These are the only two states we are interested in
        return;
    }

    const Node* node = data::findNode(nodeId);
    assert(node);

    Log(fmt::format(
        "Socket State Change [{}:{}]",
        node->ipAddress, node->port), std::string(stateToString(state)
    ));

    if (state == QAbstractSocket::SocketState::ConnectedState) {
        data::setNodeConnecting(nodeId, true);
    }
    else if (state == QAbstractSocket::SocketState::ClosingState) {
        data::setNodeDisconnecting(nodeId);
    }

    std::vector<const Cluster*> clusters = data::findClusterForNode(*node);
    for (const Cluster* cluster : clusters) {
        emit connectedStatusChanged(cluster->id, node->id);
    }
}

void ClusterConnectionHandler::handleMessage(nlohmann::json message, Node::ID nodeId) {
    const auto it = _sockets.find(nodeId);
    assert(it != _sockets.end());
    assert(data::findNode(nodeId)->isConnecting || data::findNode(nodeId)->isConnected);

#ifdef QT_DEBUG
    assert(data::findNode(nodeId));
    std::string content = common::isValidMessage<common::ProcessOutputMessage>(message) ?
        std::string(common::ProcessOutputMessage::Type) :
        message.dump();

    Debug(
        fmt::format("Received [{}:{} ({})]", data::findNode(nodeId)->ipAddress,
            data::findNode(nodeId)->port,
            data::findNode(nodeId)->name
        ),
        content
    );
#endif // QT_DEBUG

    if (common::isValidMessage<common::ProcessStatusMessage>(message)) {
        common::ProcessStatusMessage status = message;
        emit receivedTrayProcess(status);
    }
    else if (common::isValidMessage<common::TrayStatusMessage>(message)) {
        common::TrayStatusMessage status = message;
        emit receivedTrayStatus(nodeId, status);
    }
    else if (common::isValidMessage<common::TrayConnectedMessage>(message)) {
        const Node* node = data::findNode(nodeId);
        assert(node->isConnecting);
        assert(!node->isConnected);
        data::setNodeConnecting(nodeId, false);
        data::setNodeConnected(nodeId, true);

        std::vector<const Cluster*> clusters = data::findClusterForNode(*node);
        for (const Cluster* cluster : clusters) {
            emit connectedStatusChanged(cluster->id, node->id);
        }
    }
    else if (common::isValidMessage<common::InvalidAuthMessage>(message)) {
        common::InvalidAuthMessage msg = message;
        emit receivedInvalidAuthStatus(nodeId, msg);
    }
    else if (common::isValidMessage<common::ProcessOutputMessage>(message)) {
        common::ProcessOutputMessage msg = message;
        emit receivedProcessMessage(nodeId, msg);
    }
    else if (common::isValidMessage<common::ErrorOccurredMessage>(message)) {
        common::ErrorOccurredMessage msg = message;
        emit receivedErrorMessage(nodeId, msg);
    }
    else {
        const Node* n = data::findNode(nodeId);
        assert(n);
        std::vector<const Cluster*> clusters = data::findClusterForNode(*n);

        for (const Cluster* c : clusters) {
            assert(c);
            Log(fmt::format("Received [{} / {}]", c->name, n->name), message.dump());
        }
    }
}

void ClusterConnectionHandler::sendMessage(const Node& node, nlohmann::json msg) const {
    assert(!msg.is_null());

    Log(
        fmt::format("Sending [{}:{} ({})]", node.ipAddress, node.port, node.name),
        msg.dump()
    );
    const auto it = _sockets.find(node.id);
    assert(it != _sockets.end());

    it->second->write(msg);
}
