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

#ifndef __CORE__CONFIGURATIONWIDGET_H__
#define __CORE__CONFIGURATIONWIDGET_H__

#include <QWidget>

#include "configuration.h"
#include <string>

class QGridLayout;
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
    QPushButton* _colorButton = nullptr;
    Color _color;
};

class ConfigurationWidget : public QWidget {
Q_OBJECT

public:
    ConfigurationWidget(Configuration configuration, std::string filePath);

private slots:
    void removedColor(ColorWidget* sender);

    void valuesChanged();
    void resetValues();
    void saveValues();

private:
    void layoutColorWidgets();
    void createColorWidget(const Color& color);
    void createColorWidgets();

    std::vector<Color> tagColors() const;

    QLineEdit* _applicationPath = nullptr;
    QLineEdit* _clusterPath = nullptr;
    QLineEdit* _nodePath = nullptr;
    QSpinBox* _removalTimeout = nullptr;
    QGridLayout* _colorLayout = nullptr;
    std::vector<ColorWidget*> _colors;

    QLabel* _changesLabel = nullptr;

    QPushButton* _restoreButton = nullptr;
    QPushButton* _saveButton = nullptr;

    Configuration _configuration;
    const std::string _configurationFilePath;
};

#endif // __CORE__CONFIGURATIONWIDGET_H__
