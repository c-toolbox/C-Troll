/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016                                                                    *
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

#ifndef __GUICOMMAND_H__
#define __GUICOMMAND_H__

#include <QJsonDocument>
#include <QString>

namespace common {

/// This struct is the data structure that gets send from the GUI to the Core
/// to signal that the Core should perform a task for a specific cluster
struct GuiCommand {
    /// The string representing this command type, for usage in the common::GenericMessage
    static const QString Type;
    
    /// Default constructor
    GuiCommand() = default;

    /**
     * Creates a GuiCommand from the passed \p document. The \p document must contain
     * all of the following keys, all of type string:
     * \c command
     * \c application_identifier
     * \c configuration_identifier
     * \c cluster_identifier
     * \param document The QJsonDocument that contains the information about this
     * CoreCommand
     * \throws std::runtime_error If one of the required keys were not present or of the
     * wrong type
     */
    GuiCommand(const QJsonDocument& document);

    /**
     * Converts the GuiCommand into a valid QJsonDocument object and returns it.
     * \return the QJsonDocument representing this GuiCommand
     */
    QJsonDocument toJson() const;

    /// The kind of command that is to be executed
    QString command;
    /// The unique identifier of the application that is to be started
    QString applicationId;
    /// The identifier of the application's configuration that is to be started
    QString configurationId;
    /// The identifier of the cluster on which the application is to be started
    QString clusterId;
};

} // namespace

#endif // __GUICOMMAND_H__
