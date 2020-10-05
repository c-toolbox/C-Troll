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

#include "jsonload.h"
#include "program.h"
#include <QCheckBox>
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
        editLayout->addWidget(_name, 0, 1);

        editLayout->addWidget(new QLabel("Executable:"), 1, 0);
        _executable = new QLineEdit;
        editLayout->addWidget(_executable, 1, 1);

        editLayout->addWidget(new QLabel("Commandline Parameters:"), 2, 0);
        _commandLineParameters = new QLineEdit;
        editLayout->addWidget(_commandLineParameters, 2, 1);

        editLayout->addWidget(new QLabel("Working Directory:"), 3, 0);
        _workingDirectory = new QLineEdit;
        editLayout->addWidget(_workingDirectory, 3, 1);

        editLayout->addWidget(new QLabel("Forward Messages:"), 4, 0);
        _shouldForwardMessages = new QCheckBox;
        editLayout->addWidget(_shouldForwardMessages, 4, 1);

        editLayout->addWidget(new QLabel("Delay"), 5, 0);
        QWidget* delayContainer = new QWidget;
        QBoxLayout* delayLayout = new QHBoxLayout(delayContainer);
        delayLayout->setMargin(0);
        delayLayout->setContentsMargins(0, 0, 0, 0);
        delayLayout->setSpacing(0);
        _hasDelay = new QCheckBox("Enabled");
        connect(
            _hasDelay, &QCheckBox::clicked,
            [this]() { _delay->setEnabled(_hasDelay->isChecked()); }
        );
        delayLayout->addWidget(_hasDelay);
        _delay = new QSpinBox;
        _delay->setMinimum(0);
        _delay->setMaximum(std::numeric_limits<int>::max());
        delayLayout->addWidget(_delay);
        editLayout->addWidget(delayContainer, 5, 1);

        {
            // Tags

            editLayout->addWidget(new QLabel("Tags"), 6, 0);

            QPushButton* newTag = new QPushButton("+");
            newTag->setObjectName("add");
            connect(
                newTag, &QPushButton::clicked,
                this, &ProgramDialog::addTag
            );
            editLayout->addWidget(newTag, 6, 1, Qt::AlignRight);


            QScrollArea* area = new QScrollArea;
            area->setWidgetResizable(true);
            area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

            QWidget* tagContainer = new QWidget;
            area->setWidget(tagContainer);

            _tagLayout = new QVBoxLayout(tagContainer);
            _tagLayout->setAlignment(Qt::AlignTop);
            _tagLayout->setMargin(0);
            _tagLayout->setContentsMargins(0, 0, 0, 0);
            _tagLayout->setSpacing(0);
            editLayout->addWidget(area, 7, 0, 1, 2);
        }

        {
            // Configurations

            editLayout->addWidget(new QLabel("Configurations"), 8, 0);

            QPushButton* newConfiguration = new QPushButton("+");
            newConfiguration->setObjectName("add");
            connect(
                newConfiguration, &QPushButton::clicked,
                this, &ProgramDialog::addConfiguration
            );
            editLayout->addWidget(newConfiguration, 8, 1, Qt::AlignRight);


            QScrollArea* area = new QScrollArea;
            area->setWidgetResizable(true);
            area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

            QWidget* configurationContainer = new QWidget;
            area->setWidget(configurationContainer);

            _configurationLayout = new QVBoxLayout(configurationContainer);
            _configurationLayout->setAlignment(Qt::AlignTop);
            _configurationLayout->setMargin(0);
            _configurationLayout->setContentsMargins(0, 0, 0, 0);
            _configurationLayout->setSpacing(0);
            editLayout->addWidget(area, 9, 0, 1, 2);
        }

        {
            // Clusters

            editLayout->addWidget(new QLabel("Clusters"), 10, 0);

            QPushButton* newCluster = new QPushButton("+");
            newCluster->setObjectName("add");
            connect(
                newCluster, &QPushButton::clicked,
                [this]() {
                    std::string name = selectCluster();
                    if (!name.empty()) {
                        addCluster(name);
                    }
                }
            );
            editLayout->addWidget(newCluster, 10, 1, Qt::AlignRight);


            QScrollArea* area = new QScrollArea;
            area->setWidgetResizable(true);
            area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

            QWidget* clusterContainer = new QWidget;
            area->setWidget(clusterContainer);

            _clusterLayout = new QVBoxLayout(clusterContainer);
            _clusterLayout->setAlignment(Qt::AlignTop);
            _clusterLayout->setMargin(0);
            _clusterLayout->setContentsMargins(0, 0, 0, 0);
            _clusterLayout->setSpacing(0);
            editLayout->addWidget(area, 11, 0, 1, 2);
        }

        layout->addWidget(edit);
    }

    {
        QWidget* control = new QWidget;
        QBoxLayout* controlLayout = new QHBoxLayout(control);

        controlLayout->addStretch(5);

        QPushButton* save = new QPushButton("Save");
        connect(save, &QPushButton::clicked, this, &ProgramDialog::save);
        controlLayout->addWidget(save);

        QPushButton* cancel = new QPushButton("Cancel");
        connect(cancel, &QPushButton::clicked, this, &QDialog::reject);
        controlLayout->addWidget(cancel);


        layout->addWidget(control);
    }

    if (std::filesystem::exists(_programPath)) {
        Program program = common::loadFromJson<Program>(_programPath);

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
            QLineEdit* line = addTag();
            line->setText(QString::fromStdString(tag));
        }
        for (const Program::Configuration& configuration : program.configurations) {
            Configuration* line = addConfiguration();
            line->name->setText(QString::fromStdString(configuration.name));
            line->parameters->setText(QString::fromStdString(configuration.parameters));
        }
        for (const std::string& cluster : program.clusters) {
            addCluster(cluster);
        }
    }
}

void ProgramDialog::save() {
    if (_name->text().isEmpty()) {
        QMessageBox::critical(this, "Error", "Name must not be empty");
        return;
    }

    if (_executable->text().isEmpty()) {
        QMessageBox::critical(this, "Error", "Executable must not be empty");
        return;
    }


    Program program;
    program.name = _name->text().toStdString();
    program.executable = _executable->text().toStdString();
    program.commandlineParameters = _commandLineParameters->text().toStdString();
    program.workingDirectory = _workingDirectory->text().toStdString();
    program.shouldForwardMessages = _shouldForwardMessages->isChecked();
    if (_hasDelay->isChecked()) {
        program.delay = std::chrono::milliseconds(_delay->value());
    }
    for (QLineEdit* tag : _tags) {
        program.tags.push_back(tag->text().toStdString());
    }
    for (const Configuration& configuration : _configurations) {
        Program::Configuration c;
        c.name = configuration.name->text().toStdString();
        c.parameters = configuration.parameters->text().toStdString();
        program.configurations.push_back(c);
    }
    for (QLabel* cluster : _clusters) {
        program.clusters.push_back(cluster->text().toStdString());
    }

    if (std::filesystem::path(_programPath).extension().empty()) {
        common::saveToJson(_programPath + ".json", program);
    }
    else {
        common::saveToJson(_programPath, program);
    }

    accept();
}

QLineEdit* ProgramDialog::addTag() {
    QWidget* container = new QWidget;
    QBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(10, 5, 10, 5);

    QLineEdit* tag = new QLineEdit();
    layout->addWidget(tag);

    QPushButton* remove = new QPushButton("X");
    remove->setObjectName("remove");
    connect(remove, &QPushButton::clicked, [this, tag]() { removeTag(tag); });
    layout->addWidget(remove);

    _tagLayout->addWidget(container);
    _tags.push_back(tag);
    return tag;
}

void ProgramDialog::removeTag(QLineEdit* sender) {
    const auto it = std::find(_tags.cbegin(), _tags.cend(), sender);
    assert(it != _tags.cend());

    _tags.erase(it);
    _tagLayout->removeWidget(sender->parentWidget());
    sender->parent()->deleteLater();
}

ProgramDialog::Configuration* ProgramDialog::addConfiguration() {
    QWidget* container = new QWidget;
    QBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(10, 5, 10, 5);

    Configuration config;
    config.name = new QLineEdit;
    layout->addWidget(config.name);

    config.parameters = new QLineEdit;
    layout->addWidget(config.parameters);

    QPushButton* remove = new QPushButton("X");
    remove->setObjectName("remove");
    connect(
        remove, &QPushButton::clicked,
        [this, config]() { removeConfiguration(config); }
    );
    layout->addWidget(remove);

    _configurationLayout->addWidget(container);
    _configurations.push_back(config);

    return &_configurations.back();
}

void ProgramDialog::removeConfiguration(const Configuration& sender) {
    const auto it = std::find_if(
        _configurations.cbegin(), _configurations.cend(),
        [sender](const Configuration& v) {
            return v.name == sender.name && v.parameters == sender.parameters;
        }
    );

    assert(it != _configurations.cend());

    _configurations.erase(it);

    assert(sender.name->parentWidget() == sender.parameters->parentWidget());
    QWidget* parent = sender.name->parentWidget();
    _configurationLayout->removeWidget(parent);
    parent->deleteLater();
}

std::string ProgramDialog::selectCluster() {
    std::vector<Cluster> clusters = common::loadJsonFromDirectory<Cluster>(_clusterPath);

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

QLabel* ProgramDialog::addCluster(std::string clusterName) {
    QWidget* container = new QWidget;
    QBoxLayout* layout = new QHBoxLayout(container);
    layout->setContentsMargins(10, 5, 10, 5);

    QLabel* cluster = new QLabel(QString::fromStdString(clusterName));
    cluster->setTextInteractionFlags(Qt::TextSelectableByMouse);
    cluster->setCursor(QCursor(Qt::IBeamCursor));
    layout->addWidget(cluster);

    QPushButton* remove = new QPushButton("X");
    remove->setObjectName("remove");
    connect(remove, &QPushButton::clicked, [this, cluster]() { removeCluster(cluster); });
    layout->addWidget(remove);

    _clusterLayout->addWidget(container);
    _clusters.push_back(cluster);
    return cluster;
}

void ProgramDialog::removeCluster(QLabel* sender) {
    const auto it = std::find(_clusters.cbegin(), _clusters.cend(), sender);
    assert(it != _clusters.cend());

    _clusters.erase(it);
    _clusterLayout->removeWidget(sender->parentWidget());
    sender->parent()->deleteLater();
}