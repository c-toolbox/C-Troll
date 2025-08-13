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

#include "messages/processstatusmessage.h"

namespace {
    constexpr std::string_view KeyProcessId = "processId";
    constexpr std::string_view KeyStatus = "status";

    constexpr std::string_view fromStatus(common::ProcessStatusMessage::Status status) {
        using PSM = common::ProcessStatusMessage;
        switch (status) {
            case PSM::Status::Unknown:       return "Unknown";
            case PSM::Status::Starting:      return "Starting";
            case PSM::Status::Running:       return "Running";
            case PSM::Status::NormalExit:    return "NormalExit";
            case PSM::Status::CrashExit:     return "CrashExit";
            case PSM::Status::FailedToStart: return "FailedToStart";
            case PSM::Status::TimedOut:      return "TimedOut";
            case PSM::Status::WriteError:    return "WriteError";
            case PSM::Status::ReadError:     return "ReadError";
            case PSM::Status::UnknownError:  return "UnknownError";
        }
        throw std::logic_error("Unhandled case label");
    }

    constexpr common::ProcessStatusMessage::Status toStatus(std::string_view status) {
        if (status == "Unknown") {
            return common::ProcessStatusMessage::Status::Unknown;
        }
        else if (status == "Starting") {
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
            throw std::runtime_error("Unknown status");
        }
    }
} // namespace

namespace common {

ProcessStatusMessage::ProcessStatusMessage()
    : Message(std::string(ProcessStatusMessage::Type))
{}

void to_json(nlohmann::json& j, const ProcessStatusMessage& m) {
    j[Message::KeyType] = ProcessStatusMessage::Type;
    j[Message::KeyVersion] = { api::MajorVersion, api::MinorVersion, api::PatchVersion };
    j[KeyProcessId] = m.processId;
    j[KeyStatus] = fromStatus(m.status);
}

void from_json(const nlohmann::json& j, ProcessStatusMessage& m) {
    validateMessage(j, ProcessStatusMessage::Type);
    from_json(j, static_cast<Message&>(m));

    j.at(KeyProcessId).get_to(m.processId);
    std::string status = j.at(KeyStatus).get<std::string>();
    m.status = toStatus(status);
}

} // namespace common
