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
#include <tuple>

namespace {
    enum class Response {
        Ok = 0,
        BadRequest,
        Unauthorized,
        NotFound
    };

    void sendResponse(QTcpSocket& socket, Response response, std::string payload = "") {
        std::string_view code = [](Response response) {
            switch (response) {
                case Response::Ok: return "200 OK";
                case Response::BadRequest: return "400 Bad Request";
                case Response::Unauthorized: return "401 Unauthorized";
                default: throw std::logic_error("Unhandled case label");
            }
        }(response);

        std::string status = fmt::format("HTTP/1.1 {}\n", code);
        if (payload.empty()) {
            socket.write(status.data(), status.size());
        }
        else {
            std::string message = fmt::format("{}\n\n{}", status, payload);
            socket.write(message.data(), message.size());
        }
        socket.close();
    }


    RestConnectionHandler::HttpMethod parseMethod(std::string_view value) {
        if (value == "POST") {
            return RestConnectionHandler::HttpMethod::Post;
        }
        else if (value == "GET") {
            return RestConnectionHandler::HttpMethod::Get;
        }
        else {
            return RestConnectionHandler::HttpMethod::Unknown;
        }
    }

    RestConnectionHandler::Endpoint parseEndpoint( std::string_view value) {
        if (value == "/start") {
            return RestConnectionHandler::Endpoint::Start;
        }
        else if (value == "/stop") {
            return RestConnectionHandler::Endpoint::Stop;
        }
        else {
            return RestConnectionHandler::Endpoint::Unknown;
        }
    }

    struct ProgramInfo {
        const Cluster* cluster;
        const Program* program;
        const Program::Configuration* configuration;

        operator bool() const {
            return cluster && program && configuration;
        }
    };

    ProgramInfo extractProgramInfo(const std::map<std::string, std::string>& params) {
        constexpr const char* KeyProgram = "program";
        constexpr const char* KeyConfiguration = "configuration";
        constexpr const char* KeyCluster = "cluster";

        const bool hasCluster = params.find(KeyCluster) == params.end();
        const bool hasProgram = params.find(KeyProgram) == params.end();
        const bool hasConfiguration = params.find(KeyConfiguration) == params.end();
        if (hasCluster || hasProgram || hasConfiguration) {
            return { nullptr, nullptr, nullptr };
        }

        std::string cluster = params.at(KeyCluster);
        std::string program = params.at(KeyProgram);
        std::string configuration = params.at(KeyConfiguration);

        const Cluster* c = data::findCluster(cluster);
        const Program* p = data::findProgram(program);
        if ((c == nullptr) || (p == nullptr)) {
            return { c, p, nullptr };
        }
        const Program::Configuration* conf = data::findConfigurationForProgram(
            *p,
            configuration
        );

        return { c, p, conf };
    }

} // namespace

RestConnectionHandler::RestConnectionHandler(QObject* parent, int port,
                                             std::string user, std::string password)
    : QObject(parent)
{
    Log("Status", fmt::format("REST API listening on port: {}", port));

    if (!user.empty() && !password.empty()) {
        const std::string combined = user + ':' + password;
        _secret = QString::fromStdString(combined).toUtf8().toBase64().toStdString();
    }


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
        sendResponse(*socket, Response::BadRequest);
        return;
    }

    std::optional<std::string> authValue = authorization(tokens);
    const bool wantsAuth = !_secret.empty();
    const bool hasAuth = authValue.has_value();
    const bool authCorrect = hasAuth ? (*authValue == _secret) : false;
    if (wantsAuth && (!(hasAuth && authCorrect))) {
        // We only want to check if we actually want authorization. If so we only want to
        // proceed if we have authorization and if it is correct
        sendResponse(*socket, Response::Unauthorized);
        return;
    }

    std::optional<std::string> endpointValue = endpoint(tokens);
    HttpMethod method = parseMethod(tokens[0].toStdString());
    Endpoint endpoint = parseEndpoint(*endpointValue);

    const bool hasEndpoint = endpointValue.has_value();
    const bool hasMethod = method != HttpMethod::Unknown;
    const bool hasEndpoint = endpoint != Endpoint::Unknown;
    if (!hasEndpoint || !hasMethod || !hasEndpoint) {
        sendResponse(*socket, Response::BadRequest);
        return;
    }

    if (method == HttpMethod::Unknown) {
        sendResponse(*socket, Response::BadRequest);
        return;
    }

    if (endpoint == Endpoint::Unknown) {
        sendResponse(*socket, Response::NotFound);
        return;
    }

    std::map<std::string, std::string> params = parameters(tokens);
    handleMessage(*socket, method, endpoint, params);
}

void RestConnectionHandler::handleMessage(QTcpSocket& socket, HttpMethod method,
                                          Endpoint endpoint,
                                         const std::map<std::string, std::string>& params)
{
    if (method == HttpMethod::Post && endpoint == Endpoint::Start) {
        ProgramInfo pi = extractProgramInfo(params);
        if (!pi) {
            sendResponse(socket, Response::BadRequest);
            return;
        }

        handleStartMessage(socket, *pi.cluster, *pi.program, *pi.configuration);
    }
    else if (method == HttpMethod::Post && endpoint == Endpoint::Stop) {
        ProgramInfo pi = extractProgramInfo(params);
        if (!pi) {
            sendResponse(socket, Response::BadRequest);
            return;
        }

        handleStopMessage(socket, *pi.cluster, *pi.program, *pi.configuration);
    }
    else {
        sendResponse(socket, Response::BadRequest);
    }
}

void RestConnectionHandler::handleStartMessage(QTcpSocket& socket, const Cluster& cluster,
                                               const Program& program,
                                              const Program::Configuration& configuration)
{
    Log(
        "REST",
        fmt::format(
            "Received command to start {} ({}) on {}",
            program.name, configuration.name, cluster.name
        )
    );
    emit startProgram(cluster.id, program.id, configuration.id);
    sendResponse(socket, Response::Ok);
}

void RestConnectionHandler::handleStopMessage(QTcpSocket& socket, const Cluster& cluster,
                                              const Program& program,
                                              const Program::Configuration& configuration)
{
    Log(
        "REST",
        fmt::format(
            "Received command to stop {} ({}) on {}",
            program.name, configuration.name, cluster.name
        )
    );
    emit stopProgram(cluster.id, program.id, configuration.id);
    sendResponse(socket, Response::Ok);
}
