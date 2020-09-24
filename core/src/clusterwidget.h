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

#ifndef __CORE__CLUSTERWIDGET_H__
#define __CORE__CLUSTERWIDGET_H__

#include <QGroupBox>
#include <QWidget>

#include "cluster.h"
#include <map>

class QLabel;

class ConnectionWidget : public QWidget {
Q_OBJECT
public:
    enum class ConnectionStatus {
        Connected = 0,
        PartiallyConnected,
        Disconnected
    };

    ConnectionWidget();

    void setStatus(ConnectionStatus status);
};


//////////////////////////////////////////////////////////////////////////////////////////


class NodeWidget : public QGroupBox {
Q_OBJECT
public:
    NodeWidget(const Node& node);

    void updateConnectionStatus();

private:
    const Node::ID _nodeId;

    ConnectionWidget* _connectionLabel;
};


//////////////////////////////////////////////////////////////////////////////////////////


class ClusterWidget : public QGroupBox {
Q_OBJECT
public:
    ClusterWidget(const Cluster& cluster);

    void updateConnectionStatus(Node::ID nodeId);

private:
    const Cluster::ID _clusterId;

    ConnectionWidget* _connectionLabel;
    std::map<Node::ID, NodeWidget*> _nodeWidgets;
};


//////////////////////////////////////////////////////////////////////////////////////////


class ClustersWidget : public QWidget {
    Q_OBJECT
public:
    ClustersWidget();

public slots:
    void connectedStatusChanged(Cluster::ID clusterId, Node::ID nodeId);

private:
    std::map<Cluster::ID, ClusterWidget*> _clusterWidgets;
};

#endif // __CORE__CLUSTERWIDGET_H__
