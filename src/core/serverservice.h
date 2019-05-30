#ifndef SERVERSERVICE_H
#define SERVERSERVICE_H

#include "../store/sqlstore.h"
#include "../models/server.h"
#include "../models/protocol.h"

class ServerService
{
    SqlStore &_store;

public:
    ServerService(SqlStore &store);

    QList<Server> getAll();
    void append(const QString &name);
    void update(uint id, const QString &name);
    void remove(uint id);
    QList<Protocol*> getAllProtocols();
};

#endif // SERVERSERVICE_H
