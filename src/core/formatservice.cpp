#include "formatservice.h"

#include <QSqlQuery>
#include <QSqlRecord>

inline void query2format(QSqlQuery &query, Format &format) {
    auto rec = query.record();
    format.id = query.value(rec.indexOf("id")).toUInt();
    format.name = query.value(rec.indexOf("name")).toString();
    format.format = query.value(rec.indexOf("format")).toString();
}

QList<Format> FormatService::getAll()
{
    QList<Format> formats;
    QSqlQuery query("SELECT * FROM formats ORDER BY id DESC");
    assert(query.exec());
    while (query.next()) {
        Format format;
        query2format(query, format);
        formats.append(format);
    }
    return formats;
}

Format FormatService::findById(uint id)
{
    QSqlQuery query;
    query.prepare("SELECT * FROM formats WHERE id=:id");
    query.bindValue(":id", id);
    assert(query.exec());
    assert(query.next());
    Format format;
    query2format(query, format);
    return format;
}

bool FormatService::append(Format &format)
{
    QSqlQuery query;
    query.prepare("INSERT INTO formats (name, format, enabled) VALUES (:name, :format, :enabled)");
    query.bindValue(":name", format.name);
    query.bindValue(":format", format.format);
    if (query.exec()) {
        format.id = query.lastInsertId().toUInt();
        return true;
    } else {
        return false;
    }
}

bool FormatService::update(Format &format)
{
    QSqlQuery query;
    query.prepare("UPDATE formats SET name=:name, format=:format, enabled=:enabled WHERE id=:id");
    query.bindValue(":name", format.name);
    query.bindValue(":format", format.format);
    query.bindValue(":id", format.id);
    return query.exec();
}

bool FormatService::save(Format &format)
{
    return format.id ? update(format) : append(format);
}

void FormatService::remove(uint id)
{
    QSqlQuery query(QString("DELETE FROM formats where id=%1").arg(id));
    assert(query.exec());
}
