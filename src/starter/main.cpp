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

#include "httplib.h"
#include <fmt/format.h>
#include <iostream>
#include <optional>

namespace {
    constexpr std::string_view HelpMessage =
        "Must execute Starter program with at least four arguments:\n"
        "  1. [string] A valid network name for the location of C-Troll\n"
        "  2. [string] The name of the program that is to be started\n"
        "  3. [string] The name of the cluster on which the program is to be started\n"
        "  4. [string] The name of the configuration for the program to be started\n"
        "  5. [string, optional] The username for a potential authentication of the API\n"
        "  6. [string, optional] The password for a potential authentication of the API\n"
        "The 'program', 'cluster', and 'configuration' must be known to the server\n\n"
        "Example: Starter localhost:8000 Calc Local Default my-user my-password";
} // namespace

int main(int argc, const char** argv) {
    if (argc != 5 && argc != 7) {
        std::cout << HelpMessage;
        exit(EXIT_FAILURE);
    }

    // Extracting commandline parameters
    std::string server = argv[1];
    std::string program = argv[2];
    std::string cluster = argv[3];
    std::string configuration = argv[4];
    std::optional<std::string> username;
    std::optional<std::string> password;
    if (argc > 5) {
        username = argv[5];
        password = argv[6];
    }


    using namespace httplib;
    Client cli = httplib::Client(server);
    cli.set_keep_alive(false);
    cli.set_follow_location(true);
    if (username.has_value() && password.has_value()) {
        cli.set_basic_auth(*username, *password);
    }
    cli.Post(
        "/program/start",
        Params{
            { "program", program },
            { "cluster", cluster },
            { "configuration", configuration}
        }
    );
}
