#ifndef __PROCESSHANDLER_H__
#define __PROCESSHANDLER_H__

#include <QObject.h>
#include <map>
#include <string>
#include <traycommand.h>

class QProcess;

class ProcessHandler : public QObject {
    Q_OBJECT
public:
    ProcessHandler();
    ~ProcessHandler();
public slots:
    void handleSocketMessage(std::string message);
signals:
    void sendSocketMessage(std::string message);
private:
    void runProcessWithTrayCommand(QProcess* process, const common::TrayCommand& command);
    void createAndRunProcessFromTrayCommand(const common::TrayCommand& command);
    
    // The maps string is a unique id (recieved from core)
    std::map<QString, QProcess*> _processes;
    std::map<QString, common::TrayCommand> _commands;
};

#endif
