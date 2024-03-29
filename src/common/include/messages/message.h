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

#ifndef __COMMON__MESSAGE_H__
#define __COMMON__MESSAGE_H__

#include "version.h"
#include <nlohmann/json.hpp>
#include <array>
#include <string_view>

namespace common {

struct Message {
    static constexpr std::string_view KeyType = "type";
    static constexpr std::string_view KeyVersion = "version";
    static constexpr std::string_view KeySecret = "secret";

    Message() = default;
    explicit Message(std::string type);
    auto operator<=>(const Message& rhs) const = default;

    /// A string representing the type of payload contained in this Message
    std::string type;

    /// A string representing the SHA512 hashed secret that is needed to communicate with
    /// the tray socket that this message gets sent to. For messages that go from the tray
    /// to the main application, this value is ignored
    std::string secret;
};

using ApiVersion = std::array<int, 3>;

/**
 * This function verifies whether the provided \p message is actually containing message
 * for the type \tparam T. \tparam T must define a static variable named \c Type that is
 * comparable to a string. Furthermore, this function will check that the version recieved
 * is compatible with the version expected.
 *
 * \param message The message that should be checked for validity
 * \return \c true if the message is valid, \c false otherwise
 */
template <typename T = void>
    requires std::is_same_v<T, void> || std::is_base_of_v<Message, T>
[[nodiscard]] bool isValidMessage(const nlohmann::json& message) {
    const bool hasType = message.find(Message::KeyType) != message.end();
    const bool hasVersion = message.find(Message::KeyVersion) != message.end();
    if (!hasType || !hasVersion) {
        return false;
    }

    const ApiVersion version = message.at(Message::KeyVersion).get<ApiVersion>();
    if (version[0] != api::MajorVersion) {
        return false;
    }
    if constexpr (std::is_same_v<T, void>) {
        // If we don't have a type, we are done
        return true;
    }
    else {
        // Otherwise we want to check that the type is correct
        const std::string type = message.at(Message::KeyType).get<std::string>();
        return type == T::Type;
    }
}

// Throws std::logic_error if the internal type is different from the expected type
// Throws std::runtime_error if the version is different from the current version
void validateMessage(const nlohmann::json& message, std::string_view expectedType);

void from_json(const nlohmann::json& j, Message& m);

} // namespace common

#endif // __COMMON__MESSAGE_H__
