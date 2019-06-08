#ifndef TAGSERVICE_H
#define TAGSERVICE_H

#include "../store/sqlstore.h"
#include "../models/tag.h"

class TagService
{
    SqlStore &_store;

public:
    TagService(SqlStore &store);
    ~TagService();

    QList<Tag> getAll();
    void append(const QString &name);
    void update(uint id, const QString &name);
    void remove(uint id);
};

#endif // TAGSERVICE_H
