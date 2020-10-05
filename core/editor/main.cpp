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

#include "baseconfiguration.h"
#include "jsonload.h"
#include "logging.h"
#include "mainwindow.h"
#include <QApplication>
#include <QFile>
#include <QFileDialog>
#include <QIcon>
#include <QMessageBox>
#include <filesystem>

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(resources);

    qInstallMessageHandler(
        // Now that the log is enabled and available, we can pipe all Qt messages to that
        [](QtMsgType, const QMessageLogContext&, const QString& msg) {
            Log("Qt", msg.toLocal8Bit().constData());
        }
    );

    common::Log::initialize("editor", false, [](const std::string&) {});

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
        box.setText("Text");
        box.setInformativeText("InfText");
        QPushButton* b = box.addButton("Create", QMessageBox::ButtonRole::YesRole);
        box.addButton("Search", QMessageBox::ButtonRole::NoRole);
        box.setDefaultButton(b);
        const int res = box.exec();

        if (res == QMessageBox::Yes) {
            common::saveToJson(BaseConfiguration::ConfigurationFile, config);
            std::filesystem::create_directory(config.applicationPath);
            std::filesystem::create_directory(config.clusterPath);
            std::filesystem::create_directory(config.applicationPath);
        }
        else {
            QString s = QFileDialog::getOpenFileName(nullptr, "Select config file");
            if (s.isEmpty()) {
                Q_CLEANUP_RESOURCE(resources);
                return 0;
            }
            config = common::loadFromJson<BaseConfiguration>(s.toStdString());
        }
    }

    MainWindow mw(config.applicationPath, config.clusterPath, config.nodePath);
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
    return 0;
}