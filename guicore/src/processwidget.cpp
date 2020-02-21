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

#include "cluster.h"
#include <QVBoxLayout>

namespace {
    std::string statusToString(CoreProcess::ProcessStatus status) {
        switch (status) {
            case CoreProcess::ProcessStatus::Unknown:
                return "Unknown";
            case CoreProcess::ProcessStatus::Starting:
                return "Starting";
            case CoreProcess::ProcessStatus::Running:
                return "Running";
            case CoreProcess::ProcessStatus::NormalExit:
                return "Normal Exit";
            case CoreProcess::ProcessStatus::CrashExit:
                return "Crash Exit";
            case CoreProcess::ProcessStatus::FailedToStart:
                return "Failed To Start";
            case CoreProcess::ProcessStatus::TimedOut:
                return "Timed Out";
            case CoreProcess::ProcessStatus::WriteError:
                return "Write Error";
            case CoreProcess::ProcessStatus::ReadError:
                return "Read Error";
            case CoreProcess::ProcessStatus::UnknownError:
            default:
                return "UnknownError";
        }
    }
} // namespace

ProcessWidget::ProcessWidget(const CoreProcess& process)
    : _process(process)
{
    QBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    QLabel* program = new QLabel(
        QString::fromStdString("Program: " + _process.application.name)
    );
    layout->addWidget(program);

    QLabel* configuration = new QLabel(
        QString::fromStdString("Configuration: " + _process.configuration.name)
    );
    layout->addWidget(configuration);

    QLabel* cluster = new QLabel(
        QString::fromStdString("Cluster: " + _process.cluster.name)
    );
    layout->addWidget(cluster);

    _status = new QLabel(
        QString::fromStdString("Status: " + statusToString(_process.processStatus))
    );
    layout->addWidget(_status);
}

void ProcessWidget::updateStatus() {
    _status->setText(
        QString::fromStdString("Status: " + statusToString(_process.processStatus))
    );
}

int ProcessWidget::processId() const {
    return _process.id;
}


//////////////////////////////////////////////////////////////////////////////////////////


ProcessesWidget::ProcessesWidget() {
    QLayout* layout = new QVBoxLayout;
    setLayout(layout);
}

void ProcessesWidget::processAdded(const CoreProcess& process) {
    // The process has been created, but the widget did not exist yet
    ProcessWidget* w = new ProcessWidget(process);
    _widgets.push_back(w);
    layout()->addWidget(w);
}

void ProcessesWidget::processUpdated(int processId) {
    const auto wIt = std::find_if(
        _widgets.begin(), _widgets.end(),
        [processId](ProcessWidget* w) { return w->processId() == processId; }
    );
    assert(wIt != _widgets.end());
    (*wIt)->updateStatus();
}

void ProcessesWidget::processRemoved(int processId) {
    const auto wIt = std::find_if(
        _widgets.begin(), _widgets.end(),
        [processId](ProcessWidget* w) { return w->processId() == processId; }
    );
    assert(wIt != _widgets.end());

    layout()->removeWidget(*wIt);
    _widgets.erase(wIt);
}
