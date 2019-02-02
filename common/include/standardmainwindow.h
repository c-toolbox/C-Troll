#ifndef __STANDARDMAINWINDOW_H__
#define __STANDARDMAINWINDOW_H__
 
#include <QMainWindow>
#include <QCloseEvent>
#include <QSystemTrayIcon>
#include <QAction>
#include <QTextEdit>
 
namespace Ui { class StandardMainWindow; }
 
class StandardMainWindow : public QMainWindow {
    Q_OBJECT
 
public:
    explicit StandardMainWindow(const QString& title, QWidget* parent = nullptr);
    ~StandardMainWindow();

    static QTextEdit* _staticTextEdit;
    static void myMessageOutput(QtMsgType type, const QMessageLogContext& context,
        const QString& msg);
 
protected:
    /* Virtual function of the parent class in our class
     * Overridden to change the behavior of the application,
     * That it is minimized to tray when we want
     */
    void closeEvent(QCloseEvent * event);
    void changeEvent(QEvent * event);
 
private slots:
    /* The slot that will accept the signal from the event
     * Click on the application icon in the system tray
     */
    void iconActivated(QSystemTrayIcon::ActivationReason reason);
 
private:
    /* Declare the object of future applications for the tray icon */
    QSystemTrayIcon* _trayIcon;
};
 
#endif // __STANDARDMAINWINDOW_H__
