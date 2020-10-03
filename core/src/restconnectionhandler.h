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
#include "program.h"
#include <QTcpServer>
#include <QTcpSocket>

class RestConnectionHandler : public QObject {
Q_OBJECT
public:
    RestConnectionHandler(QObject* parent, int port, std::string user,
        std::string password);

signals:
    void startProgram(Cluster::ID clusterId, Program::ID programId,
        Program::Configuration::ID configurationId);
    void stopProgram(Cluster::ID clusterId, Program::ID programId,
        Program::Configuration::ID configurationId);

private slots:
    void newConnectionEstablished();
    void handleNewConnection();

private:
    enum class HttpMethod {
        Get = 0,
        Post,
        Unknown
    };

    enum class Endpoint {
        Start = 0,
        Stop,
        Unknown
    };

    HttpMethod parseMethod(std::string_view value);
    Endpoint parseEndpoint(std::string_view value);

    void handleMessage(QTcpSocket& socket, HttpMethod method, Endpoint endpoint,
        const std::map<std::string, std::string>& params);

    void handleStartMessage(QTcpSocket& socket, std::string_view cluster,
        std::string_view program, std::string_view configuration);
    void handleStopMessage(QTcpSocket& socket, std::string_view cluster,
        std::string_view program, std::string_view configuration);

    QTcpServer _server;

    std::vector<QTcpSocket*> _sockets;
    std::string _secret;
};

#endif // __CORE_RESTCONNECTIONHANDLER_H__
