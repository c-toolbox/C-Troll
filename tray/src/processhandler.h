#ifndef __PROCESSHANDLER_H__
#define __PROCESSHANDLER_H__

#include <map>
#include <string>
#include <traycommand.h>
#include <QObject>
#include <QProcess>

class ProcessHandler : public QObject {
    Q_OBJECT
public:
    ProcessHandler();
    ~ProcessHandler();
public slots:
    void handleSocketMessage(QString message);
    // Process slots
    void handlerErrorOccurred(QProcess::ProcessError error);
    void handleFinished(int exitCode);
    void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleReadyReadStandardError();
    void handleReadyReadStandardOutput();
    void handleStarted();
signals:
    void sendSocketMessage(QString message);
private:
    void executeProcessWithTrayCommand(QProcess* process, const common::TrayCommand& command);
    void createAndRunProcessFromTrayCommand(const common::TrayCommand& command);
    
    // The key of this map is a unique id (recieved from core)
    // The value is the process which is running
    typedef std::map<QString, QProcess*> ProcessMap;
    ProcessMap _processes;
};

#endif
