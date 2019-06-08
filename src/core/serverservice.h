#ifndef SERVERSERVICE_H
#define SERVERSERVICE_H

#include "../store/sqlstore.h"
#include "../models/server.h"
#include "../models/protocol.h"

class ServerService
{
    SqlStore &_store;
    QList<Protocol *> _protocols;

public:
    ServerService(SqlStore &store);

    QList<Server> getAll();
    Server findById(uint id);
    void append(Server &server);
    void update(Server &server);
    void save(Server &server);
    void remove(uint id);
    QList<Protocol*> getProtocols();
    Protocol* findProtocol(const QString &name);
};

#endif // SERVERSERVICE_H
