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

#include "configuration.h"

namespace {
    constexpr const char* KeyRemovalTimeout = "removalTimeout";

    constexpr const char* KeyLogFile = "logFile";
    constexpr const char* KeyLogRotation = "logRotation";
    
    constexpr const char* KeyTagColors = "tagColors";

    constexpr const char* KeyRest = "rest";
    constexpr const char* KeyRestUsername = "username";
    constexpr const char* KeyRestPassword = "password";
    constexpr const char* KeyRestPort = "port";
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

    if (c.rest.has_value()) {
        if (!c.rest->username.empty()) {
            j[KeyRest][KeyRestUsername] = c.rest->username;
        }
        if (!c.rest->password.empty()) {
            j[KeyRest][KeyRestPassword] = c.rest->password;
        }
        if (c.rest->port != Configuration::Rest().port) {
            j[KeyRest][KeyRestPort] = c.rest->port;
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

    if (j.find(KeyRest) != j.end()) {
        Configuration::Rest r;
        if (j[KeyRest].find(KeyRestUsername) != j.end()) {
            j[KeyRest][KeyRestUsername].get_to(r.username);
        }
        if (j[KeyRest].find(KeyRestPassword) != j.end()) {
            j[KeyRest][KeyRestPassword].get_to(r.username);
        }
        if (j[KeyRest].find(KeyRestPort) != j.end()) {
            j[KeyRest][KeyRestPort].get_to(r.port);
        }
        c.rest = r;
    }
}
