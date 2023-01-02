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

#include "messages/startcommandmessage.h"
#include <nlohmann/json.hpp>

TEST_CASE("StartCommand Default Ctor", "[StartCommand]") {
    common::StartCommandMessage msg;


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand Correct Type", "[StartCommand]") {
    common::StartCommandMessage msg;
    CHECK(msg.type == common::StartCommandMessage::Type);


    nlohmann::json j;
    to_json(j, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.type == common::StartCommandMessage::Type);
}

TEST_CASE("StartCommand.id", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.id = 13;


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.id == 13);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand.executable", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.executable = "abc";


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.executable == "abc");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand.workingDirectory", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.workingDirectory = "abc";


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.workingDirectory == "abc");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand.commandlineParameters", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.commandlineParameters = "abc";


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.commandlineParameters == "abc");

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand.forwardStdOutStdErr", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.forwardStdOutStdErr = true;


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.forwardStdOutStdErr == true);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand.programId", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.programId = 13;


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.programId == 13);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand.configurationId", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.configurationId = 13;


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.configurationId == 13);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand.clusterId", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.clusterId = 13;


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.clusterId == 13);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand.nodeId", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.nodeId = 13;


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.nodeId == 13);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand.dataHash", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.dataHash = 13;


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.dataHash == 13);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("StartCommand full", "[StartCommand]") {
    common::StartCommandMessage msg;
    msg.id = 13;
    msg.executable = "abc";
    msg.workingDirectory = "def";
    msg.commandlineParameters = "ghi";
    msg.forwardStdOutStdErr = true;
    msg.programId = 13;
    msg.configurationId = 14;
    msg.clusterId = 15;
    msg.nodeId = 16;
    msg.dataHash = 17;


    nlohmann::json j1;
    to_json(j1, msg);

    common::StartCommandMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.id == 13);
    CHECK(msgDeserialize.executable == "abc");
    CHECK(msgDeserialize.workingDirectory == "def");
    CHECK(msgDeserialize.commandlineParameters == "ghi");
    CHECK(msgDeserialize.forwardStdOutStdErr == true);
    CHECK(msgDeserialize.programId == 13);
    CHECK(msgDeserialize.configurationId == 14);
    CHECK(msgDeserialize.clusterId == 15);
    CHECK(msgDeserialize.nodeId == 16);
    CHECK(msgDeserialize.dataHash == 17);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}
