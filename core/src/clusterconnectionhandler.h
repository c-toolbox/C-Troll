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

#ifndef __CORE__CLUSTERCONNECTIONHANDLER_H__
#define __CORE__CLUSTERCONNECTIONHANDLER_H__

#include <QObject>

#include "cluster.h"
#include "node.h"
#include "processstatusmessage.h"
#include <jsonsocket.h>
#include <QMap>
#include <map>
#include <memory>

class ClusterConnectionHandler : public QObject {
Q_OBJECT
public:
    ~ClusterConnectionHandler();

    void initialize();

    void sendMessage(const Cluster& cluster, const Node& node,
        nlohmann::json message) const;

signals:
    void messageReceived(int clusterId, int nodeId, nlohmann::json message);

    void connectedStatusChanged(int clusterId, int nodeId);

    void receivedTrayProcess(common::ProcessStatusMessage status);

private slots:
    void handleSocketStateChange(int nodeId, QAbstractSocket::SocketState state);
    void readyRead(int nodeId);

private:
    std::map<int, std::unique_ptr<common::JsonSocket>> _sockets;

    //struct NodeInfo {
    //    Cluster* cluster;
    //    Node* node;
    //};

    //struct SocketData {
    //    std::unique_ptr<common::JsonSocket> socket;
    //    std::vector<NodeInfo> nodes;
    //};
    //std::map<std::string, SocketData> _nodes;
};

#endif // __CORE__CLUSTERCONNECTIONHANDLER_H__
