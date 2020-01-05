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
 * of conditions and the following disclaimer.                                           *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 * list of conditions and the following disclaimer in the documentation and/or other     *
 * materials provided with the distribution.                                             *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 * used to endorse or promote products derived from this software without specific prior *
 * written permission.                                                                   *
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

#ifndef __CORE__GUIPROCESSSTATUS_H__
#define __CORE__GUIPROCESSSTATUS_H__

#include <json/json.hpp>
#include <map>

namespace common {
    
/// This struct is the data structure that gets send from the Core to the GUI to 
/// inform the GUI about a change in process status
struct GuiProcessStatus {
    /// The string representing this command type, for usage in the common::GenericMessage
    static constexpr const char* Type = "GuiProcessStatus";

    /// The per-process unique identifier for this process status.
    int id = -1;
    /// The unique identifier for the process that will be created
    int processId = -1;
    /// The application identifier
    std::string applicationId;
    /// The cluster identifier
    std::string clusterId;
    /// The configuration identifier
    int configurationId = -1;
    /// The cluster status
    std::string clusterStatus;
    /// The process status
    std::map<std::string, std::string> nodeStatus;
    /// The time
    double time = -1.0;
};
    
void to_json(nlohmann::json& j, const GuiProcessStatus& p);
void from_json(const nlohmann::json& j, GuiProcessStatus& p);

} // namespace common

#endif // __CORE__GUIPROCESSSTATUS_H__
