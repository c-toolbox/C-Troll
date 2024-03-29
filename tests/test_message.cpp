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

#include "messages/message.h"
#include <nlohmann/json.hpp>

TEST_CASE("Message Default Ctor", "[Message]") {
    nlohmann::json j = {
        { common::Message::KeyType, "" },
        { common::Message::KeySecret, "" }
    };

    common::Message msg;

    common::Message msgDeserialize;
    from_json(j, msgDeserialize);
    CHECK(msg == msgDeserialize);
}

TEST_CASE("Message Type", "[Message]") {
    nlohmann::json j = {
        { common::Message::KeyType, "abc" },
        { common::Message::KeySecret, "" }
    };

    common::Message msg("abc");

    common::Message msgDeserialize;
    from_json(j, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msg.type == "abc");
}

TEST_CASE("Message.secret", "[Message]") {
    nlohmann::json j = {
        { common::Message::KeyType, "" },
        { common::Message::KeySecret, "abc" }
    };

    common::Message msg;
    msg.secret = "abc";

    common::Message msgDeserialize;
    from_json(j, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.secret == "abc");
}

TEST_CASE("Message full", "[Message]") {
    nlohmann::json j = {
        { common::Message::KeyType, "abc" },
        { common::Message::KeySecret, "def" }
    };

    common::Message msg;
    msg.type = "abc";
    msg.secret = "def";

    common::Message msgDeserialize;
    from_json(j, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.type == "abc");
    CHECK(msgDeserialize.secret == "def");
}
