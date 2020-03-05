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

#include "color.h"

#include <array>
#include <assert.h>
#include <map>

namespace {
    // Colors taken from https://en.wikipedia.org/wiki/Help:Distinguishable_colors
    std::array<Color, 25> AllColors = {
        Color{ 255, 255, 128 }, // Xanthin
        Color{ 43, 206, 72 }, // Green
        Color{ 240, 163, 255 }, // Amethyst
        Color{ 0, 117, 220 },   // Blue
        Color{ 255, 164, 5 }, // Orpiment
        Color{ 194, 0, 136 }, // Mallow
        Color{ 148, 255, 181 }, // Jade
        Color{ 76, 0, 92 }, // Damson
        //Color{ 25, 25, 25 }, // Ebony
        Color{ 0, 92, 49 }, // Forest
        Color{ 255, 204, 153 }, // Honeydew
        Color{ 128, 128, 128 }, // Iron
        Color{ 143, 124, 0 }, // Khaki
        Color{ 157, 204, 0 }, // Lime
        Color{ 0, 51, 128 }, // Navy
        Color{ 255, 168, 187 }, // Pink
        Color{ 66, 102, 0 }, // Quagmire
        Color{ 153, 63, 0 }, // Caramel
        Color{ 255, 0, 16 }, // Red
        Color{ 94, 241, 242 }, // Sky
        Color{ 0, 153, 143 }, // Turquoise
        Color{ 224, 255, 102 }, // Uranium
        Color{ 116, 10, 255 }, // Violet
        Color{ 153, 0, 0 }, // Wine
        Color{ 255, 255, 0 }, // Yellow
        Color{ 255, 80, 5 } // Zinnia
    };

    int LastColor = -1;
    std::map<std::string, Color> Colors;

    Color color(const std::string& tag) {
        assert(Colors.find(tag) == Colors.end());

        ++LastColor;
        if (LastColor < AllColors.size()) {
            return AllColors[LastColor];
        }
        else {
            return Color{ 255, 255, 255 };
        }
    }

} // namespace

Color colorForTag(const std::string& tag) {
    if (Colors.find(tag) != Colors.end()) {
        // Someone already requested the color for this tag
        return Colors[tag];
    }
    else {
        // Color not yet requested
        Color c = color(tag);
        Colors[tag] = c;
        return c;
    }
}
