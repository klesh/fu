#ifndef UPLOADSERVICE_H
#define UPLOADSERVICE_H

#include "../store/sqlstore.h"
#include "../models/upload.h"
#include "../protocols/base.h"

enum DuplicationHandlingMethod {
    None,
    Overwrite,
    AutoRename,
    Skip
};

class UploadService : public QObject
{
    Q_OBJECT
    SqlStore &_store;
    bool _isUploading = false;
    QString _formattedOutput;
    DuplicationHandlingMethod _applyToAll;
    QList<UploadJob> _jobs;
    int poolSeats;
    QMutex mutex;
    bool needAnotherPopulation = false;
    bool isPopulating = false;

    void upload(QList<Clip> &clips);
    void handleDuplication(UploadJob &job);
    void populatePool();
    void threadFinished();

public:
    UploadService(SqlStore &_store);

    void upload(QList<Clip> &clips, const QStringList &tags, const QString &desc);
    bool isUploading() { return _isUploading; }
    QList<Upload> getAllByClipId(uint clipId);


public slots:
    void uploadFinished();
};

#endif // UPLOADSERVICE_H
