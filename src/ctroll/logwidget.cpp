/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2022                                                             *
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

#include "logwidget.h"

#include "version.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>
#include <fmt/format.h>

LogWidget::LogWidget() {
    QBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(&_message, 1);
    layout->setContentsMargins(0, 0, 0, 0);

    QWidget* container = new QWidget;
    container->setObjectName("info");
    QBoxLayout* versionLayout = new QHBoxLayout(container);
    QMargins m = versionLayout->contentsMargins();
    m.setTop(0);
    m.setBottom(10);
    versionLayout->setContentsMargins(m);

    std::string coreVer = fmt::format("Core Version: {}", application::Version);
    versionLayout->addWidget(new QLabel(QString::fromStdString(coreVer)));

    versionLayout->addStretch();

    std::string apiVer = fmt::format("API Version: {}", api::Version);
    versionLayout->addWidget(new QLabel(QString::fromStdString(apiVer)));

    layout->addWidget(container);
}

void LogWidget::appendMessage(std::string msg) {
    _message.append(QString::fromStdString(msg));
}
