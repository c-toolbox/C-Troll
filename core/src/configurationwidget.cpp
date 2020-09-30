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

#include <QCheckBox>
#include <QColorDialog>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QVBoxLayout>
#include <fmt/format.h>
#include <json/json.hpp>
#include <fstream>
#include <random>

ColorWidget::ColorWidget(Color color)
    : _color(std::move(color))
{
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


//////////////////////////////////////////////////////////////////////////////////////////


ConfigurationWidget::ConfigurationWidget(Configuration configuration,
                                         std::string configurationFilePath)
    : _configuration(std::move(configuration))
    , _configurationFilePath(std::move(configurationFilePath))
{
    QBoxLayout* layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setAlignment(Qt::AlignTop);

    QWidget* controls = new QWidget;
    QGridLayout* controlsLayout = new QGridLayout;
    controls->setLayout(controlsLayout);
    layout->addWidget(controls);

    {
        // Application path

        QString toolTip = "Specifies the location relative to the current working "
            "directory from which application configurations are loaded. Every file with "
            "the '.json' extension will be automatically added, even if they are in "
            "subdirectories.";

        QLabel* label = new QLabel("Application Path:");
        label->setToolTip(toolTip);
        controlsLayout->addWidget(label, 0, 0);

        _applicationPath = new QLineEdit;
        _applicationPath->setToolTip(toolTip);
        connect(
            _applicationPath, &QLineEdit::textEdited,
            this, &ConfigurationWidget::valuesChanged
        );
        controlsLayout->addWidget(_applicationPath, 0, 1);
    }

    {
        // Cluster path
        QString toolTip = "Specifies the location relative to the current working "
            "directory from which the cluster information are loaded. Every file with "
            "the '.json' extension will be automatically added, even if they are in "
            "subdirectories.";

        QLabel* label = new QLabel("Cluster Path:");
        label->setToolTip(toolTip);
        controlsLayout->addWidget(label, 1, 0);

        _clusterPath = new QLineEdit;
        _clusterPath->setToolTip(toolTip);
        connect(
            _clusterPath, &QLineEdit::textEdited,
            this, &ConfigurationWidget::valuesChanged
        );
        controlsLayout->addWidget(_clusterPath, 1, 1);
    }

    {
        // Node path
        QString toolTip = "Specifies the location relative to the current working "
            "directory from which the information about nodes are loaded. Every file "
            "with the '.json' extension will be automatically added, even if they are in "
            "subdirectories.";

        QLabel* label = new QLabel("Node Path:");
        label->setToolTip(toolTip);
        controlsLayout->addWidget(label, 2, 0);

        _nodePath = new QLineEdit;
        _nodePath->setToolTip(toolTip);
        connect(
            _nodePath, &QLineEdit::textEdited,
            this, &ConfigurationWidget::valuesChanged
        );
        controlsLayout->addWidget(_nodePath, 2, 1);
    }

    {
        // Process removal time
        QString toolTip = "Specifies the timeout (in milliseconds) after which a "
            "successfully finished process is removed from the process list.";

        QLabel* label = new QLabel("Process Removal Time:");
        label->setToolTip(toolTip);
        controlsLayout->addWidget(label, 3, 0);

        _removalTimeout = new QSpinBox;
        _removalTimeout->setToolTip(toolTip);
        _removalTimeout->setSuffix(" ms");
        _removalTimeout->setMinimum(0);
        _removalTimeout->setMaximum(std::numeric_limits<int>::max());
        connect(
            _removalTimeout, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ConfigurationWidget::valuesChanged
        );
        controlsLayout->addWidget(_removalTimeout, 3, 1);
    }

    {
        // Use Log File
        QString toolTip = "Determines whether a log file should be created or not";

        QLabel* label = new QLabel("Use Log File:");
        label->setToolTip(toolTip);
        controlsLayout->addWidget(label, 4, 0);

        _logFile = new QCheckBox;
        _logFile->setToolTip(toolTip);
        connect(
            _logFile, &QCheckBox::clicked,
            this, &ConfigurationWidget::valuesChanged
        );
        controlsLayout->addWidget(_logFile, 4, 1);
    }

    {
        // Log Rotation
        QString toolTip = "Controls whether there will be a log rotation or not. Each "
            "'Frequency' hours a rotation is performed if it is enabled and if "
            "'Keep old logs' is enabled, the old log file is backed up. Otherwise, it is "
            "overwritten.";

        _logRotation = new QGroupBox("Log Rotation");
        _logRotation->setToolTip(toolTip);
        _logRotation->setCheckable(true);
        connect(
            _logRotation, &QGroupBox::clicked,
            this, &ConfigurationWidget::valuesChanged
        );

        QGridLayout* contentLayout = new QGridLayout(_logRotation);

        contentLayout->addWidget(new QLabel("Frequency:"), 0, 0);
        _frequency = new QSpinBox;
        _frequency->setSuffix(" h");
        _frequency->setMinimum(0);
        _frequency->setMaximum(std::numeric_limits<int>::max());
        connect(
            _frequency, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &ConfigurationWidget::valuesChanged
        );
        contentLayout->addWidget(_frequency, 0, 1);

        contentLayout->addWidget(new QLabel("Keep old logs:"), 1, 0);
        _keepOldLog = new QCheckBox;
        connect(
            _keepOldLog, &QCheckBox::clicked,
            this, &ConfigurationWidget::valuesChanged
        );
        contentLayout->addWidget(_keepOldLog, 1, 1);

        layout->addWidget(_logRotation);
    }

    {
        // Colors
        QString toolTip = "Provides the ability to select the colors for the individual "
            "tags. Specifying a name for a tag is optional; if it is set, that color is "
            "used for that specific color. All colors that don't have a specific tag "
            "assigned are used for the tags that are not explicitly specified.";

        QGroupBox* colorBox = new QGroupBox("Tag Color");
        colorBox->setToolTip(toolTip);
        QBoxLayout* colorLayout = new QVBoxLayout(colorBox);
        colorLayout->setContentsMargins(5, 5, 5, 5);
        layout->addWidget(colorBox);
        {
            QScrollArea* area = new QScrollArea;
            area->setWidgetResizable(true);
            area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

            QWidget* content = new QWidget;
            area->setWidget(content);
            area->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);
            _colorLayout = new QGridLayout;
            _colorLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
            createColorWidgets();
            content->setLayout(_colorLayout);

            colorLayout->addWidget(area, 1);
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
        colorLayout->addWidget(addColor);
    }

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
        _restoreButton->setObjectName("control");
        connect(
            _restoreButton, &QPushButton::clicked,
            this, &ConfigurationWidget::resetValues
        );
        l->addWidget(_restoreButton);

        _saveButton = new QPushButton("Save File");
        _saveButton->setObjectName("control");
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

    const auto it = std::find(_colors.cbegin(), _colors.cend(), sender);
    assert(it != _colors.cend());
    _colors.erase(it);

    layoutColorWidgets();

    sender->deleteLater();
    valuesChanged();
}

void ConfigurationWidget::valuesChanged() {
    const bool hasValueChanged =
        (_configuration.applicationPath != _applicationPath->text().toStdString()) ||
        (_configuration.clusterPath != _clusterPath->text().toStdString()) ||
        (_configuration.nodePath != _nodePath->text().toStdString()) ||
        (_configuration.removalTimeout.count() != _removalTimeout->value()) ||
        (_configuration.logFile != _logFile->isChecked());

    std::optional<common::LogRotation> lr = _configuration.logRotation;
    bool hasLogRotationChanged = (lr.has_value() != _logRotation->isChecked());
    if (lr.has_value() && _logRotation->isChecked()) {
        hasLogRotationChanged = (lr->frequency.count() != _frequency->value()) ||
            (lr->keepPrevious != _keepOldLog->isChecked());
    }

    const bool hasColorChanged = (_configuration.tagColors != tagColors());
    const bool hasChanged = hasValueChanged || hasLogRotationChanged || hasColorChanged;

    _changesLabel->setVisible(hasChanged);
    _restoreButton->setEnabled(hasChanged);
    _saveButton->setEnabled(hasChanged);
}

void ConfigurationWidget::resetValues() {
    _applicationPath->setText(QString::fromStdString(_configuration.applicationPath));
    _clusterPath->setText(QString::fromStdString(_configuration.clusterPath));
    _nodePath->setText(QString::fromStdString(_configuration.nodePath));
    _removalTimeout->setValue(static_cast<int>(_configuration.removalTimeout.count()));
    _logFile->setChecked(_configuration.logFile);

    if (_configuration.logRotation.has_value()) {
        _logRotation->setChecked(true);
        _frequency->setValue(_configuration.logRotation->frequency.count());
        _keepOldLog->setChecked(_configuration.logRotation->keepPrevious);
    }
    else {
        common::LogRotation lr;
        _logRotation->setChecked(false);
        _frequency->setValue(lr.frequency.count());
        _keepOldLog->setChecked(lr.keepPrevious);
    }

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
    config.logFile = _logFile->isChecked();

    if (_logRotation->isChecked()) {
        common::LogRotation lr;
        lr.frequency = std::chrono::hours(_frequency->value());
        lr.keepPrevious = _keepOldLog->isChecked();
        config.logRotation = lr;
    }

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
