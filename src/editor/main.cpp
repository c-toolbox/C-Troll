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

#include "baseconfiguration.h"
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

    std::vector<std::string> arg = { argv, argv + argc };
    const bool logDebug = common::parseDebugCommandlineArgument(arg);
    std::optional<std::pair<int, int>> pos = common::parseLocationArgument(arg);

    qInstallMessageHandler(
        // Now that the log is enabled and available, we can pipe all Qt messages to that
        [](QtMsgType, const QMessageLogContext&, const QString& msg) {
            Log("Qt", msg.toLocal8Bit().constData());
        }
    );

    common::Log::initialize("editor", false, logDebug, [](const std::string&) {});

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/C_transparent.png"));

    {
        QFile file(":/qss/editor.qss");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
    }

    BaseConfiguration config;
    if (std::filesystem::exists(BaseConfiguration::ConfigurationFile)) {
        config = common::loadFromJson<BaseConfiguration>(
            BaseConfiguration::ConfigurationFile
        );
    }
    else {
        QMessageBox box;
        box.setText("Configuration File");
        box.setInformativeText(
            "Could not detect 'config.json' file. Either select the location of an "
            "existing configuration file, or create a new one"
        );
        QPushButton* create = box.addButton("Create", QMessageBox::ButtonRole::YesRole);
        QPushButton* search = box.addButton("Search", QMessageBox::ButtonRole::NoRole);
        QPushButton* cancel = box.addButton("Cancel", QMessageBox::ButtonRole::ResetRole);
        box.setDefaultButton(create);
        box.exec();
        QAbstractButton* clicked = box.clickedButton();

        if (clicked == create) {
            common::saveToJson(BaseConfiguration::ConfigurationFile, config);
            std::filesystem::create_directory(config.applicationPath);
            std::filesystem::create_directory(config.clusterPath);
            std::filesystem::create_directory(config.nodePath);
        }
        else if (clicked == search) {
            try {
                QString s = QFileDialog::getOpenFileName(nullptr, "Select config file");
                if (s.isEmpty()) {
                    Q_CLEANUP_RESOURCE(resources);
                    return EXIT_SUCCESS;
                }
                QDir::setCurrent(QFileInfo(s).absoluteDir().path());
                config = common::loadFromJson<BaseConfiguration>(s.toStdString());
            }
            catch (const std::exception& e) {
                QMessageBox::critical(nullptr, "Exception", e.what());
                Q_CLEANUP_RESOURCE(resources);
                return EXIT_FAILURE;
            }
            catch (...) {
                QMessageBox::critical(nullptr, "Exception", "Unknown error");
                Q_CLEANUP_RESOURCE(resources);
                return EXIT_FAILURE;
            }
        }
        else if (clicked == cancel) {
            Q_CLEANUP_RESOURCE(resources);
            return EXIT_SUCCESS;
        }
        else {
            throw std::logic_error("Unknown if/else statement case");
        }
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