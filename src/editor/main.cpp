/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2023                                                               *
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

#include "baseconfiguration.h"
#include "commandlineparsing.h"
#include "jsonload.h"
#include "logging.h"
#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QIcon>
#include <QMessageBox>
#include <QPushButton>
#include <filesystem>

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(resources);

    std::vector<std::string> args = { argv, argv + argc };
    const bool logDebug = common::parseDebugCommandlineArgument(args);
    std::optional<std::pair<int, int>> pos = common::parseLocationArgument(args);

    qInstallMessageHandler(QtLogFunction);

    common::Log::initialize("editor", false, logDebug, [](const std::string&) {});

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/editor.png"));

    {
        QFile file(":/qss/editor.qss");
        file.open(QFile::ReadOnly | QFile::Text);
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
    }

    BaseConfiguration config;
    try {
        config = common::loadConfiguration<BaseConfiguration>(
            "config.json",
            ":/schema/application/editor.schema.json"
        );
    }
    catch (const std::runtime_error& err) {
        QMessageBox::critical(
            nullptr,
            "Configuration error",
            QString::fromStdString(err.what())
        );
        exit(EXIT_FAILURE);
    }

    if (!std::filesystem::exists(config.applicationPath)) {
        QMessageBox::critical(
            nullptr,
            "Directory missing",
            QString::fromStdString(fmt::format(
                "Could not find application path directory '{}'", config.applicationPath
            ))
        );
        Q_CLEANUP_RESOURCE(resources);
        return EXIT_SUCCESS;
    }
    if (!std::filesystem::exists(config.clusterPath)) {
        QMessageBox::critical(
            nullptr,
            "Directory missing",
            QString::fromStdString(fmt::format(
                "Could not find cluster path directory '{}'", config.clusterPath
            ))
        );
        Q_CLEANUP_RESOURCE(resources);
        return EXIT_SUCCESS;
    }
    if (!std::filesystem::exists(config.nodePath)) {
        QMessageBox::critical(
            nullptr,
            "Directory missing",
            QString::fromStdString(fmt::format(
                "Could not find node path directory '{}'", config.nodePath
            ))
        );
        Q_CLEANUP_RESOURCE(resources);
        return EXIT_SUCCESS;
    }

    MainWindow mw(config.applicationPath, config.clusterPath, config.nodePath);
    if (pos.has_value()) {
        mw.move(pos->first, pos->second);
    }
    mw.show();

    try {
        app.exec();
    }
    catch (const std::exception& e) {
        QMessageBox::critical(nullptr, "Exception", e.what());
    }
    catch (...) {
        QMessageBox::critical(nullptr, "Exception", "Unknown error");
    }

    Q_CLEANUP_RESOURCE(resources);
    return EXIT_SUCCESS;
}
