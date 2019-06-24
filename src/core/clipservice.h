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
    void setClipboard(const QString &text);
    static QPixmap thumbnailize(const QPixmap &origin);
    const static QPixmap &unkownFileIcon();
};

#endif // CLIPSERVICE_H
