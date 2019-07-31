#ifndef TAGSERVICE_H
#define TAGSERVICE_H

#include "../models/tag.h"

class TagService
{
public:
    QList<Tag> getAll();
    uint append(const QString &name);
    void update(uint id, const QString &name);
    void remove(uint id);
    uint find(const QString &name);
    uint findOrAppend(const QString &name);
    QList<uint> mapToIds(const QStringList &tags, bool autoInsert = false);
};

#endif // TAGSERVICE_H
