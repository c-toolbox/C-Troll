/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2020                                                             *
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

#include "processwidget.h"

#include "database.h"
#include "processstatusmessage.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QStyle>
#include <QTimer>
#include <QVBoxLayout>
#include <fmt/format.h>

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

ProcessWidget::ProcessWidget(Process::ID processId,
                             const std::chrono::milliseconds& timeout)
    : _processId(processId)
    , _timeout(timeout)
{
    const Process* process = data::findProcess(_processId);
    assert(process);
    const Program* program = data::findProgram(process->programId);
    assert(program);
    const Program::Configuration* configuration = data::findConfigurationForProgram(
        *program,
        process->configurationId
    );
    assert(configuration);
    const Cluster* cluster = data::findCluster(process->clusterId);
    assert(cluster);

    const Node* node = data::findNode(process->nodeId);
    assert(node);


    _programInfo = new QLabel(QString::fromStdString(program->name));
    _configurationInfo = new QLabel(QString::fromStdString(configuration->name));
    _clusterInfo = new QLabel(QString::fromStdString(cluster->name));
    _nodeInfo = new QLabel(QString::fromStdString(node->name));
    _processIdInfo = new QLabel(QString::number(process->id.v));
    _status = new QLabel(QString::fromStdString(statusToString(process->status)));
    _status->setObjectName("status");

    _messageContainer = createMessageContainer();

    {
        _showOutput = new QPushButton("Output");
        _showOutput->setCheckable(true);
        _showOutput->setEnabled(program->shouldForwardMessages);
        connect(
            _showOutput, &QPushButton::clicked,
            [output = _showOutput, messageContainer = _messageContainer]() {
                messageContainer->setHidden(!output->isChecked());
            }
        );
    }
    {
        _killProcess = new QPushButton("Kill");
        _killProcess->setObjectName("kill");
        connect(
            _killProcess, &QPushButton::clicked,
            [this, program, configuration, cluster]() {
                std::string text = fmt::format(
                    "Are you sure you want to kill '{}/{}' running on cluster '{}'?",
                    program->name, configuration->name, cluster->name
                );

                QMessageBox box;
                box.setText("Kill process");
                box.setInformativeText(QString::fromStdString(text));
                box.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
                box.setDefaultButton(QMessageBox::Ok);
                const int res = box.exec();

                if (res == QMessageBox::Ok) {
                    _killProcess->setEnabled(false);
                    emit kill(_processId);
                }
            }
        );
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
    }
    
    _removalTimer = new QTimer(this);
    _removalTimer->setSingleShot(true);
    connect(
        _removalTimer, &QTimer::timeout,
        [this, messageContainer = _messageContainer]() {
            if (messageContainer->isVisible()) {
                _removalTimer->start(_timeout);
            }
            else {
                emit remove(_processId);
            }
        }
    );
}

void ProcessWidget::addToLayout(QGridLayout* layout, int row) {
    layout->addWidget(_programInfo, row, 0, Qt::AlignCenter);
    layout->addWidget(_configurationInfo, row, 1, Qt::AlignCenter);
    layout->addWidget(_clusterInfo, row, 2, Qt::AlignCenter);
    layout->addWidget(_nodeInfo, row, 3, Qt::AlignCenter);
    layout->addWidget(_processIdInfo, row, 4, Qt::AlignCenter);
    layout->addWidget(_status, row, 6, Qt::AlignCenter);
    layout->addWidget(_showOutput, row, 7);
    layout->addWidget(_killProcess, row, 8);
    layout->addWidget(_remove, row, 9);
}

ProcessWidget::~ProcessWidget() {
    delete _programInfo;
    delete _configurationInfo;
    delete _clusterInfo;
    delete _nodeInfo;
    delete _processIdInfo;
    delete _status;
    delete _showOutput;
    delete _killProcess;
    delete _remove;
    delete _messageContainer;
}

void ProcessWidget::removeFromLayout(QGridLayout* layout) {
    layout->removeWidget(_programInfo);
    layout->removeWidget(_configurationInfo);
    layout->removeWidget(_clusterInfo);
    layout->removeWidget(_processIdInfo);
    layout->removeWidget(_status);
    layout->removeWidget(_showOutput);
    layout->removeWidget(_killProcess);
    layout->removeWidget(_remove);
}

QWidget* ProcessWidget::createMessageContainer() {
    QWidget* container = new QWidget;
    std::string title = fmt::format("C-Troll | Process: {}", _processId.v);
    container->setWindowTitle(QString::fromStdString(title));

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
    const Process* p = data::findProcess(_processId);
    _status->setText(QString::fromStdString(statusToString(p->status)));
    _status->setProperty("state", QString::fromStdString(statusToString(p->status)));
    _status->style()->unpolish(_status);
    _status->style()->polish(_status);

    if (p->status == common::ProcessStatusMessage::Status::NormalExit) {
        // Start a timer to automatically remove a process if it exited normally
        _removalTimer->start(15000);
    }

    // The user should only be able to remove the process entry if the process has
    // finished in some state
    _remove->setEnabled(p->status != common::ProcessStatusMessage::Status::Running);
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


ProcessesWidget::ProcessesWidget(const std::chrono::milliseconds& processTimeout)
    : _processTimeout(processTimeout)
{
    QBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 2, 2, 2);

    QScrollArea* area = new QScrollArea;
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    QWidget* content = new QWidget;
    area->setWidget(content);
    layout->addWidget(area);

    _contentLayout = new QGridLayout(content);
    _contentLayout->addWidget(new QLabel("Program"), 0, 0, Qt::AlignCenter);
    _contentLayout->addWidget(new QLabel("Configuration"), 0, 1, Qt::AlignCenter);
    _contentLayout->addWidget(new QLabel("Cluster"), 0, 2, Qt::AlignCenter);
    _contentLayout->addWidget(new QLabel("Node"), 0, 3, Qt::AlignCenter);
    _contentLayout->addWidget(new QLabel("Process ID"), 0, 4, Qt::AlignCenter);
    // column 5 is the spacer used for separating the static info from the current info
    QWidget* spacer = new QWidget;
    spacer->setObjectName("spacer");
    _contentLayout->addWidget(spacer, 0, 5, Qt::AlignCenter);
    _contentLayout->addWidget(new QLabel("Status"), 0, 6, Qt::AlignCenter);
    _contentLayout->addWidget(new QLabel("Show Output"), 0, 7, Qt::AlignCenter);
    _contentLayout->addWidget(new QLabel("Kill Process"), 0, 8, Qt::AlignCenter);
    _contentLayout->addWidget(new QLabel("Remove"), 0, 9, Qt::AlignCenter);
    _contentLayout->setRowStretch(1, 1);

    QPushButton* killAll = new QPushButton("Kill all processses");
    connect(killAll, &QPushButton::clicked, this, &ProcessesWidget::killAllProcesses);
    layout->addWidget(killAll);
}

void ProcessesWidget::receivedProcessMessage(Node::ID, common::ProcessOutputMessage msg) {
    Process::ID pid =  Process::ID(msg.processId);
    const auto it = _widgets.find(pid);
    assert(it != _widgets.end());
    it->second->addMessage(std::move(msg));
}

void ProcessesWidget::processAdded(Process::ID processId) {
    // The process has been created, but the widget did not exist yet
    ProcessWidget* w = new ProcessWidget(processId, _processTimeout);
    w->setMinimumWidth(width());
    connect(w, &ProcessWidget::remove, this, &ProcessesWidget::processRemoved);
    connect(w, &ProcessWidget::kill, this, &ProcessesWidget::killProcess);
    _widgets[processId] = w;
    const int n = static_cast<int>(_widgets.size());

    // The last row has to be unstretched
    _contentLayout->setRowStretch(n, 0);
    w->addToLayout(_contentLayout, n);
    _contentLayout->setRowStretch(n + 1, 1);
}

void ProcessesWidget::processUpdated(Process::ID processId) {
    const auto it = _widgets.find(processId);

    if (it == _widgets.end()) {
        // The only reason why the widget might not exist if:
        // 1. The process is part of a cluster
        // 2. This particular process was killed/terminated
        // 3. The QTimer above ran out and the widget was destroyed
        // 4. We are restarting the process

        const Process* p = data::findProcess(processId);
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
