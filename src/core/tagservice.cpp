#include "tagservice.h"

#include <QSqlRecord>

Tag convertResultToTag(QSqlQuery result) {
    Tag tag;
    auto rec = result.record();
    tag.setId(result.value(rec.indexOf("id")).toUInt());
    tag.setName(result.value(rec.indexOf("name")).toString());
    tag.setCreatedAt(result.value(rec.indexOf("createdAt")).toDateTime());
    tag.setLastUsedTimestamp(result.value(rec.indexOf("lastUsedTimestamp")).toUInt());
    return tag;
}

TagService::TagService(SqlStore &store)
    : _store(store)
{

}

TagService::~TagService()
{

}

QList<Tag> TagService::getAll()
{
    QList<Tag> tags;
    auto result = _store.exec("SELECT * FROM tags ORDER BY lastUsedTimestamp DESC");
    while (result.next()) {
        tags.append(convertResultToTag(result));
    }
    return tags;
}

void TagService::append(Tag &tag)
{
    QDateTime now = QDateTime::currentDateTime();
    tag.setCreatedAt(now);
    tag.setLastUsedTimestamp(now.toTime_t());

    auto query = _store.prepare("INSERT INTO tags (name, createdAt, lastUsedTimestamp) VALUES (:name, :createdAt, :lastUsedTimestamp)");
    query.bindValue(":name", tag.getName());
    query.bindValue(":createdAt", tag.getCreatedAt().toString(Qt::ISODate));
    query.bindValue(":lastUsedTimestamp", tag.getLastUsedTimestamp());

    query.exec();
    tag.setId(query.lastInsertId().toUInt());
}

void TagService::update(Tag &tag)
{

}

void TagService::remove(Tag &tag)
{

}
