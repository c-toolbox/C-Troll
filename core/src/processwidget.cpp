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

#include "processwidget.h"

#include "database.h"
#include "processstatusmessage.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

namespace {
    std::string statusToString(common::ProcessStatusMessage::Status status) {
        switch (status) {
            case common::ProcessStatusMessage::Status::Unknown:
                return "Unknown";
            case common::ProcessStatusMessage::Status::Starting:
                return "Starting";
            case common::ProcessStatusMessage::Status::Running:
                return "Running";
            case common::ProcessStatusMessage::Status::NormalExit:
                return "Normal Exit";
            case common::ProcessStatusMessage::Status::CrashExit:
                return "Crash Exit";
            case common::ProcessStatusMessage::Status::FailedToStart:
                return "Failed To Start";
            case common::ProcessStatusMessage::Status::TimedOut:
                return "Timed Out";
            case common::ProcessStatusMessage::Status::WriteError:
                return "Write Error";
            case common::ProcessStatusMessage::Status::ReadError:
                return "Read Error";
            case common::ProcessStatusMessage::Status::UnknownError:
            default:
                return "UnknownError";
        }
    }
} // namespace

ProcessWidget::ProcessWidget(Process::ID processId)
    : _processId(processId)
{
    Process* process = data::findProcess(_processId);
    assert(process);
    Program* program = data::findProgram(process->programId);
    assert(program);
    const Program::Configuration& configuration = data::findConfigurationForProgram(
        *program, process->configurationId
    );
    Cluster* cluster = data::findCluster(process->clusterId);
    assert(cluster);


    QBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    QLabel* programLabel = new QLabel(
        QString::fromStdString("Program: " + program->name)
    );
    layout->addWidget(programLabel);

    QLabel* configurationLabel = new QLabel(
        QString::fromStdString("Configuration: " + configuration.name)
    );
    layout->addWidget(configurationLabel);

    QLabel* clusterLabel = new QLabel(
        QString::fromStdString("Cluster: " + cluster->name)
    );
    layout->addWidget(clusterLabel);

    QLabel* id = new QLabel(QString::number(process->id.v));
    layout->addWidget(id);

    _status = new QLabel(
        QString::fromStdString("Status: " + statusToString(process->status))
    );
    layout->addWidget(_status);
}

void ProcessWidget::updateStatus() {
    Process* process = data::findProcess(_processId);
    _status->setText(
        QString::fromStdString("Status: " + statusToString(process->status))
    );
}


//////////////////////////////////////////////////////////////////////////////////////////


ProcessesWidget::ProcessesWidget() {
    _layout = new QVBoxLayout;
    setLayout(_layout);

    QPushButton* killAll = new QPushButton("Kill all processses");
    _layout->addWidget(killAll);

    connect(killAll, &QPushButton::clicked, this, &ProcessesWidget::killAllProcesses);
}

void ProcessesWidget::processAdded(Process::ID processId) {
    // The process has been created, but the widget did not exist yet
    ProcessWidget* w = new ProcessWidget(processId);
    _widgets[processId] = w;
    _layout->insertWidget(static_cast<int>(_widgets.size() - 1), w);
}

void ProcessesWidget::processUpdated(Process::ID processId) {
    const auto it = _widgets.find(processId);
    assert(it != _widgets.end());
    it->second->updateStatus();
}

void ProcessesWidget::processRemoved(Process::ID processId) {
    const auto it = _widgets.find(processId);
    assert(it != _widgets.end());

    _layout->removeWidget(it->second);
    _widgets.erase(processId);
}
