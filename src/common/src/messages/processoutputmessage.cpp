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

#include "messages/processoutputmessage.h"

#include "logging.h"
#include <fmt/format.h>

namespace {
    constexpr std::string_view KeyIdentifier = "processId";
    constexpr std::string_view KeyMessage = "message";
    constexpr std::string_view KeyOutputType = "outputType";
} // namespace

namespace common {

ProcessOutputMessage::ProcessOutputMessage()
    : Message(std::string(ProcessOutputMessage::Type))
{}

void to_json(nlohmann::json& j, const ProcessOutputMessage& m) {
    std::string t = [](ProcessOutputMessage::OutputType type) {
        switch (type) {
            case ProcessOutputMessage::OutputType::StdOut: return "stdout";
            case ProcessOutputMessage::OutputType::StdErr: return "stderr";
        }
        throw std::logic_error("Missing case label");
    }(m.outputType);

    j[Message::KeyType] = ProcessOutputMessage::Type;
    j[Message::KeyVersion] = { api::MajorVersion, api::MinorVersion, api::PatchVersion };
    j[KeyIdentifier] = m.processId;
    j[KeyMessage] = m.message;
    j[KeyOutputType] = t;
}

void from_json(const nlohmann::json& j, ProcessOutputMessage& m) {
    validateMessage(j, ProcessOutputMessage::Type);
    from_json(j, static_cast<Message&>(m));

    j.at(KeyIdentifier).get_to(m.processId);
    j.at(KeyMessage).get_to(m.message);
    std::string type = j.at(KeyOutputType).get<std::string>();
    if (type == "stdout") {
        m.outputType = ProcessOutputMessage::OutputType::StdOut;
    }
    else if (type == "stderr") {
        m.outputType = ProcessOutputMessage::OutputType::StdErr;
    }
    else {
        throw std::runtime_error(fmt::format("Unknown output type '{}'", type));
    }
}

} // namespace common
