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
#include <string_view>

namespace {
    void sendResponse(QTcpSocket& socket, std::string error, std::string payload = "") {
        std::string status = fmt::format("HTTP/1.1 {}\n", error);
        if (payload.empty()) {
            socket.write(status.data(), status.size());
        }
        else {
            std::string message = fmt::format("{}\n\n{}", status, payload);
            socket.write(message.data(), message.size());
        }
        socket.close();
    }
} // namespace

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
    if (tokens.empty()) {
        sendResponse(*socket, "400 Bad Request");
        return;
    }

    std::optional<std::string> authValue = authorization(tokens);
    std::optional<std::string> endpointValue = endpoint(tokens);
    if (!authValue.has_value() || !endpointValue.has_value()) {
        sendResponse(*socket, "400 Bad Request");
        return;
    }
    if (*authValue != _secret) {
        sendResponse(*socket, "401 Unauthorized");
        return;
    }

    HttpMethod method = parseMethod(tokens[0].toStdString());
    if (method == HttpMethod::Unknown) {
        sendResponse(*socket, "400 Bad Request");
        return;
    }

    Endpoint endpoint = parseEndpoint(*endpointValue);
    if (endpoint == Endpoint::Unknown) {
        sendResponse(*socket, "404 Not Found");
        return;
    }

    std::map<std::string, std::string> params = parameters(tokens);
    handleMessage(*socket, method, endpoint, params);
}


RestConnectionHandler::HttpMethod RestConnectionHandler::parseMethod(
                                                                   std::string_view value)
{
    if (value == "POST") {
        return HttpMethod::Post;
    }
    else if (value == "GET") {
        return HttpMethod::Get;
    }
    else {
        return HttpMethod::Unknown;
    }
}

RestConnectionHandler::Endpoint RestConnectionHandler::parseEndpoint(
                                                                   std::string_view value)
{
    if (value == "/start") {
        return Endpoint::Start;
    }
    else if (value == "/stop") {
        return Endpoint::Stop;
    }
    else {
        return Endpoint::Unknown;
    }
}


void RestConnectionHandler::handleMessage(QTcpSocket& socket, HttpMethod method,
                                          Endpoint endpoint,
                                         const std::map<std::string, std::string>& params)
{
    if (method == HttpMethod::Post && endpoint == Endpoint::Start) {
        if (params.find("program") == params.end() ||
            params.find("configuration") == params.end() ||
            params.find("cluster") == params.end())
        {
            sendResponse(socket, "400 Bad Request");
            return;
        }

        std::string cluster = params.at("cluster");
        std::string program = params.at("program");
        std::string configuration = params.at("configuration");
        handleStartMessage(socket, cluster, program, configuration);
    }
    else if (method == HttpMethod::Post && endpoint == Endpoint::Stop) {
        if (params.find("program") == params.end() ||
            params.find("configuration") == params.end() ||
            params.find("cluster") == params.end())
        {
            sendResponse(socket, "400 Bad Request");
            return;
        }

        std::string cluster = params.at("cluster");
        std::string program = params.at("program");
        std::string configuration = params.at("configuration");
        handleStopMessage(socket, cluster, program, configuration);
    }
    else {
        sendResponse(socket, "400 Bad Request");
    }
}

void RestConnectionHandler::handleStartMessage(QTcpSocket& socket,
                                               std::string_view cluster,
                                               std::string_view program,
                                               std::string_view configuration)
{
    const Cluster* c = data::findCluster(cluster);
    const Program* p = data::findProgram(program);
    if ((c == nullptr) || (p == nullptr)) {
        sendResponse(socket, "400 Bad Request");
        return;
    }
    const Program::Configuration* conf = data::findConfigurationForProgram(
        *p,
        configuration
    );
    if (conf == nullptr) {
        sendResponse(socket, "400 Bad Request");
        return;
    }

    Log(
        "REST",
        fmt::format(
            "Received command to start {} ({}) on {}", p->name, conf->name, c->name
        )
    );
    emit startProgram(c->id, p->id, conf->id);
    sendResponse(socket, "200 OK");
}

void RestConnectionHandler::handleStopMessage(QTcpSocket& socket,
                                              std::string_view cluster,
                                              std::string_view program,
                                              std::string_view configuration)
{
    const Cluster* c = data::findCluster(cluster);
    const Program* p = data::findProgram(program);
    if ((c == nullptr) || (p == nullptr)) {
        sendResponse(socket, "400 Bad Request");
        return;
    }
    const Program::Configuration* conf = data::findConfigurationForProgram(
        *p,
        configuration
    );
    if (conf == nullptr) {
        sendResponse(socket, "400 Bad Request");
        return;
    }

    Log(
        "REST",
        fmt::format(
            "Received command to stop {} ({}) on {}",
            p->name, conf->name, c->name
        )
    );
    emit stopProgram(c->id, p->id, conf->id);
    sendResponse(socket, "200 OK");
}
