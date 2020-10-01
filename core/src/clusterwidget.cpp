/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sund�n, Emil Axelsson                                            *
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

#include "database.h"
#include "node.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QStyleOption>
#include <QVBoxLayout>
#include <fmt/format.h>

void ConnectionWidget::setStatus(ConnectionStatus status) {
    std::string string = [](ConnectionStatus s) {
        switch (s) {
            case ConnectionStatus::Connected:          return "connected";
            case ConnectionStatus::PartiallyConnected: return "partially connected";
            case ConnectionStatus::Disconnected:       return "disconnected";
            default:                         throw std::logic_error("Missing case label");
        }
    }(status);
    setToolTip(QString::fromStdString(string));

    setProperty("state", QString::fromStdString(string));
    style()->unpolish(this);
    style()->polish(this);
}

void ConnectionWidget::paintEvent(QPaintEvent*) {
    // Since this is a pretty empty widget, we need to overwrite this function:
    // https://doc.qt.io/qt-5/stylesheet-reference.html
    QStyleOption opt;
    opt.init(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


//////////////////////////////////////////////////////////////////////////////////////////


NodeWidget::NodeWidget(const Node& node)
    : QGroupBox(QString::fromStdString(node.name))
    , _nodeId(node.id)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
    
    QBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    _connectionLabel = new ConnectionWidget;
    _connectionLabel->setStatus(
        node.isConnected ?
        ConnectionWidget::ConnectionStatus::Connected :
        ConnectionWidget::ConnectionStatus::Disconnected
    );
    layout->addWidget(_connectionLabel);

    QLabel* ip = new QLabel(QString::fromStdString(node.ipAddress));
    layout->addWidget(ip);

    //layout->addStretch();
    QPushButton* kill = new QPushButton("Kill processes");
    connect(
        kill, &QPushButton::clicked,
        [this, node]() {
            std::string text = fmt::format(
                "Are you sure you want to kill all processes on '{}'?", node.name
            );

            QMessageBox box;
            box.setText("Kill all processes on single node");
            box.setInformativeText(QString::fromStdString(text));
            box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            box.setDefaultButton(QMessageBox::Ok);
            const int res = box.exec();

            if (res == QMessageBox::Ok) {
                emit killProcesses(node.id);
            }
        }
    );
    //layout->addWidget(kill);
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
    : QGroupBox(QString::fromStdString(cluster.name))
    , _clusterId(cluster.id)
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    QGridLayout* layout = new QGridLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    _connectionLabel = new ConnectionWidget;
    _connectionLabel->setStatus(ConnectionWidget::ConnectionStatus::Disconnected);
    layout->addWidget(_connectionLabel);

    std::vector<Node*> nodes = data::findNodesForCluster(cluster);
    static constexpr const int Columns = 5;
    for (size_t i = 0; i < nodes.size(); ++i) {
        Node* n = nodes[i];
        NodeWidget* node = new NodeWidget(*n);
        connect(
            node, &NodeWidget::killProcesses,
            this, QOverload<Node::ID>::of(&ClusterWidget::killProcesses)
        );
        layout->addWidget(
            node,
            static_cast<int>(i) / Columns,
            static_cast<int>(i) % Columns
        );
        _nodeWidgets[n->id] = node;
        updateConnectionStatus(n->id);
    }

    QPushButton* kill = new QPushButton("Kill all processes");
    kill->setObjectName("killall");
    connect(
        kill, &QPushButton::clicked,
        [this, cluster]() {
            std::string text = fmt::format(
                "Are you sure you want to kill all processes on cluster '{}'?",
                cluster.name
            );

            QMessageBox box;
            box.setText("Kill process");
            box.setInformativeText(QString::fromStdString(text));
            box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            box.setDefaultButton(QMessageBox::Ok);
            const int res = box.exec();

            if (res == QMessageBox::Ok) {
                emit killProcesses(cluster.id);
            }
        }
    );
    layout->addWidget(
        kill,
        static_cast<int>(nodes.size()) / Columns + 1,
        0,
        1,
        Columns
    );
}

void ClusterWidget::updateConnectionStatus(Node::ID nodeId) {
    _nodeWidgets[nodeId]->updateConnectionStatus();

    Cluster* cluster = data::findCluster(_clusterId);
    assert(cluster);
    std::vector<Node*> nodes = data::findNodesForCluster(*cluster);

    const bool allConnected = std::all_of(
        nodes.cbegin(), nodes.cend(), std::mem_fn(&Node::isConnected)
    );
    const bool someConnected = !std::none_of(
        nodes.cbegin(), nodes.cend(), std::mem_fn(&Node::isConnected)
    );

    ConnectionWidget::ConnectionStatus status =
        allConnected ?
        ConnectionWidget::ConnectionStatus::Connected :
            someConnected ?
            ConnectionWidget::ConnectionStatus::PartiallyConnected :
            ConnectionWidget::ConnectionStatus::Disconnected;

    _connectionLabel->setStatus(status);
}


//////////////////////////////////////////////////////////////////////////////////////////


ClustersWidget::ClustersWidget() {
    setWidgetResizable(true);
    QWidget* content = new QWidget;
    setWidget(content);
    QBoxLayout* contentLayout = new QVBoxLayout(content);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    for (Cluster* c : data::clusters()) {
        ClusterWidget* widget = new ClusterWidget(*c);
        connect(
            widget, QOverload<Node::ID>::of(&ClusterWidget::killProcesses),
            this, QOverload<Node::ID>::of(&ClustersWidget::killProcesses)
        );
        connect(
            widget, QOverload<Cluster::ID>::of(&ClusterWidget::killProcesses),
            this, QOverload<Cluster::ID>::of(&ClustersWidget::killProcesses)
        );
        _clusterWidgets[c->id] = widget;
        contentLayout->addWidget(widget);
    }
}

void ClustersWidget::connectedStatusChanged(Cluster::ID clusterId, Node::ID nodeId) {
    const auto it = _clusterWidgets.find(clusterId);
    assert(it != _clusterWidgets.end());
    it->second->updateConnectionStatus(nodeId);
}
