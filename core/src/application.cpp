#include "application.h"

#include "command.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>

Application::Application(QString configurationFile) {
    QFile f(configurationFile);
    f.open(QFile::ReadOnly);
    QJsonDocument d = QJsonDocument::fromJson(f.readAll());
    QJsonObject jsonObject = d.object();

    QString programPath = jsonObject.value("applicationPath").toString();
    QString clusterPath = jsonObject.value("clusterPath").toString();
    int listeningPort = jsonObject.value("listeningPort").toInt();

    _programHandler.loadFromDirectory(programPath);
    _clusterHandler.loadFromDirectory(clusterPath);
    _socketHandler.initialize(listeningPort);

    connect(
        &_socketHandler, &SocketHandler::messageReceived,
        this, &Application::incomingMessage
    );

}

void Application::incomingMessage(QString message) {
    Command cmd(message);


}
