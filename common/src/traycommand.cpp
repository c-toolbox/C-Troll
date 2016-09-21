#include "traycommand.h"

#include "jsonsupport.h"

#include <QJsonObject>


namespace {
    const QString KeyIdentifier = "identifier";
    const QString KeyCommand = "command";
    const QString KeyExecutable = "executable";
    const QString KeyBaseDirectory = "baseDirectory";
    const QString KeyCurrentWorkingDirectory = "currentWorkingDirectory";
    const QString KeyCommandlineArguments = "commandlineArguments";
}

namespace common {

const QString TrayCommand::Type = "TrayCommand";
    
TrayCommand::TrayCommand(const QJsonDocument& document) {
    QJsonObject obj = document.object();

    command = common::testAndReturnString(obj, KeyCommand);
    executable = common::testAndReturnString(obj, KeyExecutable);
    baseDirectory = common::testAndReturnString(obj, KeyBaseDirectory);
    currentWorkingDirectory = common::testAndReturnString(obj, KeyCurrentWorkingDirectory);
    commandlineParameters = common::testAndReturnString(obj, KeyCommandlineArguments);
    identifier = common::testAndReturnString(obj, KeyIdentifier);
}

QJsonDocument TrayCommand::toJson() const {
    QJsonObject obj;
    obj[KeyIdentifier] = identifier;
    obj[KeyCommand] = command;
    obj[KeyExecutable] = executable;
    obj[KeyBaseDirectory] = baseDirectory;
    obj[KeyCurrentWorkingDirectory] = currentWorkingDirectory;
    obj[KeyCommandlineArguments] = commandlineParameters;

    return QJsonDocument(obj);
}

} // namespace common
