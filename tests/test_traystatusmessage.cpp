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

#include "messages/traystatusmessage.h"
#include <nlohmann/json.hpp>

TEST_CASE("TrayStatusMessage Default Ctor", "[TrayStatusMessage]") {
    common::TrayStatusMessage msg;


    nlohmann::json j1;
    to_json(j1, msg);

    common::TrayStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}

TEST_CASE("TrayStatusMessage Correct Type", "[TrayStatusMessage]") {
    common::TrayStatusMessage msg;
    CHECK(msg.type == common::TrayStatusMessage::Type);


    nlohmann::json j;
    to_json(j, msg);

    common::TrayStatusMessage msgDeserialize;
    from_json(j, msgDeserialize);
    CHECK(msg == msgDeserialize);
    CHECK(msgDeserialize.type == common::TrayStatusMessage::Type);
}

TEST_CASE("(TrayStatusMessage) processes", "[TrayStatusMessage]") {
    common::TrayStatusMessage msg;
    msg.processes.push_back({ 1, 2, 3, 4, 5, 6 });
    msg.processes.push_back({ 7, 8, 9, 10, 11, 12 });


    nlohmann::json j1;
    to_json(j1, msg);

    common::TrayStatusMessage msgDeserialize;
    from_json(j1, msgDeserialize);
    CHECK(msg == msgDeserialize);
    REQUIRE(msgDeserialize.processes.size() == 2);
    CHECK(msgDeserialize.processes[0].processId == 1);
    CHECK(msgDeserialize.processes[0].programId == 2);
    CHECK(msgDeserialize.processes[0].configurationId == 3);
    CHECK(msgDeserialize.processes[0].clusterId == 4);
    CHECK(msgDeserialize.processes[0].nodeId == 5);
    CHECK(msgDeserialize.processes[0].dataHash == 6);
    CHECK(msgDeserialize.processes[1].processId == 7);
    CHECK(msgDeserialize.processes[1].programId == 8);
    CHECK(msgDeserialize.processes[1].configurationId == 9);
    CHECK(msgDeserialize.processes[1].clusterId == 10);
    CHECK(msgDeserialize.processes[1].nodeId == 11);
    CHECK(msgDeserialize.processes[1].dataHash == 12);

    nlohmann::json j2;
    to_json(j2, msgDeserialize);
    CHECK(j1 == j2);
}
