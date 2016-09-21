#include "corecommand.h"

#include "jsonsupport.h"

#include <QJsonObject>


namespace {
    const QString KeyCommand = "command";
    const QString KeyApplicationId = "application_identifier";
    const QString KeyConfigurationId = "configuration_identifier";
    const QString KeyClusterId = "cluster_identifier";
}

namespace common {

const QString CoreCommand::Type = "Command";
    
CoreCommand::CoreCommand(const QJsonDocument& document) {
    QJsonObject obj = document.object();

    command = common::testAndReturnString(obj, KeyCommand);
    applicationId = common::testAndReturnString(obj, KeyApplicationId);
    configurationId = common::testAndReturnString(obj, KeyConfigurationId);
    clusterId = common::testAndReturnString(obj, KeyClusterId);
}

QJsonDocument CoreCommand::toJson() const {
    QJsonObject obj;
    obj[KeyCommand] = command;
    obj[KeyApplicationId] = applicationId;
    obj[KeyConfigurationId] = configurationId;
    obj[KeyClusterId] = clusterId;

    return QJsonDocument(obj);
}

} // namespace common
