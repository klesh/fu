#include "uploadthread.h"
#include "../application.h"

void UploadThread::run()
{
    QDataStream *stream;

    if (!_imageBytes.isEmpty()) { // share same processed image
        stream = new QDataStream(_imageBytes);
    } else { // create independent file stream
        QString filePath = _clip.data.toUrl().toLocalFile();
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly)) {
            _isSuccess = false;
            _errMessage = tr("Failed to open file %1").arg(filePath);
            APP->sendNotification(_errMessage, tr("Error"), QSystemTrayIcon::MessageIcon::Warning);
        }
        stream = new QDataStream(&file);
    }

    try {
        Uploader* uploader = APP->serverService()->createUploader(_server);
        connect(uploader, &Uploader::fileExists, [&]() {
            emit this->fileExists();
        });
        auto name = _clip.name;
        if (_suffix) {
            QFileInfo fi(name);
            name = QString("%1-%2.%3").arg(fi.baseName()).arg(_suffix).arg(fi.completeSuffix());
        }
        _output = uploader->upload(stream, name, _overwrite);
        _isSuccess = true;
    } catch (Error &e) {
        _isSuccess = false;
        _errMessage = e.text();
    }
    delete stream;
}

UploadThread::UploadThread(UploadJob &job)
        : _job(job)
{

}
