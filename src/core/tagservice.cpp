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

void TagService::append(const QString &name)
{
    QDateTime now = QDateTime::currentDateTime();

    auto query = _store.prepare("INSERT INTO tags (name, createdAt, lastUsedTimestamp) VALUES (:name, :createdAt, :lastUsedTimestamp)");
    query.bindValue(":name", name);
    query.bindValue(":createdAt", now.toString(Qt::ISODate));
    query.bindValue(":lastUsedTimestamp", now.toTime_t());

    query.exec();
}

void TagService::update(uint id, const QString &name)
{
    auto query = _store.prepare("UPDATE tags SET name=:name WHERE id=:id");
    query.bindValue(":name", name);
    query.bindValue(":id", id);
    query.exec();
}

void TagService::remove(uint id)
{
    auto query = _store.prepare("DELETE FROM tags WHERE id=:id");
    query.bindValue(":id", id);
    query.exec();
}
