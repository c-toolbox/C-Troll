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

#include "clusterwidget.h"

#include "cluster.h"
#include "database.h"
#include "node.h"
#include <fmt/format.h>
#include <QLabel>
#include <QVBoxLayout>

namespace {
    constexpr const char* ColorConnected = "#33cc33";
    constexpr const char* ColorSomeConnected = "#aaaa33";
    constexpr const char* ColorDisconnected = "#dd3333";
} // namespace

ConnectionWidget::ConnectionWidget() {
    setMinimumHeight(32);
    setMaximumWidth(20);

    QLayout* layout = new QHBoxLayout;
    layout->setMargin(0);
    setLayout(layout);

    QWidget* w = new QWidget;
    layout->addWidget(w);

    setStatus(ConnectionStatus::Disconnected);
}

void ConnectionWidget::setStatus(ConnectionStatus status) {
    const char* color = [](ConnectionStatus s) {
        switch (s) {
            case ConnectionStatus::Connected: return ColorConnected;
            case ConnectionStatus::SomeConnected: return ColorSomeConnected;
            case ConnectionStatus::Disconnected: return ColorDisconnected;
            default: throw std::logic_error("Missing case label");
        }
    }(status);

    setStyleSheet(fmt::format(R"(background: {})", color).c_str());
}


//////////////////////////////////////////////////////////////////////////////////////////


NodeWidget::NodeWidget(const Node& node)
    : QGroupBox(node.name.c_str())
    , _nodeId(node.id)
{
    setObjectName("node");

    QBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    setLayout(layout);

    _connectionLabel = new ConnectionWidget;
    _connectionLabel->setStatus(
        node.isConnected ?
        ConnectionWidget::ConnectionStatus::Connected :
        ConnectionWidget::ConnectionStatus::Disconnected
    );
    layout->addWidget(_connectionLabel);

    QLabel* ip = new QLabel(node.ipAddress.c_str());
    layout->addWidget(ip);
}

void NodeWidget::updateConnectionStatus() {
    Node* n = data::findNode(_nodeId);
    assert(n);
    _connectionLabel->setStatus(
        n->isConnected ?
        ConnectionWidget::ConnectionStatus::Connected :
        ConnectionWidget::ConnectionStatus::Disconnected
    );
}


//////////////////////////////////////////////////////////////////////////////////////////


ClusterWidget::ClusterWidget(const Cluster& cluster)
    : QGroupBox(cluster.name.c_str())
    , _clusterId(cluster.id)
{
    setObjectName("cluster");
    QBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    setLayout(layout);

    _connectionLabel = new ConnectionWidget;
    _connectionLabel->setStatus(ConnectionWidget::ConnectionStatus::Disconnected);
    layout->addWidget(_connectionLabel);

    std::vector<Node*> nodes = data::findNodesForCluster(cluster);
    for (Node* n : nodes) {
        NodeWidget* node = new NodeWidget(*n);
        layout->addWidget(node);
        _nodeWidgets[n->id] = node;
        updateConnectionStatus(n->id);
    }
}

void ClusterWidget::updateConnectionStatus(Node::ID nodeId) {
    _nodeWidgets[nodeId]->updateConnectionStatus();

    Cluster* cluster = data::findCluster(_clusterId);
    assert(cluster);
    std::vector<Node*> nodes = data::findNodesForCluster(*cluster);

    const bool allConnected = std::all_of(
        nodes.begin(), nodes.end(), std::mem_fn(&Node::isConnected)
    );
    const bool someConnected = !std::none_of(
        nodes.begin(), nodes.end(), std::mem_fn(&Node::isConnected)
    );

    ConnectionWidget::ConnectionStatus status =
        allConnected ?
        ConnectionWidget::ConnectionStatus::Connected :
            someConnected ?
            ConnectionWidget::ConnectionStatus::SomeConnected :
            ConnectionWidget::ConnectionStatus::Disconnected;

    _connectionLabel->setStatus(status);
}


//////////////////////////////////////////////////////////////////////////////////////////


ClustersWidget::ClustersWidget() {
    setObjectName("clusterwidget");

    QBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    for (Cluster* c : data::clusters()) {
        ClusterWidget* widget = new ClusterWidget(*c);
        _clusterWidgets[c->id] = widget;
        layout->addWidget(widget);
    }
    layout->addStretch();
}

void ClustersWidget::connectedStatusChanged(Cluster::ID clusterId, Node::ID nodeId) {
    const auto it = _clusterWidgets.find(clusterId);
    assert(it != _clusterWidgets.end());
    it->second->updateConnectionStatus(nodeId);
}
