#ifndef UPLOAD_H
#define UPLOAD_H

#include <QtCore>

struct Upload {
    uint id;
    uint clipId;
    uint serverId;
    QString url;
    QDateTime createdAt;
    QString serverName;
};

#endif // UPLOAD_H
