#include "command.h"

#include <QJsonDocument>
#include <QJsonObject>

Command::Command(QString command) {
    QJsonDocument d = QJsonDocument::fromJson(command.toUtf8());
    QJsonObject obj = d.object();

    cluster = obj.value("cluster").toString();
    application = obj.value("application").toString();
}
