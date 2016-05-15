#include "traycommand.h"

#include "jsonsupport.h"

#include <QJsonObject>


namespace {
    const QString KeyExecutable = "executable";
    const QString KeyBaseDirectory = "baseDirectory";
    const QString KeyCurrentWorkingDirectory = "currentWorkingDirectory";
    const QString KeyCommandlineArguments = "commandlineArguments";
}

TrayCommand::TrayCommand(const QJsonDocument& document) {
    QJsonObject obj = document.object();

    executable = json::testAndReturnString(obj, KeyExecutable);
    baseDirectory = json::testAndReturnString(obj, KeyBaseDirectory);
    currentWorkingDirectory = json::testAndReturnString(obj, KeyCurrentWorkingDirectory);
    commandlineParameters = json::testAndReturnString(obj, KeyCommandlineArguments);
}

QJsonDocument TrayCommand::toJson() const {
    QJsonObject obj;
    obj[KeyExecutable] = executable;
    obj[KeyBaseDirectory] = baseDirectory;
    obj[KeyCurrentWorkingDirectory] = currentWorkingDirectory;
    obj[KeyCommandlineArguments] = commandlineParameters;

    return QJsonDocument(obj);
}
