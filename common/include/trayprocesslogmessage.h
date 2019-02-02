/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
 * Alexander Bock, Erik Sunden, Emil Axelsson                                            *
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

#ifndef __TRAYPROCESSLOGMESSAGE_H__
#define __TRAYPROCESSLOGMESSAGE_H__

#include <QJsonDocument>
#include <QString>

namespace common {
    
/// This struct is the data structure that gets send from the Core to the Tray to signal
/// that the Tray should perform a task
struct TrayProcessLogMessage {
    /// The string representing this command type, for usage in the common::GenericMessage
    static const QString Type;

    enum class OutputType : int {
        StdOut = 0,
        StdErr
    };

    /// Default constructor
    TrayProcessLogMessage() = default;
    
    /**
     * Creates a TrayProcessLogMessage from the passed \p document. The \p document must
     * contain the following keys, all of type string:
     * \c identifier
     * \c message
     * \c type
     * \param document The QJsonDocument that contains the information about this
     * TrayProcessLogMessage
     * \throws std::runtime_error If one of the required keys were not present or of the
     * wrong type
     */
    TrayProcessLogMessage(const QJsonDocument& document);
    
    /**
     * Converts the TrayProcessLogMessage into a valid QJsonDocument object and returns
     * it.
     * \return The QJsonDocument representing this TrayProcessLogMessage
     */
    QJsonDocument toJson() const;
    
    /// The unique identifier for the process
    int processId;
    /// The process stdout/stderr line
    QString message;
    /// The type of output
    OutputType outputType;
};
    
} // namespace common

#endif // __TRAYPROCESSLOGMESSAGE_H__
