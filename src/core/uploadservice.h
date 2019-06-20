#ifndef UPLOADSERVICE_H
#define UPLOADSERVICE_H

#include "../store/sqlstore.h"
#include "../models/clip.h"

class UploadService : public QObject
{
    Q_OBJECT
    SqlStore &_store;

public:
    UploadService(SqlStore &_store);

    void upload(QList<Clip> &clips, const QStringList &tags, const QString &desc);
};

#endif // UPLOADSERVICE_H
