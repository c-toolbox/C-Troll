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

#ifndef __CORE__CONFIGURATION_H__
#define __CORE__CONFIGURATION_H__

#include <json/json.hpp>
#include <chrono>
#include <string>
#include <vector>

struct Color {
    int r = 255;
    int g = 255;
    int b = 255;
};
bool operator==(const Color& lhs, const Color& rhs);

/// This structure represents the configuration loaded at startup
struct Configuration {
    /// The path that contains the JSON objects describing the available applications
    std::string applicationPath = "application";
    /// The path that contains the JSON objects describing the available clusters
    std::string clusterPath = "cluster";
    /// The path that contains the JSON objects describing the available nodes
    std::string nodePath = "nodes";
    /// The timeout after which the information of a successful process is removed
    std::chrono::milliseconds removalTimeout = std::chrono::milliseconds(15000);

    /// The colors that are used for coloring the tags in the side selection widget
    std::vector<Color> tagColors = {
        // Colors taken from https://en.wikipedia.org/wiki/Help:Distinguishable_colors
        Color{ 255, 80, 5 }, // Zinnia
        Color{ 157, 204, 0 }, // Lime
        Color{ 94, 241, 242 }, // Sky
        Color{ 43, 206, 72 }, // Green
        Color{ 240, 163, 255 }, // Amethyst
        Color{ 0, 117, 220 },   // Blue
        Color{ 255, 164, 5 }, // Orpiment
        Color{ 194, 0, 136 }, // Mallow
        Color{ 148, 255, 181 }, // Jade
        Color{ 76, 0, 92 } // Damson
    };
};

void to_json(nlohmann::json& j, const Configuration& c);
void from_json(const nlohmann::json& j, Configuration& c);

#endif // __CORE_CONFIGURATION_H__
