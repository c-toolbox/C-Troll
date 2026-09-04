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

#include <catch2/catch_test_macros.hpp>

#include "addressallowlist.h"

TEST_CASE("AddressAllowList: Empty", "[AddressAllowList]") {
    const common::AddressAllowList list = common::AddressAllowList({});

    CHECK(list.isEmpty());
    CHECK(!list.contains("127.0.0.1"));
    CHECK(!list.contains("192.168.0.1"));
}

TEST_CASE("AddressAllowList: Literal IPv4", "[AddressAllowList]") {
    const common::AddressAllowList list =
        common::AddressAllowList({ "192.168.0.1", "10.0.0.5" });

    CHECK(!list.isEmpty());
    CHECK(list.contains("192.168.0.1"));
    CHECK(list.contains("10.0.0.5"));
    CHECK(!list.contains("192.168.0.2"));
    CHECK(!list.contains("10.0.0.6"));
}

TEST_CASE("AddressAllowList: IPv4 subnet", "[AddressAllowList]") {
    const common::AddressAllowList list =
        common::AddressAllowList({ "192.168.0.0/24" });

    CHECK(list.contains("192.168.0.1"));
    CHECK(list.contains("192.168.0.255"));
    CHECK(!list.contains("192.168.1.1"));
    CHECK(!list.contains("10.0.0.1"));
}

TEST_CASE("AddressAllowList: localhost", "[AddressAllowList]") {
    const common::AddressAllowList list = common::AddressAllowList({ "localhost" });

    CHECK(list.contains("127.0.0.1"));
    CHECK(list.contains("::1"));
    CHECK(!list.contains("192.168.0.1"));
}

TEST_CASE("AddressAllowList: IPv6", "[AddressAllowList]") {
    const common::AddressAllowList list =
        common::AddressAllowList({ "2001:6b0:17:fc08:ec96:21d7:75bb:3d50" });

    CHECK(list.contains("2001:6b0:17:fc08:ec96:21d7:75bb:3d50"));
    CHECK(!list.contains("2001:6b0:17:fc08:ec96:21d7:75bb:3d51"));
}

TEST_CASE("AddressAllowList: IPv4-mapped IPv6 peer", "[AddressAllowList]") {
    // A dual-stack server reports IPv4 peers in this form
    const common::AddressAllowList list = common::AddressAllowList({ "192.168.0.1" });

    CHECK(list.contains("::ffff:192.168.0.1"));
    CHECK(!list.contains("::ffff:192.168.0.2"));
}

TEST_CASE("AddressAllowList: Invalid entries", "[AddressAllowList]") {
    const common::AddressAllowList list =
        common::AddressAllowList({ "192.168.0.1", "not-an-address", "" });

    CHECK(list.contains("192.168.0.1"));
    REQUIRE(list.invalidEntries().size() == 1);
    CHECK(list.invalidEntries().front() == "not-an-address");
}

TEST_CASE("AddressAllowList: Only invalid entries denies everything",
          "[AddressAllowList]")
{
    const common::AddressAllowList list = common::AddressAllowList({ "not-an-address" });

    CHECK(list.isEmpty());
    CHECK(!list.contains("127.0.0.1"));
}

TEST_CASE("AddressAllowList: Malformed peer address", "[AddressAllowList]") {
    const common::AddressAllowList list = common::AddressAllowList({ "192.168.0.1" });

    CHECK(!list.contains("definitely not an address"));
    CHECK(!list.contains(""));
}
