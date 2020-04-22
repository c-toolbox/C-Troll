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

#include "logging.h"

#include <assert.h>

#ifdef WIN32
#include <Windows.h>
#endif

namespace {
    constexpr const char* LogPrefix = "log_";
    constexpr const char* LogPostfix = ".txt";
} // namespace

namespace common {

Log* Log::_log;
    
void Log::initialize(std::string application,
                     std::function<void(std::string)> loggingFunction)
{
    assert(!application.empty());
    _log = new Log(std::move(application));
    _log->_loggingFunction = std::move(loggingFunction);
}
    
Log& Log::ref() {
    assert(_log);
    return *_log;
}
    
Log::Log(std::string componentName) : _file(LogPrefix + componentName + LogPostfix) {
    assert(!componentName.empty());
}
    
Log::~Log() {
    _file.close();
}
    
void Log::logMessage(std::string message) {
    // First the file
    _file << message << '\n';
    _file.flush();

    _loggingFunction(message);

    // And if we are running in Visual Studio, this output, too
#ifdef WIN32
    message = message + "\n";
    OutputDebugString(message.c_str());
#endif
}
    
} // namespace common

void Log(std::string msg) {
    common::Log::ref().logMessage(std::move(msg));
}
