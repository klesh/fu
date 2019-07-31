#ifndef SERVERSERVICE_H
#define SERVERSERVICE_H

#include "../models/server.h"
#include "../protocols/base.h"

class ServerService
{
    QList<Protocol *> _protocols;

public:
    ServerService();
    QList<Server> getAll();
    QList<Server> getAllUploadEnabled();
    Server findById(uint id);
    bool append(Server &server);
    bool update(Server &server);
    bool save(Server &server);
    void remove(uint id);
    void setUploadEnabled(uint id, bool enabled);
    void setOutputFormatId(uint id, uint outputFormatId);
    const QList<Protocol*> &getProtocols();
    Protocol* findProtocol(const QString &name);
    Uploader *createUploader(const Server &server);
};

#endif // SERVERSERVICE_H
