/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2023                                                             *
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

#ifndef __CTROLL__SETTINGSWIDGET_H__
#define __CTROLL__SETTINGSWIDGET_H__

#include <QWidget>

#include "configuration.h"
#include <string>

class QCheckBox;
class QGridLayout;
class QGroupBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

class ColorWidget : public QWidget {
Q_OBJECT
public:
    ColorWidget(Color color);

    void setColor(Color color);
    Color color() const;

signals:
    void colorChanged(Color color);
    void removed(ColorWidget* sender);

private:
    void applyColor();

    QPushButton* _colorButton = nullptr;
    QLineEdit* _tagLine = nullptr;
    Color _color;
};


//////////////////////////////////////////////////////////////////////////////////////////


class SettingsWidget : public QWidget {
Q_OBJECT
public:
    SettingsWidget(Configuration configuration, std::string filePath);

private slots:
    void removedColor(ColorWidget* sender);

    void valuesChanged();
    void resetValues();
    void saveValues();

private:
    struct RestControls;

    void layoutColorWidgets();
    void createColorWidget(Color color);
    void createColorWidgets();
    void createRestWidgets(RestControls& rest, QString title);

    std::vector<Color> tagColors() const;

    QLineEdit* _applicationPath = nullptr;
    QLineEdit* _clusterPath = nullptr;
    QLineEdit* _nodePath = nullptr;
    QSpinBox* _removalTimeout = nullptr;
    QCheckBox* _logFile = nullptr;

    // Log Rotation
    QGroupBox* _logRotation = nullptr;
    QSpinBox* _frequency = nullptr;
    QCheckBox* _keepOldLog = nullptr;

    // REST controls
    struct RestControls {
        QGroupBox* box = nullptr;
        QLineEdit* username = nullptr;
        QLineEdit* password = nullptr;
        QSpinBox* port = nullptr;
        QCheckBox* allowCustomPrograms = nullptr;
    };
    RestControls _restLoopback;
    RestControls _restGeneral;

    QGridLayout* _colorLayout = nullptr;
    std::vector<ColorWidget*> _colors;

    QLabel* _changesLabel = nullptr;

    QPushButton* _restoreButton = nullptr;
    QPushButton* _saveButton = nullptr;

    Configuration _configuration;
    const std::string _configurationFilePath;
};

#endif // __CTROLL__SETTINGSWIDGET_H__
