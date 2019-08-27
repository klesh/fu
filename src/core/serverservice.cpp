#include "serverservice.h"
#include "./utils.h"
#include "../protocols/localstorageprotocol.h"
#include "../protocols/ftpprotocol.h"
#include "../protocols/sftpprotocol.h"
#include "../protocols/imgurprotocol.h"

#include <QSqlQuery>
#include <QSqlRecord>

QString toJson(QVariantMap &settings)
{
    return QJsonDocument::fromVariant(settings).toJson(QJsonDocument::Compact);
}

inline void query2server(QSqlQuery &query, Server &server) {
    auto rec = query.record();
    server.id = query.value(rec.indexOf("id")).toUInt();
    server.name = query.value(rec.indexOf("name")).toString();
    server.protocol = query.value(rec.indexOf("protocol")).toString();
    server.createdAt = query.value(rec.indexOf("createdAt")).toDateTime();
    server.settings = QJsonDocument::fromJson(query.value(rec.indexOf("settings")).toByteArray()).toVariant().toMap();
    server.uploadEnabled = query.value(rec.indexOf("uploadEnabled")).toBool();
    server.formatId = query.value(rec.indexOf("formatId")).toUInt();
}

ServerService::ServerService()
{
    _protocols.append(new LocalStorageProtocol());
    _protocols.append(new FtpProtocol());
    _protocols.append(new SftpProtocol());
    _protocols.append(new ImgurProtocol());
}

QList<Server> ServerService::getAll()
{
    QList<Server> servers;
    QSqlQuery query("SELECT * FROM servers ORDER BY id DESC");
    assert(query.exec());
    while (query.next()) {
        Server server;
        query2server(query, server);
        servers.append(server);
    }
    return servers;
}

QList<Server> ServerService::getAllUploadEnabled()
{
    QList<Server> servers;
    QSqlQuery query("SELECT * FROM servers WHERE uploadEnabled=1 ORDER BY id DESC");
    assert(query.exec());
    while (query.next()) {
        Server server;
        query2server(query, server);
        servers.append(server);
    }

    return servers;
}

Server ServerService::findById(uint id)
{
    QSqlQuery query(QString("SELECT * FROM servers WHERE id=%1").arg(id));
    assert(query.exec());
    assert(query.next());
    Server server;
    query2server(query, server);
    return server;
}

bool ServerService::append(Server &server)
{
    QSqlQuery query;
    query.prepare("INSERT INTO servers (name, protocol, settings, createdAt) "
                  "VALUES (:name, :protocol, :settings, :createdAt)");
    query.bindValue(":name", server.name);
    query.bindValue(":protocol", server.protocol);
    query.bindValue(":settings", toJson(server.settings));
    query.bindValue(":createdAt", datetimeToISO());

    if (query.exec()) {
        server.id = query.lastInsertId().toUInt();
        return true;
    }
    return false;
}

bool ServerService::update(Server &server)
{
    QSqlQuery query;
    query.prepare("UPDATE servers "
                  "SET name=:name, protocol=:protocol, settings=:settings "
                  "WHERE id=:id");
    query.bindValue(":name", server.name);
    query.bindValue(":protocol", server.protocol);
    query.bindValue(":settings", toJson(server.settings));
    query.bindValue(":id", server.id);
    return query.exec();
}

bool ServerService::save(Server &server)
{
    return server.id ? update(server) : append(server);
}

void ServerService::remove(uint id)
{
    QSqlQuery query(QString("DELETE FROM servers where id=%1").arg(id));
    assert(query.exec());
}

void ServerService::setUploadEnabled(uint id, bool enabled)
{
    QSqlQuery query;
    query.prepare("UPDATE servers SET uploadEnabled=:enabled WHERE id=:id");
    query.bindValue(":enabled", enabled);
    query.bindValue(":id", id);
    assert(query.exec());
}

void ServerService::setOutputFormatId(uint id, uint formatId)
{
    QSqlQuery query;
    query.prepare("UPDATE servers SET formatId=:formatId WHERE id=:id");
    query.bindValue(":formatId", formatId);
    query.bindValue(":id", id);
    assert(query.exec());
}

const QList<Protocol *> &ServerService::getProtocols()
{
    return _protocols;
}

Protocol *ServerService::findProtocol(const QString &name)
{
    for (auto &protocol : _protocols) {
        if (protocol->getName() == name)
            return protocol;
    }
    assert(false);
    return nullptr;
}

Uploader *ServerService::createUploader(const Server &server)
{
    auto protocol = findProtocol(server.protocol);
    return protocol->createUploader(server.settings);
}
