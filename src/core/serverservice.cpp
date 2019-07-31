#include "serverservice.h"
#include "error.h"
#include "../application.h"
#include "../protocols/localstorageprotocol.h"
#include "../protocols/ftpprotocol.h"

QString toJson(QVariantMap &settings)
{
    return QJsonDocument::fromVariant(settings).toJson(QJsonDocument::Compact);
}

Server convertResultToServer(QSqlQuery &result) {
    Server server;
    auto rec = result.record();
    server.id = result.value(rec.indexOf("id")).toUInt();
    server.name = result.value(rec.indexOf("name")).toString();
    server.protocol = result.value(rec.indexOf("protocol")).toString();
    server.createdAt = result.value(rec.indexOf("createdAt")).toDateTime();
    server.settings = QJsonDocument::fromJson(result.value(rec.indexOf("settingsJSON")).toByteArray()).toVariant().toMap();
    server.uploadEnabled = result.value(rec.indexOf("uploadEnabled")).toBool();
    server.outputFormatId = result.value(rec.indexOf("outputFormatId")).toUInt();
    return server;
}

ServerService::ServerService(SqlStore &store)
    : _store(store)
{
    _protocols.append(new LocalStorageProtocol());
    _protocols.append(new FtpProtocol());
}

QList<Server> ServerService::getAll()
{
    QList<Server> servers;
    auto result = _store.exec("SELECT * FROM servers ORDER BY id DESC");
    while (result.next()) {
        servers.append(convertResultToServer(result));
    }
    return servers;
}

QList<Server> ServerService::getAllUploadEnabled()
{
    QList<Server> servers;
    auto result = _store.exec("SELECT * FROM servers WHERE uploadEnabled=1 ORDER BY id DESC");
    while (result.next()) {
        auto server = convertResultToServer(result);
        servers.append(server);
    }

    QMap<uint, OutputFormat> cache;
    for (auto &server : servers) {
        if (server.outputFormatId) {
            if (!cache.contains(server.outputFormatId))
                cache[server.outputFormatId] = APP->outputFormatService()->findById(server.outputFormatId);
            server.outputFormat = cache[server.outputFormatId];
        }
    }
    return servers;
}


Server ServerService::findById(uint id)
{
    auto query = _store.prepare("SELECT * FROM servers WHERE id=:id");
    query.bindValue(":id", id);
    auto result = _store.exec();
    if (!result.next()) {
        throw_error("Server not found");
    }
    return convertResultToServer(result);
}

void ServerService::append(Server &server)
{
    auto query = _store.prepare("INSERT INTO servers (name, protocol, settingsJSON, createdAt) VALUES (:name, :protocol, :settings, :createdAt)");
    query.bindValue(":name", server.name);
    query.bindValue(":protocol", server.protocol);
    query.bindValue(":settings", toJson(server.settings));
    query.bindValue(":createdAt", datetimeToISO());

    auto result = _store.exec();
    server.id = result.lastInsertId().toUInt();
}

void ServerService::update(Server &server)
{
    auto query = _store.prepare("UPDATE servers SET name=:name, protocol=:protocol, settingsJSON=:settings WHERE id=:id");
    query.bindValue(":name", server.name);
    query.bindValue(":protocol", server.protocol);
    query.bindValue(":settings", toJson(server.settings));
    query.bindValue(":id", server.id);
    _store.exec();
}

void ServerService::save(Server &server)
{
    if (server.id) {
        update(server);
    } else {
        append(server);
    }

}

void ServerService::remove(uint id)
{
    auto query = _store.prepare("DELETE FROM servers where id=:id");
    query.bindValue(":id", id);
    _store.exec();
}

void ServerService::setUploadEnabled(uint id, bool enabled)
{
    auto query = _store.prepare("UPDATE servers SET uploadEnabled=:enabled WHERE id=:id");
    query.bindValue(":enabled", enabled);
    query.bindValue(":id", id);
    _store.exec();
}

void ServerService::setOutputFormatId(uint id, uint outputFormatId)
{
    auto query = _store.prepare("UPDATE servers SET outputFormatId=:outputFormatId WHERE id=:id");
    query.bindValue(":outputFormatId", outputFormatId);
    query.bindValue(":id", id);
    _store.exec();
}

QList<Protocol *> ServerService::getProtocols()
{
    return _protocols;
}

Protocol *ServerService::findProtocol(const QString &name)
{
    for (auto protocol : _protocols) {
        if (protocol->getName() == name)
            return protocol;
    }
    throw_error("Protocol not found");
}

Uploader *ServerService::createUploader(const Server &server)
{
    return findProtocol(server.protocol)->createUploader(server.settings);
}
