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

#include <QLabel>
#include <QVBoxLayout>

ClusterWidget::ClusterWidget(const Cluster& cluster)
    : _cluster(cluster)
{
    QBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    QLabel* clusterName = new QLabel(QString::fromStdString(cluster.name));
    layout->addWidget(clusterName);

    _connectionLabel = new QLabel("disconnected");
    layout->addWidget(_connectionLabel);

    for (const Cluster::Node& n : cluster.nodes) {
        QWidget* node = new QWidget;
        layout->addWidget(node);
        QBoxLayout* nodeLayout = new QVBoxLayout;
        node->setLayout(nodeLayout);

        QLabel* name = new QLabel(QString::fromStdString(n.name));
        nodeLayout->addWidget(name);

        QLabel* ip = new QLabel(QString::fromStdString(n.ipAddress));
        nodeLayout->addWidget(ip);

        QString text = n.isConnected ? "connected" : "disconnected";
        QLabel* connected = new QLabel(text);
        _nodeConnectionLabels.push_back(connected);
        nodeLayout->addWidget(connected);
    }
}

void ClusterWidget::updateConnectionStatus(const Cluster::Node& node) {
    for (size_t i = 0; i < _cluster.nodes.size(); ++i) {
        const Cluster::Node& it = _cluster.nodes[i];
        if (it.id == node.id) {
            _nodeConnectionLabels[i]->setText(
                it.isConnected ? "connected" : "disconnected"
            );
        }
    }

    const bool allConnected = std::all_of(
        _cluster.nodes.begin(), _cluster.nodes.end(),
        std::mem_fn(&Cluster::Node::isConnected)
    );
    _connectionLabel->setText(allConnected ? "connected" : "disconnected");
}


//////////////////////////////////////////////////////////////////////////////////////////


ClustersWidget::ClustersWidget(const std::vector<Cluster>& clusters)
    : _clusters(clusters)
{
    QBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    for (const Cluster& c : clusters) {
        ClusterWidget* widget = new ClusterWidget(c);
        _clusterWidgets.push_back(widget);
        layout->addWidget(widget);
    }
}

void ClustersWidget::connectedStatusChanged(const Cluster& cluster,
                                            const Cluster::Node& node)
{
    assert(_clusters.size() == _clusterWidgets.size());

    for (size_t i = 0; i < _clusters.size(); ++i) {
        const Cluster& it = _clusters[i];
        if (it.id == cluster.id) {
            _clusterWidgets[i]->updateConnectionStatus(node);
            return;
        }
    }

    assert(false);
}
