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

#include "configurationwidget.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinbox>
#include <QVBoxLayout>
#include <json/json.hpp>
#include <fstream>

ConfigurationWidget::ConfigurationWidget(Configuration configuration,
                                         std::string configurationFilePath)
    : _configuration(std::move(configuration))
    , _configurationFilePath(std::move(configurationFilePath))
{
    QBoxLayout* layout = new QVBoxLayout;
    layout->setAlignment(Qt::AlignTop);

    // Application path
    {
        QWidget* line = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(line);

        l->addWidget(new QLabel("Application Path:"));

        _applicationPath = new QLineEdit;
        connect(
            _applicationPath, &QLineEdit::textEdited,
            this, &ConfigurationWidget::valuesChanged
        );
        l->addWidget(_applicationPath);

        layout->addWidget(line);
    }

    // Cluster path
    {
        QWidget* line = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(line);

        l->addWidget(new QLabel("Cluster Path:"));

        _clusterPath = new QLineEdit;
        connect(
            _clusterPath, &QLineEdit::textEdited,
            this, &ConfigurationWidget::valuesChanged
        );
        l->addWidget(_clusterPath);

        layout->addWidget(line);
    }

    // Node path
    {
        QWidget* line = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(line);

        l->addWidget(new QLabel("Node Path:"));

        _nodePath = new QLineEdit;
        connect(
            _nodePath, &QLineEdit::textEdited,
            this, &ConfigurationWidget::valuesChanged
        );
        l->addWidget(_nodePath);

        layout->addWidget(line);
    }

    // Process removal time
    {
        QWidget* line = new QWidget;
        QVBoxLayout* l = new QVBoxLayout(line);

        l->addWidget(new QLabel("Process Removal Time:"));

        _removalTimeout = new QSpinBox;
        _removalTimeout->setSuffix(" ms");
        _removalTimeout->setMinimum(0);
        _removalTimeout->setMaximum(std::numeric_limits<int>::max());
        connect(
            _removalTimeout, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ConfigurationWidget::valuesChanged
        );
        l->addWidget(_removalTimeout);

        layout->addWidget(line);
    }

    layout->addStretch();

    _changesLabel = new QLabel("Changes are only used after a restart of C-Troll");
    layout->addWidget(_changesLabel);

    // Buttons
    {
        QWidget* line = new QWidget;
        QBoxLayout* l = new QHBoxLayout(line);

        l->addStretch();
        _restoreButton = new QPushButton("Restore");
        connect(
            _restoreButton, &QPushButton::clicked,
            this, &ConfigurationWidget::resetValues
        );
        l->addWidget(_restoreButton);

        _saveButton = new QPushButton("Save File");
        connect(
            _saveButton, &QPushButton::clicked,
            this, &ConfigurationWidget::saveValues
        );
        l->addWidget(_saveButton);

        layout->addWidget(line);
    }

    setLayout(layout);
    resetValues();
}

void ConfigurationWidget::valuesChanged() {
    const bool hasChanged =
        (_configuration.applicationPath != _applicationPath->text().toStdString()) ||
        (_configuration.clusterPath != _clusterPath->text().toStdString()) ||
        (_configuration.nodePath != _nodePath->text().toStdString()) ||
        (_configuration.removalTimeout.count() != _removalTimeout->value());

    _changesLabel->setVisible(hasChanged);
    _restoreButton->setEnabled(hasChanged);
    _saveButton->setEnabled(hasChanged);
}

void ConfigurationWidget::resetValues() {
    _applicationPath->setText(QString::fromStdString(_configuration.applicationPath));
    _clusterPath->setText(QString::fromStdString(_configuration.clusterPath));
    _nodePath->setText(QString::fromStdString(_configuration.nodePath));
    _removalTimeout->setValue(static_cast<int>(_configuration.removalTimeout.count()));

    valuesChanged();
}

void ConfigurationWidget::saveValues() {
    Configuration config;
    config.applicationPath = _applicationPath->text().toStdString();
    config.clusterPath = _clusterPath->text().toStdString();
    config.nodePath = _nodePath->text().toStdString();
    config.removalTimeout = std::chrono::milliseconds(_removalTimeout->value());

    nlohmann::json j;
    to_json(j, config);

    std::ofstream file(_configurationFilePath);
    file << j.dump(2);

    _configuration = config;
    valuesChanged();
}
