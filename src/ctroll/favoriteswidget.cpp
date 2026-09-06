/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2026                                                               *
 * Alexander Bock                                                                        *
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

#include "favoriteswidget.h"

#include "cluster.h"
#include "database.h"
#include "programwidget.h"
#include <QGridLayout>
#include <QScrollArea>
#include <QVBoxLayout>
#include <algorithm>
#include <cassert>
#include <format>

namespace {
    // The number of favorite buttons that are placed next to each other
    constexpr int Columns = 4;
    constexpr int ButtonWidth = 200;
    constexpr int ButtonHeight = 100;
} // namespace

namespace programs {

bool hasFavorites() {
    const std::vector<const Program*> programs = data::programs();
    return std::any_of(
        programs.begin(), programs.end(),
        [](const Program* p) { return !p->favorites.empty(); }
    );
}

FavoritesWidget::FavoritesWidget() {
    QBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);

    QScrollArea* area = new QScrollArea;
    area->setWidgetResizable(true);
    area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    QWidget* content = new QWidget;
    area->setWidget(content);
    layout->addWidget(area);

    QGridLayout* grid = new QGridLayout(content);
    grid->setContentsMargins(5, 5, 5, 5);
    grid->setSpacing(10);

    int idx = 0;
    for (const Program* program : data::programs()) {
        assert(program);

        for (const Program::Favorite& favorite : program->favorites) {
            // Both references have already been verified while loading the data
            const Cluster* cluster = data::findCluster(favorite.cluster);
            assert(cluster);
            const Program::Configuration* configuration =
                data::findConfigurationForProgram(*program, favorite.configuration);
            assert(configuration);

            ProgramButton* button = new ProgramButton(
                cluster,
                configuration,
                favorite.name.empty() ? program->name : favorite.name
            );
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
            button->setMinimumSize(ButtonWidth, ButtonHeight);
            button->setToolTip(QString::fromStdString(std::format(
                "{} ({} | {})", program->name, cluster->name, configuration->name
            )));

            connect(
                button, &ProgramButton::startProgram,
                [this, programId = program->id, clusterId = cluster->id]
                (Program::Configuration::ID configurationId)
                {
                    emit startProgram(clusterId, programId, configurationId);
                }
            );
            connect(
                button, &ProgramButton::stopProgram,
                [this, programId = program->id, clusterId = cluster->id]
                (Program::Configuration::ID configurationId)
                {
                    emit stopProgram(clusterId, programId, configurationId);
                }
            );
            connect(
                button, &ProgramButton::restartProcess,
                this, &FavoritesWidget::restartProcess
            );
            connect(
                button, &ProgramButton::stopProcess,
                this, &FavoritesWidget::stopProcess
            );

            grid->addWidget(button, idx / Columns, idx % Columns);
            _buttons.push_back({
                .programId = program->id,
                .clusterId = cluster->id,
                .configurationId = configuration->id,
                .button = button
            });
            idx++;
        }
    }

    for (int i = 0; i < Columns; i++) {
        grid->setColumnStretch(i, 1);
    }
    // Add an empty stretching row at the bottom so that the buttons stay at the top
    grid->setRowStretch((idx + Columns - 1) / Columns, 1);
}

void FavoritesWidget::processUpdated(Process::ID processId) {
    const Process* process = data::findProcess(processId);
    assert(process);

    for (const ButtonInfo& info : _buttons) {
        const bool isMatch =
            info.programId == process->programId &&
            info.clusterId == process->clusterId &&
            info.configurationId == process->configurationId;
        if (isMatch) {
            info.button->processUpdated(processId);
        }
    }
}

void FavoritesWidget::connectedStatusChanged(Cluster::ID cluster, Node::ID) {
    for (const ButtonInfo& info : _buttons) {
        if (info.clusterId == cluster) {
            info.button->updateStatus();
        }
    }
}

} // namespace programs
