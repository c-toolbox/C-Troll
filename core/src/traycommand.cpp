#include "traycommand.h"

#include <QJsonDocument>

TrayCommand::TrayCommand(const Program& program, QString configuration) {
    executable = program.executable();
    baseDirectory = program.baseDirectory();
    commandlineParameters = program.commandlineParameters() + " " + configuration;
}

QString TrayCommand::json() const {
    QJsonObject obj;
    obj["executable"] = executable;
    obj["baseDirectory"] = baseDirectory;
    obj["commandlineArguments"] = commandlineParameters;

    QJsonDocument doc(obj);
    QByteArray data = doc.toJson();
    return QString::fromUtf8(data);
}
