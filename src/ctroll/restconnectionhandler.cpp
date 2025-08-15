/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2025                                                               *
 * Alexander Bock                                                                        *
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
#include <QRegularExpression>
#include <QTcpSocket>
#include <nlohmann/json.hpp>
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

    void Debug(std::string msg) {
        ::Debug("RestConnectionHandler", std::move(msg));
    }

    void Log(std::string msg) {
        ::Log("REST", std::move(msg));
    }

    void sendResponse(QTcpSocket& socket, Response response, std::string content) {
        if (content.empty()) {
            return;
        }

        Debug(content);

        std::string_view code = [](Response resp) {
            switch (resp) {
                case Response::Ok:           return "200 OK";
                case Response::BadRequest:   return "400 Bad Request";
                case Response::Unauthorized: return "401 Unauthorized";
                case Response::Forbidden:    return "403 Forbidden";
                case Response::NotFound:     return "404 Not Found";
            }
            throw std::logic_error("Unhandled case label");
        }(response);

        std::string message = std::format(
            "HTTP/1.1 {}\nContent-Length: {}\n\n{}", code, content.size(), content
        );
        socket.write(message.data(), static_cast<qint64>(message.size()));
    }

    void sendJSONResponse(QTcpSocket& socket, Response response, nlohmann::json payload) {
        if (payload.empty()) {
            return;
        }

        std::string_view code = [](Response resp) {
            switch (resp) {
                case Response::Ok:           return "200 OK";
                case Response::BadRequest:   return "400 Bad Request";
                case Response::Unauthorized: return "401 Unauthorized";
                case Response::Forbidden:    return "403 Forbidden";
                case Response::NotFound:     return "404 Not Found";
            }
            throw std::logic_error("Unhandled case label");
        }(response);

        std::string content = payload.dump();
        std::string message = std::format(
            "HTTP/1.1 {}\nContent-Type: application/json\nContent-Length: {}\n\n{}",
            code, content.size(), content
        );
        socket.write(message.data(), static_cast<qint64>(message.size()));
    }

    constexpr HttpMethod parseMethod(std::string_view value) {
        if (value == "POST")     { return HttpMethod::Post;    }
        else if (value == "GET") { return HttpMethod::Get;     }
        else                     { return HttpMethod::Unknown; }
    }

    Endpoint parseEndpoint(std::string_view value) {
        if (!value.empty() && value.back() == '/') {
            value.remove_suffix(1);
        }

        if (value == "/program/start")       { return Endpoint::StartProgram;       }
        else if (value == "/program/stop")   { return Endpoint::StopProgram;        }
        else if (value == "/program/custom") { return Endpoint::StartCustomProgram; }
        else if (value == "/program")        { return Endpoint::InfoProgram;        }
        else if (value == "/cluster")        { return Endpoint::InfoCluster;        }
        else if (value == "/node")           { return Endpoint::InfoNode;           }
        else if (value == "/api")            { return Endpoint::InfoApi;            }
        else                                 { return Endpoint::Unknown;            }
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
        for (int i = 0; i < tokens.size() - 2; i++) {
            std::string value = tokens[i].toStdString();
            if ((value == "Authorization:") && (tokens[i + 1].toStdString() == "Basic")) {
                return tokens[i + 2].toStdString();
            }
        }
        return std::nullopt;
    }

    std::map<std::string, std::string> parameters(QStringList tokens) {
        std::map<std::string, std::string> res;
        QStringList parameters = tokens.back().split('&');
        for (int i = 0; i < parameters.size(); i++) {
            QStringList kv = parameters[i].split('=');
            if (kv.size() == 2) {
                std::string key = kv[0].toStdString();
                std::string value = kv[1].toStdString();
                res[key] = value;
            }
        }
        return res;
    }

    struct ProgramInfo {
        const Cluster* cluster = nullptr;
        const Program* program = nullptr;
        const Program::Configuration* configuration = nullptr;
    };

    ProgramInfo extractProgramInfo(const std::map<std::string, std::string>& params) {
        constexpr const char* KeyCluster = "cluster";
        constexpr const char* KeyProgram = "program";
        constexpr const char* KeyConfiguration = "configuration";

        const bool hasCluster = params.contains(KeyCluster);
        const bool hasProgram = params.contains(KeyProgram);
        const bool hasConfiguration = params.contains(KeyConfiguration);
        if (!hasCluster || !hasProgram || !hasConfiguration) {
            return { nullptr, nullptr, nullptr };
        }

        const std::string& cluster = params.at(KeyCluster);
        const std::string& program = params.at(KeyProgram);
        const std::string& configuration = params.at(KeyConfiguration);

        const Cluster* c = data::findCluster(cluster);
        const Program* p = data::findProgram(program);
        const Program::Configuration* conf =
            p != nullptr ? data::findConfigurationForProgram(*p, configuration) : nullptr;
        return { c, p, conf };
    }
} // namespace

RestConnectionHandler::RestConnectionHandler(QObject* parent, int port,
                                             bool acceptOnlyLoopbackConnection,
                                             std::string user, std::string password,
                                             bool provideCustomProgramAPI)
    : QObject(parent)
    , _hasCustomProgramAPI(provideCustomProgramAPI)
    , _acceptOnlyLoopbackConnection(acceptOnlyLoopbackConnection)
    , _secret(
        (!user.empty() && !password.empty()) ?
        QString::fromStdString(user + ':' + password).toUtf8().toBase64().toStdString() :
        ""
    )
{
    Log("Status", std::format("REST API listening on port: {}", port));

    const bool success = _server.listen(QHostAddress::Any, static_cast<quint16>(port));
    if (!success) {
        Log("Error", std::format("Listening to REST API on port {} failed", port));
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
        Debug(std::format(
            "New connection from {}", socket->peerAddress().toString().toStdString()
        ));

        connect(
            socket, &QTcpSocket::readyRead,
            this, &RestConnectionHandler::handleNewConnection
        );
    }
}

void RestConnectionHandler::handleNewConnection() {
    QTcpSocket* socket = dynamic_cast<QTcpSocket*>(QObject::sender());
    assert(socket);
    Debug(std::format(
        "Handling new message from {}", socket->peerAddress().toString().toStdString()
    ));

    if (_acceptOnlyLoopbackConnection && !socket->peerAddress().isLoopback()) {
        Debug("Rejecting due to not from a loopback");
        return;
    }

    QString content = socket->readAll();
    QStringList tokens = content.split(QRegularExpression("[ \r\n][ \r\n]*"));
    if (tokens.empty()) {
        sendResponse(*socket, Response::BadRequest, "Rejecting due to no payload");
        return;
    }

    std::optional<std::string> authValue = authorization(tokens);
    const bool wantsAuth = !_secret.empty();
    const bool hasAuth = authValue.has_value();
    const bool authCorrect = hasAuth && (*authValue == _secret);
    if (wantsAuth && (!(hasAuth && authCorrect))) {
        // We only want to check if we actually want authorization. If so we only want to
        // proceed if we have authorization and if it is correct
        sendResponse(
            *socket,
            Response::Unauthorized,
            "Rejecting due to bad authorization"
        );
        return;
    }

    HttpMethod method = parseMethod(tokens[0].toStdString());
    if (method == HttpMethod::Unknown) {
        sendResponse(
            *socket,
            Response::BadRequest,
            "Rejecting due to unknown HTTP method"
        );
        return;
    }

    Endpoint endPoint = Endpoint::Unknown;
    std::optional<std::string> endpointValue = endpoint(tokens);
    std::map<std::string, std::string> params;
    if (method == HttpMethod::Get && endpointValue.has_value()) {
        QString endPointStr = QString::fromStdString(*endpointValue);
        QStringList endPointList = endPointStr.split('?');
        endPoint = parseEndpoint(endPointList[0].toStdString());
        if (endPointList.size() > 1) {
            endPointList.pop_front();
            params = parameters(endPointList);
        }
    }
    else if (method == HttpMethod::Post) {
        params = parameters(tokens);
        if (endpointValue.has_value()) {
            endPoint = parseEndpoint(*endpointValue);
        }
    }

    if (endPoint == Endpoint::Unknown) {
        sendResponse(*socket, Response::NotFound, "No endpoint found");
        return;
    }

    //
    // Handle message
    if (endPoint == Endpoint::StartProgram) {
        ProgramInfo pi = extractProgramInfo(params);
        if (!pi.cluster) {
            sendResponse(*socket, Response::BadRequest, "Cluster not found");
            return;
        }
        if (!pi.program) {
            sendResponse(*socket, Response::BadRequest, "Program not found");
            return;
        }
        if (!pi.configuration) {
            sendResponse(*socket, Response::BadRequest, "Configuration not found");
            return;
        }

        handleStartProgramMessage(*socket, *pi.cluster, *pi.program, *pi.configuration);
    }
    else if (endPoint == Endpoint::StopProgram) {
        ProgramInfo pi = extractProgramInfo(params);
        if (!pi.cluster) {
            sendResponse(*socket, Response::BadRequest, "Cluster not found");
            return;
        }
        if (!pi.program) {
            sendResponse(*socket, Response::BadRequest, "Program not found");
            return;
        }
        if (!pi.configuration) {
            sendResponse(*socket, Response::BadRequest, "Configuration not found");
            return;
        }

        handleStopProgramMessage(*socket, *pi.cluster, *pi.program, *pi.configuration);
    }
    else if (endPoint == Endpoint::StartCustomProgram) {
        if (!_hasCustomProgramAPI) {
            sendResponse(*socket, Response::Forbidden, "No program found");
            return;
        }

        constexpr const char* KeyCluster = "cluster";
        constexpr const char* KeyNode = "node";
        constexpr const char* KeyExecutable = "executable";
        constexpr const char* KeyWorkingDir = "workingDir";
        constexpr const char* KeyArguments = "arguments";


        const bool hasCluster = params.contains(KeyCluster);
        const bool hasNode = params.contains(KeyNode);
        const bool hasExecutable = params.contains(KeyExecutable);
        const bool hasWorkingDir = params.contains(KeyWorkingDir);
        const bool hasArguments = params.contains(KeyArguments);

        if (!(hasCluster || hasNode) || !hasExecutable) {
            sendResponse(*socket, Response::BadRequest, "Missing parameters");
            return;
        }

        const std::string exec = params[KeyExecutable];
        const std::string workingDir = hasWorkingDir ? params[KeyWorkingDir] : "";
        const std::string arguments = hasArguments ? params[KeyArguments] : "";


        if (hasCluster) {
            const Cluster* c = data::findCluster(params[KeyCluster]);
            if (!c) {
                sendResponse(*socket, Response::BadRequest, "Could not find cluster");
                return;
            }

            handleStartCustomProgramMessage(*socket, *c, exec, workingDir, arguments);
        }
        else if (hasNode) {
            const Node* n = data::findNode(params[KeyNode]);
            if (!n) {
                sendResponse(*socket, Response::BadRequest, "Could not find node");
                return;
            }

            handleStartCustomProgramMessage(*socket, *n, exec, workingDir, arguments);
        }
        else {
            throw std::logic_error("Shouldn't get here");
        }
    }
    else if (endPoint == Endpoint::InfoProgram) {
        handleProgramInfoMessage(*socket);
    }
    else if (endPoint == Endpoint::InfoCluster) {
        handleClusterInfoMessage(*socket);
    }
    else if (endPoint == Endpoint::InfoNode) {
        handleNodeInfoMessage(*socket);
    }
    else if (endPoint == Endpoint::InfoApi) {
        handleApiInfoMessage(*socket);
    }
    else {
        sendResponse(*socket, Response::BadRequest, "No endpoint method found");
    }
}

void RestConnectionHandler::handleStartProgramMessage(QTcpSocket& socket,
                                                      const Cluster& cluster,
                                                      const Program& program,
                                              const Program::Configuration& configuration)
{
    std::string message = std::format(
        "Received command to start {} ({}) on {}",
        program.name, configuration.name, cluster.name
    );
    Log(message);

    emit startProgram(cluster.id, program.id, configuration.id);
    sendResponse(socket, Response::Ok, message);
}

void RestConnectionHandler::handleStopProgramMessage(QTcpSocket& socket,
                                                     const Cluster& cluster,
                                                     const Program& program,
                                              const Program::Configuration& configuration)
{
    std::string message = std::format(
        "Received command to stop {} ({}) on {}",
        program.name, configuration.name, cluster.name
    );
    Log(message);

    emit stopProgram(cluster.id, program.id, configuration.id);
    sendResponse(socket, Response::Ok, message);
}

void RestConnectionHandler::handleStartCustomProgramMessage(QTcpSocket& socket,
                                                            const Cluster& cluster,
                                                            std::string executable,
                                                            std::string workingDir,
                                                            std::string arguments)
{
    std::string message = std::format(
        "Received command to start {} {} in {} on cluster {}",
        executable, arguments, workingDir, cluster.name
    );
    Log(message);

    for (const std::string& node : cluster.nodes) {
        const Node* n = data::findNode(node);
        assert(n);

        emit startCustomProgram(n->id, executable, workingDir, arguments);
    }

    sendResponse(socket, Response::Ok, message);
}

void RestConnectionHandler::handleStartCustomProgramMessage(QTcpSocket& socket,
                                                            const Node& node,
                                                            std::string executable,
                                                            std::string workingDir,
                                                            std::string arguments)
{
    std::string message = std::format(
        "Received command to start {} {} in {} on node {}",
        executable, arguments, workingDir, node.name
    );
    Log(message);

    emit startCustomProgram(node.id, executable, workingDir, arguments);
    sendResponse(socket, Response::Ok, message);
}

void RestConnectionHandler::handleProgramInfoMessage(QTcpSocket& socket) {
    Debug("Received command to send programs info message");

    std::vector<const Program*> programs = data::programs();
    nlohmann::json result = nlohmann::json::array();
    for (const Program* program : programs) {
        assert(program);
        nlohmann::json p;
        p["name"] = program->name;
        p["tags"] = program->tags;
        p["configurations"] = nlohmann::json::array();
        for (const Program::Configuration& conf : program->configurations) {
            p["configurations"].push_back(conf.name);
        }
        p["clusters"] = nlohmann::json::array();
        for (const Program::Cluster& cluster : program->clusters) {
            const Cluster* c = data::findCluster(cluster.name);
            p["clusters"].push_back(c->name);

        }
        result.push_back(p);
    }

    sendJSONResponse(socket, Response::Ok, result);
}

void RestConnectionHandler::handleClusterInfoMessage(QTcpSocket& socket) {
    Debug("Received command to send clusters info message");

    std::vector<const Cluster*> clusters = data::clusters();
    nlohmann::json result = nlohmann::json::array();
    for (const Cluster* cluster : clusters) {
        assert(cluster);
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

    sendJSONResponse(socket, Response::Ok, result);
}

void RestConnectionHandler::handleNodeInfoMessage(QTcpSocket& socket) {
    Debug("Received command to send nodes info message");

    std::vector<const Node*> nodes = data::nodes();
    nlohmann::json result = nlohmann::json::array();
    for (const Node* node : nodes) {
        assert(node);
        nlohmann::json n;
        n["name"] = node->name;
        n["isConnected"] = node->isConnected;
        result.push_back(n);
    }

    sendJSONResponse(socket, Response::Ok, result);
}

void RestConnectionHandler::handleApiInfoMessage(QTcpSocket& socket) {
    Debug("Received command to send API info message");

    nlohmann::json result;
    result["endpoints"] = nlohmann::json::array();

    result["endpoints"].push_back({
        { "url", "/program/start" },
        { "description", "Starts already registered programs" },
        { "parameters", {
            { "program", "Name of the program to start" },
            { "configuration", "Which configuration of the program should be started" },
            { "cluster", "On which cluster should the program be started" }
        }}
    });

    result["endpoints"].push_back({
        { "url", "/program/stop" },
        { "description", "Stops already registered programs" },
        { "parameters", {
            { "program", "Name of the program to start" },
            { "configuration", "Which configuration of the program should be started" },
            { "cluster", "On which cluster should the program be started" }
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
                { "workingDir", "The working directory where to start the executable" },
                {
                    "arguments",
                    "Additional commandline arguments to pass to the program"
                }
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

    sendJSONResponse(socket, Response::Ok, result);
}
