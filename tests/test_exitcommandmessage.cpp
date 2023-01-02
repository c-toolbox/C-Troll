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

#include "messages/exitcommandmessage.h"
#include <nlohmann/json.hpp>

TEST_CASE("ExitCommandMessage Default Ctor", "[ExitCommandMessage]") {
    common::ExitCommandMessage msg;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ExitCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    CHECK(j1 == j2);
}

TEST_CASE("ExitCommandMessage Correct Type", "[ExitCommandMessage]") {
    common::ExitCommandMessage msg;
    CHECK(msg.type == common::ExitCommandMessage::Type);

    nlohmann::json j;
    to_json(j, msg);

    common::ExitCommandMessage msgDeserialize;
    from_json(j, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.type == common::ExitCommandMessage::Type);
}

TEST_CASE("ExitCommandMessage.id", "[ExitCommandMessage]") {
    common::ExitCommandMessage msg;
    msg.id = 13;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ExitCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);

    CHECK(j1 == j2);
}
