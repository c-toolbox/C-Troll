/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
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

#include "processstatusmessage.h"

#include <fmt/format.h>

namespace {
    constexpr const char* KeyProcessId = "processId";
    constexpr const char* KeyStatus = "status";

    std::string fromStatus(common::ProcessStatusMessage::Status status) {
        switch (status) {
            case common::ProcessStatusMessage::Status::Starting: return "Starting";
            case common::ProcessStatusMessage::Status::Running: return "Running";
            case common::ProcessStatusMessage::Status::NormalExit: return "NormalExit";
            case common::ProcessStatusMessage::Status::CrashExit: return "CrashExit";
            case common::ProcessStatusMessage::Status::FailedToStart: return "FailedToStart";
            case common::ProcessStatusMessage::Status::TimedOut: return "TimedOut";
            case common::ProcessStatusMessage::Status::WriteError: return "WriteError";
            case common::ProcessStatusMessage::Status::ReadError: return "ReadError";
            case common::ProcessStatusMessage::Status::UnknownError: return "UnknownError";
            default: throw std::logic_error("Unhandled case label");
        }
    }

    common::ProcessStatusMessage::Status toStatus(const std::string& status) {
        if (status == "Starting") {
            return common::ProcessStatusMessage::Status::Starting;
        }
        else if (status == "Running") {
            return common::ProcessStatusMessage::Status::Running;
        }
        else if (status == "NormalExit") {
            return common::ProcessStatusMessage::Status::NormalExit;
        }
        else if (status == "CrashExit") {
            return common::ProcessStatusMessage::Status::CrashExit;
        }
        else if (status == "FailedToStart") {
            return common::ProcessStatusMessage::Status::FailedToStart;
        }
        else if (status == "TimedOut") {
            return common::ProcessStatusMessage::Status::TimedOut;
        }
        else if (status == "WriteError") {
            return common::ProcessStatusMessage::Status::WriteError;
        }
        else if (status == "ReadError") {
            return common::ProcessStatusMessage::Status::ReadError;
        }
        else {
            return common::ProcessStatusMessage::Status::UnknownError;
        }
    }

} // namespace

namespace common {

void to_json(nlohmann::json& j, const ProcessStatusMessage& p) {
    std::string status = fromStatus(p.status);
    j = {
        { Message::KeyType, ProcessStatusMessage::Type },
        { Message::KeyVersion, p.version },
        { KeyProcessId, p.processId },
        { KeyStatus, status }
    };
}

void from_json(const nlohmann::json& j, ProcessStatusMessage& p) {
    validateMessage(j, ProcessStatusMessage::Type);

    j.at(KeyProcessId).get_to(p.processId);
    std::string status = j.at(KeyStatus).get<std::string>();
    p.status = toStatus(status);
}
    
} // namespace common
