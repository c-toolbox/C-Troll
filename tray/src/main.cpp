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
#include "erroroccurredmessage.h"
#include "jsonload.h"
#include "logging.h"
#include "mainwindow.h"
#include "processhandler.h"
#include "sockethandler.h"
#include <QApplication>
#include <QMessageBox>
#include <QTimer>
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
                case QtInfoMsg:
                    std::cerr << "Info: ";
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
            std::cerr.flush();
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

    qInstallMessageHandler(
        // Now that the log is enabled and available, we can pipe all Qt messages to that
        [](QtMsgType, const QMessageLogContext&, const QString& msg) {
            Log("Qt", msg.toLocal8Bit().constData());
        }
    );

    std::string_view configurationFile = "config-tray.json";
    std::string absPath = std::filesystem::absolute(configurationFile).string();
    if (!std::filesystem::exists(configurationFile)) {
        std::cout << fmt::format("Creating new configuration at '{}'", absPath) << '\n';

        nlohmann::json obj = Configuration();
        std::ofstream file(absPath);
        file << obj.dump(2);
    }

    std::cout << fmt::format("Loading configuration file from '{}'", absPath) << '\n';
    Configuration config = common::loadFromJson<Configuration>(absPath);
    common::Log::initialize(
        "tray",
        config.logFile,
        [&mw](std::string msg) { mw.log(std::move(msg)); }
    );

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

    if (config.logRotation.has_value()) {
        const bool keepLog = config.logRotation->keepPrevious;
        const std::chrono::hours freq = config.logRotation->frequency;

        QTimer* timer = new QTimer(&mw);
        timer->setTimerType(Qt::VeryCoarseTimer);
        QObject::connect(
            timer, &QTimer::timeout,
            [keepLog]() { common::Log::ref().performLogRotation(keepLog); }
        );
        timer->start(std::chrono::duration_cast<std::chrono::milliseconds>(freq));
    }

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
        &processHandler, &ProcessHandler::closeApplication,
        &app, &QCoreApplication::quit, Qt::QueuedConnection
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

    // This looks a bit weird, but here it goes:  There might be an unexpected exception
    // that would kill the Tray process if it would leak, which would be no bueno.
    //  a. An exception happens -> it gets caught in the catch blocks below, but we still
    //     want to continue running the tray, so we go back into the event loop
    //  b. The user wants to close the Tray application which means that the exec function
    //     actually returns, in which case we immediately break out of the loop to get to
    //     the cleanup functions
    while (true) {
        try {
            app.exec();
            break;
        }
        catch (const std::exception& e) {
            Log("Leaked Exception", e.what());
            Log("Debug", "Last received messages:");

            common::ErrorOccurredMessage message;
            message.error = e.what();
            std::array<SocketHandler::MessageLog, 3> mls = socketHandler.lastMessages();
            for (const SocketHandler::MessageLog& m : mls) {
                std::string msg = fmt::format(
                    "{} ({}): {}", m.time, m.peer, m.message.dump()
                );
                message.lastMessages.push_back(msg);
                Log("Msg", msg);
            }

            nlohmann::json j = message;
            socketHandler.sendMessage(j);
        }
        catch (...) {
            Log("Leaked Exception", "Unknown error");
            Log("Debug", "Last received messages:");
            common::ErrorOccurredMessage message;
            message.error = "Unknown exception";
            std::array<SocketHandler::MessageLog, 3> mls = socketHandler.lastMessages();
            for (const SocketHandler::MessageLog& m : mls) {
                std::string msg = fmt::format(
                    "{} ({}): {}", m.time, m.peer, m.message.dump()
                );
                message.lastMessages.push_back(msg);
                Log("Msg", msg);
            }
            nlohmann::json j = message;
            socketHandler.sendMessage(j);
        }
    }

    Q_CLEANUP_RESOURCE(resources);
    return 0;
}
