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
#include <QLabel>
#include <QVBoxLayout>

ClusterWidget::ClusterWidget(std::string clusterId)
    : _clusterId(clusterId)
{
    Cluster* cluster = data::findCluster(clusterId);
    assert(cluster);

    QBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    QLabel* clusterName = new QLabel(QString::fromStdString(cluster->name));
    layout->addWidget(clusterName);

    _connectionLabel = new QLabel("disconnected");
    layout->addWidget(_connectionLabel);

    for (const std::unique_ptr<Cluster::Node>& n : cluster->nodes) {
        QWidget* node = new QWidget;
        layout->addWidget(node);
        QBoxLayout* nodeLayout = new QVBoxLayout;
        node->setLayout(nodeLayout);

        QLabel* name = new QLabel(QString::fromStdString(n->name));
        nodeLayout->addWidget(name);

        QLabel* ip = new QLabel(QString::fromStdString(n->ipAddress));
        nodeLayout->addWidget(ip);

        QString text = n->isConnected ? "connected" : "disconnected";
        QLabel* connected = new QLabel(text);
        _nodeConnectionLabels.push_back(connected);
        nodeLayout->addWidget(connected);
    }
}

void ClusterWidget::updateConnectionStatus(const std::string& node) {
    Cluster* cluster = data::findCluster(_clusterId);
    assert(cluster);

    for (size_t i = 0; i < cluster->nodes.size(); ++i) {
        const Cluster::Node& it = *cluster->nodes[i];
        if (it.id == node) {
            _nodeConnectionLabels[i]->setText(
                it.isConnected ? "connected" : "disconnected"
            );
        }
    }

    const bool allConnected = std::all_of(
        cluster->nodes.begin(), cluster->nodes.end(),
        std::mem_fn(&Cluster::Node::isConnected)
    );
    _connectionLabel->setText(allConnected ? "connected" : "disconnected");
}


//////////////////////////////////////////////////////////////////////////////////////////


ClustersWidget::ClustersWidget() {
    QBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    for (Cluster* c : data::clusters()) {
        ClusterWidget* widget = new ClusterWidget(c->id);
        _clusterWidgets[c->id] = widget;
        layout->addWidget(widget);
    }
}

void ClustersWidget::connectedStatusChanged(const std::string& cluster,
                                            const std::string& node)
{
    const auto it = _clusterWidgets.find(cluster);
    assert(it != _clusterWidgets.end());
    it->second->updateConnectionStatus(node);
}
