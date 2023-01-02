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

#include "cluster.h"
#include <nlohmann/json.hpp>

TEST_CASE("Cluster Example: Local+Home", "[Cluster]") {
    using namespace nlohmann;

    json conf = R"(
{
  "name": "Local+Home",
  "enabled": true,
  "nodes": [ "Local", "Home" ]
}
)"_json;

    Cluster cluster = conf;
    CHECK(cluster.name == "Local+Home");
    CHECK(cluster.isEnabled == true);
    CHECK(cluster.description.empty());
    REQUIRE(cluster.nodes.size() == 2);
    CHECK(cluster.nodes[0] == "Local");
    CHECK(cluster.nodes[1] == "Home");
}

TEST_CASE("Cluster Example: Local", "[Cluster]") {
    using namespace nlohmann;

    json conf = R"(
{
  "description": "A local cluster",
  "enabled": true,
  "name": "Local",
  "nodes": [
    "Local"
  ]
}
)"_json;

    Cluster cluster = conf;
    CHECK(cluster.name == "Local");
    CHECK(cluster.isEnabled == true);
    CHECK(cluster.description == "A local cluster");
    REQUIRE(cluster.nodes.size() == 1);
    CHECK(cluster.nodes[0] == "Local");
}
