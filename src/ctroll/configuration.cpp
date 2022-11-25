/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2022                                                             *
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

#include "configuration.h"

#include <string_view>

namespace {
    constexpr std::string_view KeyRemovalTimeout = "removalTimeout";

    constexpr std::string_view KeyLogFile = "logFile";
    constexpr std::string_view KeyLogRotation = "logRotation";

    constexpr std::string_view KeyTagColors = "tagColors";

    constexpr std::string_view KeyRestLoopback = "restLoopback";
    constexpr std::string_view KeyRestGeneral = "restGeneral";
    constexpr std::string_view KeyRestUsername = "username";
    constexpr std::string_view KeyRestPassword = "password";
    constexpr std::string_view KeyRestPort = "port";
    constexpr std::string_view KeyRestAllowCustomPrograms = "allowCustomPrograms";

    // Legacy
    constexpr std::string_view KeyRest = "rest"; // should be mapped to restGeneral

} // namespace

void to_json(nlohmann::json& j, const Configuration& c) {
    to_json(j, static_cast<const BaseConfiguration&>(c));

    if (c.removalTimeout != Configuration().removalTimeout) {
        j[KeyRemovalTimeout] = static_cast<int>(c.removalTimeout.count());
    }

    if (c.tagColors != Configuration().tagColors) {
        j[KeyTagColors] = c.tagColors;
    }

    if (c.logFile != Configuration().logFile) {
        j[KeyLogFile] = c.logFile;
    }

    if (c.logRotation.has_value()) {
        j[KeyLogRotation] = *c.logRotation;
    }

    if (c.restLoopback.has_value()) {
        j[KeyRestLoopback] = nlohmann::json::object();
        if (!c.restLoopback->username.empty()) {
            j[KeyRestLoopback][KeyRestUsername] = c.restLoopback->username;
        }
        if (!c.restLoopback->password.empty()) {
            j[KeyRestLoopback][KeyRestPassword] = c.restLoopback->password;
        }
        if (c.restLoopback->port != Configuration::Rest().port) {
            j[KeyRestLoopback][KeyRestPort] = c.restLoopback->port;
        }
        if (c.restLoopback->allowCustomPrograms !=
            Configuration::Rest().allowCustomPrograms)
        {
            j[KeyRestLoopback][KeyRestAllowCustomPrograms] =
                c.restLoopback->allowCustomPrograms;
        }
    }

    if (c.restGeneral.has_value()) {
        j[KeyRestGeneral] = nlohmann::json::object();
        if (!c.restGeneral->username.empty()) {
            j[KeyRestGeneral][KeyRestUsername] = c.restGeneral->username;
        }
        if (!c.restGeneral->password.empty()) {
            j[KeyRestGeneral][KeyRestPassword] = c.restGeneral->password;
        }
        if (c.restGeneral->port != Configuration::Rest().port) {
            j[KeyRestGeneral][KeyRestPort] = c.restGeneral->port;
        }
        if (c.restGeneral->allowCustomPrograms !=
            Configuration::Rest().allowCustomPrograms)
        {
            j[KeyRestGeneral][KeyRestAllowCustomPrograms] =
                c.restGeneral->allowCustomPrograms;
        }
    }
}

void from_json(const nlohmann::json& j, Configuration& c) {
    from_json(j, static_cast<BaseConfiguration&>(c));

    if (j.find(KeyRemovalTimeout) != j.end()) {
        int ms = j[KeyRemovalTimeout].get<int>();

        if (ms < 0) {
            throw std::runtime_error("Negative process removal time is not allowed");
        }

        c.removalTimeout = std::chrono::milliseconds(ms);
    }

    if (j.find(KeyTagColors) != j.end()) {
        // get_to adds the values to the end of the vector, so we have to clear it first
        c.tagColors.clear();
        j[KeyTagColors].get_to(c.tagColors);
    }

    if (j.find(KeyLogFile) != j.end()) {
        j[KeyLogFile].get_to(c.logFile);
    }

    if (j.find(KeyLogRotation) != j.end()) {
        c.logRotation = j[KeyLogRotation].get<common::LogRotation>();
    }

    if (j.find(KeyRestLoopback) != j.end()) {
        const nlohmann::json& rest = j[KeyRestLoopback];

        Configuration::Rest r;
        if (rest.find(KeyRestUsername) != rest.end()) {
            rest[KeyRestUsername].get_to(r.username);
        }
        if (rest.find(KeyRestPassword) != rest.end()) {
            rest[KeyRestPassword].get_to(r.password);
        }
        if (rest.find(KeyRestPort) != rest.end()) {
            rest[KeyRestPort].get_to(r.port);
        }
        if (rest.find(KeyRestAllowCustomPrograms) != rest.end()) {
            rest[KeyRestAllowCustomPrograms].get_to(r.allowCustomPrograms);
        }
        c.restLoopback = r;
    }

    // @VER2: Remove KeyRest and only allow KeyRestGeneral
    if (j.find(KeyRest) != j.end() || j.find(KeyRestGeneral) != j.end()) {
        const nlohmann::json& rest =
            j.find(KeyRest) != j.end() ? j[KeyRest] : j[KeyRestGeneral];

        Configuration::Rest r;
        if (rest.find(KeyRestUsername) != rest.end()) {
            rest[KeyRestUsername].get_to(r.username);
        }
        if (rest.find(KeyRestPassword) != rest.end()) {
            rest[KeyRestPassword].get_to(r.password);
        }
        if (rest.find(KeyRestPort) != rest.end()) {
            rest[KeyRestPort].get_to(r.port);
        }
        if (rest.find(KeyRestAllowCustomPrograms) != rest.end()) {
            rest[KeyRestAllowCustomPrograms].get_to(r.allowCustomPrograms);
        }
        c.restGeneral = r;
    }
}
