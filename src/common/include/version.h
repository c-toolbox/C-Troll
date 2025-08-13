/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2025                                                               *
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

#ifndef __COMMON__VERSION_H__
#define __COMMON__VERSION_H__

#include <string_view>

// The overall version of the applications. This version number should be updated
// according to the following rules. The overarching design is that applications of
// the same major version should be able to interoperate, but applications of different
// major version do not have to be able to interoperate. This should be the case for both
// C-Troll <-> C-Troll interaction but also for C-Troll <-> Editor interactions across
// different version numbers.
//
// Update to major version
//   - A change was introduced that requires any change to existing configuration files
//   - The major version of the API version was increased
//   - Any user-facing functionality that existed before has been removed
//
// Update the minor version
//   - A new feature was added to the applications that does not require user intervention
//     for their existing files
//   - The minor version of the API was increased
//
// Update to the patch version
//   - For bugfixes only
//   - If the patch version of the API was increased


namespace app {
    constexpr int MajorVersion = 2;
    constexpr int MinorVersion = 0;
    constexpr int PatchVersion = 1;

    constexpr std::string_view Version = "2.0.1";
} // namespace app


// This is the version of the JSON based socket API and its messages that are used to
// communicate between C-Troll and the Tray. This version number should be updated
// according to the following rules.
//
// Update to major version
//   - A new required message was added
//   - A previously existing message was removed
//   - The format of a previously existing message was changed making the new message
//     not readable with the previous version
//
// Update to minor version
//   - A new optional message was added
//   - The format of an existing message was changed but the message is still readable
//     with the previous version
//
// Update to patch version
//   - For bugfixes only

namespace api {
    constexpr int MajorVersion = 2;
    constexpr int MinorVersion = 0;
    constexpr int PatchVersion = 0;

    constexpr std::string_view Version = "2.0.0";
} // namespace api

#endif // __COMMON__VERSION_H__
