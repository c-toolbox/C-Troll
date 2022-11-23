/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
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
#include <json/json.hpp>

TEST_CASE("(Program) Default Ctor", "[Program]") {
    Program msg;


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) id", "[Program]") {
    Program msg;
    msg.id = 13;


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) name", "[Program]") {
    Program msg;
    msg.name = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) executable", "[Program]") {
    Program msg;
    msg.executable = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) commandlineParameters", "[Program]") {
    Program msg;
    msg.commandlineParameters = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) workingDirectory", "[Program]") {
    Program msg;
    msg.workingDirectory = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) shouldForwardMessages", "[Program]") {
    Program msg;
    msg.shouldForwardMessages = true;


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) delay", "[Program]") {
    Program msg;
    msg.delay = std::chrono::milliseconds(13);


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) tags", "[Program]") {
    Program msg;
    msg.tags.push_back("foo");
    msg.tags.push_back("bar");


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) description", "[Program]") {
    Program msg;
    msg.description = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) configurations", "[Program]") {
    Program msg;
    msg.configurations.push_back({
        Program::Configuration::ID(13), "foo", "bar", "baz"
    });


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program) clusters", "[Program]") {
    Program msg;
    msg.clusters.push_back("foo");
    msg.clusters.push_back("bar");


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}


//////////////////////////////////////////////////////////////////////////////////////////


TEST_CASE("(Program::Configuration) Default Ctor", "[Program::Configuration]") {
    Program msg;


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program::Configuration) id", "[Program::Configuration]") {
    Program msg;
    Program::Configuration c;
    c.id = 5;


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program::Configuration) name", "[Program::Configuration]") {
    Program msg;
    Program::Configuration c;
    c.name = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program::Configuration) parameters", "[Program::Configuration]") {
    Program msg;
    Program::Configuration c;
    c.parameters = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}

TEST_CASE("(Program::Configuration) description", "[Program::Configuration]") {
    Program msg;
    Program::Configuration c;
    c.description = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    Program msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    REQUIRE(j1 == j2);
}
