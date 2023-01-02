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

#include "messages/processstatusmessage.h"
#include <nlohmann/json.hpp>

TEST_CASE("ProcessStatusMessage Default Ctor", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ProcessStatusMessage Correct Type", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    CHECK(msg.type == common::ProcessStatusMessage::Type);


    nlohmann::json j;
    to_json(j, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j, msgDeserialize);
    CHECK(msgDeserialize.type == common::ProcessStatusMessage::Type);
}

TEST_CASE("ProcessStatusMessage.processId", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    msg.processId = 13;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.processId == 13);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ProcessStatusMessage.status = Starting", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    msg.status = common::ProcessStatusMessage::Status::Starting;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.status == common::ProcessStatusMessage::Status::Starting);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ProcessStatusMessage.status = Running", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    msg.status = common::ProcessStatusMessage::Status::Running;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.status == common::ProcessStatusMessage::Status::Running);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ProcessStatusMessage.status = NormalExit", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    msg.status = common::ProcessStatusMessage::Status::NormalExit;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.status == common::ProcessStatusMessage::Status::NormalExit);
    
    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ProcessStatusMessage.status = CrashExit", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    msg.status = common::ProcessStatusMessage::Status::CrashExit;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.status == common::ProcessStatusMessage::Status::CrashExit);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ProcessStatusMessage.status = FailedToStart", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    msg.status = common::ProcessStatusMessage::Status::FailedToStart;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.status == common::ProcessStatusMessage::Status::FailedToStart);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ProcessStatusMessage.status = TimedOut", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    msg.status = common::ProcessStatusMessage::Status::TimedOut;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.status == common::ProcessStatusMessage::Status::TimedOut);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ProcessStatusMessage.status = WriteError", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    msg.status = common::ProcessStatusMessage::Status::WriteError;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.status == common::ProcessStatusMessage::Status::WriteError);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ProcessStatusMessage.status = ReadError", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    msg.status = common::ProcessStatusMessage::Status::ReadError;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.status == common::ProcessStatusMessage::Status::ReadError);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("ProcessStatusMessage.status = UnknownError", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    msg.status = common::ProcessStatusMessage::Status::UnknownError;


    nlohmann::json j1;
    to_json(j1, msg);

    common::ProcessStatusMessage msgDeserialize;
    CHECK_THROWS(from_json(j1, msgDeserialize));
}

TEST_CASE("ProcessStatusMessage.status wrong", "[ProcessStatusMessage]") {
    common::ProcessStatusMessage msg;
    nlohmann::json j;
    to_json(j, msg);

    j["status"] = "foobar";
    common::ProcessStatusMessage msgDeserialize;
    CHECK_THROWS(from_json(j, msgDeserialize));
}
