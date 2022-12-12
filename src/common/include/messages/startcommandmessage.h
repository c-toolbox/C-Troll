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

#ifndef __COMMON__STARTCOMMANDMESSAGE_H__
#define __COMMON__STARTCOMMANDMESSAGE_H__

#include "message.h"

#include <nlohmann/json.hpp>
#include <string_view>

namespace common {

/// This struct is the data structure that gets send from the Core to the Tray to signal
/// that the Tray should perform a task
struct StartCommandMessage : public Message {
    static constexpr std::string_view Type = "StartCommandMessage";

    /// The unique identifier for the process that will be created
    int id = -1;
    /// The name of the executable
    std::string executable;
    /// The location that should be set as the working directory prior to execution
    std::string workingDirectory;
    /// The list of commandline parameters to be passed to executable
    std::string commandlineParameters;
    /// This value determines whether the process should send back console messages
    bool forwardStdOutStdErr = false;

    // This information is not used directly, but mirrored back by the tray in case the
    // C-Troll reconnects and the process started by this command is still running
    int programId = -1;
    int configurationId = -1;
    int clusterId = -1;
    int nodeId = -1;
    std::size_t dataHash = 0;
};

void to_json(nlohmann::json& j, const StartCommandMessage& m);
void from_json(const nlohmann::json& j, StartCommandMessage& m);

} // namespace commmon

#endif // __COMMON__STARTCOMMANDMESSAGE_H__
