/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2023                                                             *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
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

#include "program.h"
#include <nlohmann/json.hpp>

TEST_CASE("Program Default Ctor", "[Program]") {
    Program msg;


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program.name", "[Program]") {
    Program msg;
    msg.name = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.name == "foobar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program.executable", "[Program]") {
    Program msg;
    msg.executable = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.executable == "foobar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program.commandlineParameters", "[Program]") {
    Program msg;
    msg.commandlineParameters = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.commandlineParameters == "foobar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program.workingDirectory", "[Program]") {
    Program msg;
    msg.workingDirectory = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.workingDirectory == "foobar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program.shouldForwardMessages", "[Program]") {
    Program msg;
    msg.shouldForwardMessages = true;


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.shouldForwardMessages == true);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program.delay", "[Program]") {
    Program msg;
    msg.delay = std::chrono::milliseconds(13);


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.delay == std::chrono::milliseconds(13));

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program.tags", "[Program]") {
    Program msg;
    msg.tags.push_back("foo");
    msg.tags.push_back("bar");


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    REQUIRE(msgDeserialize.tags.size() == 2);
    CHECK(msgDeserialize.tags[0] == "foo");
    CHECK(msgDeserialize.tags[1] == "bar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program.description", "[Program]") {
    Program msg;
    msg.description = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.description == "foobar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program.configurations", "[Program]") {
    Program msg;
    msg.configurations.push_back({
        Program::Configuration::ID(13), "foo", "bar", "baz"
    });
    msg.configurations.push_back({
        Program::Configuration::ID(17), "fooz", "barz", "bazz"
    });


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    // IDs are not serialized but bork up the equality check here, so we patch manually
    REQUIRE(msgDeserialize.configurations.size() == 2);
    msgDeserialize.configurations[0].id = Program::Configuration::ID(13);
    msgDeserialize.configurations[1].id = Program::Configuration::ID(17);

    CHECK(msg == msgDeserialize);
    REQUIRE(msgDeserialize.configurations.size() == 2);
    CHECK(msgDeserialize.configurations[0].name == "foo");
    CHECK(msgDeserialize.configurations[0].parameters == "bar");
    CHECK(msgDeserialize.configurations[0].description == "baz");
    CHECK(msgDeserialize.configurations[1].name == "fooz");
    CHECK(msgDeserialize.configurations[1].parameters == "barz");
    CHECK(msgDeserialize.configurations[1].description == "bazz");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program.clusters", "[Program]") {
    Program msg;
    msg.name = "foobar";
    msg.executable = "foobar";
    msg.commandlineParameters = "foobar";
    msg.workingDirectory = "foobar";
    msg.shouldForwardMessages = true;
    msg.delay = std::chrono::milliseconds(13);
    msg.tags.push_back("foo");
    msg.tags.push_back("bar");
    msg.description = "foobar";
    msg.configurations.push_back({
        Program::Configuration::ID(13), "foo", "bar", "baz"
    });
    msg.configurations.push_back({
        Program::Configuration::ID(17), "fooz", "barz", "bazz"
    });
    msg.clusters.push_back({ "foo", "abc" });
    msg.clusters.push_back({ "bar", "" });


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    // IDs are not serialized but bork up the equality check here, so we patch manually
    REQUIRE(msgDeserialize.configurations.size() == 2);
    msgDeserialize.configurations[0].id = Program::Configuration::ID(13);
    msgDeserialize.configurations[1].id = Program::Configuration::ID(17);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.name == "foobar");
    CHECK(msgDeserialize.executable == "foobar");
    CHECK(msgDeserialize.commandlineParameters == "foobar");
    CHECK(msgDeserialize.workingDirectory == "foobar");
    CHECK(msgDeserialize.shouldForwardMessages == true);
    CHECK(msgDeserialize.delay == std::chrono::milliseconds(13));
    REQUIRE(msgDeserialize.tags.size() == 2);
    CHECK(msgDeserialize.tags[0] == "foo");
    CHECK(msgDeserialize.tags[1] == "bar");
    CHECK(msgDeserialize.description == "foobar");
    REQUIRE(msgDeserialize.configurations.size() == 2);
    CHECK(msgDeserialize.configurations[0].name == "foo");
    CHECK(msgDeserialize.configurations[0].parameters == "bar");
    CHECK(msgDeserialize.configurations[0].description == "baz");
    CHECK(msgDeserialize.configurations[1].name == "fooz");
    CHECK(msgDeserialize.configurations[1].parameters == "barz");
    CHECK(msgDeserialize.configurations[1].description == "bazz");
    REQUIRE(msgDeserialize.clusters.size() == 2);
    CHECK(msgDeserialize.clusters[0].name == "foo");
    CHECK(msgDeserialize.clusters[0].parameters == "abc");
    CHECK(msgDeserialize.clusters[1].name == "bar");
    CHECK(msgDeserialize.clusters[1].parameters == "");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("Program full", "[Program]") {
    Program msg;
    msg.clusters.push_back({ "foo", "abc" });
    msg.clusters.push_back({ "bar", "" });


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    REQUIRE(msgDeserialize.clusters.size() == 2);
    CHECK(msgDeserialize.clusters[0].name == "foo");
    CHECK(msgDeserialize.clusters[0].parameters == "abc");
    CHECK(msgDeserialize.clusters[1].name == "bar");
    CHECK(msgDeserialize.clusters[1].parameters == "");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}
