#include "traycommand.h"

#include "jsonsupport.h"

#include <QJsonObject>


namespace {
    const QString KeyExecutable = "executable";
    const QString KeyBaseDirectory = "baseDirectory";
    const QString KeyCurrentWorkingDirectory = "currentWorkingDirectory";
    const QString KeyCommandlineArguments = "commandlineArguments";
}

namespace common {

TrayCommand::TrayCommand(const QJsonDocument& document) {
    QJsonObject obj = document.object();

    executable = common::testAndReturnString(obj, KeyExecutable);
    baseDirectory = common::testAndReturnString(obj, KeyBaseDirectory);
    currentWorkingDirectory = common::testAndReturnString(obj, KeyCurrentWorkingDirectory);
    commandlineParameters = common::testAndReturnString(obj, KeyCommandlineArguments);
}

QJsonDocument TrayCommand::toJson() const {
    QJsonObject obj;
    obj[KeyExecutable] = executable;
    obj[KeyBaseDirectory] = baseDirectory;
    obj[KeyCurrentWorkingDirectory] = currentWorkingDirectory;
    obj[KeyCommandlineArguments] = commandlineParameters;

    return QJsonDocument(obj);
}

} // namespace common
