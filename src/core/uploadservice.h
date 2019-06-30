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
    QString _formattedOutput;

    void upload(const QList<Clip> &clips);

public:
    UploadService(SqlStore &_store);

    void upload(QList<Clip> &clips, const QStringList &tags, const QString &desc);
    bool isUploading() { return _isUploading; }
    QList<Upload> getAllByClipId(uint clipId);

public slots:
    void uploadFinished();
};

#endif // UPLOADSERVICE_H
