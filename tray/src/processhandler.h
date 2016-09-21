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
    void handleSocketMessage(std::string message);
    // Process slots
    void handlerErrorOccurred(QProcess::ProcessError error);
    void handleFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void handleReadyReadStandardError();
    void handleReadyReadStandardOutput();
    void handleStarted();
signals:
    void sendSocketMessage(std::string message);
private:
    void runProcessWithTrayCommand(QProcess* process, const common::TrayCommand& command);
    void createAndRunProcessFromTrayCommand(const common::TrayCommand& command);
    
    // The key of these maps is a unique id (recieved from core)
    std::map<QString, QProcess*> _processes;
    std::map<QString, common::TrayCommand> _commands;
    
    // The key of this map is a process id
    std::map<qint64, QString> _processIdToTrayId;
};

#endif
