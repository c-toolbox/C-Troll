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

#include "trayprocessstatus.h"

#include <fmt/format.h>

namespace {
    constexpr const char* KeyProcessId = "processId";
    constexpr const char* KeyStatus = "status";

    std::string fromStatus(common::TrayProcessStatus::Status status) {
        switch (status) {
            case common::TrayProcessStatus::Status::Starting: return "Starting";
            case common::TrayProcessStatus::Status::Running: return "Running";
            case common::TrayProcessStatus::Status::NormalExit: return "NormalExit";
            case common::TrayProcessStatus::Status::CrashExit: return "CrashExit";
            case common::TrayProcessStatus::Status::FailedToStart: return "FailedToStart";
            case common::TrayProcessStatus::Status::TimedOut: return "TimedOut";
            case common::TrayProcessStatus::Status::WriteError: return "WriteError";
            case common::TrayProcessStatus::Status::ReadError: return "ReadError";
            case common::TrayProcessStatus::Status::UnknownError: return "UnknownError";
            default: throw std::logic_error("Unhandled case label");
        }
    }

    common::TrayProcessStatus::Status toStatus(const std::string& status) {
        if (status == "Starting") {
            return common::TrayProcessStatus::Status::Starting;
        }
        else if (status == "Running") {
            return common::TrayProcessStatus::Status::Running;
        }
        else if (status == "NormalExit") {
            return common::TrayProcessStatus::Status::NormalExit;
        }
        else if (status == "CrashExit") {
            return common::TrayProcessStatus::Status::CrashExit;
        }
        else if (status == "FailedToStart") {
            return common::TrayProcessStatus::Status::FailedToStart;
        }
        else if (status == "TimedOut") {
            return common::TrayProcessStatus::Status::TimedOut;
        }
        else if (status == "WriteError") {
            return common::TrayProcessStatus::Status::WriteError;
        }
        else if (status == "ReadError") {
            return common::TrayProcessStatus::Status::ReadError;
        }
        else {
            return common::TrayProcessStatus::Status::UnknownError;
        }
    }

} // namespace

namespace common {

bool isValidTrayProcessStatus(const nlohmann::json& j) {
    std::string type;
    j.at(GenericMessage::KeyType).get_to(type);

    int version;
    j.at(GenericMessage::KeyVersion).get_to(version);

    return type == TrayProcessStatus::Type && version == GenericMessage::version;
}

void to_json(nlohmann::json& j, const TrayProcessStatus& p) {
    std::string status = fromStatus(p.status);
    j = {
        { GenericMessage::KeyType, TrayProcessStatus::Type },
        { GenericMessage::KeyVersion, p.version },
        { KeyProcessId, p.processId },
        { KeyStatus, status }
    };
}

void from_json(const nlohmann::json& j, TrayProcessStatus& p) {
    validateMessage(j, TrayProcessStatus::Type);

    j.at(KeyProcessId).get_to(p.processId);
    std::string status = j.at(KeyStatus).get<std::string>();
    p.status = toStatus(status);
}
    
} // namespace common
