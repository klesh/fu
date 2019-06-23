#ifndef UPLOADTHREAD_H
#define UPLOADTHREAD_H

#include "../models/clip.h"
#include "../models/server.h"

#include <QtCore>

class UploadThread : public QThread
{
    Q_OBJECT

    const Server &_server;
    const Clip &_clip;
    const QByteArray &_imageBytes;

    QString _errMessage;
    bool _isSuccess;

    void run() override;

public:
    UploadThread(const Server &server, const Clip &clip, const QByteArray &imageBytes);

    const Server &server() { return _server; }
    const Clip &clip() { return _clip; }
    const QByteArray &imageBytes() { return _imageBytes; }

    bool isSuccess () { return _isSuccess; }
    const QString errMessage() { return _errMessage; }
};

#endif // UPLOADTHREAD_H
