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

#include <QColorDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinbox>
#include <QVBoxLayout>
#include <fmt/format.h>
#include <json/json.hpp>
#include <fstream>
#include <random>

ColorWidget::ColorWidget(Color color)
    : _color(std::move(color))
{
    setObjectName("color");

    QGridLayout* layout = new QGridLayout;

    _colorButton = new QPushButton;
    applyColor();

    connect(
        _colorButton, &QPushButton::clicked,
        [this]() {
            QColor initial = QColor(_color.r, _color.g, _color.b);
            QColor selected = QColorDialog::getColor(initial);
            if (selected.isValid()) {
                Color res;
                res.r = selected.red();
                res.g = selected.green();
                res.b = selected.blue();
                res.tag = _tagLine->text().toStdString();
                emit colorChanged(res);
            }
        }
    );
    layout->addWidget(_colorButton, 0, 0);

    QPushButton* remove = new QPushButton;
    remove->setIcon(QIcon(":/images/x.png"));
    connect(
        remove, &QPushButton::clicked,
        [this]() { emit removed(this); }
    );
    layout->addWidget(remove, 0, 1);


    _tagLine = new QLineEdit;
    _tagLine->setText(QString::fromStdString(_color.tag));
    _tagLine->setPlaceholderText("Tag this color applies to");
    connect(
        _tagLine, &QLineEdit::textEdited,
        [this]() {
        Color res = _color;
        res.tag = _tagLine->text().toStdString();
        emit colorChanged(res);
    }
    );
    layout->addWidget(_tagLine, 1, 0, 1, 2);

    setLayout(layout);
}

void ColorWidget::setColor(Color color) {
    _color = std::move(color);
    applyColor();
}

Color ColorWidget::color() const {
    return _color;
}

void ColorWidget::applyColor() {
    _colorButton->setStyleSheet(
        QString::fromStdString(
            fmt::format(
                "background-color: #{0:02x}{1:02x}{2:02x};", _color.r, _color.g, _color.b
            )
        )
    );
}


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

    // Colors
    {
        QScrollArea* area = new QScrollArea;
        area->setWidgetResizable(true);
        area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        QWidget* content = new QWidget;
        area->setWidget(content);
        area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
        _colorLayout = new QGridLayout;
        createColorWidgets();
        content->setLayout(_colorLayout);

        layout->addWidget(area);
    }

    QPushButton* addColor = new QPushButton("Add new color");
    connect(
        addColor, &QPushButton::clicked,
        [this]() {
            std::random_device rd;
            std::uniform_int_distribution<int> dist(0, 255);

            Color c;
            c.r = dist(rd);
            c.g = dist(rd);
            c.b = dist(rd);
            createColorWidget(c);
            layoutColorWidgets();
            valuesChanged();
        }
    );
    layout->addWidget(addColor);

    // Separator
    {
        
        QFrame* separator = new QFrame;
        separator->setObjectName("line");
        separator->setFrameShape(QFrame::HLine);
        separator->setFrameShadow(QFrame::Sunken);
        layout->addWidget(separator);
    }

    // Buttons
    {
        QWidget* line = new QWidget;
        QBoxLayout* l = new QHBoxLayout(line);

        _changesLabel = new QLabel("Changes are only used after a restart of C-Troll");
        _changesLabel->setObjectName("important");
        l->addWidget(_changesLabel);

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

void ConfigurationWidget::removedColor(ColorWidget* sender) {
    _colorLayout->removeWidget(sender);

    const auto it = std::find(_colors.begin(), _colors.end(), sender);
    assert(it != _colors.end());
    _colors.erase(it);

    layoutColorWidgets();

    sender->deleteLater();
    valuesChanged();
}

void ConfigurationWidget::valuesChanged() {
    const bool hasChanged =
        (_configuration.applicationPath != _applicationPath->text().toStdString()) ||
        (_configuration.clusterPath != _clusterPath->text().toStdString()) ||
        (_configuration.nodePath != _nodePath->text().toStdString()) ||
        (_configuration.removalTimeout.count() != _removalTimeout->value()) ||
        _configuration.tagColors != tagColors();

    _changesLabel->setVisible(hasChanged);
    _restoreButton->setEnabled(hasChanged);
    _saveButton->setEnabled(hasChanged);
}

void ConfigurationWidget::resetValues() {
    _applicationPath->setText(QString::fromStdString(_configuration.applicationPath));
    _clusterPath->setText(QString::fromStdString(_configuration.clusterPath));
    _nodePath->setText(QString::fromStdString(_configuration.nodePath));
    _removalTimeout->setValue(static_cast<int>(_configuration.removalTimeout.count()));

    for (ColorWidget* cw : _colors) {
        _colorLayout->removeWidget(cw);
        cw->deleteLater();
    }
    _colors.clear();
    createColorWidgets();

    valuesChanged();
}

void ConfigurationWidget::saveValues() {
    Configuration config;
    config.applicationPath = _applicationPath->text().toStdString();
    config.clusterPath = _clusterPath->text().toStdString();
    config.nodePath = _nodePath->text().toStdString();
    config.removalTimeout = std::chrono::milliseconds(_removalTimeout->value());
    config.tagColors = tagColors();


    nlohmann::json j;
    to_json(j, config);

    std::ofstream file(_configurationFilePath);
    file << j.dump(2);

    _configuration = config;
    valuesChanged();
}

void ConfigurationWidget::layoutColorWidgets() {
    constexpr const int Columns = 5;

    // First remove all widgets from the layout
    for (ColorWidget* w : _colors) {
        _colorLayout->removeWidget(w);
    }

    // Then readd them
    for (size_t i = 0; i < _colors.size(); ++i) {
        const int rowIdx = static_cast<int>(i / Columns);
        const int columnIdx = static_cast<int>(i % Columns);

        _colorLayout->addWidget(_colors[i], rowIdx, columnIdx);
    }
}

void ConfigurationWidget::createColorWidget(Color color) {
    ColorWidget* cw = new ColorWidget(std::move(color));
    connect(
        cw, &ColorWidget::colorChanged,
        [this, cw](Color c) {
            cw->setColor(c);
            valuesChanged();
        }
    );
    connect(
        cw, &ColorWidget::removed,
        this, &ConfigurationWidget::removedColor
    );

    _colors.push_back(cw);
}

void ConfigurationWidget::createColorWidgets() {
    for (const Color& c : _configuration.tagColors) {
        createColorWidget(c);
    }
    layoutColorWidgets();
}

std::vector<Color> ConfigurationWidget::tagColors() const {
    std::vector<Color> tagColors;
    tagColors.reserve(_colors.size());
    for (ColorWidget* w : _colors) {
        tagColors.push_back(w->color());
    }
    return tagColors;
}
