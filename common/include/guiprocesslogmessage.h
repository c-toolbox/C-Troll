/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2019                                                             *
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

#ifndef __GUIPROCESSLOGMESSAGE_H__
#define __GUIPROCESSLOGMESSAGE_H__

#include <QJsonDocument>
#include <QString>
#include <QMap>

namespace common {
    
/// This struct is the data structure that gets send from the Core to the GUI to 
/// inform the GUI about a change in process status
struct GuiProcessLogMessage {
    /// The string representing this command type, for usage in the common::GenericMessage
    static const QString Type;
    
    /// Default constructor
    GuiProcessLogMessage() = default;
    
    /**
     * Creates a GuiProcessLogMessage from the passed \p document. The \p document must
     * contain the following keys:
     * \c processId (int)
     * \c applicationId (string)
     * \c clusterId (string)
     * \c message (string)
     * \c type (string)
     * \param document The QJsonDocument that contains the information about this
     * GuiProcessLogMessage
     * \throws std::runtime_error If one of the required keys were not present or of the
     * wrong type
     */
    GuiProcessLogMessage(const QJsonDocument& document);
    
    /**
     * Converts the GuiProcessStatus into a valid QJsonDocument object and returns
     * it.
     * \return The QJsonDocument representing this GuiProcessStatus
     */
    QJsonDocument toJson() const;
    
    /// The per-process unique id for this log message.
    int id;
    /// The unique identifier for the process that will be created
    int processId;
    /// The application identifier
    QString applicationId;
    /// The cluster identifier
    QString clusterId;
    /// The node identifier
    QString nodeId;
    /// The configuration identifier
    int configurationId;
    /// The log message
    QString logMessage;
    /// The output type of log message ("stdout" or "stderr")
    QString outputType;
    /// The time
    double time;
};
    
} // namespace common

#endif // __GUIPROCESSSTATUS_H__
