#ifndef UPLOADSERVICE_H
#define UPLOADSERVICE_H

#include "../store/sqlstore.h"
#include "../models/clip.h"
#include "../models/upload.h"


class UploadService : public QObject
{
    Q_OBJECT
    SqlStore &_store;
    bool _isUploading = false;
    QList<QThread> _uploadingThreads;
    QList<Upload> _uploads;
    void _upload(const QList<Clip> &clips, const QStringList &tags, const QString &desc);

public:
    UploadService(SqlStore &_store);

    void upload(const QList<Clip> &clips, const QStringList &tags, const QString &desc);
    bool isUploading() { return _isUploading; }
};

#endif // UPLOADSERVICE_H
