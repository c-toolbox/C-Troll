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

#include "centralwidget.h"

#include "apiversion.h"
#include "version.h"
#include <QGroupBox>
#include <QVBoxLayout>

CentralWidget::CentralWidget() {
    QLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);

    _messageBox = new QTextEdit();
    layout->addWidget(_messageBox);

    QGroupBox* connectionsWidget = new QGroupBox("Connected controllers");
    _connectionsLayout = new QVBoxLayout(connectionsWidget);
    layout->addWidget(connectionsWidget);

    QGroupBox* processesWidget = new QGroupBox("Processes");
    _processesLayout = new QVBoxLayout(processesWidget);
    layout->addWidget(processesWidget);

    QWidget* info = createInfoWidget();
    layout->addWidget(info);
}

void CentralWidget::setPort(int port) {
    QString p = "Port: " + QString::number(port);
    _portLabel->setText(p);
}

void CentralWidget::log(std::string msg) {
    _messageBox->append(QString::fromStdString(msg));
    _messageBox->ensureCursorVisible();
}

void CentralWidget::newConnection(const std::string& peerAddress) {
    QLabel* label = new QLabel(QString::fromStdString(peerAddress));
    _connectionsLayout->addWidget(label);
    _connections[peerAddress] = label;
}

void CentralWidget::closedConnection(const std::string& peerAddress) {
    const auto it = _connections.find(peerAddress);
    assert(it != _connections.end());

    it->second->deleteLater();
    _connectionsLayout->removeWidget(it->second);
    _connections.erase(it);
}

void CentralWidget::newProcess(ProcessHandler::ProcessInfo process) {
    std::string text = std::to_string(process.processId) + ": " + process.executable;
    QLabel* label = new QLabel(QString::fromStdString(text));
    _processesLayout->addWidget(label);
    _processes[process.processId] = label;
}

void CentralWidget::endedProcess(ProcessHandler::ProcessInfo process) {
    const auto it = _processes.find(process.processId);
    assert(it != _processes.end());

    it->second->deleteLater();
    _processesLayout->removeWidget(it->second);
    _processes.erase(it);
}

QWidget* CentralWidget::createInfoWidget() {
    using namespace std::string_literals;

    QWidget* info = new QWidget;
    info->setObjectName("info");
    QBoxLayout* infoLayout = new QHBoxLayout;
    infoLayout->setContentsMargins(5, 1, 5, 5);
    info->setLayout(infoLayout);

    QLabel* trayVersion = new QLabel(QString::fromStdString("Tray Version: "s + Version));
    infoLayout->addWidget(trayVersion);

    infoLayout->addStretch();
    _portLabel = new QLabel;
    infoLayout->addWidget(_portLabel);
    infoLayout->addStretch();

    QLabel* apiVersion = new QLabel(
        QString::fromStdString("API Version: "s + api::Version)
    );
    infoLayout->addWidget(apiVersion);

    return info;
}
