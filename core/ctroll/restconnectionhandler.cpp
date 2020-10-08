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
#include <json/json.hpp>
#include <optional>
#include <string_view>
#include <tuple>

namespace {
    enum class Response {
        Ok,
        BadRequest,
        Unauthorized,
        Forbidden,
        NotFound
    };

    enum class HttpMethod {
        Get,
        Post,
        Unknown
    };

    enum class Endpoint {
        StartProgram,
        StopProgram,
        InfoApi,
        InfoCluster,
        InfoProgram,
        InfoNode,
        StartCustomProgram,
        Unknown
    };

    void sendResponse(QTcpSocket& socket, Response response,
                      nlohmann::json payload = nlohmann::json())
    {
        std::string_view code = [](Response response) {
            switch (response) {
                case Response::Ok: return "200 OK";
                case Response::BadRequest: return "400 Bad Request";
                case Response::Unauthorized: return "401 Unauthorized";
                case Response::Forbidden: return "403 Forbidden";
                case Response::NotFound: return "404 Not Found";
                default: throw std::logic_error("Unhandled case label");
            }
        }(response);

        std::string status = fmt::format("HTTP/1.1 {}\n", code);
        if (payload.empty()) {
            socket.write(status.data(), status.size());
        }
        else {
            std::string content = payload.dump();
            std::string message = fmt::format(
                "{}Content-Type: application/json\nContent-Length: {}\n\n{}",
                status, content.size(), content
            );
            socket.write(message.data(), message.size());
        }
        socket.close();
    }


    HttpMethod parseMethod(std::string_view value) {
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

    Endpoint parseEndpoint(std::string_view value) {
        if ((value == "/program/start") || (value == "/program/start/")) {
            return Endpoint::StartProgram;
        }
        else if ((value == "/program/stop") || (value == "/program/stop/")) {
            return Endpoint::StopProgram;
        }
        else if ((value == "/program/custom") || (value == "/program/custom/")) {
            return Endpoint::StartCustomProgram;
        }
        else if ((value == "/program") || (value == "/program/")) {
            return Endpoint::InfoProgram;
        }
        else if ((value == "/cluster") || (value == "/cluster/")) {
            return Endpoint::InfoCluster;
        }
        else if ((value == "/node") || (value == "/node/")) {
            return Endpoint::InfoNode;
        }
        else if ((value == "/api") || (value == "/api/")) {
            return Endpoint::InfoApi;
        }
        else {
            return Endpoint::Unknown;
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

        const bool hasCluster = params.find(KeyCluster) != params.end();
        const bool hasProgram = params.find(KeyProgram) != params.end();
        const bool hasConfiguration = params.find(KeyConfiguration) != params.end();
        if (!hasCluster || !hasProgram || !hasConfiguration) {
            return { nullptr, nullptr, nullptr };
        }

        const std::string cluster = params.at(KeyCluster);
        const std::string program = params.at(KeyProgram);
        const std::string configuration = params.at(KeyConfiguration);

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
                                             std::string user, std::string password,
                                             bool provideCustomProgramAPI)
    : QObject(parent)
    , _hasCustomProgramAPI(provideCustomProgramAPI)
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
    Endpoint endpoint = endpointValue.has_value() ?
        parseEndpoint(*endpointValue) :
        Endpoint::Unknown;

    const bool hasMethod = method != HttpMethod::Unknown;
    const bool hasEndpoint = endpoint != Endpoint::Unknown;
    if (method == HttpMethod::Unknown) {
        sendResponse(*socket, Response::BadRequest);
        return;
    }

    if (endpoint == Endpoint::Unknown) {
        sendResponse(*socket, Response::NotFound);
        return;
    }

    std::map<std::string, std::string> params = parameters(tokens);

    //
    // Handle message
    if (method == HttpMethod::Post && endpoint == Endpoint::StartProgram) {
        ProgramInfo pi = extractProgramInfo(params);
        if (!pi) {
            sendResponse(*socket, Response::BadRequest);
            return;
        }

        handleStartProgramMessage(*socket, *pi.cluster, *pi.program, *pi.configuration);
    }
    else if (method == HttpMethod::Post && endpoint == Endpoint::StopProgram) {
        ProgramInfo pi = extractProgramInfo(params);
        if (!pi) {
            sendResponse(*socket, Response::BadRequest);
            return;
        }

        handleStopProgramMessage(*socket, *pi.cluster, *pi.program, *pi.configuration);
    }
    else if (method == HttpMethod::Post && endpoint == Endpoint::StartCustomProgram) {
        if (!_hasCustomProgramAPI) {
            sendResponse(*socket, Response::Forbidden);
            return;
        }

        constexpr const char* KeyCluster = "cluster";
        constexpr const char* KeyNode = "node";
        constexpr const char* KeyExecutable = "executable";
        constexpr const char* KeyWorkingDir = "workingDir";
        constexpr const char* KeyArguments = "arguments";


        const bool hasCluster = params.find(KeyCluster) != params.end();
        const bool hasNode = params.find(KeyNode) != params.end();
        const bool hasExecutable = params.find(KeyExecutable) != params.end();
        const bool hasWorkingDir = params.find(KeyWorkingDir) != params.end();
        const bool hasArguments = params.find(KeyArguments) != params.end();

        if (!(hasCluster || hasNode) || !hasExecutable) {
            sendResponse(*socket, Response::BadRequest);
            return;
        }

        const std::string exec = params[KeyExecutable];
        const std::string workingDir = hasWorkingDir ? params[KeyWorkingDir] : "";
        const std::string arguments = KeyArguments ? params[KeyArguments] : "";


        if (hasCluster) {
            const Cluster* c = data::findCluster(params[KeyCluster]);
            if (!c) {
                sendResponse(*socket, Response::BadRequest);
                return;
            }

            handleStartCustomProgramMessage(*socket, c, exec, workingDir, arguments);
        }
        else if (hasNode) {
            const Node* n = data::findNode(params[KeyNode]);
            if (!n) {
                sendResponse(*socket, Response::BadRequest);
                return;
            }

            handleStartCustomProgramMessage(*socket, n, exec, workingDir, arguments);
        }
        else {
            throw std::logic_error("Shouldn't get here");
        }
    }
    else if (method == HttpMethod::Get && endpoint == Endpoint::InfoProgram) {
        handleProgramInfoMessage(*socket);
    }
    else if (method == HttpMethod::Get && endpoint == Endpoint::InfoCluster) {
        handleClusterInfoMessage(*socket);
    }
    else if (method == HttpMethod::Get && endpoint == Endpoint::InfoNode) {
        handleNodeInfoMessage(*socket);
    }
    else if (method == HttpMethod::Get && endpoint == Endpoint::InfoApi) {
        handleApiInfoMessage(*socket);
    }
    else {
        sendResponse(*socket, Response::BadRequest);
    }
}

void RestConnectionHandler::handleStartProgramMessage(QTcpSocket& socket,
                                                      const Cluster& cluster,
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

void RestConnectionHandler::handleStopProgramMessage(QTcpSocket& socket,
                                                     const Cluster& cluster,
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

void RestConnectionHandler::handleStartCustomProgramMessage(QTcpSocket& socket,
                                         std::variant<const Cluster*, const Node*> target,
                                                                   std::string executable,
                                                                   std::string workingDir,
                                                                    std::string arguments)
{
    if (std::holds_alternative<const Cluster*>(target)) {
        const Cluster* cluster = std::get<const Cluster*>(target);
        assert(cluster);

        Log(
            "REST",
            fmt::format(
                "Received command to start {} {} in {} on cluster {}",
                executable, arguments, workingDir, cluster->name
            )
        );

        for (const std::string& node : cluster->nodes) {
            const Node* n = data::findNode(node);
            assert(n);

            emit startCustomProgram(n->id, executable, workingDir, arguments);
        }
    }
    else {
        const Node* node = std::get<const Node*>(target); 
        assert(node);

        Log(
            "REST",
            fmt::format(
                "Received command to start {} {} in {} on node {}",
                executable, arguments, workingDir, node->name
            )
        );

        emit startCustomProgram(node->id, executable, workingDir, arguments);
    }
    sendResponse(socket, Response::Ok);
}

void RestConnectionHandler::handleProgramInfoMessage(QTcpSocket& socket) {
    std::vector<const Program*> programs = data::programs();
    nlohmann::json result = nlohmann::json::array();
    for (const Program* program : programs) {
        nlohmann::json p;
        p["name"] = program->name;
        p["tags"] = program->tags;
        p["configurations"] = nlohmann::json::array();
        for (const Program::Configuration& conf : program->configurations) {
            p["configurations"].push_back(conf.name);
        }
        p["clusters"] = nlohmann::json::array();
        for (const std::string& clusterName : program->clusters) {
            const Cluster* cluster = data::findCluster(clusterName);
            p["clusters"].push_back(cluster->name);
        }
        result.push_back(p);
    }

    sendResponse(socket, Response::Ok, result);
}

void RestConnectionHandler::handleClusterInfoMessage(QTcpSocket& socket) {
    std::vector<const Cluster*> clusters = data::clusters();
    nlohmann::json result = nlohmann::json::array();
    for (const Cluster* cluster : clusters) {
        if (cluster->isEnabled) {
            nlohmann::json c;
            c["name"] = cluster->name;
            c["nodes"] = nlohmann::json::array();
            bool allConnected = true;
            for (const std::string& nodeName : cluster->nodes) {
                const Node* node = data::findNode(nodeName);
                c["nodes"].push_back(nodeName);
                allConnected &= node->isConnected;
            }
            c["allConnected"] = allConnected;
            result.push_back(c);
        }
    }

    sendResponse(socket, Response::Ok, result);
}

void RestConnectionHandler::handleNodeInfoMessage(QTcpSocket& socket) {
    std::vector<const Node*> nodes = data::nodes();
    nlohmann::json result = nlohmann::json::array();
    for (const Node* node : nodes) {
        nlohmann::json n;
        n["name"] = node->name;
        n["isConnected"] = node->isConnected;
        result.push_back(n);
    }

    sendResponse(socket, Response::Ok, result);
}

void RestConnectionHandler::handleApiInfoMessage(QTcpSocket& socket) {
    nlohmann::json result;
    result["endpoints"] = nlohmann::json::array();

    result["endpoints"].push_back({
        { "url", "/program/start" },
        { "description", "Starts already registered programs" },
        { "parameters", {
            { "program", "Name of the program to start" },
            { "configuration", "Which configuration of the program should be started " },
            { "cluster", "On which cluster should the program be started"}
        }}
    });

    result["endpoints"].push_back({
        { "url", "/program/stop" },
        { "description", "Stops already registered programs" },
        { "parameters", {
            { "program", "Name of the program to start" },
            { "configuration", "Which configuration of the program should be started " },
            { "cluster", "On which cluster should the program be started"}
        }}
    });

    if (_hasCustomProgramAPI) {
        result["endpoints"].push_back({
            { "url", "/program/custom" },
            { "description", "Starts an arbitrary executable on a node or a cluster" },
            { "parameters", {
                {
                    "cluster",
                    "The name of the cluster on which to start the executable, this "
                    "parameter and the 'nodes' parameter are mutually exclusive and "
                    "cannot be used at the same time"
                },
                {
                    "node",
                    "The name of the node on which to start the executable, this "
                    "parameter and the 'cluster' parameter are mutually exclusive and "
                    "cannot be used at the same time"
                },
                { "executable", "The name of the executable to start" },
                { "workingDir", "The working directory where to start the executable "},
                { "arguments", "Additional commandline arguments to pass to the program "}
            }}
        });
    }

    result["endpoints"].push_back({
        { "url", "/program" },
        { "description", "Gets information about the available programs" }
    });

    result["endpoints"].push_back({
        { "url", "/cluster" },
        { "description", "Gets information about the available clusters" }
    });

    result["endpoints"].push_back({
        { "url", "/node" },
        { "description", "Gets information about the available nodes" }
    });

    sendResponse(socket, Response::Ok, result);
}
