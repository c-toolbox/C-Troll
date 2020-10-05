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

#ifndef __CORE__CLUSTERCONNECTIONHANDLER_H__
#define __CORE__CLUSTERCONNECTIONHANDLER_H__

#include <QObject>

#include "cluster.h"
#include "invalidauthmessage.h"
#include "jsonsocket.h"
#include "node.h"
#include "processoutputmessage.h"
#include "processstatusmessage.h"
#include "traystatusmessage.h"
#include <QAbstractSocket>
#include <map>
#include <memory>

struct Cluster;
struct Node;

class ClusterConnectionHandler : public QObject {
Q_OBJECT
public:
    ~ClusterConnectionHandler();

    void initialize();
    void sendMessage(const Node& node, nlohmann::json message) const;

signals:
    void connectedStatusChanged(Cluster::ID clusterId, Node::ID nodeId);

    void receivedTrayProcess(common::ProcessStatusMessage status);
    void receivedTrayStatus(Node::ID id, common::TrayStatusMessage status);
    void receivedInvalidAuthStatus(Node::ID id, common::InvalidAuthMessage message);
    void receivedProcessMessage(Node::ID id, common::ProcessOutputMessage message);

private:
    void handleSocketStateChange(Node::ID nodeId, QAbstractSocket::SocketState state);
    void handleMessage(nlohmann::json message, Node::ID nodeId);
    
    std::map<Node::ID, std::unique_ptr<common::JsonSocket>> _sockets;
};

#endif // __CORE__CLUSTERCONNECTIONHANDLER_H__