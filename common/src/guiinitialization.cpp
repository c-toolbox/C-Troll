#include "guiinitialization.h"

#include "jsonsupport.h"

#include <QJsonArray>
#include <QJsonObject>

namespace {
    const QString KeyApplications = "applications";
    const QString KeyClusters = "clusters";
    
    const QString KeyApplicationName = "name";
    const QString KeyApplicationIdentifier = "identifier";
    const QString KeyApplicationTags = "tags";
    const QString KeyApplicationClusters = "clusters";
    const QString KeyApplicationConfigurations = "configurations";
    const QString KeyApplicationConfigurationName = "name";
    const QString KeyApplicationConfigurationIdentifier = "identifier";
    
    const QString KeyClusterName = "name";
    const QString KeyClusterIdentifier = "id";
    const QString KeyClusterEnabled = "enabled";
}

namespace common {

const QString GuiInitialization::Type = "GuiInit";
    
GuiInitialization::Application::Application(QJsonObject obj) {
    name = common::testAndReturnString(obj, KeyApplicationName);
    identifier = common::testAndReturnString(obj, KeyApplicationIdentifier);

    tags = common::testAndReturnStringList(obj, KeyApplicationTags);
    clusters = common::testAndReturnStringList(obj, KeyApplicationClusters);
    
    QJsonArray conf = common::testAndReturnArray(obj, KeyApplicationConfigurations);
    for (const QJsonValue& val : conf) {
        if (!val.isObject()) {
            throw std::runtime_error(
                "Application configuration is not an object"
            );
        }
        QJsonObject valObj = val.toObject();
        
        Configuration configuration;
        configuration.name = common::testAndReturnString(
            valObj, KeyApplicationConfigurationName
        );
        configuration.identifier = common::testAndReturnString(
            valObj, KeyApplicationConfigurationIdentifier
        );
        
        configurations.push_back(configuration);
    }
}
    
QJsonObject GuiInitialization::Application::toJson() const {
    QJsonObject res;
    
    res[KeyApplicationName] = name;
    res[KeyApplicationIdentifier] = identifier;
    res[KeyApplicationTags] = QJsonArray::fromStringList(tags);
    res[KeyApplicationClusters] = QJsonArray::fromStringList(clusters);
    
    QJsonArray confs;
    for (const Configuration& conf : configurations) {
        QJsonObject obj;
        obj[KeyApplicationConfigurationName] = conf.name;
        obj[KeyApplicationConfigurationIdentifier] = conf.identifier;
        confs.append(obj);
    }
    res[KeyApplicationConfigurations] = confs;
    
    return res;
}
    
GuiInitialization::Cluster::Cluster(QJsonObject cluster) {
    name = common::testAndReturnString(cluster, KeyClusterName);
    identifier = common::testAndReturnString(cluster, KeyClusterIdentifier);
    enabled = common::testAndReturnBool(cluster, KeyClusterEnabled);
}
    
QJsonObject GuiInitialization::Cluster::toJson() const {
    QJsonObject res;
    res[KeyClusterName] = name;
    res[KeyClusterIdentifier] = identifier;
    res[KeyClusterEnabled] = enabled;
    return res;
}

GuiInitialization::GuiInitialization(const QJsonDocument& document) {
    QJsonObject obj = document.object();
    
    QJsonArray applicationsJson = common::testAndReturnArray(obj, KeyApplications);
    for (const QJsonValue& val : applicationsJson) {
        if (!val.isObject()) {
            throw std::runtime_error("Value inside an application was not an object");
        }
        applications.push_back(Application(val.toObject()));
    }
    
    QJsonArray clustersJson = common::testAndReturnArray(obj, KeyClusters);
    for (const QJsonValue& val : clustersJson) {
        if (!val.isObject()) {
            throw std::runtime_error("Value inside a cluster was not an object");
        }
        clusters.push_back(Cluster(val.toObject()));
    }
}

QJsonDocument GuiInitialization::toJson() const {
    QJsonObject obj;

    QJsonArray apps;
    for (const Application& app : applications) {
        apps.append(app.toJson());
    }
    obj[KeyApplications] = apps;
    
    QJsonArray cls;
    for (const Cluster& c : clusters) {
        cls.append(c.toJson());
    }
    obj[KeyClusters] = cls;
    
    return QJsonDocument(obj);
}

} // namespace common
