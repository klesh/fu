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
        _server.uploader->upload(stream, _clip.name);
        _isSuccess = true;
    } catch (Error &e) {
        _isSuccess = false;
        _errMessage = e.text();
    }
    delete stream;
}

UploadThread::UploadThread(const Server &server, const Clip &clip, const QByteArray &imageBytes)
        : _server(server), _clip(clip), _imageBytes(imageBytes)
{

}
