#include "tagservice.h"

#include <QSqlQuery>
#include <QSqlRecord>

inline void query2tag(QSqlQuery &query, Tag &tag) {
    auto rec = query.record();
    tag.id = query.value(rec.indexOf("id")).toUInt();
    tag.name = query.value(rec.indexOf("name")).toString();
    tag.createAt = query.value(rec.indexOf("createdAt")).toDateTime();
    tag.lastUsedTimestamp = query.value(rec.indexOf("lastUsedTimestamp")).toUInt();
}

QList<Tag> TagService::getAll()
{
    QSqlQuery query("SELECT * FROM tags ORDER BY lastUsedTimestamp DESC");
    assert(query.exec());
    QList<Tag> tags;
    while (query.next()) {
        Tag tag;
        query2tag(query, tag);
        tags.append(tag);
    }
    return tags;
}

uint TagService::append(const QString &name)
{
    QDateTime now = QDateTime::currentDateTime();

    QSqlQuery query;
    query.prepare("INSERT INTO tags (name, createdAt, lastUsedTimestamp) "
                  "VALUES (:name, :createdAt, :lastUsedTimestamp)");
    query.bindValue(":name", name);
    query.bindValue(":createdAt", now.toString(Qt::ISODate));
    query.bindValue(":lastUsedTimestamp", now.toTime_t());

    if (query.exec())
        return query.lastInsertId().toUInt();

    return find(name);
}

void TagService::update(uint id, const QString &name)
{
    QSqlQuery query;
    query.prepare("UPDATE tags SET name=:name WHERE id=:id");
    query.bindValue(":name", name);
    query.bindValue(":id", id);
    assert(query.exec());
}

void TagService::remove(uint id)
{
    QSqlQuery query(QString("DELETE FROM tags WHERE id=%1").arg(id));
    assert(query.exec());
}

uint TagService::find(const QString &name)
{
    QSqlQuery query;
    query.prepare("SELECT id FROM tags WHERE name=:name");
    query.bindValue(":name", name);
    assert(query.exec());
    if (query.next()) {
        return query.value(0).toUInt();
    }
    return 0;
}

uint TagService::findOrAppend(const QString &name)
{
    auto tagId = find(name);
    if (tagId)
        return tagId;
    return append(name);
}

QList<uint> TagService::mapToIds(const QStringList &tags, bool autoInsert)
{
    QList<uint> tagIds;
    for (auto &tag : tags) {
        auto tagId = autoInsert ? this->findOrAppend(tag) : this->find(tag);
        if (tagId)
            tagIds.append(tagId);
    }
    return tagIds;
}
