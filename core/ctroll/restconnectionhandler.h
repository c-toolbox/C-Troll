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

#ifndef __CORE__RESTCONNECTIONHANDLER_H__
#define __CORE__RESTCONNECTIONHANDLER_H__

#include <QObject>

#include "cluster.h"
#include "node.h"
#include "program.h"
#include <QTcpServer>
#include <QTcpSocket>
#include <variant>

class RestConnectionHandler : public QObject {
Q_OBJECT
public:
    RestConnectionHandler(QObject* parent, int port, std::string user,
        std::string password, bool provideCustomProgramAPI);

signals:
    void startProgram(Cluster::ID clusterId, Program::ID programId,
        Program::Configuration::ID configurationId);
    void stopProgram(Cluster::ID clusterId, Program::ID programId,
        Program::Configuration::ID configurationId);
    void startCustomProgram(Node::ID nodeId, std::string executable,
        std::string workingDir, std::string arguments);

private slots:
    void newConnectionEstablished();
    void handleNewConnection();

private:
    void handleStartProgramMessage(QTcpSocket& socket, const Cluster& cluster,
        const Program& program, const Program::Configuration& configuration);
    void handleStopProgramMessage(QTcpSocket& socket, const Cluster& cluster,
        const Program& program, const Program::Configuration& configuration);
    void handleStartCustomProgramMessage(QTcpSocket& socket,
        std::variant<const Cluster*, const Node*> target, std::string executable,
        std::string workingDir, std::string arguments);
    void handleProgramInfoMessage(QTcpSocket& socket);
    void handleClusterInfoMessage(QTcpSocket& socket);
    void handleNodeInfoMessage(QTcpSocket& socket);
    void handleApiInfoMessage(QTcpSocket& socket);

    QTcpServer _server;
    std::vector<QTcpSocket*> _sockets;
    bool _hasCustomProgramAPI = false;
    std::string _secret;
};

#endif // __CORE_RESTCONNECTIONHANDLER_H__
