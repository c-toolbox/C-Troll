#include "traycommand.h"

#include "jsonsupport.h"

#include <QJsonObject>

namespace {
    const QString KeyIdentifier = "identifier";
    const QString KeyCommand = "command";
    const QString KeyExecutable = "executable";
    const QString KeyBaseDirectory = "baseDirectory";
    const QString KeyWorkingDirectory = "currentWorkingDirectory";
    const QString KeyCommandlineArguments = "commandlineArguments";
}

namespace common {

const QString TrayCommand::Type = "TrayCommand";
    
TrayCommand::TrayCommand(const QJsonDocument& document) {
    QJsonObject payload = document.object();
    QJsonObject obj = common::testAndReturnObject(payload, "payload");

    identifier = common::testAndReturnString(obj, KeyIdentifier);
    command = common::testAndReturnString(obj, KeyCommand);
    executable = common::testAndReturnString(obj, KeyExecutable);
    baseDirectory = common::testAndReturnString(obj, KeyBaseDirectory);
    currentWorkingDirectory = common::testAndReturnString(obj, KeyWorkingDirectory);
    commandlineParameters = common::testAndReturnString(obj, KeyCommandlineArguments);
}

QJsonDocument TrayCommand::toJson() const {
    QJsonObject obj;
    obj[KeyIdentifier] = identifier;
    obj[KeyCommand] = command;
    obj[KeyExecutable] = executable;
    obj[KeyBaseDirectory] = baseDirectory;
    obj[KeyWorkingDirectory] = currentWorkingDirectory;
    obj[KeyCommandlineArguments] = commandlineParameters;

    return QJsonDocument(obj);
}

} // namespace common
