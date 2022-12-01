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

#include "centralwidget.h"

#include "apiversion.h"
#include "logging.h"
#include "version.h"
#include <QGroupBox>
#include <QVBoxLayout>
#include <fmt/format.h>

CentralWidget::CentralWidget() {
    QLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    _messageBox = new QTextEdit;
    layout->addWidget(_messageBox);

    QGroupBox* connectionsWidget = new QGroupBox("Connected controllers");
    _connectionsLayout = new QVBoxLayout(connectionsWidget);
    layout->addWidget(connectionsWidget);

    QGroupBox* processesWidget = new QGroupBox("Processes");
    _processesLayout = new QVBoxLayout(processesWidget);
    layout->addWidget(processesWidget);

    layout->addWidget(createInfoWidget());
}

void CentralWidget::setPort(int port) {
    _portLabel->setText(QString::fromStdString(fmt::format("Port: {}", port)));
}

void CentralWidget::log(std::string msg) {
    _messageBox->append(QString::fromStdString(msg));
    _messageBox->ensureCursorVisible();
}

void CentralWidget::newConnection(const std::string& peerAddress) {
    Debug("CentralWidget", fmt::format("Opened connection to {}", peerAddress));
    QLabel* label = new QLabel(QString::fromStdString(peerAddress));
    _connectionsLayout->addWidget(label);
    _connections[peerAddress] = label;
}

void CentralWidget::closedConnection(const std::string& peerAddress) {
    Debug("CentralWidget", fmt::format("Closed connection to {}", peerAddress));

    const auto it = _connections.find(peerAddress);
    assert(it != _connections.end());

    it->second->deleteLater();
    _connectionsLayout->removeWidget(it->second);
    _connections.erase(it);
}

void CentralWidget::newProcess(ProcessHandler::ProcessInfo process) {
    Debug(
        "CentralWidget",
        fmt::format("New process: {}, {}", process.processId, process.executable)
    );
    std::string text = fmt::format("{}: {}", process.processId, process.executable);
    QLabel* label = new QLabel(QString::fromStdString(text));
    _processesLayout->addWidget(label);
    _processes[process.processId] = label;
}

void CentralWidget::endedProcess(ProcessHandler::ProcessInfo process) {
    Debug(
        "CentralWidget",
        fmt::format("Close process: {}, {}", process.processId, process.executable)
    );
    const auto it = _processes.find(process.processId);
    // The processId might not exist yet if the process starting fails (for example if the
    // requested executable does not exist)
    if (it != _processes.end()) {
        Debug("CentralWidget", "Found in list");
        it->second->deleteLater();
        _processesLayout->removeWidget(it->second);
        _processes.erase(it);
    }
}

QWidget* CentralWidget::createInfoWidget() {
    QWidget* info = new QWidget;
    info->setObjectName("info");
    QBoxLayout* infoLayout = new QHBoxLayout;
    infoLayout->setContentsMargins(5, 1, 5, 5);
    info->setLayout(infoLayout);

    std::string trayVer = fmt::format("Tray Version: {}", Version);
    infoLayout->addWidget(new QLabel(QString::fromStdString(trayVer)));

    infoLayout->addStretch();
    _portLabel = new QLabel;
    infoLayout->addWidget(_portLabel);
    infoLayout->addStretch();

    std::string apiVer = fmt::format("API Version: {}", api::Version);
    infoLayout->addWidget(new QLabel(QString::fromStdString(apiVer)));

    return info;
}
