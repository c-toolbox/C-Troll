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

#include "restconnectionhandler.h"

#include "database.h"
#include "logging.h"
#include <QTcpSocket>
#include <fmt/format.h>
#include <optional>


RestConnectionHandler::RestConnectionHandler(QObject* parent, int port,
                                             std::string user, std::string password)
    : QObject(parent)
{
    Log("Status", fmt::format("REST API listening on port: {}", port));

    QByteArray auth = QString::fromStdString(user + ':' + password).toUtf8().toBase64();
    _secret = auth.toStdString();


    const bool success = _server.listen(QHostAddress::Any, static_cast<quint16>(port));
    if (!success) {
        Log("Error", fmt::format("Listening to REST API on port {} failed", port));
        return;
    }

    connect(
        &_server, &QTcpServer::newConnection,
        this, &RestConnectionHandler::newConnectionEstablished
    );
}

void RestConnectionHandler::newConnectionEstablished() {
    while (_server.hasPendingConnections()) {
        QTcpSocket* socket = _server.nextPendingConnection();

        connect(
            socket, &QTcpSocket::disconnected,
            socket, &QTcpSocket::deleteLater
        );

        connect(
            socket, &QTcpSocket::readyRead,
            this, &RestConnectionHandler::handleNewConnection
        );

        _sockets.push_back(socket);
    }
}

std::optional<std::string> endpoint(QStringList tokens) {
    if (tokens.size() >= 2) {
        return tokens[1].toStdString();
    }
    else {
        return std::nullopt;
    }
}

std::optional<std::string> authorization(QStringList tokens) {
    for (int i = 0; i < tokens.size() - 2; ++i) {
        std::string value = tokens[i].toStdString();
        if ((value == "Authorization:") && (tokens[i+1].toStdString() == "Basic")) {
            return tokens[i + 2].toStdString();
        }
    }
    return std::nullopt;
}

std::map<std::string, std::string> parameters(QStringList tokens) {
    std::map<std::string, std::string> res;
    QStringList parameters = tokens.back().split('&');
    for (int i = 0; i < parameters.size(); ++i) {
        QStringList kv = parameters[i].split('=');
        if (kv.size() == 2) {
            std::string key = kv[0].toStdString();
            std::string value = kv[1].toStdString();
            res[key] = value;
        }
    }
    return res;
}

void RestConnectionHandler::handleNewConnection() {
    QTcpSocket* socket = dynamic_cast<QTcpSocket*>(QObject::sender());
    assert(socket);
    assert(std::find(_sockets.begin(), _sockets.end(), socket) != _sockets.end());

    QStringList tokens = QString(socket->readAll()).split(QRegExp("[ \r\n][ \r\n]*"));
    qDebug() << tokens;
    if (tokens.size() < 5 || tokens[0] != "POST") {
        qDebug() << "1";
        socket->close();
        return;
    }

    std::optional<std::string> authValue = authorization(tokens);
    std::optional<std::string> endpointValue = endpoint(tokens);
    if (!authValue.has_value() || !endpointValue.has_value()) {
        qDebug() << "2" << authValue.has_value() << ' ' << endpointValue.has_value();
        socket->close();
        return;
    }

    if (*authValue != _secret) {
        qDebug() << "3";
        socket->close();
        return;
    }

    std::map<std::string, std::string> params = parameters(tokens);
    if (params.find("program") == params.end() ||
        params.find("configuration") == params.end() ||
        params.find("cluster") == params.end())
    {
        qDebug() << "4";
        socket->close();
        return;
    }

    std::string clusterName = params["cluster"];
    Cluster* cluster = data::findCluster(clusterName);
    if (cluster == nullptr) {
        qDebug() << "5";
        socket->close();
        return;
    }

    std::string programName = params["program"];
    Program* program = data::findProgram(programName);
    if (program == nullptr) {
        qDebug() << "6";
        socket->close();
        return;
    }

    std::string configurationName = params["configuration"];
    const Program::Configuration& configuration = data::findConfigurationForProgram(
        *program,
        configurationName
    );
    //if (configuration == nullptr) {
    //    qDebug() << "7";
    //    socket->close();
    //    return;
    //}

    if (endpointValue == "/start") {
        Log(
            "REST",
            fmt::format(
                "Received command to start {} ({}) on {}",
                program->name, configuration.name, cluster->name
            )
        );
        emit startProgram(cluster->id, program->id, configuration.id);
    }
    else if (endpointValue == "/stop") {
        Log(
            "REST",
            fmt::format(
                "Received command to stop {} ({}) on {}",
                program->name, configuration.name, cluster->name
            )
        );
        emit stopProgram(cluster->id, program->id, configuration.id);
    }
    else {
        qDebug() << "7";
        socket->close();
        return;
    }

    //socket->write(res.c_str());
    socket->close();
}
