/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2023                                                               *
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

#include "messages/erroroccurredmessage.h"
#include <nlohmann/json.hpp>

TEST_CASE("ErrorOccurred Default Ctor", "[ErrorOccurred]") {
    common::ErrorOccurredMessage msg;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ErrorOccurredMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ErrorOccurred Correct Type", "[ErrorOccurred]") {
    common::ErrorOccurredMessage msg;
    CHECK(msg.type == common::ErrorOccurredMessage::Type);


    nlohmann::json j;
    to_json(j, msg);

    common::ErrorOccurredMessage msgDeserialize;
    from_json(j, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.type == common::ErrorOccurredMessage::Type);
}

TEST_CASE("ErrorOccurred.error", "[ErrorOccurred]") {
    common::ErrorOccurredMessage msg;
    msg.error = "foobar";


    nlohmann::json j1;
    to_json(j1, msg);

    common::ErrorOccurredMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.error == "foobar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ErrorOccurred.lastMessages", "[ErrorOccurred]") {
    common::ErrorOccurredMessage msg;
    msg.lastMessages.push_back("foo");
    msg.lastMessages.push_back("bar");


    nlohmann::json j1;
    to_json(j1, msg);

    common::ErrorOccurredMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    REQUIRE(msgDeserialize.lastMessages.size() == 2);
    CHECK(msgDeserialize.lastMessages[0] == "foo");
    CHECK(msgDeserialize.lastMessages[1] == "bar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ErrorOccurred full", "[ErrorOccurred]") {
    common::ErrorOccurredMessage msg;
    msg.error = "foobar";
    msg.lastMessages.push_back("foo");
    msg.lastMessages.push_back("bar");


    nlohmann::json j1;
    to_json(j1, msg);

    common::ErrorOccurredMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.error == "foobar");
    REQUIRE(msgDeserialize.lastMessages.size() == 2);
    CHECK(msgDeserialize.lastMessages[0] == "foo");
    CHECK(msgDeserialize.lastMessages[1] == "bar");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}
