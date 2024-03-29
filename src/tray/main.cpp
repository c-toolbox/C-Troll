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

#include "commandlineparsing.h"
#include "configuration.h"
#include "jsonload.h"
#include "logging.h"
#include "mainwindow.h"
#include "messages.h"
#include "processhandler.h"
#include "sockethandler.h"
#include <QApplication>
#include <QMessageBox>
#include <QSharedMemory>
#include <QTimer>
#include <fmt/format.h>
#include <nlohmann/json.hpp>
#include <filesystem>
#include <iostream>
#include <string_view>

namespace {
    struct SharedMemoryMarker {
        std::byte unused[32] = {};
    };

    void Debug(std::string msg) {
        ::Debug("Initialization", std::move(msg));
    }
} // namespace

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(resources);
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/tray_cog.png"));

    QSharedMemory mem("/Tray/Single-Instance-Marker");
    bool ret = mem.create(sizeof(SharedMemoryMarker));
    if (!ret) {
        // Something went wrong with creating the memory
        QSharedMemory::SharedMemoryError err = mem.error();
        if (err == QSharedMemory::AlreadyExists) {
            // Another instance of the Tray already runs on the computer and we need to
            // signal that to the user and stop starting

            QMessageBox::critical(
                nullptr,
                "Tray already running",
                "Trying to start Tray application while it is already running"
            );
            exit(EXIT_FAILURE);
        }
        else {
            QMessageBox::critical(
                nullptr,
                "Memory error",
                QString::fromStdString(fmt::format(
                    "Error creating shared memory: {}", mem.errorString().toStdString()
                ))
            );
        }
    }



    std::vector<std::string> args = { argv, argv + argc };
    const bool logDebug = common::parseDebugCommandlineArgument(args);
    std::optional<std::pair<int, int>> pos = common::parseLocationArgument(args);

    qInstallMessageHandler(QtLogFunction);

    {
        QFile file(":/qss/tray.qss");
        file.open(QFile::ReadOnly | QFile::Text);
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
    }


    MainWindow mw;
    if (pos.has_value()) {
        mw.move(pos->first, pos->second);
    }

    
    std::string cfg = "config-tray.json";
    Configuration config;
    try {
        if (QProcessEnvironment::systemEnvironment().contains("CTRAY_CONFIG")) {
            QString c = QProcessEnvironment::systemEnvironment().value("CTRAY_CONFIG");
            cfg = c.toStdString();
        
            // We assume the paths in the configuration file to be relative to the file,
            // so we need to change the current working directory to the folder where the
            // configuration file exists
            std::filesystem::current_path(std::filesystem::path(cfg).parent_path());
        }

        config = common::loadConfiguration<Configuration>(
            cfg,
            ":/schema/application/tray.schema.json"
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

    common::Log::initialize(
        "tray",
        config.logFile,
        logDebug,
        [&mw](std::string msg) { mw.log(std::move(msg)); }
    );
    Log("Config", fmt::format("Finished loading configuration file '{}'", cfg));

#ifdef QT_DEBUG
    config.showWindow = true;
#endif // QT_DEBUG

    if (config.showWindow || logDebug) {
        Debug("Showing main window");
        mw.show();
    }
    else {
        Debug("Hiding main window");
        mw.hide();
    }

    mw.setPort(config.port);

    if (config.logRotation.has_value()) {
        Debug("Enabling log rotation");
        const bool keepLog = config.logRotation->keepPrevious;
        const std::chrono::hours freq = config.logRotation->frequency;

        QTimer* timer = new QTimer(&mw);
        timer->setTimerType(Qt::VeryCoarseTimer);
        QObject::connect(
            timer, &QTimer::timeout,
            [keepLog]() {
                Debug("Logging", "Performing log rotation");
                common::Log::ref()->performLogRotation(keepLog);
            }
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

            socketHandler.sendMessage(message);
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
            socketHandler.sendMessage(message);
        }
    }

    Q_CLEANUP_RESOURCE(resources);
    return 0;
}
