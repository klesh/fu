#ifndef UPLOADSERVICE_H
#define UPLOADSERVICE_H

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

    bool _isUploading = false;
    DuplicationHandlingMethod _applyToAll;
    QList<UploadJob> _jobs;
    int poolSeats;
    QMutex mutex;
    bool needAnotherPopulation = false;
    bool isPopulating = false;

    void upload(QList<Clip> &clips);
    void populatePool();
    void handleDuplication(UploadJob &job);

private slots:
    void threadFinished();
    void uploadFinished();

public:
    void upload(QList<Clip> &clips, const QStringList &tags, const QString &desc);
    bool isUploading() { return _isUploading; }
    QList<Upload> getAllByClipId(uint clipId);
};

#endif // UPLOADSERVICE_H
