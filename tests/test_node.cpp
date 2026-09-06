/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2026                                                               *
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

#include "catch2/catch_test_macros.hpp"

#include "node.h"
#include "version.h"
#include <nlohmann/json.hpp>

TEST_CASE("Node Default Ctor", "[Node]") {
    Node msg;


    nlohmann::json j1;
    to_json(j1, msg);

    Node msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Node.name", "[Node]") {
    Node msg;
    msg.name = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Node msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.name == "foobar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("(Node) ipAddress", "[Node]") {
    Node msg;
    msg.ipAddress = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Node msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.ipAddress == "foobar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("(Node) port", "[Node]") {
    Node msg;
    msg.port = 13;


    nlohmann::json j1;
    to_json(j1, msg);

    Node msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.port == 13);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("(Node) description", "[Node]") {
    Node msg;
    msg.description = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Node msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.description == "foobar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("(Node) version", "[Node]") {
    Node msg;

    nlohmann::json j;
    to_json(j, msg);
    CHECK(j.at("version").get<int>() == config::NodeFileVersion);
}

TEST_CASE("(Node) version missing", "[Node]") {
    using namespace nlohmann;

    // Files that were written before the 'version' tag was introduced have to keep
    // working and are treated as version 1
    const json conf = R"({ "name": "name", "ip": "localhost", "port": 5000 })"_json;

    Node node = conf;
    CHECK(node.name == "name");
}

TEST_CASE("(Node) version unsupported", "[Node]") {
    using namespace nlohmann;

    json conf = R"({ "name": "name", "ip": "localhost", "port": 5000 })"_json;
    conf["version"] = config::NodeFileVersion + 1;

    Node node;
    CHECK_THROWS_AS(::from_json(conf, node), std::runtime_error);
}
