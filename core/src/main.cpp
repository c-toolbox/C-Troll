/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016 - 2019                                                             *
 * Alexander Bock, Erik Sundén, Emil Axelsson                                            *
 *                                                                                       *
 * All rights reserved.                                                                  *
 *                                                                                       *
 * Redistribution and use in source and binary forms, with or without modification, are  *
 * permitted provided that the following conditions are met:                             *
 *                                                                                       *
 * 1. Redistributions of source code must retain the above copyright notice, this list   *
 * of conditions and the following disclaimer.                                           *
 *                                                                                       *
 * 2. Redistributions in binary form must reproduce the above copyright notice, this     *
 * list of conditions and the following disclaimer in the documentation and/or other     *
 * materials provided with the distribution.                                             *
 *                                                                                       *
 * 3. Neither the name of the copyright holder nor the names of its contributors may be  *
 * used to endorse or promote products derived from this software without specific prior *
 * written permission.                                                                   *
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

#include "application.h"
#include "logging.h"
#include "mainwindow.h"
#include <QApplication>
#include <QDir>
#include <QFileInfo>
#include <iostream>
#include <filesystem>

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

            std::cerr << localMsg.constData() << " (" << context.file << ":" <<
                context.line << ", " << context.function << ")\n";
        }
    );


    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/C_transparent.png"));

    MainWindow mw("C-Troll-Core");
#ifdef QT_DEBUG
    mw.show();
#else
    mw.hide();
#endif // QT_DEBUG
    
    common::Log::initialize("core", [&mw](std::string msg) { mw.log(msg); });

    qInstallMessageHandler(
        // Now that the log is enabled and available, we can pipe all Qt messages to that
        [](QtMsgType, const QMessageLogContext&, const QString& msg) {
            Log(msg.toStdString());
        }
    );
    
    // Load configuration file;  use the passed argument if it exists
    std::string configurationFile = (argc == 2) ? argv[1] : "config.json";
    if (!std::filesystem::exists(configurationFile)) {
        std::string absPath = std::filesystem::absolute(configurationFile).string();
        Log("Could not find configuration file " + absPath);
        exit(EXIT_FAILURE);
    }

    Application application(configurationFile);
    app.exec();
}
