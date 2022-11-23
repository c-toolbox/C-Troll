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

#include "color.h"

#include <string_view>

namespace {
    constexpr std::string_view KeyTagColorsRed = "r";
    constexpr std::string_view KeyTagColorsGreen = "g";
    constexpr std::string_view KeyTagColorsBlue = "b";
    constexpr std::string_view KeyTagColorsTag = "tag";
} // namespace

bool operator==(const Color& lhs, const Color& rhs) {
    return
        (lhs.r == rhs.r) && (lhs.g == rhs.g) && (lhs.b == rhs.b) && (lhs.tag == rhs.tag);
}

void to_json(nlohmann::json& j, const Color& c) {
    j[KeyTagColorsRed] = c.r;
    j[KeyTagColorsGreen] = c.g;
    j[KeyTagColorsBlue] = c.b;
    if (!c.tag.empty()) {
        j[KeyTagColorsTag] = c.tag;
    }

    if (c.r < 0 || c.r > 255 || c.g < 0 || c.g > 255 || c.b < 0 || c.b > 255) {
        throw std::runtime_error("Color components must be in the range [0, 255]");
    }
}

void from_json(const nlohmann::json& j, Color& c) {
    j.at(KeyTagColorsRed).get_to(c.r);
    j.at(KeyTagColorsGreen).get_to(c.g);
    j.at(KeyTagColorsBlue).get_to(c.b);

    if (j.find(KeyTagColorsTag) != j.end()) {
        j[KeyTagColorsTag].get_to(c.tag);
    }

    if (c.r < 0 || c.r > 255 || c.g < 0 || c.g > 255 || c.b < 0 || c.b > 255) {
        throw std::runtime_error("All color components must be in [0, 255]");
    }
}
