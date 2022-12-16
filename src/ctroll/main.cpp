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

#include "logging.h"
#include "mainwindow.h"
#include "version.h"
#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QMessageBox>
#include <QSharedMemory>
#include <QTimer>
#include <fmt/format.h>
#include <chrono>

struct SharedMemoryMarker {
    short majorVersion = -1;
    short minorVersion = -1;
    short patchVersion = -1;
    std::byte showWindowMarker = std::byte(0);
    std::byte unused[25] = {};
};

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(resources);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/C_transparent.png"));

    QSharedMemory mem("/C-Troll/Single-Instance-Marker");
    bool ret = mem.create(sizeof(SharedMemoryMarker));
    if (!ret) {
        // Something went wrong with creating the memory
        QSharedMemory::SharedMemoryError err = mem.error();
        if (err == QSharedMemory::AlreadyExists) {
            // Another instance of C-Troll already run on the computer and we need to
            // signal to it to come to the front instead
            mem.attach();
            mem.lock();
            SharedMemoryMarker* data = reinterpret_cast<SharedMemoryMarker*>(mem.data());
            if (data->majorVersion != application::MajorVersion) {
                QMessageBox::critical(
                    nullptr,
                    "Version mismatch",
                    QString::fromStdString(fmt::format(
                        "Starting to launch C-Troll of version {}.{}.{} while "
                        "incompatible version {}.{}.{} is still running",
                        data->majorVersion,
                        data->minorVersion,
                        data->patchVersion,
                        application::MajorVersion,
                        application::MinorVersion,
                        application::PatchVersion
                    ))
                );
                mem.unlock();
                exit(EXIT_FAILURE);
            }

            data->showWindowMarker = std::byte(1);
            mem.unlock();
            exit(EXIT_SUCCESS);
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
    else {
        // Creation of the SharedMemory block succeeded, so we are the first to start
        mem.attach();
        mem.lock();
        SharedMemoryMarker* data = new (mem.data()) SharedMemoryMarker;
        data->majorVersion = application::MajorVersion;
        data->minorVersion = application::MinorVersion;
        data->patchVersion = application::PatchVersion;
        mem.unlock();
    }


    std::vector<std::string> arg = { argv, argv + argc };
    const bool logDebug = common::parseDebugCommandlineArgument(arg);
    std::optional<std::pair<int, int>> pos = common::parseLocationArgument(arg);

    qInstallMessageHandler(
        // Now that the log is enabled and available, we can pipe all Qt messages to that
        [](QtMsgType, const QMessageLogContext&, const QString& msg) {
            Log("Qt", msg.toLocal8Bit().constData());
        }
    );


    {
        QFile file(":/qss/core.qss");
        file.open(QFile::ReadOnly);
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
    }

    try {
        MainWindow mw = MainWindow(logDebug);
        if (pos.has_value()) {
            mw.move(pos->first, pos->second);
        }
        mw.show();

        QTimer* timer = new QTimer(&mw);
        timer->setTimerType(Qt::VeryCoarseTimer);
        QObject::connect(
            timer, &QTimer::timeout,
            [&]() {
                mem.attach();
                SharedMemoryMarker* m = reinterpret_cast<SharedMemoryMarker*>(mem.data());
                if (m->showWindowMarker == std::byte(1)) {
                    mw.show();
                    mw.setWindowState(Qt::WindowState::WindowActive);
                    m->showWindowMarker = std::byte(0);
                }
            }
        );
        timer->start(std::chrono::milliseconds(1000));

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
