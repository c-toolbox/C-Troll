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

#include "mainwindow.h"

#include "centralwidget.h"
#include "version.h"
#include <fmt/format.h>
#include <QApplication>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QVBoxLayout>
#include <iostream>

MainWindow::MainWindow() {
    setWindowTitle("C-Troll Tray");

    _centralWidget = new CentralWidget;
    setCentralWidget(_centralWidget);

    // Initialize the tray icon, set the icon of a set of system icons,
    // as well as set a tooltip
    _trayIcon = new QSystemTrayIcon(QIcon(":/images/tray_offline_cog.png"), this);
    _trayIcon->setToolTip("C-Troll Tray");

    // After that create a context menu of two items
    QMenu* menu = new QMenu(this);
    // The first menu item expands the application from the tray,
    QAction* viewWindow = new QAction("Show", this);
    connect(viewWindow, &QAction::triggered, this, &MainWindow::show);
    menu->addAction(viewWindow);

    // The second menu item terminates the application
    QAction* quit = new QAction("Quit", this);
    connect(quit, &QAction::triggered, QApplication::instance(), &QApplication::quit);
    menu->addAction(quit);

    // Set the context menu on the icon and show the application icon in the system tray
    _trayIcon->setContextMenu(menu);
    _trayIcon->show();

    // Also connect clicking on the icon to the signal processor of this press
    connect(_trayIcon, &QSystemTrayIcon::activated, this, &MainWindow::iconActivated);
    connect(qApp, &QCoreApplication::aboutToQuit, _trayIcon, &QSystemTrayIcon::hide);
}

void MainWindow::setPort(int port) {
    _centralWidget->setPort(port);
}

void MainWindow::log(std::string msg) {
    _centralWidget->log(std::move(msg));
}

void MainWindow::newConnection(const std::string& peerAddress) {
    _centralWidget->newConnection(peerAddress);

    updateTrayIcon();
}

void MainWindow::closedConnection(const std::string& peerAddress) {
    _centralWidget->closedConnection(peerAddress);

    updateTrayIcon();
}

void MainWindow::newProcess(ProcessHandler::ProcessInfo process) {
    _centralWidget->newProcess(process);
}

void MainWindow::endedProcess(ProcessHandler::ProcessInfo process) {
    _centralWidget->endedProcess(process);
}

// The method that handles the closing event of the application window
void MainWindow::closeEvent(QCloseEvent* event) {
    // If the window is visible, and the checkbox is checked, then the completion of the
    // application. Ignored, and the window simply hides that accompanied the
    // corresponding pop-up message
    if (isVisible()) {
        event->ignore();
        hide();
    }
}

void MainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::WindowStateChange) {
        // Hide the taskbar icon if the window is minimized
        if (isMinimized()) {
            hide();
        }
        event->ignore();
    }
}

// The method that handles click on the application icon in the system tray
void MainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        // If the window is visible, it is hidden
        // Conversely, if hidden, it unfolds on the screen
        if (isVisible()) {
            hide(); // Hide the taskbar icon
        }
        else {
            show(); // Show the taskbar icon
            showNormal(); // Bring the window to the front
        }
    }
}

void MainWindow::updateTrayIcon() {
    constexpr const char Offline[] = ":/images/tray_offline_cog.png";
    constexpr const char Online[] = ":/images/tray_online_cog.png";

    _trayIcon->setIcon(QIcon(_centralWidget->hasConnections() ? Online : Offline));
}
