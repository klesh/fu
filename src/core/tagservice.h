#ifndef TAGSERVICE_H
#define TAGSERVICE_H

#include "../models/tag.h"
#include "../store/sqlstore.h"

class TagService
{
    SqlStore &_store;

public:
    TagService(SqlStore &store);
    ~TagService();

    QList<Tag> getAll();
    void append(Tag &tag);
    void update(Tag &tag);
    void remove(Tag &tag);
};

#endif // TAGSERVICE_H
