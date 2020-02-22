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
#include <QMenu>
#include <QPushButton>
#include <QVBoxLayout>

namespace programs {

ProgramButton::ProgramButton(std::string name) 
    : QPushButton(QString::fromStdString(name))
{
    //QMenu* menu = new QMenu(this);
    //menu->addAction("Act");
    //setMenu(menu);
}

    
//////////////////////////////////////////////////////////////////////////////////////////


ClusterWidget::ClusterWidget(const std::string& cluster,
                             const std::vector<Program::Configuration>& configurations)
{
    QBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    QLabel* name = new QLabel(QString::fromStdString(cluster));
    layout->addWidget(name);

    for (const Program::Configuration& configuration : configurations) {
        ProgramButton* button = new ProgramButton(configuration.name);
        button->setEnabled(false);

        connect(
            button, &QPushButton::clicked,
            [this, configuration]() { emit startProgram(configuration); }
        );

        _startButtons[configuration.name] = button;
        layout->addWidget(button);
    }
}

void ClusterWidget::updateStatus(const Cluster& cluster) {
    const bool allConnected = std::all_of(
        cluster.nodes.begin(),
        cluster.nodes.end(),
        std::mem_fn(&Cluster::Node::isConnected)
    );

    for (auto& [key, value] : _startButtons) {
        value->setEnabled(allConnected);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////


ProgramWidget::ProgramWidget(Program* program) {
    QBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    QLabel* name = new QLabel(QString::fromStdString(program->name));
    layout->addWidget(name);

    for (const std::string& cluster : program->clusters) {
        ClusterWidget* w = new ClusterWidget(cluster, program->configurations);

        connect(
            w, &ClusterWidget::startProgram,
            [this, cluster](const Program::Configuration conf) {
                emit startProgram(cluster, conf);
            }
        );

        _widgets[cluster] = w;
        layout->addWidget(w);
    }
}

void ProgramWidget::updateStatus(const Cluster& cluster) {
    const auto it = _widgets.find(cluster.id);
    // We have to check as a cluster that is active might not have any associated programs
    if (it != _widgets.end()) {
        it->second->updateStatus(cluster);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////


ProgramsWidget::ProgramsWidget(const std::vector<Program*>& programs,
                               const std::vector<Cluster*>& clusters)
{
    QBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    for (Program* p : programs) {
        ProgramWidget* w = new ProgramWidget(p);

        connect(
            w, &ProgramWidget::startProgram,
            [this, p](const std::string& cluster, const Program::Configuration& conf) {
                emit startProgram(cluster, *p, conf);
            }
        );

        _widgets.push_back(w);
        layout->addWidget(w);
    }
}

void ProgramsWidget::connectedStatusChanged(const Cluster& cluster, const Cluster::Node&)
{
    for (ProgramWidget* w : _widgets) {
        w->updateStatus(cluster);
    }
}

} // namespace programs
