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

#include "configuration.h"

namespace {
    constexpr const char* KeyPort = "port";
    constexpr const char* KeySecret = "secret";
    constexpr const char* KeyShowWindow = "showWindow";

    constexpr const char* KeyLogFile = "logFile";
    constexpr const char* KeyLogRotation = "logRotation";
} // namespace

void to_json(nlohmann::json& j, const Configuration& c) {
    j[KeyPort] = c.port;
    j[KeySecret] = c.secret;
    j[KeyShowWindow] = c.showWindow;
    j[KeyLogFile] = c.logFile;
    if (c.logRotation.has_value()) {
        j[KeyLogRotation] = *c.logRotation;
    }
}

void from_json(const nlohmann::json& j, Configuration& c) {
    j.at(KeyPort).get_to(c.port);

    if (j.find(KeySecret) != j.end()) {
        j.at(KeySecret).get_to(c.secret);
    }
    if (j.find(KeyShowWindow) != j.end()) {
        j.at(KeyShowWindow).get_to(c.showWindow);
    }
    if (j.find(KeyLogFile) != j.end()) {
        j.at(KeyLogFile).get_to(c.logFile);
    }
    if (j.find(KeyLogRotation) != j.end()) {
        c.logRotation = j.at(KeyLogRotation).get<common::LogRotation>();
    }
}
