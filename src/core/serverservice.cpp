#include "error.h"
#include "serverservice.h"
#include "../protocols/localstorageprotocol.h"
#include "../protocols/ftpprotocol.h"

QString toJson(QVariantMap &settings)
{
    return QJsonDocument::fromVariant(settings).toJson(QJsonDocument::Compact);
}

Server convertResultToServer(QSqlQuery result) {
    Server server;
    auto rec = result.record();
    server.id = result.value(rec.indexOf("id")).toUInt();
    server.name = result.value(rec.indexOf("name")).toString();
    server.protocol = result.value(rec.indexOf("protocol")).toString();
    server.createAt = result.value(rec.indexOf("createdAt")).toDateTime();
    server.settings = QJsonDocument::fromJson(result.value(rec.indexOf("settingsJSON")).toByteArray()).toVariant().toMap();
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
    qDebug() << "appending new server" << server.name << server.protocol << toJson(server.settings);
    query.bindValue(":name", server.name);
    query.bindValue(":protocol", server.protocol);
    query.bindValue(":settings", toJson(server.settings));
    query.bindValue(":createdAt",  QDateTime::currentDateTime().toString(Qt::ISODate));

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
