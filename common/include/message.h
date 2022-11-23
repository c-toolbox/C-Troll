/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
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

#ifndef __COMMON__MESSAGE_H__
#define __COMMON__MESSAGE_H__

#include <nlohmann/json.hpp>
#include <string_view>

namespace common {

struct Message {
    static constexpr const char* KeyType = "type";
    static constexpr const char* KeyVersion = "version";
    static constexpr const char* KeySecret = "secret";

    /// The version of the API that should be increased with breaking changes
    static constexpr const int CurrentVersion = 1;

    /// A string representing the type of payload contained in this Message
    std::string type;

    /// A string representing the SHA512 hashed secret that is needed to communicate with
    /// the tray socket that this message gets sent to. For messages that go from the tray
    /// to the main application, this value is ignored
    std::string secret;
};

template <typename T>
[[ nodiscard ]] bool isValidMessage(const nlohmann::json& message) {
    const std::string type = message.at(Message::KeyType).get<std::string>();
    const int version = message.at(Message::KeyVersion).get<int>();

    return type == T::Type && version == Message::CurrentVersion;
}

// Returns true, if the message has the required fields. Returns false if they are not
// present, preventing us from deciphering the message
[[ nodiscard ]] bool validateMessage(const nlohmann::json& message);

// Throws std::logic_error if the internal type is different from the expected type
// Throws std::runtime_error if the version is different from the current version
void validateMessage(const nlohmann::json& message, std::string_view expectedType);
    
void from_json(const nlohmann::json& j, Message& m);

} // namespace common
    
#endif // __COMMON__MESSAGE_H__
