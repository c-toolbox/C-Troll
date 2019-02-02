#include <standardmainwindow.h>

#include <QMenu>
#include <QHBoxLayout>
#include <iostream>

QTextEdit* StandardMainWindow::_staticTextEdit = 0;

void StandardMainWindow::myMessageOutput(QtMsgType type,
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

        return;
    }

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
 
StandardMainWindow::StandardMainWindow(const QString& title, QWidget* parent)
    : QMainWindow(parent)
{
    setWindowTitle(title);
    setMinimumSize(512, 256);

    _staticTextEdit = new QTextEdit();
    layout()->addWidget(_staticTextEdit);
 
    // Initialize the tray icon, set the icon of a set of system icons,
    // as well as set a tooltip
    _trayIcon = new QSystemTrayIcon(QIcon(":/images/C_transparent.png"), this);
    _trayIcon->setToolTip(title + QString("\nCluster Launcher Application"));

    // After that create a context menu of two items
    QMenu* menu = new QMenu(this);
    QAction* viewWindow = new QAction(trUtf8("Show"), this);
    QAction* quitAction = new QAction(trUtf8("Quit"), this);
 
    // connect the signals clicks on menu items to by appropriate slots.
    // The first menu item expands the application from the tray,
    // And the second menu item terminates the application
    connect(viewWindow, SIGNAL(triggered()), this, SLOT(show()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
 
    menu->addAction(viewWindow);
    menu->addAction(quitAction);
 
    // Set the context menu on the icon and show the application icon in the system tray
    _trayIcon->setContextMenu(menu);
    _trayIcon->show();
 
    // Also connect clicking on the icon to the signal processor of this press 
    connect(
        _trayIcon,
        SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
        this,
        SLOT(iconActivated(QSystemTrayIcon::ActivationReason))
    );
}
 
StandardMainWindow::~StandardMainWindow() {}
 
// The method that handles the closing event of the application window
void StandardMainWindow::closeEvent(QCloseEvent* event) {
    // If the window is visible, and the checkbox is checked, then the completion of the
    // application. Ignored, and the window simply hides that accompanied the
    // corresponding pop-up message
    if (isVisible()) {
        event->ignore();
        hide();
        QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(
            QSystemTrayIcon::Information
        );
 
        _trayIcon->showMessage(
            windowTitle(),
            trUtf8("The application is still running in the background"),
            icon,
            2000
        );
    }
}

void StandardMainWindow::changeEvent(QEvent* event) {
    if (event->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            hide();
        }
        event->ignore();
    }
}
 
// The method that handles click on the application icon in the system tray
void StandardMainWindow::iconActivated(QSystemTrayIcon::ActivationReason reason) {
    if (reason == QSystemTrayIcon::Trigger) {
        // If the window is visible, it is hidden
        // Conversely, if hidden, it unfolds on the screen
        isVisible() ? hide() : show();
    }
}
