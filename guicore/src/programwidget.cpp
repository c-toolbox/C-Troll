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

#include "programwidget.h"

#include "logging.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ConfigurationWidget::ConfigurationWidget(const Program::Configuration& configuration)
    : _configuration(configuration)
{
    QBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    QLabel* label = new QLabel(QString::fromStdString(configuration.name));
    layout->addWidget(label);

    for (const std::pair<const std::string, std::string>& p :
         configuration.clusterCommandlineParameters)
    {
        _startButton = new QPushButton(
            QString::fromStdString(p.first + '/' + configuration.name)
        );
        _startButton->setEnabled(false);
        connect(
            _startButton, &QPushButton::clicked,
            [this, configuration, p]() { emit startProgram(p.first); }
        );
        layout->addWidget(_startButton);
    }
}

void ConfigurationWidget::updateStatus(const Cluster& cluster) {
    for (const std::pair<const std::string, std::string>& p : 
         _configuration.clusterCommandlineParameters)
    {
        if (p.first == cluster.id) {
            bool allConnected = std::all_of(
                cluster.nodes.begin(),
                cluster.nodes.end(),
                std::mem_fn(&Cluster::Node::isConnected)
            );
            _startButton->setEnabled(allConnected);
        }
    }
}


//////////////////////////////////////////////////////////////////////////////////////////


ProgramWidget::ProgramWidget(const Program& program)
    : _program(program)
{
    QBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    QLabel* name = new QLabel(QString::fromStdString(program.name));
    layout->addWidget(name);

    for (const Program::Configuration& c : program.configurations) {
        ConfigurationWidget* w = new ConfigurationWidget(c);
        _widgets.push_back(w);
        connect(
            w, &ConfigurationWidget::startProgram,
            [this, c](const std::string& clusterId) { emit startProgram(c, clusterId); }
        );
        layout->addWidget(w);
    }
}

void ProgramWidget::updateStatus(const Cluster& cluster) {
    for (ConfigurationWidget* w : _widgets) {
        w->updateStatus(cluster);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////


ProgramsWidget::ProgramsWidget(const std::vector<Program>& programs)
    : _programs(programs)
{
    QBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    for (const Program& p : programs) {
        ProgramWidget* w = new ProgramWidget(p);
        _widgets.push_back(w);

        connect(
            w, &ProgramWidget::startProgram,
            [this, p](const Program::Configuration& configuration, const std::string& clusterId) {
            emit startProgram(p, configuration, clusterId);
        }
        );

        layout->addWidget(w);
    }
}

void ProgramsWidget::connectedStatusChanged(const Cluster& cluster) {
    for (ProgramWidget* w : _widgets) {
        w->updateStatus(cluster);
    }
}
