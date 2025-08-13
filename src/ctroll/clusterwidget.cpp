/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2025                                                               *
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

#include "clusterwidget.h"

#include "database.h"
#include "node.h"
#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPainter>
#include <QPushButton>
#include <QStyle>
#include <QStyleOption>
#include <QVBoxLayout>

void ConnectionWidget::setStatus(Status status) {
    std::string string = [](Status s) {
        switch (s) {
            case Status::Connected:           return "connected";
            case Status::ConnectedButInvalid: return "connected invalid";
            case Status::PartiallyConnected:  return "partially connected";
            case Status::Disconnected:        return "disconnected";
        }
        throw std::logic_error("Missing case label");
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
    opt.initFrom(this);
    QPainter p(this);
    style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, this);
}


//////////////////////////////////////////////////////////////////////////////////////////


NodeWidget::NodeWidget(const Node& node, bool showShutdownButton)
    : QGroupBox(QString::fromStdString(node.name))
    , _nodeId(node.id)
{
    setToolTip(QString::fromStdString(node.description));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    QBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    QWidget* topRow = new QWidget;
    QBoxLayout* topLayout = new QHBoxLayout(topRow);
    topLayout->setContentsMargins(0, 0, 0, 0);

    _connectionLabel = new ConnectionWidget;
    assert(!(node.isConnecting && node.isConnected));
    if (node.isConnecting) {
        _connectionLabel->setStatus(ConnectionWidget::Status::ConnectedButInvalid);
    }
    else if (node.isConnected) {
        _connectionLabel->setStatus(ConnectionWidget::Status::Connected);
    }
    else {
        _connectionLabel->setStatus(ConnectionWidget::Status::Disconnected);
    }
    topLayout->addWidget(_connectionLabel);

    QLabel* ip = new QLabel(QString::fromStdString(node.ipAddress));
    topLayout->addWidget(ip);
    layout->addWidget(topRow);

    QWidget* bottomRow = new QWidget;
    QGridLayout* bottomLayout = new QGridLayout(bottomRow);
    bottomLayout->setContentsMargins(0, 0, 0, 0);

    _killProcesses = new QPushButton("Kill processes");
    _killProcesses->setToolTip("Kills all processes on this particular node");
    connect(
        _killProcesses, &QPushButton::clicked,
        [this, node]() {
            std::string text = std::format(
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
    bottomLayout->addWidget(_killProcesses, 0, 0);

    _killTray = new QPushButton("Kill Tray");
    _killTray->setObjectName("killtray");
    _killTray->setToolTip("Kills the Tray application on this particular node");
    connect(
        _killTray, &QPushButton::clicked,
        [this, node]() {
            std::string text = std::format(
                "Are you sure you want to kill the TRAY on '{}'?", node.name
            );

            QMessageBox box;
            box.setText("Kill TRAY application");
            box.setInformativeText(QString::fromStdString(text));
            box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            box.setDefaultButton(QMessageBox::Ok);
            int res = box.exec();
            if (res == QMessageBox::Ok) {
                box.setInformativeText(QString::fromStdString("AGAIN: " + text));
                res = box.exec();

                if (res == QMessageBox::Ok) {
                    emit killTray(node.id);
                }
            }
        }
    );
    bottomLayout->addWidget(_killTray, 0, 1);

    _restartNode = new QPushButton("Restart node");
    _restartNode->setObjectName("restartnode");
    _restartNode->setToolTip("Restarts this particular node");
    connect(
        _restartNode, &QPushButton::clicked,
        [this, node]() {
            std::string text = std::format(
                "Are you sure you want to restart '{}'?", node.name
            );

            QMessageBox box;
            box.setText("Restart node");
            box.setInformativeText(QString::fromStdString(text));
            box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            box.setDefaultButton(QMessageBox::Ok);
            int res = box.exec();
            if (res == QMessageBox::Ok) {
                box.setInformativeText(QString::fromStdString("AGAIN: " + text));
                res = box.exec();

                if (res == QMessageBox::Ok) {
                    emit restartNode(node.id);
                }
            }
        }
    );
    // If we want to show the shutdown buttons, the restart node button only needs to be
    // normal width instead. Otherwise we make it double width to fill the widget
    bottomLayout->addWidget(_restartNode, 1, 0, 1, showShutdownButton ? 1 : 2);

    if (showShutdownButton) {
        _shutdownNode = new QPushButton("Shutdown node");
        _shutdownNode->setObjectName("shutdownnode");
        _shutdownNode->setToolTip("Shuts down this particular node");
        connect(
            _shutdownNode, &QPushButton::clicked,
            [this, node]() {
                std::string text = std::format(
                    "Are you sure you want to shut down '{}'?", node.name
                );

                QMessageBox box;
                box.setText("Shutdown node");
                box.setInformativeText(QString::fromStdString(text));
                box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                box.setDefaultButton(QMessageBox::Ok);
                int res = box.exec();
                if (res == QMessageBox::Ok) {
                    box.setInformativeText(QString::fromStdString("AGAIN: " + text));
                    res = box.exec();

                    if (res == QMessageBox::Ok) {
                        emit shutdownNode(node.id);
                    }
                }
            }
        );
        bottomLayout->addWidget(_shutdownNode, 1, 1);
    }

    layout->addWidget(bottomRow);
}

void NodeWidget::updateConnectionStatus() {
    const Node* n = data::findNode(_nodeId);
    assert(n);
    if (n->isConnecting) {
        _connectionLabel->setStatus(ConnectionWidget::Status::ConnectedButInvalid);
    }
    else if (n->isConnected) {
        _connectionLabel->setStatus(ConnectionWidget::Status::Connected);
    }
    else {
        _connectionLabel->setStatus(ConnectionWidget::Status::Disconnected);
    }

    _killProcesses->setEnabled(n->isConnected);
    _killTray->setEnabled(n->isConnected);
    _restartNode->setEnabled(n->isConnected);
    if (_shutdownNode) {
        _shutdownNode->setEnabled(n->isConnected);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////


ClusterWidget::ClusterWidget(const Cluster& cluster, bool showShutdownButton)
    : QGroupBox(QString::fromStdString(cluster.name))
    , _clusterId(cluster.id)
{
    setToolTip(QString::fromStdString(cluster.description));
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);

    QGridLayout* layout = new QGridLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    _connectionLabel = new ConnectionWidget;
    _connectionLabel->setStatus(ConnectionWidget::Status::Disconnected);

    std::vector<const Node*> nodes = data::findNodesForCluster(cluster);
    constexpr size_t Columns = 5;
    for (size_t i = 0; i < nodes.size(); i++) {
        const Node* n = nodes[i];
        NodeWidget* node = new NodeWidget(*n, showShutdownButton);
        connect(
            node, &NodeWidget::killProcesses,
            this, QOverload<Node::ID>::of(&ClusterWidget::killProcesses)
        );
        connect(node, &NodeWidget::killTray, this, &ClusterWidget::killTray);
        connect(node, &NodeWidget::restartNode, this, &ClusterWidget::restartNode);
        connect(node, &NodeWidget::shutdownNode, this, &ClusterWidget::shutdownNode);
        layout->addWidget(
            node,
            static_cast<int>(i / Columns),
            static_cast<int>(i % Columns)
        );
        _nodeWidgets[n->id] = node;
    }

    QWidget* boxContainer = new QWidget;
    QGridLayout* btnLayout = new QGridLayout(boxContainer);
    btnLayout->setContentsMargins(0, 0, 0, 0);

    _killProcesses = new QPushButton("Kill all processes");
    connect(
        _killProcesses, &QPushButton::clicked,
        [this, cluster]() {
            std::string text = std::format(
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
    btnLayout->addWidget(_killProcesses, 0, 0);

    _killTrays = new QPushButton("Kill all trays");
    _killTrays->setObjectName("killtrays");
    connect(
        _killTrays, &QPushButton::clicked,
        [this, cluster]() {
            std::string text = std::format(
                "Are you sure you want to kill all TRAYs on cluster '{}'?", cluster.name
            );

            QMessageBox box;
            box.setText("Kill Trays");
            box.setInformativeText(QString::fromStdString(text));
            box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            box.setDefaultButton(QMessageBox::Ok);
            int res = box.exec();
            if (res == QMessageBox::Ok) {
                box.setInformativeText(QString::fromStdString("AGAIN: " + text));
                res = box.exec();
                if (res == QMessageBox::Ok) {
                    emit killTrays(cluster.id);
                }
            }
        }
    );
    btnLayout->addWidget(_killTrays, 0, 1);

    _restartNodes = new QPushButton("Restart all nodes");
    _restartNodes->setObjectName("restartnodes");
    connect(
        _restartNodes, &QPushButton::clicked,
        [this, cluster]() {
            std::string text = std::format(
                "Are you sure you want to restart all nodes of cluster '{}'?",
                cluster.name
            );

            QMessageBox box;
            box.setText("Restart nodes");
            box.setInformativeText(QString::fromStdString(text));
            box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
            box.setDefaultButton(QMessageBox::Ok);
            int res = box.exec();
            if (res == QMessageBox::Ok) {
                box.setInformativeText(QString::fromStdString("AGAIN: " + text));
                res = box.exec();
                if (res == QMessageBox::Ok) {
                    emit restartNodes(cluster.id);
                }
            }
        }
    );
    // If we want to show the shutdown buttons, the restart node button only needs to be
    // normal width instead. Otherwise we make it double width to fill the widget
    btnLayout->addWidget(_restartNodes, 1, 0, 1, showShutdownButton ? 1 : 2);

    if (showShutdownButton) {
        _shutdownNodes = new QPushButton("Shutdown all nodes");
        _shutdownNodes->setObjectName("shutdownnodes");
        connect(
            _shutdownNodes, &QPushButton::clicked,
            [this, cluster]() {
                std::string text = std::format(
                    "Are you sure you want to shut down all nodes of cluster '{}'?",
                    cluster.name
                );

                QMessageBox box;
                box.setText("Shut down nodes");
                box.setInformativeText(QString::fromStdString(text));
                box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                box.setDefaultButton(QMessageBox::Ok);
                int res = box.exec();
                if (res == QMessageBox::Ok) {
                    box.setInformativeText(QString::fromStdString("AGAIN: " + text));
                    res = box.exec();
                    if (res == QMessageBox::Ok) {
                        emit shutdownNodes(cluster.id);
                    }
                }
            }
        );
        btnLayout->addWidget(_shutdownNodes, 1, 1);
    }

    layout->setRowMinimumHeight(static_cast<int>(nodes.size()) / Columns + 1, 5);

    layout->addWidget(
        boxContainer,
        static_cast<int>(nodes.size() / Columns + 2),
        0,
        1,
        Columns
    );

    for (const Node* node : nodes) {
        updateConnectionStatus(node->id);
    }
}

void ClusterWidget::updateConnectionStatus(Node::ID nodeId) {
    _nodeWidgets[nodeId]->updateConnectionStatus();

    const Cluster* cluster = data::findCluster(_clusterId);
    assert(cluster);
    std::vector<const Node*> nodes = data::findNodesForCluster(*cluster);

    const bool allConnected = std::all_of(
        nodes.begin(), nodes.end(),
        std::mem_fn(&Node::isConnected)
    );
    const bool someConnected = !std::none_of(
        nodes.begin(), nodes.end(),
        std::mem_fn(&Node::isConnected)
    );

    ConnectionWidget::Status status =
        allConnected ?
        ConnectionWidget::Status::Connected :
            someConnected ?
            ConnectionWidget::Status::PartiallyConnected :
            ConnectionWidget::Status::Disconnected;

    _connectionLabel->setStatus(status);
    _killProcesses->setEnabled(someConnected);
    _killTrays->setEnabled(someConnected);
    _restartNodes->setEnabled(someConnected);
    if (_shutdownNodes) {
        _shutdownNodes->setEnabled(someConnected);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////


ClustersWidget::ClustersWidget(bool showShutdownButton) {
    setWidgetResizable(true);
    QWidget* content = new QWidget;
    setWidget(content);
    QBoxLayout* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(10, 2, 2, 2);
    contentLayout->setSpacing(20);
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    for (const Cluster* c : data::clusters()) {
        ClusterWidget* w = new ClusterWidget(*c, showShutdownButton);
        connect(
            w, QOverload<Node::ID>::of(&ClusterWidget::killProcesses),
            this, QOverload<Node::ID>::of(&ClustersWidget::killProcesses)
        );
        connect(
            w, QOverload<Cluster::ID>::of(&ClusterWidget::killProcesses),
            this, QOverload<Cluster::ID>::of(&ClustersWidget::killProcesses)
        );
        connect(w, &ClusterWidget::killTray, this, &ClustersWidget::killTray);
        connect(w, &ClusterWidget::killTrays, this, &ClustersWidget::killTrays);
        connect(w, &ClusterWidget::restartNode, this, &ClustersWidget::restartNode);
        connect(w, &ClusterWidget::restartNodes, this, &ClustersWidget::restartNodes);
        connect(w, &ClusterWidget::shutdownNode, this, &ClustersWidget::shutdownNode);
        connect(w, &ClusterWidget::shutdownNodes, this, &ClustersWidget::shutdownNodes);
        _clusterWidgets[c->id] = w;
        contentLayout->addWidget(w);
    }

    contentLayout->addStretch();
}

void ClustersWidget::connectedStatusChanged(Cluster::ID clusterId, Node::ID nodeId) {
    const auto it = _clusterWidgets.find(clusterId);
    assert(it != _clusterWidgets.end());
    it->second->updateConnectionStatus(nodeId);
}
