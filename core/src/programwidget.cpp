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

ProgramButton::ProgramButton(Cluster* cluster,
                             const Program::Configuration* configuration)
    : QPushButton(QString::fromStdString(cluster->name))
    , _cluster(cluster)
    , _configuration(configuration)
{
    //QMenu* menu = new QMenu(this);
    //menu->addAction("Act");
    //setMenu(menu);

    setEnabled(false);
}

void ProgramButton::updateStatus() {
    const bool allConnected = std::all_of(
        _cluster->nodes.begin(),
        _cluster->nodes.end(),
        std::mem_fn(&Cluster::Node::isConnected)
    );
    setEnabled(allConnected);
}

void ProgramButton::addMenu() {

}

void ProgramButton::removeMenu() {

}

    
//////////////////////////////////////////////////////////////////////////////////////////


ClusterWidget::ClusterWidget(Cluster* cluster,
                             const std::vector<Program::Configuration>& configurations)
{
    assert(cluster);

    QBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    QLabel* name = new QLabel(QString::fromStdString(cluster->name));
    layout->addWidget(name);

    for (const Program::Configuration& configuration : configurations) {
        ProgramButton* button = new ProgramButton(cluster, &configuration);

        connect(
            button, &QPushButton::clicked,
            [this, configuration]() { emit startProgram(&configuration); }
        );

        _startButtons.push_back(button);
        layout->addWidget(button);
    }
}

void ClusterWidget::updateStatus() {
    std::for_each(
        _startButtons.begin(), _startButtons.end(),
        std::mem_fn(&ProgramButton::updateStatus)
    );
}

//////////////////////////////////////////////////////////////////////////////////////////


ProgramWidget::ProgramWidget(Program* program, std::vector<Cluster*> clusters) {
    assert(program);
    assert(std::all_of(clusters.begin(), clusters.end(), [](Cluster* c) { return c; }));

    QBoxLayout* layout = new QHBoxLayout;
    setLayout(layout);

    QLabel* name = new QLabel(QString::fromStdString(program->name));
    layout->addWidget(name);

    for (Cluster* cluster : clusters) {
        ClusterWidget* w = new ClusterWidget(cluster, program->configurations);

        connect(
            w, &ClusterWidget::startProgram,
            [this, cluster](const Program::Configuration* conf) {
                emit startProgram(cluster, conf);
            }
        );

        _widgets[cluster] = w;
        layout->addWidget(w);
    }
}

void ProgramWidget::updateStatus(Cluster* cluster) {
    assert(cluster);

    const auto it = _widgets.find(cluster);
    // We have to check as a cluster that is active might not have any associated programs
    if (it != _widgets.end()) {
        it->second->updateStatus();
    }
}

void ProgramWidget::processUpdated(Process* process) {
    assert(process);

    //const auto it = _widgets.find(process.cluster);
}


//////////////////////////////////////////////////////////////////////////////////////////


ProgramsWidget::ProgramsWidget(const std::vector<Program*>& programs,
                               const std::vector<Cluster*>& clusters)
{
    assert(std::all_of(programs.begin(), programs.end(), [](Program* p) { return p; }));
    assert(std::all_of(clusters.begin(), clusters.end(), [](Cluster* c) { return c; }));


    QBoxLayout* layout = new QVBoxLayout;
    setLayout(layout);

    for (Program* p : programs) {
        // Filter the full cluster list to only contain clusters that the program is
        // interested in

        std::vector<Cluster*> cs;
        for (const std::string& cluster : p->clusters) {
            const auto it = std::find_if(
                clusters.begin(), clusters.end(),
                [cluster](Cluster* c) { return c->id == cluster; }
            );
            assert(it != clusters.end());
            cs.push_back(*it);
        }

        ProgramWidget* w = new ProgramWidget(p, cs);

        connect(
            w, &ProgramWidget::startProgram,
            [this, p](Cluster* cluster, const Program::Configuration* conf) {
                emit startProgram(cluster, p, conf);
            }
        );

        _widgets.push_back(w);
        layout->addWidget(w);
    }
}

void ProgramsWidget::processUpdated(Process* process) {
    assert(process);

    for (ProgramWidget* w : _widgets) {
        w->processUpdated(process);
    }
}


void ProgramsWidget::connectedStatusChanged(Cluster* cluster, Cluster::Node*) {
    assert(cluster);
    for (ProgramWidget* w : _widgets) {
        w->updateStatus(cluster);
    }
}

} // namespace programs
