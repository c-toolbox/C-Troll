/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2019                                                             *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
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

#ifndef __JSONLOAD_H__
#define __JSONLOAD_H__

#include "logging.h"
#include <filesystem>
#include <string>
#include <vector>
#include <QDirIterator>

namespace common {

template <typename T>
T loadFromJson(const std::string& jsonFile, const std::string& baseDirectory) {
    std::string id = std::filesystem::relative(jsonFile, baseDirectory).string();

    // Remove the last 5 characters '.json'
    id = id.substr(0, id.length() - 5);

#ifdef WIN32
    std::replace(id.begin(), id.end(), '\\', '/');
#endif // WIN32

    std::ifstream f(jsonFile);
    nlohmann::json obj;
    f >> obj;
    obj["id"] = id;

    return T(obj);
}

template <typename T>
std::vector<T> loadJsonFromDirectory(const std::string& directory, const std::string& type) {
    std::vector<T> res;

    namespace fs = std::filesystem;
    for (const fs::directory_entry& p : fs::recursive_directory_iterator(directory)) {
        if (p.is_regular_file()) {
            fs::path ext = p.path().extension();
            if (ext == ".json") {
                std::string file = p.path().string();
                ::Log("Loading " + type + " file " + file);
                try {
                    T obj = common::loadFromJson<T>(file, directory);
                    res.push_back(std::move(obj));
                }
                catch (const std::runtime_error& e) {
                    ::Log("Failed to load " + type + " file " + file + ". " + e.what());
                }
            }
        }
    }

    return res;
}

} // namespace common

#endif // __JSONLOAD_H__
