/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2023                                                             *
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

#include "messages/message.h"

#include <fmt/format.h>
#include <string>

namespace common {

void validateMessage(const nlohmann::json& message, std::string_view expectedType) {
    // Sanity checks
    const std::string type = message.at(Message::KeyType).get<std::string>();
    if (type != expectedType) {
        throw std::logic_error(fmt::format(
            "Validation failed. Expected type '{}', got '{}'", expectedType, type
        ));
    }

    const ApiVersion version = message.at(Message::KeyVersion).get<ApiVersion>();
    if (version[0] != api::MajorVersion) {
        throw std::runtime_error(fmt::format(
            "Mismatching version number. Expected {} got {}.{}.{}",
            api::MajorVersion, version[0], version[1], version[2]
        ));
    }
}

void from_json(const nlohmann::json& message, Message& m) {
    message.at(Message::KeyType).get_to(m.type);
    if (message.find(Message::KeySecret) != message.end()) {
        message[Message::KeySecret].get_to(m.secret);
    }
}

} // namespace common
