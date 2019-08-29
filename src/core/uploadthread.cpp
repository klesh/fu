#include "uploadthread.h"
#include "../application.h"

UploadThread::UploadThread(UploadJob &job)
    : _job(job)
{
}

void UploadThread::run()
{
    QIODevice *stream;

    if (!_job.data.isEmpty()) { // share same processed image
        stream = new QBuffer(&_job.data);
        stream->setProperty("filename", _job.name);
        stream->open(QIODevice::ReadOnly);
    } else { // create independent file stream
        stream = new QFile(_job.path);
        if (!stream->open(QIODevice::ReadOnly)) {
            _job.status = Error;
            _job.msg = tr("Failed to open file %1").arg(_job.path);
            APP->sendNotification(_job.msg, tr("Error"), QSystemTrayIcon::MessageIcon::Warning);
        }
    }

    Uploader *uploader = APP->serverService()->createUploader(_job.server);
    uploader->upload(stream, _job);

    delete uploader;
    delete stream;
}
