/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2026                                                               *
 * Alexander Bock                                                                        *
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

#include "programdialog.h"

#include "addbutton.h"
#include "jsonload.h"
#include "node.h"
#include "program.h"
#include "removebutton.h"
#include "spacer.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStyle>
#include <QVBoxLayout>
#include <filesystem>

ProgramDialog::ConfigurationWidget::ConfigurationWidget() {
    QBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    name = new QLineEdit;
    name->setToolTip("The user-facing name of this configuration");
    layout->addWidget(name);
    layout->setStretch(0, 3);

    parameters = new QLineEdit;
    parameters->setToolTip("Additional commandline parameters that are passed");
    parameters->setPlaceholderText("optional");
    layout->addWidget(parameters);
    layout->setStretch(1, 2);

    description = new QLineEdit;
    description->setToolTip("Additional user information about this configuration");
    description->setPlaceholderText("optional");
    layout->addWidget(description);
    layout->setStretch(2, 2);
}

ProgramDialog::ClusterWidget::ClusterWidget(const std::string& cluster,
                                            const std::string& parameters)
{
    QBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    label = new QLabel(QString::fromStdString(cluster));
    layout->addWidget(label);
    layout->setStretch(0, 2);

    arguments = new QLineEdit;
    arguments->setText(QString::fromStdString(parameters));
    arguments->setCursorPosition(0);
    arguments->setToolTip("Additional commandline parameters that are passed");
    arguments->setPlaceholderText("optional");
    layout->addWidget(arguments);
    layout->setStretch(1, 3);
}

ProgramDialog::NodeWidget::NodeWidget(const std::string& node,
                                      const std::string& parameters)
{
    QBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    label = new QLabel(QString::fromStdString(node));
    layout->addWidget(label);
    layout->setStretch(0, 2);

    arguments = new QLineEdit;
    arguments->setText(QString::fromStdString(parameters));
    arguments->setCursorPosition(0);
    arguments->setToolTip("Additional commandline parameters that are passed");
    arguments->setPlaceholderText("optional");
    layout->addWidget(arguments);
    layout->setStretch(1, 3);
}

ProgramDialog::FavoriteWidget::FavoriteWidget(const std::string& cluster,
                                              const std::string& configuration,
                                              const std::string& name)
{
    QBoxLayout* layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);

    this->cluster = new QLabel(QString::fromStdString(cluster));
    this->cluster->setToolTip("The cluster on which this favorite starts the program");
    layout->addWidget(this->cluster);
    layout->setStretch(0, 2);

    this->configuration = new QLabel(QString::fromStdString(configuration));
    this->configuration->setToolTip(
        "The configuration with which this favorite starts the program"
    );
    layout->addWidget(this->configuration);
    layout->setStretch(1, 2);

    this->name = new QLineEdit;
    this->name->setText(QString::fromStdString(name));
    this->name->setCursorPosition(0);
    this->name->setToolTip(
        "The name shown on the button on the Favorites tab. If this is empty, the name "
        "of the program is used instead"
    );
    this->name->setPlaceholderText("optional");
    layout->addWidget(this->name);
    layout->setStretch(2, 3);
}

ProgramDialog::ProgramDialog(QWidget* parent, std::string programPath,
                             std::string clusterPath, std::string nodePath)
    : QDialog(parent)
    , _programPath(std::move(programPath))
    , _clusterPath(std::move(clusterPath))
    , _nodePath(std::move(nodePath))
{
    setWindowTitle(QString::fromStdString(std::format("Program: {}", _programPath)));
    resize(800, 900);

    QBoxLayout* l = new QVBoxLayout(this);
    l->setContentsMargins(0, 0, 0, 0);

    QScrollArea* area = new QScrollArea;
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    area->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    l->addWidget(area);

    QWidget* content = new QWidget;
    area->setWidget(content);
    QBoxLayout* layout = new QVBoxLayout(content);
    

    QWidget* edit = new QWidget;
    QGridLayout* editLayout = new QGridLayout(edit);
    editLayout->setContentsMargins(0, 0, 0, 0);
    editLayout->setSpacing(5);

    editLayout->addWidget(new QLabel("Name:"), 0, 0);
    _name = new QLineEdit;
    _name->setObjectName("name");
    _name->setToolTip("The name of this program");
    connect(_name, &QLineEdit::textChanged, this, &ProgramDialog::updateSaveButton);
    editLayout->addWidget(_name, 0, 1, 1, 2);

    editLayout->addWidget(new QLabel("Executable:"), 1, 0);
    _executable = new QLineEdit;
    _executable->setToolTip(
        "The command that will be executed when this program is started"
    );
    connect(_executable, &QLineEdit::textChanged, this, &ProgramDialog::updateSaveButton);
    editLayout->addWidget(_executable, 1, 1, 1, 2);

    QPushButton* pickExecutable = new QPushButton;
    pickExecutable->setObjectName("gotoFolder");
    pickExecutable->setIcon(pickExecutable->style()->standardIcon(QStyle::SP_DirIcon));
    pickExecutable->setFlat(true);
    pickExecutable->setContentsMargins(0, 0, 0, 0);
    pickExecutable->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Minimum);

    connect(
        pickExecutable, &QPushButton::clicked,
        [this]() {
            QString file = QFileDialog::getOpenFileName(
                this,
                "Pick executable",
                _executable->text(),
                "Executable (*.exe)"
            );

            if (!file.isEmpty()) {
                _executable->setText(file);
            }
        }
    );

    editLayout->addWidget(pickExecutable, 1, 2);

    editLayout->addWidget(new QLabel("Commandline Parameters:"), 2, 0);
    _commandLineParameters = new QLineEdit;
    _commandLineParameters->setPlaceholderText("optional");
    _commandLineParameters->setToolTip(
        "Global commandline parameters that will be added to the command regardless of "
        "the configuration that was selected"
    );
    editLayout->addWidget(_commandLineParameters, 2, 1, 1, 2);

    editLayout->addWidget(new QLabel("Working Directory:"), 3, 0);
    _workingDirectory = new QLineEdit;
    _workingDirectory->setPlaceholderText("optional");
    _workingDirectory->setToolTip("The directory in which the program will run");
    editLayout->addWidget(_workingDirectory, 3, 1, 1, 2);

    editLayout->addWidget(new QLabel("Enabled:"), 4, 0);
    _isEnabled = new QCheckBox;
    _isEnabled->setToolTip(
        "If this is disabled, this program will not be listed in the C-Troll application"
    );
    _isEnabled->setChecked(true);
    editLayout->addWidget(_isEnabled, 4, 1, 1, 2);

    editLayout->addWidget(new QLabel("Forward Messages:"), 5, 0);
    _shouldForwardMessages = new QCheckBox;
    _shouldForwardMessages->setToolTip(
        "If this is enabled, all console messages from the executable will be sent back "
        "to C-Troll"
    );
    editLayout->addWidget(_shouldForwardMessages, 5, 1, 1, 2);

    editLayout->addWidget(new QLabel("Autorestart on Crash:"), 6, 0);
    _shouldAutoRestart = new QCheckBox;
    _shouldAutoRestart->setToolTip(
        "If this is enabled, the program will automatically be restarted if it is "
        "running and it crashes"
    );
    editLayout->addWidget(_shouldAutoRestart, 6, 1, 1, 2);

    editLayout->addWidget(new QLabel("Delay"), 7, 0);
    QWidget* delayContainer = new QWidget;
    QBoxLayout* delayLayout = new QHBoxLayout(delayContainer);
    delayLayout->setContentsMargins(0, 0, 0, 0);
    delayLayout->setSpacing(0);
    _hasDelay = new QCheckBox("Enabled");
    _hasDelay->setToolTip(
        "If this is enabled, each program in a cluster will be started with a delay"
    );
    connect(
        _hasDelay, &QCheckBox::clicked,
        [this]() { _delay->setEnabled(_hasDelay->isChecked()); }
    );
    delayLayout->addWidget(_hasDelay);
    _delay = new QSpinBox;
    _delay->setToolTip("The delay in milliseconds that is added between programs");
    _delay->setMinimum(0);
    _delay->setMaximum(std::numeric_limits<int>::max());
    delayLayout->addWidget(_delay);
    editLayout->addWidget(delayContainer, 7, 1, 1, 2);

    editLayout->addWidget(new QLabel("PreStart Script"), 8, 0);
    _preStart = new QLineEdit;
    _preStart->setToolTip(
        "A script that gets executed on the C-Troll computer before this program starts"
    );
    _preStart->setPlaceholderText("optional");
    editLayout->addWidget(_preStart, 8, 1, 1, 2);

    editLayout->addWidget(new QLabel("PreStart Script (Node)"), 9, 0);
    _preStartNode = new QLineEdit;
    _preStartNode->setToolTip(
        "A script that gets executed on each node before this program is started on that "
        "node. Each node waits for its own script to finish, but not for the other nodes"
    );
    _preStartNode->setPlaceholderText("optional");
    editLayout->addWidget(_preStartNode, 9, 1, 1, 2);

    editLayout->addWidget(new QLabel("Description:"), 10, 0);
    _description = new QLineEdit;
    _description->setToolTip("Additional information for the user about the program");
    _description->setPlaceholderText("optional");
    editLayout->addWidget(_description, 10, 1, 1, 2);

    QLabel* parametersLabel = new QLabel(
        "The complete arguments for the program are given in the following order: 1. the "
        "global parameters;  2. the configuration-specific parameters;  3. the "
        "cluster-specific parameters;  4. the node-specific parameters."
    );
    parametersLabel->setWordWrap(true);
    parametersLabel->setObjectName("information-label");
    editLayout->addWidget(parametersLabel, 11, 0, 1, 3);

    editLayout->addWidget(new Spacer, 12, 0, 1, 3);

    {
        // Configurations

        editLayout->addWidget(new QLabel("Configurations"), 13, 0);

        QPushButton* newConfiguration = new AddButton;
        connect(
            newConfiguration, &QPushButton::clicked,
            [this]() {
                ConfigurationWidget* config = new ConfigurationWidget;
                connect(
                    config->name, &QLineEdit::textChanged,
                    this, &ProgramDialog::updateSaveButton
                );
                _configurations->addItem(config);
                config->name->setFocus();
                updateSaveButton();
            }
        );
        editLayout->addWidget(newConfiguration, 13, 1, 1, 2, Qt::AlignRight);

        _configurations = new DynamicList;
        _configurations->setToolTip(
            "A list of all configurations that are available for this program"
        );
        connect(
            _configurations, &DynamicList::updated,
            this, &ProgramDialog::updateSaveButton
        );
        editLayout->addWidget(_configurations, 14, 0, 1, 3);
    }

    editLayout->addWidget(new Spacer, 15, 0, 1, 3);

    {
        // Clusters

        editLayout->addWidget(new QLabel("Clusters"), 16, 0);

        QPushButton* newCluster = new AddButton;
        connect(
            newCluster, &QPushButton::clicked,
            [this]() {
                std::string name = selectCluster();
                if (!name.empty()) {
                    ClusterWidget* cluster = new ClusterWidget(name, "");
                    _clusters->addItem(cluster);
                    updateSaveButton();
                }
            }
        );
        editLayout->addWidget(newCluster, 16, 1, 1, 2, Qt::AlignRight);

        _clusters = new DynamicList;
        _clusters->setToolTip("The list of clusters on which the program can be run");
        connect(_clusters, &DynamicList::updated, this, &ProgramDialog::updateSaveButton);
        editLayout->addWidget(_clusters, 17, 0, 1, 3);
    }

    editLayout->addWidget(new Spacer, 18, 0, 1, 3);

    {
        // Nodes

        editLayout->addWidget(new QLabel("Node Parameters (optional)"), 19, 0);

        QPushButton* newNode = new AddButton;
        connect(
            newNode, &QPushButton::clicked,
            [this]() {
                std::string name = selectNode();
                if (!name.empty()) {
                    NodeWidget* node = new NodeWidget(name, "");
                    _nodes->addItem(node);
                    updateSaveButton();
                }
            }
        );
        editLayout->addWidget(newNode, 19, 1, 1, 2, Qt::AlignRight);

        _nodes = new DynamicList;
        _nodes->setToolTip(
            "Additional commandline parameters that are only added when the program is "
            "started on that specific node"
        );
        connect(_nodes, &DynamicList::updated, this, &ProgramDialog::updateSaveButton);
        editLayout->addWidget(_nodes, 20, 0, 1, 3);
    }

    editLayout->addWidget(new Spacer, 21, 0, 1, 3);

    {
        // Favorites

        editLayout->addWidget(new QLabel("Favorites (optional)"), 22, 0);

        QPushButton* newFavorite = new AddButton;
        connect(
            newFavorite, &QPushButton::clicked,
            [this]() {
                std::optional<std::pair<std::string, std::string>> f = selectFavorite();
                if (f.has_value()) {
                    FavoriteWidget* favorite =
                        new FavoriteWidget(f->first, f->second, "");
                    _favorites->addItem(favorite);
                    updateSaveButton();
                }
            }
        );
        editLayout->addWidget(newFavorite, 22, 1, 1, 2, Qt::AlignRight);

        _favorites = new DynamicList;
        _favorites->setToolTip(
            "The cluster and configuration combinations of this program that are shown "
            "on the Favorites tab of C-Troll"
        );
        connect(
            _favorites, &DynamicList::updated,
            this, &ProgramDialog::updateSaveButton
        );
        editLayout->addWidget(_favorites, 23, 0, 1, 3);
    }

    editLayout->addWidget(new Spacer, 24, 0, 1, 3);

    {
        // Tags
        editLayout->addWidget(new QLabel("Tags (optional)"), 25, 0);

        QPushButton* t = new AddButton;
        connect(
            t, &QPushButton::clicked,
            [this]() {
                QLineEdit* tag = new QLineEdit;
                _tags->addItem(tag);
                tag->setFocus();
                updateSaveButton();
            }
        );
        editLayout->addWidget(t, 25, 1, 1, 2, Qt::AlignRight);

        _tags = new DynamicList;
        _tags->setToolTip("A list of all tags that this program is associated with");
        connect(_tags, &DynamicList::updated, this, &ProgramDialog::updateSaveButton);

        editLayout->addWidget(_tags, 26, 0, 1, 3);
    }

    layout->addWidget(edit);


    QDialogButtonBox* box = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel
    );
    _saveButton = box->button(QDialogButtonBox::Save);
    connect(box, &QDialogButtonBox::accepted, this, &ProgramDialog::save);
    connect(box, &QDialogButtonBox::rejected, this, &ProgramDialog::reject);
    layout->addWidget(box, 0, Qt::AlignRight);


    if (std::filesystem::exists(_programPath)) {
        Program program = common::loadFromJson<Program>(_programPath);
        std::pair<std::vector<Cluster>, bool> clusters =
            common::loadJsonFromDirectory<Cluster>(_clusterPath);

        _name->setText(QString::fromStdString(program.name));
        _name->setCursorPosition(0);
        _executable->setText(QString::fromStdString(program.executable));
        _executable->setCursorPosition(0);
        _commandLineParameters->setText(
            QString::fromStdString(program.commandlineParameters)
        );
        _commandLineParameters->setCursorPosition(0);
        _workingDirectory->setText(QString::fromStdString(program.workingDirectory));
        _workingDirectory->setCursorPosition(0);
        _isEnabled->setChecked(program.isEnabled);
        _shouldForwardMessages->setChecked(program.shouldForwardMessages);
        _shouldAutoRestart->setChecked(program.shouldAutoRestart);
        _hasDelay->setChecked(program.delay.has_value());
        if (program.delay.has_value()) {
            _delay->setValue(static_cast<int>(program.delay->count()));
        }
        _preStart->setText(QString::fromStdString(program.preStart));
        _preStart->setCursorPosition(0);
        _preStartNode->setText(QString::fromStdString(program.preStartNode));
        _preStartNode->setCursorPosition(0);
        _description->setText(QString::fromStdString(program.description));
        for (const std::string& tag : program.tags) {
            QLineEdit* t = new QLineEdit(QString::fromStdString(tag));
            _tags->addItem(t);
        }
        for (const Program::Configuration& configuration : program.configurations) {
            ConfigurationWidget* config = new ConfigurationWidget;
            connect(
                config->name, &QLineEdit::textChanged,
                this, &ProgramDialog::updateSaveButton
            );

            config->name->setText(QString::fromStdString(configuration.name));
            config->name->setCursorPosition(0);
            config->parameters->setText(QString::fromStdString(configuration.parameters));
            config->parameters->setCursorPosition(0);
            config->description->setText(
                QString::fromStdString(configuration.description)
            );
            config->description->setCursorPosition(0);
            _configurations->addItem(config);
        }
        for (const Program::Cluster& cluster : program.clusters) {
            ClusterWidget* c = new ClusterWidget(cluster.name, cluster.parameters);
            _clusters->addItem(c);

            const auto it = std::find_if(
                clusters.first.begin(), clusters.first.end(),
                [cluster](const Cluster& c) { return c.name == cluster.name; }
            );
            if (it == clusters.first.end()) {
                c->label->setObjectName("invalid");
                c->setToolTip("Could not find cluster in clusters folder");
            }
        }

        std::pair<std::vector<Node>, bool> nodes =
            common::loadJsonFromDirectory<Node>(_nodePath);
        for (const Program::NodeParameters& node : program.nodes) {
            NodeWidget* n = new NodeWidget(node.name, node.parameters);
            _nodes->addItem(n);

            const auto it = std::find_if(
                nodes.first.begin(),
                nodes.first.end(),
                [node](const Node& n) { return n.name == node.name; }
            );
            if (it == nodes.first.end()) {
                n->label->setObjectName("invalid");
                n->setToolTip("Could not find node in nodes folder");
            }
        }

        for (const Program::Favorite& favorite : program.favorites) {
            FavoriteWidget* f = new FavoriteWidget(
                favorite.cluster,
                favorite.configuration,
                favorite.name
            );
            _favorites->addItem(f);

            const bool hasCluster = std::any_of(
                program.clusters.begin(), program.clusters.end(),
                [&favorite](const Program::Cluster& c) {
                    return c.name == favorite.cluster;
                }
            );
            if (!hasCluster) {
                f->cluster->setObjectName("invalid");
                f->setToolTip("Could not find cluster in the clusters of this program");
            }

            const bool hasConfiguration = std::any_of(
                program.configurations.begin(), program.configurations.end(),
                [&favorite](const Program::Configuration& c) {
                    return c.name == favorite.configuration;
                }
            );
            if (!hasConfiguration) {
                f->configuration->setObjectName("invalid");
                f->setToolTip(
                    "Could not find configuration in the configurations of this program"
                );
            }
        }
    }
    else {
        // If it doesn't exist, we want to create at least a default configuration to
        // minimize the effort the user has to put in to create a basic configuration
        ConfigurationWidget* config = new ConfigurationWidget;
        config->name->setText("default");
        connect(
            config->name, &QLineEdit::textChanged,
            this, &ProgramDialog::updateSaveButton
        );
        _configurations->addItem(config);
    }

    updateSaveButton();
}

void ProgramDialog::setExecutableInformation(std::filesystem::path path) {
    std::string fullPath = path.string();
    std::string filename = path.filename().replace_extension("").string();
    _name->setText(QString::fromStdString(filename));
    _executable->setText(QString::fromStdString(fullPath));
}

void ProgramDialog::save() {
    Program program;
    program.name = _name->text().toStdString();
    program.executable = _executable->text().toStdString();
    program.commandlineParameters = _commandLineParameters->text().toStdString();
    program.workingDirectory = _workingDirectory->text().toStdString();
    program.isEnabled = _isEnabled->isChecked();
    program.shouldForwardMessages = _shouldForwardMessages->isChecked();
    program.shouldAutoRestart = _shouldAutoRestart->isChecked();
    if (_hasDelay->isChecked()) {
        program.delay = std::chrono::milliseconds(_delay->value());
    }
    program.preStart = _preStart->text().toStdString();
    program.preStartNode = _preStartNode->text().toStdString();
    program.description = _description->text().toStdString();
    for (QLineEdit* tag : _tags->items<QLineEdit>()) {
        std::string t = tag->text().toStdString();
        if (!t.empty()) {
            program.tags.push_back(std::move(t));
        }
    }
    for (ConfigurationWidget* conf : _configurations->items<ConfigurationWidget>()) {
        Program::Configuration c;
        c.name = conf->name->text().toStdString();
        c.parameters = conf->parameters->text().toStdString();
        c.description = conf->description->text().toStdString();
        program.configurations.push_back(c);
    }
    for (ClusterWidget* cluster : _clusters->items<ClusterWidget>()) {
        Program::Cluster c;
        c.name = cluster->label->text().toStdString();
        c.parameters = cluster->arguments->text().toStdString();
        program.clusters.push_back(c);
    }
    for (NodeWidget* node : _nodes->items<NodeWidget>()) {
        Program::NodeParameters n;
        n.name = node->label->text().toStdString();
        n.parameters = node->arguments->text().toStdString();
        program.nodes.push_back(n);
    }
    for (FavoriteWidget* favorite : _favorites->items<FavoriteWidget>()) {
        Program::Favorite f;
        f.cluster = favorite->cluster->text().toStdString();
        f.configuration = favorite->configuration->text().toStdString();
        f.name = favorite->name->text().toStdString();
        program.favorites.push_back(f);
    }

    common::saveToJson(_programPath, program);

    accept();
}

std::string ProgramDialog::selectCluster() {
    std::pair<std::vector<Cluster>, bool> clusters =
        common::loadJsonFromDirectory<Cluster>(_clusterPath);

    std::vector<ClusterWidget*> currClusters = _clusters->items<ClusterWidget>();
    clusters.first.erase(
        std::remove_if(
            clusters.first.begin(), clusters.first.end(),
            [&currClusters](const Cluster& c) {
                const auto it = std::find_if(
                    currClusters.begin(), currClusters.end(),
                    [c](ClusterWidget* cw) {
                        return cw->label->text().toStdString() == c.name;
                    }
                );
                return it != currClusters.end();
            }
        ),
        clusters.first.end()
    );

    if (clusters.first.empty()) {
        QMessageBox::information(
            this,
            "Add clusters",
            "No available clusters left to add"
        );
    }

    QStringList list;
    for (const Cluster& cluster : clusters.first) {
        list.push_back(QString::fromStdString(cluster.name));
    }

    bool ok;
    QString selected = QInputDialog::getItem(
        this,
        "Add Cluster",
        "Select the cluster to add",
        list,
        0,
        true,
        &ok
    );

    return ok ? selected.toStdString() : "";
}

std::string ProgramDialog::selectNode() {
    std::pair<std::vector<Node>, bool> nodes =
        common::loadJsonFromDirectory<Node>(_nodePath);

    std::vector<NodeWidget*> currNodes = _nodes->items<NodeWidget>();
    nodes.first.erase(
        std::remove_if(
            nodes.first.begin(), nodes.first.end(),
            [&currNodes](const Node& n) {
                const auto it = std::find_if(
                    currNodes.begin(), currNodes.end(),
                    [n](NodeWidget* nw) {
                        return nw->label->text().toStdString() == n.name;
                    }
                );
                return it != currNodes.end();
            }
        ),
        nodes.first.end()
    );

    if (nodes.first.empty()) {
        QMessageBox::information(this, "Add nodes", "No available nodes left to add");
    }

    QStringList list;
    for (const Node& node : nodes.first) {
        list.push_back(QString::fromStdString(node.name));
    }

    bool ok;
    QString selected = QInputDialog::getItem(
        this,
        "Add Node",
        "Select the node to add",
        list,
        0,
        true,
        &ok
    );

    return ok ? selected.toStdString() : "";
}

std::optional<std::pair<std::string, std::string>> ProgramDialog::selectFavorite() {
    QStringList clusters;
    for (ClusterWidget* c : _clusters->items<ClusterWidget>()) {
        clusters.push_back(c->label->text());
    }
    QStringList configurations;
    for (ConfigurationWidget* c : _configurations->items<ConfigurationWidget>()) {
        if (!c->name->text().isEmpty()) {
            configurations.push_back(c->name->text());
        }
    }

    if (clusters.empty() || configurations.empty()) {
        QMessageBox::information(
            this,
            "Add favorite",
            "A favorite requires at least one cluster and one configuration"
        );
        return std::nullopt;
    }

    bool ok = false;
    const QString cluster = QInputDialog::getItem(
        this,
        "Add Favorite",
        "Select the cluster on which the favorite starts the program",
        clusters,
        0,
        false,
        &ok
    );
    if (!ok) {
        return std::nullopt;
    }

    const QString configuration = QInputDialog::getItem(
        this,
        "Add Favorite",
        "Select the configuration with which the favorite starts the program",
        configurations,
        0,
        false,
        &ok
    );
    if (!ok) {
        return std::nullopt;
    }

    return std::pair(cluster.toStdString(), configuration.toStdString());
}

void ProgramDialog::updateSaveButton() {
    std::vector<ConfigurationWidget*> configurations =
        _configurations->items<ConfigurationWidget>();

    const bool confHasName = std::all_of(
        configurations.begin(), configurations.end(),
        [](ConfigurationWidget* config) { return !config->name->text().isEmpty(); }
    );

    _saveButton->setEnabled(
        !_name->text().isEmpty() && !_executable->text().isEmpty() &&
        !_clusters->items<ClusterWidget>().empty() && confHasName
    );
}
