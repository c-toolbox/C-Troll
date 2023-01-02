/*****************************************************************************************
 *                                                                                       *
 * Copyright (c) 2016-2023                                                             *
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

#include "commandlineparsing.h"
#include "configuration.h"
#include "jsonload.h"
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

namespace {
    struct SharedMemoryMarker {
        short majorVersion = -1;
        short minorVersion = -1;
        short patchVersion = -1;
        std::byte showWindowMarker = std::byte(0);
        std::byte unused[25] = {};
    };

    std::vector<std::string> tokenizeString(const std::string& input, char separator) {
        size_t separatorPos = input.find(separator);
        if (separatorPos == std::string::npos) {
            return { input };
        }
        else {
            std::vector<std::string> res;
            size_t prevSeparator = 0;
            while (separatorPos != std::string::npos) {
                res.push_back(input.substr(prevSeparator, separatorPos - prevSeparator));
                prevSeparator = separatorPos + 1;
                separatorPos = input.find(separator, separatorPos + 1);
            }
            res.push_back(input.substr(prevSeparator));
            return res;
        }
    }

    std::vector<std::string> parseTagsArgument(const std::vector<std::string>& args) {
        auto it = std::find(args.begin(), args.end(), "--tags");
        if (it != args.end()) {
            // The rest of the commandline argument is a comma-separated list of tags
            std::string allTags = std::accumulate(
                it + 1,
                args.end(),
                std::string(),
                [](std::string lhs, std::string rhs) {
                    return std::move(lhs) + " " + std::move(rhs);
                }
            );
            // We have an empty character at the beginning we need to remove
            allTags = allTags.substr(1);

            return tokenizeString(allTags, ',');
        }
        else {
            return std::vector<std::string>();
        }
    }
} // namespace

int main(int argc, char** argv) {
    Q_INIT_RESOURCE(resources);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/C_transparent.png"));

    //
    // Handle shared memory
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
            auto data = reinterpret_cast<SharedMemoryMarker*>(mem.data());
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
                    "Error creating shared memory: {}",
                    mem.errorString().toStdString()
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

    std::vector<std::string> args = { argv, argv + argc };
    const bool logDebug = common::parseDebugCommandlineArgument(args);
    std::optional<std::pair<int, int>> pos = common::parseLocationArgument(args);
    std::vector<std::string> defaultTags = parseTagsArgument(args);

    qInstallMessageHandler(QtLogFunction);

    {
        QFile file(":/qss/c-troll.qss");
        file.open(QFile::ReadOnly | QFile::Text);
        QString styleSheet = QLatin1String(file.readAll());
        app.setStyleSheet(styleSheet);
    }

    Configuration config;
    try {
        config = common::loadConfiguration<Configuration>(
            "config.json",
            ":/schema/application/ctroll.schema.json"
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

    common::Log::initialize("ctroll", config.logFile, logDebug);


    try {
        MainWindow mw = MainWindow(defaultTags, config);
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
