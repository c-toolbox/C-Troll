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

#include "programdialog.h"

#include "addbutton.h"
#include "jsonload.h"
#include "program.h"
#include "removebutton.h"
#include "spacer.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>
#include <filesystem>

ProgramDialog::Configuration::Configuration() {
    QBoxLayout* layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setContentsMargins(0, 0, 0, 0);
    name = new QLineEdit;
    layout->addWidget(name);

    parameters = new QLineEdit;
    parameters->setPlaceholderText("optional");
    layout->addWidget(parameters);
}

bool operator==(const Cluster& c, const std::string& name) {
    return c.name == name;
}

ProgramDialog::ProgramDialog(QWidget* parent, std::string programPath,
                             std::string clusterPath)
    : QDialog(parent)
    , _programPath(std::move(programPath))
    , _clusterPath(std::move(clusterPath))
{
    setWindowTitle(QString::fromStdString("Program: " + _programPath));

    QBoxLayout* layout = new QVBoxLayout(this);

    {
        QWidget* edit = new QWidget;
        QGridLayout* editLayout = new QGridLayout(edit);
        editLayout->setMargin(0);

        editLayout->addWidget(new QLabel("Name:"), 0, 0);
        _name = new QLineEdit;
        _name->setToolTip("The name of this program");
        editLayout->addWidget(_name, 0, 1);

        editLayout->addWidget(new QLabel("Executable:"), 1, 0);
        _executable = new QLineEdit;
        _executable->setToolTip(
            "The command that will be executed when this program is started"
        );
        editLayout->addWidget(_executable, 1, 1);

        editLayout->addWidget(new QLabel("Commandline Parameters:"), 2, 0);
        _commandLineParameters = new QLineEdit;
        _commandLineParameters->setPlaceholderText("optional");
        _commandLineParameters->setToolTip(
            "Global commandline parameters that will be added to the command regardless "
            "of the configuration that was selected"
        );
        editLayout->addWidget(_commandLineParameters, 2, 1);

        editLayout->addWidget(new QLabel("Working Directory:"), 3, 0);
        _workingDirectory = new QLineEdit;
        _workingDirectory->setPlaceholderText("optional");
        _workingDirectory->setToolTip("The directory in which the program will run");
        editLayout->addWidget(_workingDirectory, 3, 1);

        editLayout->addWidget(new QLabel("Forward Messages:"), 4, 0);
        _shouldForwardMessages = new QCheckBox;
        _shouldForwardMessages->setToolTip(
            "If this is enabled, all console messages from the executable will be sent "
            "back to C-Troll"
        );
        editLayout->addWidget(_shouldForwardMessages, 4, 1);

        editLayout->addWidget(new QLabel("Delay"), 5, 0);
        QWidget* delayContainer = new QWidget;
        QBoxLayout* delayLayout = new QHBoxLayout(delayContainer);
        delayLayout->setMargin(0);
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
        editLayout->addWidget(delayContainer, 5, 1);

        editLayout->addWidget(new Spacer, 6, 0);

        {
            // Tags
            editLayout->addWidget(new QLabel("Tags"), 7, 0);

            QPushButton* t = new AddButton;
            connect(
                t, &QPushButton::clicked,
                [this]() {
                    QLineEdit* tag = new QLineEdit;
                    _tags->addItem(tag);
                    updateSaveButton();
                }
            );
            editLayout->addWidget(t, 7, 1, Qt::AlignRight);

            _tags = new DynamicList;
            _tags->setToolTip("A list of all tags that this program is associated with");
            connect(_tags, &DynamicList::updated, this, &ProgramDialog::updateSaveButton);
         
            editLayout->addWidget(_tags, 8, 0, 1, 2);
        }

        editLayout->addWidget(new Spacer, 9, 0);

        {
            // Configurations

            editLayout->addWidget(new QLabel("Configurations"), 10, 0);

            QPushButton* newConfiguration = new AddButton;
            connect(
                newConfiguration, &QPushButton::clicked,
                [this]() {
                    Configuration* config = new Configuration;
                    connect(
                        config->name, &QLineEdit::textChanged,
                        this, &ProgramDialog::updateSaveButton
                    );
                    _configurations->addItem(config);
                }
            );
            editLayout->addWidget(newConfiguration, 10, 1, Qt::AlignRight);

            _configurations = new DynamicList;
            _configurations->setToolTip(
                "A list of all configurations that are available for this program"
            );
            connect(
                _configurations, &DynamicList::updated,
                this, &ProgramDialog::updateSaveButton
            );
            editLayout->addWidget(_configurations, 11, 0, 1, 2);
        }

        editLayout->addWidget(new Spacer, 12, 0);

        {
            // Clusters

            editLayout->addWidget(new QLabel("Clusters"), 13, 0);

            QPushButton* newCluster = new AddButton;
            connect(
                newCluster, &QPushButton::clicked,
                [this]() {
                    std::string name = selectCluster();
                    if (!name.empty()) {
                        QLabel* cluster = new QLabel(QString::fromStdString(name));
                        _clusters->addItem(cluster);
                        updateSaveButton();
                    }
                }
            );
            editLayout->addWidget(newCluster, 13, 1, Qt::AlignRight);

            _clusters = new DynamicList;
            _clusters->setToolTip("The list of clusters on which the program can be run");
            connect(
                _clusters, &DynamicList::updated,
                this, &ProgramDialog::updateSaveButton
            );
            editLayout->addWidget(_clusters, 14, 0, 1, 2);
        }

        layout->addWidget(edit);
    }


    QDialogButtonBox* box = new QDialogButtonBox(
        QDialogButtonBox::Save | QDialogButtonBox::Cancel
    );
    _saveButton = box->button(QDialogButtonBox::Save);
    connect(box, &QDialogButtonBox::accepted, this, &ProgramDialog::save);
    connect(box, &QDialogButtonBox::rejected, this, &ProgramDialog::reject);
    layout->addWidget(box, 0, Qt::AlignRight);


    if (std::filesystem::exists(_programPath)) {
        Program program = common::loadFromJson<Program>(_programPath);
        std::vector<Cluster> clusters = common::loadJsonFromDirectory<Cluster>(
            _clusterPath
        );

        _name->setText(QString::fromStdString(program.name));
        _executable->setText(QString::fromStdString(program.executable));
        _commandLineParameters->setText(
            QString::fromStdString(program.commandlineParameters)
        );
        _workingDirectory->setText(QString::fromStdString(program.workingDirectory));
        _shouldForwardMessages->setChecked(program.shouldForwardMessages);
        _hasDelay->setChecked(program.delay.has_value());
        if (program.delay.has_value()) {
            _delay->setValue(program.delay->count());
        }
        for (const std::string& tag : program.tags) {
            QLineEdit* t = new QLineEdit(QString::fromStdString(tag));
            _tags->addItem(t);
        }
        for (const Program::Configuration& configuration : program.configurations) {
            Configuration* config = new Configuration;
            connect(
                config->name, &QLineEdit::textChanged,
                this, &ProgramDialog::updateSaveButton
            );

            config->name->setText(QString::fromStdString(configuration.name));
            config->parameters->setText(QString::fromStdString(configuration.parameters));
            _configurations->addItem(config);
        }
        for (const std::string& cluster : program.clusters) {
            QLabel* clusterLabel = new QLabel(QString::fromStdString(cluster));
            _clusters->addItem(clusterLabel);

            const auto it = std::find(clusters.cbegin(), clusters.cend(), cluster);
            if (it == clusters.end()) {
                clusterLabel->setObjectName("invalid");
                clusterLabel->setToolTip("Could not find cluster in clusters folder");
            }
        }
    }

    updateSaveButton();
}

void ProgramDialog::save() {
    Program program;
    program.name = _name->text().toStdString();
    program.executable = _executable->text().toStdString();
    program.commandlineParameters = _commandLineParameters->text().toStdString();
    program.workingDirectory = _workingDirectory->text().toStdString();
    program.shouldForwardMessages = _shouldForwardMessages->isChecked();
    if (_hasDelay->isChecked()) {
        program.delay = std::chrono::milliseconds(_delay->value());
    }
    for (QLineEdit* tag : _tags->items<QLineEdit>()) {
        program.tags.push_back(tag->text().toStdString());
    }
    for (Configuration* configuration : _configurations->items<Configuration>()) {
        Program::Configuration c;
        c.name = configuration->name->text().toStdString();
        c.parameters = configuration->parameters->text().toStdString();
        program.configurations.push_back(c);
    }
    for (QLabel* cluster : _clusters->items<QLabel>()) {
        program.clusters.push_back(cluster->text().toStdString());
    }

    common::saveToJson(_programPath, program);

    accept();
}

bool operator==(QLabel* name, const Cluster& cluster) {
    return cluster.name == name->text().toStdString();
}

std::string ProgramDialog::selectCluster() {
    std::vector<Cluster> clusters = common::loadJsonFromDirectory<Cluster>(_clusterPath);

    std::vector<QLabel*> currClusters = _clusters->items<QLabel>();
        clusters.erase(
        std::remove_if(
            clusters.begin(), clusters.end(),
            [&currClusters](const Cluster& c) {
                const auto it = std::find(currClusters.cbegin(), currClusters.cend(), c);
                return it != currClusters.cend();
            }
        ),
        clusters.end()
    );

    if (clusters.empty()) {
        QMessageBox::information(
            this,
            "Add clusters",
            "No available clusters left to add"
        );
    }

    QStringList list;
    for (const Cluster& cluster : clusters) {
        list.push_back(QString::fromStdString(cluster.name));
    }

    QString selected = QInputDialog::getItem(
        this,
        "Add Cluster",
        "Select the cluster to add",
        list
    );

    return !selected.isEmpty() ? selected.toStdString() : "";
}

void ProgramDialog::updateSaveButton() {
    std::vector<Configuration*> configurations = _configurations->items<Configuration>();
    const bool confHasName = std::all_of(
        configurations.cbegin(), configurations.cend(),
        [](Configuration* config) { return !config->name->text().isEmpty(); }
    );

    _saveButton->setEnabled(
        !_name->text().isEmpty() && !_executable->text().isEmpty() &&
        !_clusters->items<QLabel>().empty() && confHasName
    );
}
