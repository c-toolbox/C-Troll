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
    void createProcessFromTrayCommand(common::TrayCommand command);
 private:
    // The maps string is a unique id (recieved from core)
    std::map<QString, QProcess*> _processes;
    std::map<QString, common::TrayCommand> _commands;
};

#endif
