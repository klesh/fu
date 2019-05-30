#ifndef TAG_H
#define TAG_H

#include <QtCore>

struct Tag
{
    uint id;
    QString name;
    QDateTime createAt;
    uint lastUsedTimestamp;
};

#endif // TAG_H
