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

#include "mainwindow.h"

#include <QMenu>
#include <QHBoxLayout>
#include <iostream>

QTextEdit* MainWindow::_staticTextEdit = 0;

void MainWindow::myMessageOutput(QtMsgType type,
                                         const QMessageLogContext& context,
                                         const QString& msg)
{
    if (!_staticTextEdit) {
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

        if (type == QtFatalMsg) {
            abort();
        }
    }
    else {
        switch (type) {
            case QtDebugMsg:
            case QtWarningMsg:
            case QtCriticalMsg:
                _staticTextEdit->append(msg);
                break;
            case QtFatalMsg:
                abort();
        }
    }
}
 
MainWindow::MainWindow(const QString& title, QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(title);
    setMinimumSize(512, 256);

    _staticTextEdit = new QTextEdit();
    setCentralWidget(_staticTextEdit);
 
    // Initialize the tray icon, set the icon of a set of system icons,
    // as well as set a tooltip
    _trayIcon = new QSystemTrayIcon(QIcon(":/images/C_transparent.png"), this);
    _trayIcon->setToolTip(title + QString("\nCluster Launcher Application"));

    // After that create a context menu of two items
    QMenu* menu = new QMenu(this);
    // The first menu item expands the application from the tray,
    QAction* viewWindow = new QAction(trUtf8("Show"), this);
    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    menu->addAction(viewWindow);

    // The second menu item terminates the application
    QAction* quitAction = new QAction(trUtf8("Quit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
    menu->addAction(quitAction);
 
    // Set the context menu on the icon and show the application icon in the system tray
    _trayIcon->setContextMenu(menu);
    _trayIcon->show();
 
    // Also connect clicking on the icon to the signal processor of this press 
    connect(
        _trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason))
    );
}
 
// The method that handles the closing event of the application window
void MainWindow::closeEvent(QCloseEvent* event) {
    // If the window is visible, and the checkbox is checked, then the completion of the
    // application. Ignored, and the window simply hides that accompanied the
    // corresponding pop-up message
    if (isVisible()) {
        event->ignore();
        hide();
 
        _trayIcon->showMessage(
            windowTitle(),
            trUtf8("The application is still running in the background"),
            QSystemTrayIcon::Information,
            2000
        );
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
