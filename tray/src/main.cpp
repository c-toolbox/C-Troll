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

#include "configuration.h"
#include "jsonload.h"
#include "logging.h"
#include "mainwindow.h"
#include "processhandler.h"
#include "sockethandler.h"
#include <QApplication>
#include <fmt/format.h>
#include <json/json.hpp>
#include <filesystem>
#include <iostream>
#include <string_view>

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(resources);

    qInstallMessageHandler(
        // The first message handler is used for Qt error messages that show up before
        // the main window is initialized
        [](QtMsgType type, const QMessageLogContext& context, const QString& msg) {
            QByteArray localMsg = msg.toLocal8Bit();
            switch (type) {
            case QtDebugMsg:
                std::cerr << "Debug: ";
                break;
            case QtWarningMsg:
                std::cerr << "Warning: ";
                break;
            case QtCriticalMsg:
                std::cerr << "Critical: ";
                break;
            case QtFatalMsg:
                std::cerr << "Fatal: ";
                break;
            }

            std::cerr << fmt::format(
                "{} ({}: {}, {})\n",
                localMsg.constData(), context.file, context.line, context.function
            );
        }
    );


    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/C_transparent.png"));

    {
        QFile file(":/qss/tray.qss");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
    }


    MainWindow mw;
    common::Log::initialize("tray", [&mw](std::string msg) { mw.log(std::move(msg)); });

    qInstallMessageHandler(
        // Now that the log is enabled and available, we can pipe all Qt messages to that
        [](QtMsgType, const QMessageLogContext&, const QString& msg) {
            Log("Qt", msg.toLocal8Bit().constData());
        }
    );

    std::string_view configurationFile = "config-tray.json";
    std::string absPath = std::filesystem::absolute(configurationFile).string();
    if (!std::filesystem::exists(configurationFile)) {
        Log("Status", fmt::format("Creating new configuration at '{}'", absPath));

        nlohmann::json obj = Configuration();
        std::string content = obj.dump(2);
        std::ofstream file(absPath);
        file.write(content.data(), content.size());
    }

    Log("Status", fmt::format("Loading configuration file from '{}'", absPath));
    Configuration config = common::loadFromJson<Configuration>(absPath);

#ifdef QT_DEBUG
    config.showWindow = true;
#endif // QT_DEBUG

    if (config.showWindow) {
        mw.show();
    }
    else {
        mw.hide();
    }

    mw.setPort(config.port);

    SocketHandler socketHandler(config.port, config.secret);
    ProcessHandler processHandler;
    
    QObject::connect(
        &socketHandler, &SocketHandler::messageReceived,
        &processHandler, &ProcessHandler::handleSocketMessage
    );
    QObject::connect(
        &socketHandler, &SocketHandler::newConnection,
        &processHandler, &ProcessHandler::newConnection
    );
    QObject::connect(
        &processHandler, &ProcessHandler::sendSocketMessage,
        &socketHandler, &SocketHandler::sendMessage
    );
    QObject::connect(
        &socketHandler, &SocketHandler::newConnection,
        &mw, &MainWindow::newConnection
    );
    QObject::connect(
        &socketHandler, &SocketHandler::closedConnection,
        &mw, &MainWindow::closedConnection
    );
    QObject::connect(
        &processHandler, &ProcessHandler::startedProcess,
        &mw, &MainWindow::newProcess
    );
    QObject::connect(
        &processHandler, &ProcessHandler::closedProcess,
        &mw, &MainWindow::endedProcess
    );

    app.exec();
    Log("Status", "Application finished");
}
