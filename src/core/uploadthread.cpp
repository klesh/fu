#include "uploadthread.h"
#include "../application.h"

UploadThread::UploadThread(UploadJob &job)
    : _job(job)
{
}

void UploadThread::run()
{
    QDataStream *stream;

    if (!_job.data.isEmpty()) { // share same processed image
        stream = new QDataStream(_job.data);
    } else { // create independent file stream
        QString filePath = _job.path;
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            _job.status = Error;
            _job.msg = tr("Failed to open file %1").arg(filePath);
            APP->sendNotification(_job.msg, tr("Error"), QSystemTrayIcon::MessageIcon::Warning);
        }
        stream = new QDataStream(&file);
    }

    Uploader *uploader = APP->serverService()->createUploader(_job.server);
    uploader->upload(stream, _job);

    delete uploader;
    delete stream;
}
