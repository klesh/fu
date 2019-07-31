#ifndef UPLOADTHREAD_H
#define UPLOADTHREAD_H

#include "../protocols/base.h"

#include <QtCore>

class UploadThread : public QThread
{
    Q_OBJECT

    UploadJob &_job;
    void run() override;

public:
    UploadThread(UploadJob &job);
};

#endif // UPLOADTHREAD_H
