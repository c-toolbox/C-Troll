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

#include "programwidget.h"

#include "cluster.h"
#include "color.h"
#include "database.h"
#include "logging.h"
#include <fmt/format.h>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QVBoxLayout>
#include <set>

namespace programs {

ProgramButton::ProgramButton(const Cluster* cluster,
                             const Program::Configuration* configuration)
    : QPushButton(configuration->name.c_str())
    , _cluster(cluster)
    , _configuration(configuration)
{
    _actionMenu = new QMenu(this);

    setEnabled(false);

    connect(this, &QPushButton::clicked, this, &ProgramButton::handleButtonPress);
}

void ProgramButton::updateStatus() {
    std::vector<Node*> nodes = data::findNodesForCluster(*_cluster);

    const bool allConnected = std::all_of(
        nodes.begin(), nodes.end(),
        std::mem_fn(&Node::isConnected)
    );
    setEnabled(allConnected);
}

void ProgramButton::processUpdated(Process::ID processId) {
    Process* process = data::findProcess(processId);

    auto it = std::find_if(
        _processes.begin(), _processes.end(),
        [processId](const std::pair<const Node::ID, ProcessInfo>& p) {
            return p.second.processId.v == processId.v;
        }
    );
    if (it == _processes.end()) {
        // This is a brand new process, so it better be in a Starting status
        assert(process->status == common::ProcessStatusMessage::Status::Starting);

        ProcessInfo info;
        info.processId = processId;
        Node* node = data::findNode(process->nodeId);
        info.menuAction = new QAction(node->name.c_str());
        // We store the name of the node as the user data in order to sort them later
        info.menuAction->setData(node->name.c_str());
        _processes[process->nodeId] = info;
    }
    else {
        // This is a process that already exists and we should update it depending on the
        // status of the incoming process
        assert(it->second.processId.v == processId.v);
    }

    updateButton();
}

void ProgramButton::handleButtonPress() {
    // This slot should only be invoked if either all processes are running, or no process
    // are running
    assert(hasNoProcessRunning() || hasAllProcessesRunning());

    if (hasNoProcessRunning()) {
        emit startProgram(_configuration->id);
        
        // We disable the button until we get another message back from the tray
        setEnabled(false);
    }
    else if (hasAllProcessesRunning()) {
        emit stopProgram(_configuration->id);

        // We disable the button until we get another message back from the tray
        setEnabled(false);
    }
    else {
        // This slot should only be invoked if either all processes are running, or no
        // process are running
        assert(false);
    }
}

void ProgramButton::updateButton() {
    setEnabled(true);

    // If all processes don't exist or are in NormalExit/CrashExit/FailedToStart, we show
    // the regular start button without any menus attached
    if (hasNoProcessRunning()) {
        setMenu(nullptr);
        setObjectName("start"); // used in the QSS sheet to style this button
        // @TODO (abock, 2020-02-25) Replace when putting the QSS in place
        setText(_cluster->name.c_str());
    }
    else if (hasAllProcessesRunning()) {
        setMenu(nullptr);
        setObjectName("stop"); // used in the QSS sheet to style this button
        // @TODO (abock, 2020-02-25) Replace when putting the QSS in place
        setText(("Stop:" + _cluster->name).c_str());
    }
    else {
        setMenu(_actionMenu);
        setObjectName("mixed"); // used in the QSS sheet to style this button
        // @TODO (abock, 2020-02-25) Replace when putting the QSS in place
        setText(("Mixed:" + _cluster->name).c_str());

        updateMenu();
    }
}

void ProgramButton::updateMenu() {
    // First a bit of cleanup so that we don't have old signal connections laying around
    for (const std::pair<Node::ID, ProcessInfo>& p : _processes) {
        QObject::disconnect(p.second.menuAction);
    }
    _actionMenu->clear();

    std::vector<QAction*> actions;
    std::transform(
        _processes.begin(), _processes.end(),
        std::back_inserter(actions),
        [](const std::pair<const Node::ID, ProcessInfo>& p) {
            return p.second.menuAction;
        }
    );
    std::sort(
        actions.begin(), actions.end(),
        [](QAction* lhs, QAction* rhs) {
            return lhs->data().toString() < rhs->data().toString();
        }
    );

    for (QAction* action : actions) {
        const std::string nodeName = action->data().toString().toLocal8Bit().constData();
        const auto it = std::find_if(
            _processes.begin(), _processes.end(),
            [nodeName](const std::pair<const Node::ID, ProcessInfo>& p) {
                Node* node = data::findNode(p.first);
                return node->name == nodeName;
            }
        );
        // If we are getting this far, the node for this action has to exist in the map
        assert(it != _processes.end());

        // We only going to update the actions if some of the nodes are not running but
        // some others are. So we basically have to provide the ability to start the nodes
        // that are currently not running and close the ones that currently are
        if (isProcessRunning(it->first)) {
            setObjectName("stop"); // used in the QSS sheet to style this button
            connect(
                action, &QAction::triggered,
                [this, id = it->second.processId]() { emit stopProcess(id); }
            );

            // @TODO (abock, 2020-02-25) Replace when putting the QSS in place
            action->setText("Stop: " + action->data().toString());
        }
        else {
            setObjectName("restart"); // used in the QSS sheet to style this button
            connect(
                action, &QAction::triggered,
                [this, id = it->second.processId]() { emit restartProcess(id); }
            );

            // @TODO (abock, 2020-02-25) Replace when putting the QSS in place
            action->setText("Restart: " + action->data().toString());
        }

        _actionMenu->addAction(action);
    }
}

bool ProgramButton::isProcessRunning(Node::ID nodeId) const {
    using Status = common::ProcessStatusMessage::Status;
    const auto it = _processes.find(nodeId);
    return (it != _processes.end()) &&
        data::findProcess(it->second.processId)->status == Status::Running;
}

bool ProgramButton::hasNoProcessRunning() const {
    return std::none_of(
        _cluster->nodes.begin(), _cluster->nodes.end(),
        [this](Node::ID n) { return isProcessRunning(n); }
    );
}

bool ProgramButton::hasAllProcessesRunning() const {
    return std::all_of(
        _cluster->nodes.begin(), _cluster->nodes.end(),
        [this](Node::ID n) { return isProcessRunning(n); }
    );
}

    
//////////////////////////////////////////////////////////////////////////////////////////


ClusterWidget::ClusterWidget(Cluster* cluster,
                             const std::vector<Program::Configuration>& configurations)
{
    assert(cluster);
    setTitle(cluster->name.c_str());

    QBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    setLayout(layout);

    for (const Program::Configuration& configuration : configurations) {
        ProgramButton* button = new ProgramButton(cluster, &configuration);

        connect(
            button, &ProgramButton::startProgram,
            this, &ClusterWidget::startProgram
        );
        connect(
            button, &ProgramButton::stopProgram,
            this, &ClusterWidget::stopProgram
        );

        connect(
            button, &ProgramButton::restartProcess,
            this, &ClusterWidget::restartProcess
        );
        connect(
            button, &ProgramButton::stopProcess,
            this, &ClusterWidget::stopProcess
        );

        _startButtons[configuration.id] = button;
        layout->addWidget(button);
    }
}

void ClusterWidget::updateStatus() {
    std::for_each(
        _startButtons.begin(), _startButtons.end(),
        [](const std::pair<const Program::Configuration::ID, ProgramButton*>& p) {
            p.second->updateStatus();
        }
    );
}

void ClusterWidget::processUpdated(Process::ID processId) {
    Process* process = data::findProcess(processId);
    const auto it = _startButtons.find(process->configurationId);
    if (it != _startButtons.end()) {
        it->second->processUpdated(processId);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////


ProgramWidget::ProgramWidget(const Program& program)
    : QGroupBox(program.name.c_str())
{
    QBoxLayout* layout = new QHBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    setLayout(layout);

    QBoxLayout* tagsLayout = new QVBoxLayout;
    QWidget* tagsBox = new QWidget;
    tagsBox->setLayout(tagsLayout);
    for (const std::string& tag : program.tags) {
        QLabel* label = new QLabel(tag.c_str());
        tagsLayout->addWidget(label);
    }
    layout->addWidget(tagsBox);


    std::vector<Cluster*> clusters = data::findClustersForProgram(program);
    for (Cluster* cluster : clusters) {
        assert(cluster);
        ClusterWidget* w = new ClusterWidget(cluster, program.configurations);

        connect(
            w, &ClusterWidget::startProgram,
            [this, clusterId = cluster->id](Program::Configuration::ID configurationId) {
                emit startProgram(clusterId, configurationId);
            }
        );

        connect(
            w, &ClusterWidget::stopProgram,
            [this, clusterId = cluster->id](Program::Configuration::ID configurationId) {
                emit stopProgram(clusterId, configurationId);
            }
        );

        connect(w, &ClusterWidget::restartProcess, this, &ProgramWidget::restartProcess);
        connect(w, &ClusterWidget::stopProcess, this, &ProgramWidget::stopProcess);

        _widgets[cluster->id] = w;
        layout->addWidget(w);
    }
}

void ProgramWidget::updateStatus(Cluster::ID clusterId) {
    const auto it = _widgets.find(clusterId);
    // We have to check as a cluster that is active might not have any associated programs
    if (it != _widgets.end()) {
        it->second->updateStatus();
    }
}

void ProgramWidget::processUpdated(Process::ID processId) {
    Process* process = data::findProcess(processId);
    const auto it = _widgets.find(process->clusterId);
    assert(it != _widgets.end());
    it->second->processUpdated(processId);
}


//////////////////////////////////////////////////////////////////////////////////////////


TagsWidget::TagsWidget() 
    : QGroupBox("Tags")
{
    std::set<std::string> tags = data::findTags();

    QBoxLayout* layout = new QVBoxLayout;
    layout->setContentsMargins(5, 5, 5, 5);
    setLayout(layout);

    for (const std::string& tag : tags) {
        QPushButton* button = new QPushButton(tag.c_str());

        Color color = colorForTag(tag);
        constexpr const int Delta = 40;
        Color lightColor = Color {
            std::min(255, color.r + Delta),
            std::min(255, color.g + Delta),
            std::min(255, color.b + Delta)
        };
        std::string colorText = fmt::format(
            R"(
                QPushButton {{
                    background-color: #{0:x}{1:x}{2:x}; color: #202020;
                    border-color: #303030;
                }}

                QPushButton:hover {{
                    background-color: #{3:x}{4:x}{5:x}; color: #202020;
                }}
            )",
            color.r, color.g, color.b,
            lightColor.r, lightColor.g, lightColor.b
        );
        button->setStyleSheet(colorText.c_str());

        //QPixmap pixmap(25, 25);
        //pixmap.fill(QColor(color.r, color.g, color.b));
        //button->setIcon(QIcon(pixmap));


        button->setCheckable(true);
        connect(button, &QPushButton::clicked, this, &TagsWidget::buttonPressed);

        layout->addWidget(button);
        _buttons[button] = tag;
    }

    layout->addStretch();
}

void TagsWidget::buttonPressed() {
    std::vector<std::string> selectedTags;
    for (const std::pair<const QPushButton*, std::string>& p : _buttons) {
        if (p.first->isChecked()) {
            selectedTags.push_back(p.second);
        }
    }

    emit pickedTags(selectedTags);
}


//////////////////////////////////////////////////////////////////////////////////////////


ProgramsWidget::ProgramsWidget() {
    QBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    QWidget* controls = createControls();
    layout->addWidget(controls);

    QWidget* programs = createPrograms();
    layout->addWidget(programs);

    layout->setStretch(1, 5);
}

QWidget* ProgramsWidget::createControls() {
    QWidget* controls = new QWidget;
    QBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    controls->setLayout(layout);

    QLineEdit* search = new QLineEdit;
    search->setPlaceholderText("Search...");
    layout->addWidget(search);

    connect(search, &QLineEdit::textChanged,
        [this](const QString& str) { emit searchUpdated(str.toLocal8Bit().constData()); }
    );

    TagsWidget* tags = new TagsWidget;
    connect(tags, &TagsWidget::pickedTags, this, &ProgramsWidget::tagsPicked);
    layout->addWidget(tags);



    return controls;
}

QWidget* ProgramsWidget::createPrograms() {
    QWidget* controls = new QWidget;
    QBoxLayout* layout = new QVBoxLayout;
    layout->setMargin(0);
    controls->setLayout(layout);

    for (Program* p : data::programs()) {
        ProgramWidget* w = new ProgramWidget(*p);

        connect(
            w, &ProgramWidget::startProgram,
            [this, programId = p->id](Cluster::ID clusterId,
                Program::Configuration::ID configurationId)
        {
            emit startProgram(clusterId, programId, configurationId);
        }
        );
        connect(
            w, &ProgramWidget::stopProgram,
            [this, programId = p->id](Cluster::ID clusterId,
                Program::Configuration::ID configurationId)
        {
            emit stopProgram(clusterId, programId, configurationId);
        }
        );

        connect(w, &ProgramWidget::restartProcess, this, &ProgramsWidget::restartProcess);
        connect(w, &ProgramWidget::stopProcess, this, &ProgramsWidget::stopProcess);

        _widgets[p->id] = w;
        _visibilities[p->id] = VisibilityInfo{ true, true };
        layout->addWidget(w);
    }

    layout->addStretch();

    return controls;
}

void ProgramsWidget::processUpdated(Process::ID processId) {
    Process* process = data::findProcess(processId);

    const auto it = _widgets.find(process->programId);
    if (it != _widgets.end()) {
        it->second->processUpdated(processId);
    }
}

void ProgramsWidget::connectedStatusChanged(Cluster::ID cluster, Node::ID) {
    for (const std::pair<const Program::ID, ProgramWidget*>& p : _widgets) {
        p.second->updateStatus(cluster);
    }
}

void ProgramsWidget::tagsPicked(std::vector<std::string> tags) {
    if (tags.empty()) {
        for (std::pair<const Program::ID, VisibilityInfo>& p : _visibilities) {
            p.second.byTag = true;
        }
    }
    else {
        for (std::pair<const Program::ID, VisibilityInfo>& p : _visibilities) {
            const bool hasTag = data::hasTag(p.first, tags);
            p.second.byTag = hasTag;
        }
    }

    updatedVisibilityState();
}

void ProgramsWidget::searchUpdated(std::string text) {
    if (text.empty()) {
        for (std::pair<const Program::ID, VisibilityInfo>& p : _visibilities) {
            p.second.bySearch = true;
        }
    }
    else {
        for (std::pair<const Program::ID, VisibilityInfo>& p : _visibilities) {
            Program* program = data::findProgram(p.first);
            if (text.size() > program->name.size()) {
                p.second.bySearch = false;
            }
            else {
                auto toLower = [](const std::string& str) {
                    std::string res;
                    std::transform(
                        str.begin(), str.end(),
                        std::back_inserter(res),
                        [](char c) { return static_cast<char>(::tolower(c)); }
                    );
                    return res;
                };

                const std::string sub = program->name.substr(0, text.size());
                p.second.bySearch = toLower(sub) == toLower(text);
            }
        }
    }

    updatedVisibilityState();
}

void ProgramsWidget::updatedVisibilityState() {
    for (std::pair<const Program::ID, ProgramWidget*>& p : _widgets) {
        VisibilityInfo vi = _visibilities[p.first];
        p.second->setVisible(vi.bySearch && vi.byTag);
    }
}


} // namespace programs
