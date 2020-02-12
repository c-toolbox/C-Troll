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

#ifndef __CORE__PROGRAMWIDGET_H__
#define __CORE__PROGRAMWIDGET_H__

#include <QWidget>

#include <QPushButton>
#include "cluster.h"
#include "program.h"
#include <vector>

class ConfigurationWidget : public QWidget {
Q_OBJECT
public:
    ConfigurationWidget(const Program::Configuration& configuration);

    void updateStatus(const Cluster& cluster);

signals:
    void startProgram(const std::string& clusterId);

private:
    const Program::Configuration& _configuration;

    QPushButton* _startButton = nullptr;
};


 //////////////////////////////////////////////////////////////////////////////////////////


class ProgramWidget : public QWidget {
Q_OBJECT
public:
    ProgramWidget(const Program& program);

    void updateStatus(const Cluster& cluster);

signals:
    void startProgram(const Program::Configuration& configuration, const std::string& clusterId);

private:
    const Program& _program;
    std::vector<ConfigurationWidget*> _widgets;
};


//////////////////////////////////////////////////////////////////////////////////////////


class ProgramsWidget : public QWidget {
    Q_OBJECT
public:
    ProgramsWidget(const std::vector<Program>& programs);

public slots:
    void connectedStatusChanged(const Cluster& cluster);

signals:
    void startProgram(const Program& program, const Program::Configuration& configuration,
        const std::string& clusterId);

private:
    const std::vector<Program>& _programs;
    std::vector<ProgramWidget*> _widgets;
};

#endif // __CORE__PROGRAMWIDGET_H__
