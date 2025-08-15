/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2025                                                               *
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
#include <QProcessEnvironment>
#include <QPushButton>
#include <filesystem>

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(resources);

    std::vector<std::string> args = { argv, argv + argc };
    const bool logDebug = common::parseDebugCommandlineArgument(args);
    std::optional<std::pair<int, int>> pos = common::parseLocationArgument(args);
    std::string configLoc = common::parseConfigLocationArgument(args);

    qInstallMessageHandler(QtLogFunction);

    common::Log::initialize("editor", false, logDebug, [](const std::string&) {});

    QApplication app = QApplication(argc, argv);
    app.setWindowIcon(QIcon(":/images/editor.png"));

    {
        QFile file = QFile(":/qss/editor.qss");
        file.open(QFile::ReadOnly | QFile::Text);
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
    }

    if (!configLoc.empty()) {
        std::filesystem::current_path(configLoc);
    }

    std::string cfg = "config.json";
    if (QProcessEnvironment::systemEnvironment().contains("CTROLL_CONFIG")) {
        QString c = QProcessEnvironment::systemEnvironment().value("CTROLL_CONFIG");
        cfg = c.toStdString();

        // We assume the paths in the configuration file to be relative to the file,
        // so we need to change the current working directory to the folder where the
        // configuration file exists
        std::filesystem::current_path(std::filesystem::path(cfg).parent_path());
    }
    
    BaseConfiguration config;
    try {
        config = common::loadConfiguration<BaseConfiguration>(
            cfg,
            ":/schema/application/editor.schema.json"
        );
        Log("Config", std::format("Finished loading configuration file '{}'", cfg));
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
        std::filesystem::create_directory(config.applicationPath);
    }
    if (!std::filesystem::exists(config.clusterPath)) {
        std::filesystem::create_directory(config.clusterPath);
    }
    if (!std::filesystem::exists(config.nodePath)) {
        std::filesystem::create_directory(config.nodePath);
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
