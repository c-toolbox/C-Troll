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

#include "commandmessage.h"

#include "logging.h"
#include <fmt/format.h>
#include <optional>

namespace {
    constexpr const char* KeyId = "id";
    constexpr const char* KeyForwardOutErr = "forwardOutErr";
    constexpr const char* KeyCommand = "command";
    constexpr const char* KeyExecutable = "executable";
    constexpr const char* KeyWorkingDirectory = "workingDirectory";
    constexpr const char* KeyCommandlineArguments = "commandlineArguments";

    std::string fromCommand(common::CommandMessage::Command command) {
        switch (command) {
            case common::CommandMessage::Command::Start: return "Start";
            case common::CommandMessage::Command::Kill: return "Kill";
            case common::CommandMessage::Command::Exit: return "Exit";
            default: throw std::logic_error("Unhandled case label");
        }
    }

    common::CommandMessage::Command toCommand(const std::string& command) {
        if (command == "Start") {
            return common::CommandMessage::Command::Start;
        }
        else if (command == "Kill") {
            return common::CommandMessage::Command::Kill;
        }
        else if (command == "Exit") {
            return common::CommandMessage::Command::Exit;
        }
        else {
            return common::CommandMessage::Command::None;
        }
    }
} // namespace

namespace common {

void to_json(nlohmann::json& j, const CommandMessage& p) {
    j = {
        { Message::KeyType, CommandMessage::Type },
        { Message::KeyVersion, p.CurrentVersion },
        { Message::KeySecret, p.secret },
        { KeyId, p.id },
        { KeyForwardOutErr, p.forwardStdOutStdErr },
        { KeyCommand, fromCommand(p.command) },
        { KeyExecutable, p.executable },
        { KeyWorkingDirectory, p.workingDirectory },
        { KeyCommandlineArguments, p.commandlineParameters }
    };
}

void from_json(const nlohmann::json& j, CommandMessage& p) {
    validateMessage(j, CommandMessage::Type);

    j.at(KeyId).get_to(p.id);
    j.at(KeyForwardOutErr).get_to(p.forwardStdOutStdErr);
    
    const std::string commandString = j.at(KeyCommand).get<std::string>();
    p.command = toCommand(commandString);

    j.at(KeyExecutable).get_to(p.executable);
    j.at(KeyWorkingDirectory).get_to(p.workingDirectory);
    j.at(KeyCommandlineArguments).get_to(p.commandlineParameters);

    if (p.command == CommandMessage::Command::None) {
        ::Log(fmt::format("Received unknown command {} with payload {}",
            commandString, j.dump()));
    }
}

} // namespace common
