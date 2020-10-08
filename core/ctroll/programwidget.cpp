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
#include "configuration.h"
#include "database.h"
#include "logging.h"
#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QDesktopWidget>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QScrollArea>
#include <QVBoxLayout>
#include <fmt/format.h>
#include <set>

namespace programs {

ProgramButton::ProgramButton(const Cluster* cluster,
                             const Program::Configuration* configuration)
    : QPushButton(QString::fromStdString(configuration->name))
    , _cluster(cluster)
    , _configuration(configuration)
    , _actionMenu(new QMenu(this))
{
    assert(cluster);
    assert(configuration);

    setToolTip(QString::fromStdString(configuration->description));
    setEnabled(false);
    connect(this, &QPushButton::clicked, this, &ProgramButton::handleButtonPress);
}

void ProgramButton::updateStatus() {
    std::vector<const Node*> nodes = data::findNodesForCluster(*_cluster);

    const bool allConnected = std::all_of(
        nodes.cbegin(), nodes.cend(),
        std::mem_fn(&Node::isConnected)
    );
    setEnabled(allConnected);
}

void ProgramButton::processUpdated(Process::ID processId) {
    const Process* process = data::findProcess(processId);

    auto it = std::find_if(
        _processes.cbegin(), _processes.cend(),
        [processId](const std::pair<const Node::ID, ProcessInfo>& p) {
            return p.second.processId.v == processId.v;
        }
    );
    if (it == _processes.end()) {
        // This is a brand new process, so it better be in a Starting status

        ProcessInfo info;
        info.processId = processId;
        const Node* node = data::findNode(process->nodeId);
        assert(node);
        info.menuAction = new QAction(QString::fromStdString(node->name));
        // We store the name of the node as the user data in order to sort them later
        info.menuAction->setData(QString::fromStdString(node->name));
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
        setText(QString::fromStdString(_configuration->name));
    }
    else if (hasAllProcessesRunning()) {
        setMenu(nullptr);
        setObjectName("stop"); // used in the QSS sheet to style this button
        // @TODO (abock, 2020-02-25) Replace when putting the QSS in place
        setText(QString::fromStdString("Stop:" + _cluster->name));
    }
    else {
        setMenu(_actionMenu);
        setObjectName("mixed"); // used in the QSS sheet to style this button
        // @TODO (abock, 2020-02-25) Replace when putting the QSS in place
        setText(QString::fromStdString("Mixed:" + _cluster->name));

        updateMenu();
    }
}

void ProgramButton::updateMenu() {
    // First a bit of cleanup so that we don't have old signal connections laying around
    for (const std::pair<const Node::ID, ProcessInfo>& p : _processes) {
        QObject::disconnect(p.second.menuAction);
    }
    _actionMenu->clear();

    std::vector<QAction*> actions;
    actions.reserve(_processes.size());
    for (const std::pair<const Node::ID, ProcessInfo>& p : _processes) {
        actions.push_back(p.second.menuAction);
    }
    std::sort(
        actions.begin(), actions.end(),
        [](QAction* lhs, QAction* rhs) {
            return lhs->data().toString() < rhs->data().toString();
        }
    );

    for (QAction* action : actions) {
        QString actName = action->data().toString();
        const std::string nodeName = actName.toLocal8Bit().constData();
        const auto it = std::find_if(
            _processes.cbegin(), _processes.cend(),
            [nodeName](const std::pair<const Node::ID, ProcessInfo>& p) {
            const Node* node = data::findNode(p.first);
                return node->name == nodeName;
            }
        );
        // If we are getting this far, the node for this action has to exist in the map
        assert(it != _processes.cend());

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
            action->setText("Stop: " + actName);
        }
        else {
            setObjectName("restart"); // used in the QSS sheet to style this button
            connect(
                action, &QAction::triggered,
                [this, id = it->second.processId]() { emit restartProcess(id); }
            );

            // @TODO (abock, 2020-02-25) Replace when putting the QSS in place
            action->setText("Restart: " + actName);
        }

        _actionMenu->addAction(action);
    }
}

bool ProgramButton::isProcessRunning(Node::ID nodeId) const {
    using Status = common::ProcessStatusMessage::Status;
    const auto it = _processes.find(nodeId);
    return (it != _processes.cend()) &&
        data::findProcess(it->second.processId)->status == Status::Running;
}

bool ProgramButton::hasNoProcessRunning() const {
    return std::none_of(
        _cluster->nodes.cbegin(), _cluster->nodes.cend(),
        [this](const std::string& n) {
            const Node* node = data::findNode(n);
            return isProcessRunning(node->id);
        }
    );
}

bool ProgramButton::hasAllProcessesRunning() const {
    return std::all_of(
        _cluster->nodes.cbegin(), _cluster->nodes.cend(),
        [this](const std::string& n) {
            const Node* node = data::findNode(n);
            return isProcessRunning(node->id);
        }
    );
}

    
//////////////////////////////////////////////////////////////////////////////////////////


ClusterWidget::ClusterWidget(const Cluster* cluster,
                             const std::vector<Program::Configuration>& configurations)
{
    assert(cluster);
    setTitle(QString::fromStdString(cluster->name));
    setToolTip(QString::fromStdString(cluster->description));

    QBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

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
        _startButtons.cbegin(), _startButtons.cend(),
        [](const std::pair<const Program::Configuration::ID, ProgramButton*>& p) {
            p.second->updateStatus();
        }
    );
}

void ClusterWidget::processUpdated(Process::ID processId) {
    const Process* process = data::findProcess(processId);
    const auto it = _startButtons.find(process->configurationId);
    if (it != _startButtons.end()) {
        it->second->processUpdated(processId);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////


TagInfoWidget::TagInfoWidget(const std::vector<std::string>& tags) {
    QBoxLayout* layout = new QVBoxLayout(this);
    layout->setMargin(0);

    for (const std::string& tag : tags) {
        QWidget* w = new QWidget;

        Color color = data::colorForTag(tag);
        w->setStyleSheet(QString::fromStdString(fmt::format(
            "background: #{0:02x}{1:02x}{2:02x}", color.r, color.g, color.b
        )));

        w->setToolTip(QString::fromStdString(tag));
        layout->addWidget(w);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////


ProgramWidget::ProgramWidget(const Program& program)
    : QGroupBox(QString::fromStdString(program.name))
{
    setToolTip(QString::fromStdString(program.description));

    QBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(10);

    layout->addWidget(new TagInfoWidget(program.tags));

    std::vector<const Cluster*> clusters = data::findClustersForProgram(program);
    for (const Cluster* cluster : clusters) {
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
    const Process* process = data::findProcess(processId);
    const auto it = _widgets.find(process->clusterId);
    assert(it != _widgets.end());
    it->second->processUpdated(processId);
}


//////////////////////////////////////////////////////////////////////////////////////////


TagsWidget::TagsWidget() 
    : QGroupBox("Tags")
{
    QBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    std::set<std::string> tags = data::findTags();
    for (const std::string& tag : tags) {
        QPushButton* button = new QPushButton(QString::fromStdString(tag));

        Color color = data::colorForTag(tag);
        constexpr const int Delta = 40;
        Color lightColor = Color {
            std::min(255, color.r + Delta),
            std::min(255, color.g + Delta),
            std::min(255, color.b + Delta),
            ""
        };
        Color darkColor = Color {
            std::max(0, color.r - Delta),
            std::max(0, color.g - Delta),
            std::max(0, color.b - Delta),
            ""
        };

        std::string colorText = fmt::format(
            R"(
                QPushButton {{ background-color: #{0:02x}{1:02x}{2:02x}; }}
                QPushButton:hover {{ background-color: #{3:02x}{4:02x}{5:02x}; }}
                QPushButton:pressed {{ background-color: #{6:02x}{7:02x}{8:02x}; }}
                QPushButton:open {{ background-color: #{6:02x}{7:02x}{8:02x}; }}
            )",
            color.r, color.g, color.b,
            lightColor.r, lightColor.g, lightColor.b,
            darkColor.r, darkColor.g, darkColor.b
        );
        button->setStyleSheet(QString::fromStdString(colorText));

        // @TODO (abock, 2020-09-29):  Calculate the value of the background color and
        // set a dynamic property to set the text color to either bright or dark to make
        // the text legible regardless of what the user chose for the background color

        button->setCheckable(true);
        connect(button, &QPushButton::clicked, this, &TagsWidget::buttonPressed);

        layout->addWidget(button);
        _buttons[button] = tag;
    }

    layout->addStretch();
}

void TagsWidget::buttonPressed() {
    std::vector<std::string> selectedTags;
    for (std::pair<const QPushButton*, std::string> p : _buttons) {
        if (p.first->isChecked()) {
            selectedTags.push_back(p.second);
        }
    }

    emit pickedTags(selectedTags);
}


//////////////////////////////////////////////////////////////////////////////////////////


CustomProgramWidget::CustomProgramWidget(QWidget* parent)
    : QWidget(parent)
{
    QBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    QComboBox* targetList = new QComboBox;
    std::vector<const Cluster*> clusters = data::clusters();
    if (!clusters.empty()) {
        targetList->addItem("Clusters", TagSeparator);
        for (const Cluster* c : clusters) {
            targetList->addItem(QString::fromStdString(c->name), TagCluster);
        }
    }

    std::vector<const Node*> nodes = data::nodes();
    if (!nodes.empty()) {
        if (!clusters.empty()) {
            targetList->addItem("", TagSeparator);
        }
        targetList->addItem("Nodes", TagSeparator);
        targetList->addItem("------------", TagSeparator);
        for (const Node* n : nodes) {
            targetList->addItem(QString::fromStdString(n->name), TagNode);
        }
    }
    targetList->setCurrentIndex(-1);
    layout->addWidget(targetList);

    QLineEdit* executable = new QLineEdit;
    executable->setPlaceholderText("Executable name");
    layout->addWidget(executable);

    QLineEdit* workingDirectory = new QLineEdit;
    workingDirectory->setPlaceholderText("Working directory");
    layout->addWidget(workingDirectory);

    QLineEdit* parameters = new QLineEdit;
    parameters->setPlaceholderText("Parameters (optional)");
    layout->addWidget(parameters);

    layout->addStretch(0);

    QPushButton* run = new QPushButton("Run");
    run->setObjectName("run");
    run->setEnabled(false); // since the "Clusters" target will be selected by default
    connect(
        run, &QPushButton::clicked,
        [this, targetList, executable, workingDirectory, parameters]() {
            const std::string exec = executable->text().toStdString();
            const std::string workDir = workingDirectory->text().toStdString();
            const std::string args = parameters->text().toStdString();

            const int tag = targetList->currentData().toInt();
            assert(tag == TagCluster || tag == TagNode);
            if (tag == TagCluster) {
                const std::string cluster = targetList->currentText().toStdString();
                const Cluster* c = data::findCluster(cluster);
                assert(c);
                for (const std::string& node : c->nodes) {
                    const Node* n = data::findNode(node);
                    assert(n);
                    emit startCustomProgram(n->id, exec, workDir, args);
                }
            }
            else {
                const std::string node = targetList->currentText().toStdString();
                const Node* n = data::findNode(node);
                assert(n);
                emit startCustomProgram(n->id, exec, workDir, args);
            }
        }
    );
    layout->addWidget(run);

    QPushButton* clear = new QPushButton("Clear");
    connect(
        clear, &QPushButton::clicked,
        [this, targetList, executable, workingDirectory, parameters]() {
            targetList->setCurrentIndex(-1);
            executable->setText("");
            workingDirectory->setText("");
            parameters->setText("");
        }
    );
    layout->addWidget(clear);

    auto updateRunButton = [this, targetList, executable, run]() {
        const bool goodTarget = targetList->currentData().toInt() != TagSeparator;
        const bool goodExec = !executable->text().isEmpty();
        run->setEnabled(goodTarget && goodExec);
    };

    connect(
        targetList, QOverload<int>::of(&QComboBox::currentIndexChanged),
        updateRunButton
    );
    connect(
        executable, &QLineEdit::textChanged,
        updateRunButton
    );
}


//////////////////////////////////////////////////////////////////////////////////////////


ProgramsWidget::ProgramsWidget() {
    QGridLayout* layout = new QGridLayout(this);
    layout->setContentsMargins(10, 2, 2, 2);
    layout->addWidget(createControls(), 0, 0);
    layout->addWidget(createPrograms(), 0, 1);
    layout->setColumnStretch(1, 5);

    CustomProgramWidget* customPrograms = new CustomProgramWidget(this);
    connect(
        customPrograms, &CustomProgramWidget::startCustomProgram,
        this, &ProgramsWidget::startCustomProgram
    );
    layout->addWidget(customPrograms, 1, 0, 1, 2);
}

QWidget* ProgramsWidget::createControls() {
    QWidget* controls = new QWidget;
    QBoxLayout* layout = new QVBoxLayout(controls);
    layout->setMargin(0);

    QLineEdit* search = new QLineEdit;
    search->setPlaceholderText("Search...");
    layout->addWidget(search);

    connect(
        search, &QLineEdit::textChanged,
        [this](const QString& str) { emit searchUpdated(str.toLocal8Bit().constData()); }
    );

    TagsWidget* tags = new TagsWidget;
    connect(tags, &TagsWidget::pickedTags, this, &ProgramsWidget::tagsPicked);
    layout->addWidget(tags);

    return controls;
}

QWidget* ProgramsWidget::createPrograms() {
    QScrollArea* area = new QScrollArea;
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget* content = new QWidget;
    area->setWidget(content);
    QBoxLayout* contentLayout = new QVBoxLayout(content);
    contentLayout->setContentsMargins(5, 5, 5, 5);
    area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

    for (const Program* p : data::programs()) {
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
        VisibilityInfo info;
        info.bySearch = true;
        info.byTag = true;
        _visibilities[p->id] = info;
        contentLayout->addWidget(w);
    }

    contentLayout->addStretch(0);

    return area;
}

void ProgramsWidget::processUpdated(Process::ID processId) {
    const Process* process = data::findProcess(processId);

    const auto it = _widgets.find(process->programId);
    if (it != _widgets.cend()) {
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
            const Program* program = data::findProgram(p.first);
            if (text.size() > program->name.size()) {
                p.second.bySearch = false;
            }
            else {
                auto toLower = [](const std::string& str) {
                    std::string res;
                    std::transform(
                        str.cbegin(), str.cend(),
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
