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

#ifndef __EDITOR__PROGRAMDIALOG_H__
#define __EDITOR__PROGRAMDIALOG_H__

#include <QDialog>

#include "dynamiclist.h"
#include <QWidget>
#include <string>
#include <vector>

class QBoxLayout;
class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

class ProgramDialog : public QDialog {
Q_OBJECT
public:
    struct Configuration : QWidget {
        Configuration();

        QLineEdit* name = nullptr;
        QLineEdit* parameters = nullptr;
    };

    ProgramDialog(QWidget* parent, std::string programPath, std::string clusterPath);

private slots:
    void save();
    void updateSaveButton();

private:
    std::string selectCluster();
    
    const std::string _programPath;
    const std::string _clusterPath;

    QLineEdit* _name = nullptr;
    QLineEdit* _executable = nullptr;
    QLineEdit* _commandLineParameters = nullptr;
    QLineEdit* _workingDirectory = nullptr;
    QCheckBox* _shouldForwardMessages = nullptr;

    QCheckBox* _hasDelay = nullptr;
    QSpinBox* _delay = nullptr;

    DynamicList* _tags = nullptr;
    DynamicList* _configurations = nullptr;
    DynamicList* _clusters = nullptr;

    QPushButton* _saveButton = nullptr;
};

#endif // __EDITOR__PROGRAMDIALOG_H__
