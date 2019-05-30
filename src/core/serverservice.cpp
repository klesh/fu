#include "serverservice.h"
#include "../protocols/localstorageprotocol.h"

ServerService::ServerService(SqlStore &store)
    : _store(store)
{
    _protocols.append(new LocalStorageProtocol());
}

QList<Protocol *> ServerService::getAllProtocols()
{
    return _protocols;
}
