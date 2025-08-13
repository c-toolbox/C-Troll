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

#include "catch2/catch_test_macros.hpp"

#include "node.h"
#include <nlohmann/json.hpp>

TEST_CASE("Node Example: Home", "[Node]") {
    using namespace nlohmann;

    const json conf = R"(
{
  "name": "Home",
  "ip": "localhost",
  "port": 6001
}
)"_json;

    Node node = conf;
    CHECK(node.name == "Home");
    CHECK(node.ipAddress == "localhost");
    CHECK(node.port == 6001);
    CHECK(node.secret.empty());
    CHECK(node.description.empty());
}

TEST_CASE("Node Example: Laptop", "[Node]") {
    using namespace nlohmann;

    const json conf = R"(
{
  "name": "Laptop",
  "ip": "2001:6b0:17:fc08:ec96:21d7:75bb:3d50",
  "port": 5000
}
)"_json;

    Node node = conf;
    CHECK(node.name == "Laptop");
    CHECK(node.ipAddress == "2001:6b0:17:fc08:ec96:21d7:75bb:3d50");
    CHECK(node.port == 5000);
    CHECK(node.secret.empty());
    CHECK(node.description.empty());
}

TEST_CASE("Node Example: Local", "[Node]") {
    using namespace nlohmann;

    const json conf = R"(
{
  "description": "The local machine",
  "ip": "localhost",
  "name": "Local",
  "port": 5000
}
)"_json;

    Node node = conf;
    CHECK(node.name == "Local");
    CHECK(node.ipAddress == "localhost");
    CHECK(node.port == 5000);
    CHECK(node.secret.empty());
    CHECK(node.description == "The local machine");
}

TEST_CASE("Node Example: Local w/ secret", "[Node]") {
    using namespace nlohmann;

    const json conf = R"(
{
  "description": "The local machine",
  "ip": "localhost",
  "name": "Local",
  "port": 5000,
  "secret": "My super secret well-known fact"
}
)"_json;

    Node node = conf;
    CHECK(node.name == "Local");
    CHECK(node.ipAddress == "localhost");
    CHECK(node.port == 5000);
    CHECK(node.secret == "My super secret well-known fact");
    CHECK(node.description == "The local machine");
}
