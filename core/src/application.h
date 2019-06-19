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

#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "cluster.h"
#include "program.h"
#include "coreprocess.h"
#include "handler/incomingsockethandler.h"
#include "handler/outgoingsockethandler.h"
#include "traycommand.h"
#include "genericmessage.h"
#include <QProcess>

namespace common {
    struct GuiStartCommand;
    struct GuiProcessCommand;
    struct TrayProcessStatus;
    struct TrayProcessLogMessage;
    class JsonSocket;
} // namespace common

class Application : public QObject {
Q_OBJECT

public:
    Application(std::string configurationFile);

private:
    void initalize(bool resetGUIconnection);
    void deinitalize(bool resetGUIconnection);

    void incomingGuiMessage(const nlohmann::json& message);
    void incomingTrayMessage(const Cluster& cluster, const Cluster::Node& node,
        const nlohmann::json& message);
    
    void handleTrayProcessStatus(const Cluster& cluster, const Cluster::Node& node,
        common::TrayProcessStatus status);
    void handleTrayProcessLogMessage(const Cluster& cluster, const Cluster::Node& node,
        common::TrayProcessLogMessage status);
    void handleIncomingGuiStartCommand(common::GuiStartCommand cmd);
    void handleIncomingGuiProcessCommand(common::GuiProcessCommand cmd);
    void handleIncomingGuiReloadConfigCommand();
    
    common::GenericMessage initializationInformation();
    common::GenericMessage guiProcessLogMessageHistory(const CoreProcess& process);

    void sendGuiProcessStatus(const CoreProcess& process, const std::string& nodeId);
    void sendLatestLogMessage(const CoreProcess& process, const std::string& nodeId);

    void sendTrayCommand(const Cluster& cluster, const common::TrayCommand& command);

    std::vector<Program> _programs;
    std::vector<Cluster> _clusters;
    std::vector<CoreProcess> _processes;
    IncomingSocketHandler _incomingSocketHandler;
    OutgoingSocketHandler _outgoingSocketHandler;

    std::vector<std::unique_ptr<QProcess>> _services;
    std::string _configurationFile;
};

#endif // __APPLICATION_H__
