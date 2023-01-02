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

TEST_CASE("Program Example: Calc", "[Program]") {
    using namespace nlohmann;

    json conf = R"(
{
  "clusters": [
    { "name": "Local" },
    { "name": "Local+Office" }
  ],
  "configurations": [
    {
      "name": "Default"
    }
  ],
  "enabled": true,
  "executable": "calc.exe",
  "name": "Calc",
  "shouldForwardMessages": false,
  "tags": [
    "test",
    "test2"
  ]
}
)"_json;

    Program program = conf;
    CHECK(program.name == "Calc");
    CHECK(program.executable == "calc.exe");
    CHECK(program.commandlineParameters.empty());
    CHECK(program.workingDirectory.empty());
    CHECK(program.shouldForwardMessages == false);
    CHECK(program.isEnabled == true);
    CHECK(program.delay.has_value() == false);
    CHECK(program.preStart.empty());
    REQUIRE(program.tags.size() == 2);
    CHECK(program.tags[0] == "test");
    CHECK(program.tags[1] == "test2");
    CHECK(program.description.empty());
    REQUIRE(program.configurations.size() == 1);
    CHECK(program.configurations[0].name == "Default");
    CHECK(program.configurations[0].parameters.empty());
    CHECK(program.configurations[0].description.empty());
    REQUIRE(program.clusters.size() == 2);
    CHECK(program.clusters[0].name == "Local");
    CHECK(program.clusters[0].parameters.empty());
    CHECK(program.clusters[1].name == "Local+Office");
    CHECK(program.clusters[1].parameters.empty());
}

TEST_CASE("Program Example: Calibrator", "[Program]") {
    using namespace nlohmann;

    json conf = R"(
{
  "name": "Calibrator",
  "executable": "A:/Calibrator/calibrator.exe",
  "workingDirectory": "A:/Calibrator",
  "tags": [ "test" ],
  "configurations": [
    {
      "name": "Default",
      "parameters": ""
    },
    {
      "name": "Fancy",
      "parameters": "-foo bar",
      "description": "A more fancy configuration"
    }
  ],
  "clusters": [
    { "name": "Local" },
    { "name": "Local+Office" },
    { "name": "Local+Home", "parameters": "--bar baz" }
  ]
}
)"_json;

    Program program = conf;
    CHECK(program.name == "Calibrator");
    CHECK(program.executable == "A:/Calibrator/calibrator.exe");
    CHECK(program.commandlineParameters.empty());
    CHECK(program.workingDirectory == "A:/Calibrator");
    CHECK(program.shouldForwardMessages == false);
    CHECK(program.isEnabled == true);
    CHECK(program.delay.has_value() == false);
    CHECK(program.preStart.empty());
    REQUIRE(program.tags.size() == 1);
    CHECK(program.tags[0] == "test");
    CHECK(program.description.empty());
    REQUIRE(program.configurations.size() == 2);
    CHECK(program.configurations[0].name == "Default");
    CHECK(program.configurations[0].parameters.empty());
    CHECK(program.configurations[0].description.empty());
    CHECK(program.configurations[1].name == "Fancy");
    CHECK(program.configurations[1].parameters == "-foo bar");
    CHECK(program.configurations[1].description == "A more fancy configuration");
    REQUIRE(program.clusters.size() == 3);
    CHECK(program.clusters[0].name == "Local");
    CHECK(program.clusters[0].parameters.empty());
    CHECK(program.clusters[1].name == "Local+Office");
    CHECK(program.clusters[1].parameters.empty());
    CHECK(program.clusters[2].name == "Local+Home");
    CHECK(program.clusters[2].parameters == "--bar baz");
}

TEST_CASE("Program Example: VLC", "[Program]") {
    using namespace nlohmann;

    json conf = R"(
{
  "name": "VLC",
  "executable": "C:/Program Files (x86)/VideoLAN/VLC/vlc.exe",
  "workingDirectory": "C:/Program Files (x86)/VideoLAN/VLC",
  "tags": [ "test" ],
  "configurations": [
    {
      "name": "Default"
    },
    {
      "name": "Fancy",
      "parameters": "-foo bar"
    }
  ],
  "clusters": [
    { "name": "Local" },
    { "name": "Local+Office" },
    { "name": "Local+Home" }
  ]
}
)"_json;

    Program program = conf;
    CHECK(program.name == "VLC");
    CHECK(program.executable == "C:/Program Files (x86)/VideoLAN/VLC/vlc.exe");
    CHECK(program.commandlineParameters.empty());
    CHECK(program.workingDirectory == "C:/Program Files (x86)/VideoLAN/VLC");
    CHECK(program.shouldForwardMessages == false);
    CHECK(program.isEnabled == true);
    CHECK(program.delay.has_value() == false);
    CHECK(program.preStart.empty());
    REQUIRE(program.tags.size() == 1);
    CHECK(program.tags[0] == "test");
    CHECK(program.description.empty());
    REQUIRE(program.configurations.size() == 2);
    CHECK(program.configurations[0].name == "Default");
    CHECK(program.configurations[0].parameters.empty());
    CHECK(program.configurations[0].description.empty());
    CHECK(program.configurations[1].name == "Fancy");
    CHECK(program.configurations[1].parameters == "-foo bar");
    CHECK(program.configurations[1].description.empty());
    REQUIRE(program.clusters.size() == 3);
    CHECK(program.clusters[0].name == "Local");
    CHECK(program.clusters[0].parameters.empty());
    CHECK(program.clusters[1].name == "Local+Office");
    CHECK(program.clusters[1].parameters.empty());
    CHECK(program.clusters[2].name == "Local+Home");
    CHECK(program.clusters[2].parameters.empty());
}
