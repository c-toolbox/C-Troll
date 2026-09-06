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

#include "cluster.h"
#include "version.h"
#include <nlohmann/json.hpp>

TEST_CASE("Cluster Default Ctor", "[Cluster]") {
    Cluster msg;


    nlohmann::json j1;
    to_json(j1, msg);

    Cluster msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);


    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Cluster.name", "[Cluster]") {
    Cluster msg;
    msg.name = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Cluster msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.name == "foobar");


    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Cluster.isEnabled", "[Cluster]") {
    Cluster msg;
    msg.isEnabled = false;


    nlohmann::json j1;
    to_json(j1, msg);

    Cluster msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.isEnabled == false);


    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Cluster.description", "[Cluster]") {
    Cluster msg;
    msg.description = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Cluster msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.description == "foobar");


    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Cluster.nodes", "[Cluster]") {
    Cluster msg;
    msg.nodes.push_back("foo");
    msg.nodes.push_back("bar");


    nlohmann::json j1;
    to_json(j1, msg);

    Cluster msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    REQUIRE(msgDeserialize.nodes.size() == 2);
    CHECK(msgDeserialize.nodes[0] == "foo");
    CHECK(msgDeserialize.nodes[1] == "bar");


    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Cluster.version", "[Cluster]") {
    Cluster msg;

    nlohmann::json j;
    to_json(j, msg);
    CHECK(j.at("version").get<int>() == config::ClusterFileVersion);
}

TEST_CASE("Cluster.version missing", "[Cluster]") {
    using namespace nlohmann;

    // Files that were written before the 'version' tag was introduced have to keep
    // working and are treated as version 1
    const json conf = R"({ "name": "name", "nodes": [ "node" ] })"_json;

    Cluster cluster = conf;
    CHECK(cluster.name == "name");
}

TEST_CASE("Cluster.version unsupported", "[Cluster]") {
    using namespace nlohmann;

    json conf = R"({ "name": "name", "nodes": [ "node" ] })"_json;
    conf["version"] = config::ClusterFileVersion + 1;

    Cluster cluster;
    CHECK_THROWS_AS(::from_json(conf, cluster), std::runtime_error);
}
