#include "traycommand.h"

#include <QJsonObject>

namespace {
    const QString KeyExecutable = "executable";
    const QString KeyBaseDirectory = "baseDirectory";
    const QString KeyCurrentWorkingDirectory = "currentWorkingDirectory";
    const QString KeyCommandlineArguments = "commandlineArguments";
}

TrayCommand::TrayCommand(const QJsonDocument& document) {
    QJsonObject obj = document.object();

    auto testAndSet = [&obj](const QString& key) -> QString {
        if (!obj.contains(key)) {
            throw std::runtime_error(
                "Missing key '" + key.toStdString() + "' in TrayCommand"
            );
        }
        if (!obj[key].isString()) {
            throw std::runtime_error(
                "Key '" + key.toStdString() + "' in TrayCommand is not a string"
            );
        }

        return obj[key].toString();
    };

    executable = testAndSet(KeyExecutable);
    baseDirectory = testAndSet(KeyBaseDirectory);
    currentWorkingDirectory = testAndSet(KeyCurrentWorkingDirectory);
    commandlineParameters = testAndSet(KeyCommandlineArguments);
}

QJsonDocument TrayCommand::toJson() const {
    QJsonObject obj;
    obj[KeyExecutable] = executable;
    obj[KeyBaseDirectory] = baseDirectory;
    obj[KeyCurrentWorkingDirectory] = currentWorkingDirectory;
    obj[KeyCommandlineArguments] = commandlineParameters;

    return QJsonDocument(obj);
}
