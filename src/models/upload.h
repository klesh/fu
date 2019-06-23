#ifndef UPLOAD_H
#define UPLOAD_H

#include <QtCore>

struct Upload {
    uint id;
    uint clipId;
    uint serverId;
    QString rawOutput;
    QDateTime createdAt;
};

#endif // UPLOAD_H
