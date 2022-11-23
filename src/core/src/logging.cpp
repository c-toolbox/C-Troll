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

#include "logging.h"

#include <fmt/format.h>
#include <assert.h>
#include <filesystem>

#ifdef WIN32
#include <Windows.h>
#else
#include <sys/time.h>
#endif

namespace {
    constexpr std::string_view LogPrefix = "log_";
    constexpr std::string_view LogPostfix = ".txt";

    std::string currentTime() {
#ifdef WIN32
        SYSTEMTIME t = {};
        GetLocalTime(&t);

        return fmt::format(
            "{:0>2}:{:0>2}:{:0>2}.{:0<3}", t.wHour, t.wMinute, t.wSecond, t.wMilliseconds
        );
#else
        struct timeval t;
        gettimeofday(&t, nullptr);
        tm* m = gmtime(&t.tv_sec);

        return fmt::format(
            "{:0>2}:{:0>2}:{:0>2}.{:0<3}",
            m->tm_hour, m->tm_min, m->tm_sec, t.tv_usec / 1000
        );
#endif
    }

    std::string currentDate() {
#ifdef WIN32
        SYSTEMTIME t = {};
        GetLocalTime(&t);

        return fmt::format("{:0>4}-{:0>2}-{:0>2}", t.wYear, t.wMonth, t.wDay);
#else
        struct timeval t;
        gettimeofday(&t, nullptr);
        tm* m = gmtime(&t.tv_sec);

        return fmt::format("{:0>4}-{:0>2}-{:0>2}", m->tm_year, m->tm_mon, m->tm_mday);
#endif
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

Log* Log::_log;
    
void Log::initialize(std::string application, bool createLogFile,
                     std::function<void(std::string)> loggingFunction)
{
    assert(!application.empty());
    _log = new Log(std::move(application), createLogFile);
    _log->_loggingFunction = std::move(loggingFunction);
}
    
Log& Log::ref() {
    assert(_log);
    return *_log;
}
    
Log::Log(std::string componentName, bool createLogFile) {
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

    // First the file
    if (!_filePath.empty()) {
        std::unique_lock lock(_access);
        _file << message << '\n';
        _file.flush();
    }

    _loggingFunction(message);

    // And if we are running in Visual Studio, this output, too
#ifdef WIN32
    OutputDebugString((message + '\n').c_str());
#endif
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

} // namespace common

void Log(std::string category, std::string msg) {
    common::Log::ref().logMessage(std::move(category), std::move(msg));
}
