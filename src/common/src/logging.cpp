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

#include "logging.h"

#include <fmt/format.h>
#include <Windows.h>
#include <assert.h>
#include <filesystem>

namespace {
    constexpr std::string_view LogPrefix = "log_";
    constexpr std::string_view LogPostfix = ".txt";

    std::string currentTime() {
        SYSTEMTIME t = {};
        GetLocalTime(&t);

        return fmt::format(
            "{:0>4}-{:0>2}-{:0>2} {:0>2}:{:0>2}:{:0>2}.{:0<3}",
            t.wYear, t.wMonth, t.wDay, t.wHour, t.wMinute, t.wSecond, t.wMilliseconds
        );
    }

    std::string currentDate() {
        SYSTEMTIME t = {};
        GetLocalTime(&t);
        return fmt::format("{:0>4}-{:0>2}-{:0>2}", t.wYear, t.wMonth, t.wDay);
    }

    std::string nextFreeFilename(std::string_view path) {
        std::string_view fileBegin = path.substr(0, path.size() - LogPostfix.size());
        std::string date = currentDate();

        int i = 0;
        while (true) {
            std::string versionPostFix = (i == 0) ? "" : fmt::format("-{}", i);
            std::string newFile = fmt::format(
                "{}_{}{}{}", fileBegin, date, versionPostFix, LogPostfix
            );

            if (!std::filesystem::exists(newFile)) {
                return newFile;
            }

            ++i;
        }
    }
} // namespace

namespace common {

Log* Log::_log = nullptr;

void Log::initialize(std::string application, bool createLogFile, bool shouldLogDebug,
                     std::function<void(std::string)> loggingFunction)
{
    assert(!application.empty());
    _log = new Log(std::move(application), createLogFile, shouldLogDebug);
    _log->_loggingFunction = std::move(loggingFunction);
}

Log* Log::ref() {
    return _log;
}

Log::Log(std::string componentName, bool createLogFile, bool shouldLogDebug)
    : _shouldLogDebug(shouldLogDebug)
{
    assert(!componentName.empty());
    if (createLogFile) {
        _filePath = fmt::format("{}{}{}", LogPrefix, componentName, LogPostfix);
        _file = std::ofstream(_filePath);
    }
}

Log::~Log() {
    _file.close();
}

void Log::logMessage(std::string category, std::string message) {
    message = fmt::format("{}  ({}): {}", currentTime(), category, message);

    if (_log) {
        // First the file
        if (!_log->_filePath.empty()) {
            std::unique_lock lock(_log->_access);
            _log->_file << message << '\n';
            _log->_file.flush();
        }

        _log->_loggingFunction(message);
    }
    else {
        std::cout << message << '\n';
    }

    OutputDebugString((message + '\n').c_str());
}

void Log::logDebugMessage(std::string category, std::string message) {
    if (_log && _log->_shouldLogDebug) {
        logMessage(std::move(category), "{Debug} " + std::move(message));
    }

    // No else case since we don't want to log debug messages before we have a log
}

void Log::performLogRotation(bool keepLog) {
    if (_filePath.empty()) {
        return;
    }

    std::unique_lock lock(_access);

    // First close the old file
    _file.close();

    if (keepLog) {
        std::string newFile = nextFreeFilename(_filePath);
        std::filesystem::copy_file(_filePath, newFile);
    }

    _file = std::ofstream(_filePath, std::ofstream::trunc);
}

bool Log::shouldLogDebugMessage() const {
    return _shouldLogDebug;
}

void Log::setLoggingFunction(std::function<void(std::string)> loggingFunction) {
    _loggingFunction = std::move(loggingFunction);
}

} // namespace common
