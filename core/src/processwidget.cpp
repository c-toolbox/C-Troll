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
#include <QGroupBox>
#include <QLabel>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QTimer>
#include <QVBoxLayout>

namespace {
    std::string statusToString(common::ProcessStatusMessage::Status status) {
        using Status = common::ProcessStatusMessage::Status;
        switch (status) {
            case Status::Unknown:       return "Unknown";
            case Status::Starting:      return "Starting";
            case Status::Running:       return "Running";
            case Status::NormalExit:    return "Normal Exit";
            case Status::CrashExit:     return "Crash Exit";
            case Status::FailedToStart: return "Failed To Start";
            case Status::TimedOut:      return "Timed Out";
            case Status::WriteError:    return "Write Error";
            case Status::ReadError:     return "Read Error";
            case Status::UnknownError:  return "UnknownError";
            default:                    throw std::logic_error("Missing case label");
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

    QBoxLayout* layout = new QHBoxLayout(this);

    layout->addWidget(new QLabel(("Program: " + program->name).c_str()));
    layout->addWidget(new QLabel(("Configuration: " + configuration.name).c_str()));
    layout->addWidget(new QLabel(("Cluster: " + cluster->name).c_str()));
    layout->addWidget(new QLabel(QString::number(process->id.v)));

    _status = new QLabel(("Status: " + statusToString(process->status)).c_str());
    layout->addWidget(_status);

    QWidget* messageContainer = createMessageContainer();

    {
        QPushButton* output = new QPushButton("Output");
        output->setCheckable(true);
        output->setObjectName("output");
        connect(
            output, &QPushButton::clicked,
            [output, messageContainer]() {
                messageContainer->setHidden(!output->isChecked());
            }
        );
        layout->insertWidget(5, output, 0, Qt::AlignRight);
    }
    {
        QPushButton* kill = new QPushButton("Kill");
        kill->setObjectName("kill");
        connect(
            kill, &QPushButton::clicked,
            [this]() { emit this->kill(_processId); }
        );
        layout->insertWidget(6, kill, 0, Qt::AlignRight);
    }
    {
        _remove = new QPushButton("Remove");
        _remove->setObjectName("removeprocess");
        _remove->setEnabled(false);
        connect(
            _remove, &QPushButton::clicked,
            [this]() {
            _removalTimer->stop();
            emit remove(_processId);
        }
        );
        layout->insertWidget(7, _remove, 0, Qt::AlignRight);
    }
    
    {
        _removalTimer = new QTimer();
        _removalTimer->setSingleShot(true);
        connect(
            _removalTimer, &QTimer::timeout,
            [this, messageContainer]() {
                if (messageContainer->isVisible()) {
                    _removalTimer->start(15000);
                }
                else {
                    emit remove(_processId);
                }
            }
        );
    }
}

QWidget* ProcessWidget::createMessageContainer() {
    QWidget* container = new QWidget;
    container->setMinimumSize(1200, 500);
    QVBoxLayout* containerLayout = new QVBoxLayout(container);

    {
        QGroupBox* messages = new QGroupBox("Stdout");
        QVBoxLayout* l = new QVBoxLayout(messages);
        l->setContentsMargins(0, 0, 0, 0);
        _messages = new QPlainTextEdit;
        _messages->setReadOnly(true);
        _messages->setCenterOnScroll(true);
        l->addWidget(_messages);
        containerLayout->addWidget(messages);
    }
    {
        QGroupBox* messages = new QGroupBox("Stderr");
        QVBoxLayout* l = new QVBoxLayout(messages);
        l->setContentsMargins(0, 0, 0, 0);

        _errorMessages = new QPlainTextEdit;
        _errorMessages->setReadOnly(true);
        _messages->setCenterOnScroll(true);
        l->addWidget(_errorMessages);
        containerLayout->addWidget(messages);
    }

    containerLayout->setStretch(0, 3);
    containerLayout->setStretch(1, 1);

    return container;
}

void ProcessWidget::updateStatus() {
    Process* process = data::findProcess(_processId);
    _status->setText(("Status: " + statusToString(process->status)).c_str());

    if (process->status == common::ProcessStatusMessage::Status::NormalExit) {
        // Start a timer to automatically remove a process if it exited normally
        _removalTimer->start(15000);
    }

    // The user should only be able to remove the process entry if the process has
    // finished in some state
    _remove->setEnabled(process->status != common::ProcessStatusMessage::Status::Running);
}

void ProcessWidget::addMessage(common::ProcessOutputMessage message) {
    std::string msg = message.message;
    // Some of the incoming messages might have a newline character at the end, but we
    // want to normalize that
    if (msg.back() != '\n') {
        msg.push_back('\n');
    }
    if (message.outputType == common::ProcessOutputMessage::OutputType::StdOut) {
        _messages->insertPlainText(QString::fromStdString(msg));
        _messages->ensureCursorVisible();
    }
    else {
        _errorMessages->insertPlainText(QString::fromStdString(msg));
        _errorMessages->ensureCursorVisible();
    }
}


//////////////////////////////////////////////////////////////////////////////////////////


ProcessesWidget::ProcessesWidget() {
    QBoxLayout* layout = new QVBoxLayout(this);
    
    QScrollArea* area = new QScrollArea;
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    QWidget* content = new QWidget;
    area->setWidget(content);

    _contentLayout = new QVBoxLayout(content);
    area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
    //_contentLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    layout->addWidget(area);

    _contentLayout->addStretch();

    QPushButton* killAll = new QPushButton("Kill all processses");
    connect(killAll, &QPushButton::clicked, this, &ProcessesWidget::killAllProcesses);
    layout->addWidget(killAll);
}

void ProcessesWidget::receivedProcessMessage(Node::ID,
                                             common::ProcessOutputMessage message)
{
    Process::ID pid =  Process::ID(message.processId);
    const auto it = _widgets.find(pid);
    assert(it != _widgets.end());
    it->second->addMessage(std::move(message));
}

void ProcessesWidget::processAdded(Process::ID processId) {
    // The process has been created, but the widget did not exist yet
    ProcessWidget* w = new ProcessWidget(processId);
    w->setMinimumWidth(width());
    connect(w, &ProcessWidget::remove, this, &ProcessesWidget::processRemoved);
    connect(w, &ProcessWidget::kill, this, &ProcessesWidget::killProcess);
    _widgets[processId] = w;
    _contentLayout->insertWidget(static_cast<int>(_widgets.size() - 1), w);
    //layout()->update();
}

void ProcessesWidget::processUpdated(Process::ID processId) {
    const auto it = _widgets.find(processId);

    if (it == _widgets.end()) {
        // The only reason why the widget might not exist if:
        // 1. The process is part of a cluster
        // 2. This particular process was killed/terminated
        // 3. The QTimer above ran out and the widget was destroyed
        // 4. We are restarting the process

        Process* p = data::findProcess(processId);
        assert(p->status == common::ProcessStatusMessage::Status::Starting);
        processAdded(processId);
    }
    else {
        it->second->updateStatus();
    }
}

void ProcessesWidget::processRemoved(Process::ID processId) {
    const auto it = _widgets.find(processId);

    if (it != _widgets.end()) {
        // We need to check this because there might be timer running in the background
        // that will try to remove a process while the user has removed the same process
        // manually in the meantime
        it->second->deleteLater();
        _contentLayout->removeWidget(it->second);
        _widgets.erase(processId);
    }
}
