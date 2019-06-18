#ifndef CLIPSERVICE_H
#define CLIPSERVICE_H

#include "../store/sqlstore.h"
#include "../models/clip.h"

class ClipService
{
    SqlStore &_store;

public:
    ClipService(SqlStore &store);

    QList<Clip> getAllFromClipboard();
};

#endif // CLIPSERVICE_H
