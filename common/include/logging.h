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

#ifndef __COMMON__LOGGING_H__
#define __COMMON__LOGGING_H__

#include <fstream>
#include <functional>
#include <mutex>
#include <string>

namespace common {
    
/**
 * This static class provides the ability to log information to both the console and a
 * persistent file. The file will be created in working directory of the application will
 * is partially determined by the \c application variable that is passed to the initialize
 * function. Every following call to logMessage will be relegated to both a file that is
 * called <code>log_{application}.txt</code.> as well as the console. Every time the log
 * file is created, the old contents will be silently overwritten.
 */
class Log {
public:
    /**
     * Initializes the static Log and opens the log file for reading.
     *
     * \param application The name of the application. This is used for creating
     *        unique(ish) names for the log.
     * \param createLogFile Determines whether all log messages should also be printed
     *        to file
     * \param loggingFunction This callback function is called whenever a message is
     *        logged
     */
    static void initialize(std::string application,
        bool createLogFile, std::function<void(std::string)> loggingFunction);
    
    /**
     * Returns the static reference to the Log instance.
     *
     * \return The static reference to the Log instance
     * \pre Log::initialize needs to be called before the first call to Log::ref
     */
    static Log& ref();
 
    /**
     * Logs a message with the Log. This message is both logged to the log file as well
     * as to the console using the \c qDebug macro. Every content to the file is flushed
     * immediately.
     *
     * \param message The message that is to be logged
     */
    void logMessage(std::string category, std::string message);

    /**
     * Performs a log rotation action. If \p keepLog is \c true, the old log file is saved
     * to a new filename by adding the current date. If the parameter is \c false, the old
     * log file is discarded.
     */
    void performLogRotation(bool keepLog);
  
private:
    /**
     * Constructs a Log and opens the file for reading, overwriting any old content that
     * was in the file previously.
     *
     * \param application The name of the application that requested the log file
     * \param createLogFile Determines whether all log messages should also be printed
     *        to file
     */
    Log(std::string application, bool createLogFile);
    
    /// Destructor the will close the file.
    ~Log();

    // The static Log that is returned in the Log::ref method.
    static Log* _log;
    
    /// Mutex that protects the access to the log file
    std::mutex _access;

    /// The log file to which all messages from the logMessage method get logged
    std::string _filePath;
    std::ofstream _file;

    std::function<void(std::string)> _loggingFunction;
};
    
} // namespace common

/**
 * This method is a shortcut for a more convenient logging. Calling this function is
 * equivalent to calling <code>common::Log::ref().logMessage(msg)</code>.
 *
 * \param message The message that is to be logged and passed to the Log::logMessage
 *        function
 */
void Log(std::string category, std::string message);

#endif // __COMMON__LOGGING_H__
