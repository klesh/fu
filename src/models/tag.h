#ifndef TAG_H
#define TAG_H

#include <QtCore>

class Tag
{
private:
    uint id;
    QString name;
    QDateTime createdAt;
    uint lastUsedTimestamp;

public:

    uint getId() { return id; }
    void setId(uint value) { id = value; }

    QString &getName() { return name; }
    void setName(QString value) { name = value; }

    QDateTime getCreatedAt() { return createdAt; }
    void setCreatedAt(QDateTime value) { createdAt = value; }

    uint getLastUsedTimestamp() { return lastUsedTimestamp; }
    void setLastUsedTimestamp(uint value) { lastUsedTimestamp = value; }
};

#endif // TAG_H
