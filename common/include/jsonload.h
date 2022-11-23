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

#ifndef __COMMON__JSONLOAD_H__
#define __COMMON__JSONLOAD_H__

#include "logging.h"
#include <QDirIterator>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <string>
#include <string_view>
#include <vector>

namespace common {

/**
 * This function loads an object \tparam T from the specified \p jsonFile. \p jsonFile has
 * to be a fully qualified path to the JSON file on disk and \p baseDirectory is the part
 * of the path that is removed to make a unique identifier that is stored in the object.
 * For example: <code>loadFromJson('c/d/e/foobar.json', 'c/d')</code> will result in an
 * identifier <code>e/foobar</code>.
 *
 * \tparam T The type of the object that should be constructed from the JSON file
 * \param jsonFile The path to the JSON file that contains the data for the object T
 * \return A constructed object T that is initialized from the JSON file \p jsonFile
 */
template <typename T>
T loadFromJson(std::string_view jsonFile) {
    std::ifstream f = std::ifstream(std::string(jsonFile));
    nlohmann::json obj;
    f >> obj;
    return T(obj);
}

template <typename T>
void saveToJson(std::string_view filename, const T& value) {
    nlohmann::json obj;
    to_json(obj, value);

    std::ofstream j = std::ofstream(std::string(filename));
    j << obj.dump(2);
}

/**
 * This function reads all <code>.json</code> files in the provided \p directory and
 * creates a list of \tparam T objects from these files. Each file in the directory will
 * lead to a single \tparam T in the result vector. For each of the valid files in
 * \p directory, the loadFromJson function will be called
 *
 * \tparam T The type of the object that is created from the JSON files in \p directory
 * \param directory The path to the directory that contains a list of JSON files, this
 *        directory is traversed recursively
 * \return A list of \tparam T objects that was created from JSON files in \p directory
 */
template <typename T>
std::vector<T> loadJsonFromDirectory(std::string_view directory) {
    namespace fs = std::filesystem;
    if (!fs::is_directory(directory)) {
        throw std::runtime_error(fmt::format("Could not find directory '{}'", directory));
    }

    std::vector<T> res;

    for (const fs::directory_entry& p : fs::recursive_directory_iterator(directory)) {
        if (!p.is_regular_file()) {
            continue;
        }

        const fs::path ext = p.path().extension();
        if (ext != ".json") {
            continue;
        }

        const std::string file = p.path().string();
        ::Log("Status", fmt::format("Loading file '{}'", file));
        try {
            T obj = common::loadFromJson<T>(file);
            res.push_back(std::move(obj));
        }
        catch (const std::runtime_error& e) {
            ::Log("Error",fmt::format("Failed to load file '{}': {}", file, e.what()));
        }
    }

    return res;
}

} // namespace common

#endif // __COMMON__JSONLOAD_H__
