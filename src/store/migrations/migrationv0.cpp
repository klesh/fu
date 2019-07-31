#include "migrationv0.h"

#include <QtCore>
#include <QSqlQuery>

MigrationV0::MigrationV0()
{

}

int MigrationV0::getVersion()
{
    return 0;
}

void MigrationV0::run()
{
    QSqlQuery query;
    query.exec("CREATE TABLE tags ( "
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL UNIQUE, "
               "createdAt TEXT NOT NULL,"
               "lastUsedTimestamp INTEGER"
               ")");
    query.exec("CREATE INDEX idx_tags_lastUsedTimestamp ON tags (lastUsedTimestamp DESC);");
    emit progressChanged(0.3);

    query.exec("CREATE TABLE formats ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL UNIQUE, "
               "format TEXT NOT NULL "
               ")");

    query.exec("CREATE TABLE servers ( "
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL UNIQUE, "
               "protocol TEXT NOT NULL, "
               "settings TEXT NOT NULL DEFAULT '{}', "
               "uploadEnabled INTEGER NOT NULL DEFAULT 1, "
               "formatId INTEGER REFERENCES formats (id) ON DELETE SET NULL, "
               "createdAt TEXT NOT NULL"
               ")");

    query.exec("CREATE TABLE settings ( "
               "settingKey TEXT NOT NULL PRIMARY KEY, "
               "settingValue TEXT DEFAULT ''"
               ")");

    query.exec("INSERT INTO settings (settingKey, settingValue) VALUES ('imageCompressionEnabled', 'true')");
    query.exec("INSERT INTO settings (settingKey, settingValue) VALUES ('imageWatermarkEnabled', 'false')");
    query.exec("INSERT INTO settings (settingKey, settingValue) VALUES ('imageWatermarkPath', '')");
    query.exec("INSERT INTO settings (settingKey, settingValue) VALUES ('imageWatermarkPosition', 'MiddleCenter')");

    emit progressChanged(0.6);

    query.prepare("INSERT INTO formats (name, format) VALUES (:name, :format)");

    query.bindValue(":name", tr("Raw"));
    query.bindValue(":format", "%1");
    query.exec();

    query.bindValue(":name", tr("Markdown Link"));
    query.bindValue(":format", "[%2](%1)");
    query.exec();

    query.bindValue(":name", tr("Markdown Image"));
    query.bindValue(":format", "![%2](%1)");
    query.exec();

    query.bindValue(":name", tr("HTML Link"));
    query.bindValue(":format", "<a href=\"%1\">%2</a>");
    query.exec();

    query.bindValue(":name", tr("HTML Image"));
    query.bindValue(":format", "<img src=\"%1\" alt=\"%2\" />");
    query.exec();

    query.exec("CREATE TABLE clips ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "isImage INTEGER NOT NULL,"
               "isFile INTEGER NOT NULL,"
               "thumbnail BLOB,"
               "description TEXT,"
               "phash SQLITE3_UINT64,"
               "createdAt TEXT NOT NULL"
               ")");

    query.exec("CREATE TABLE clips_tags ("
               "clipId INTEGER NOT NULL REFERENCES clips (id) ON DELETE CASCADE,"
               "tagId INTEGER NOT NULL REFERENCES tags (id) ON DELETE CASCADE,"
               "PRIMARY KEY (clipId, tagId)"
               ")");

    query.exec("CREATE TABLE uploads ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "clipId INTEGER NOT NULL REFERENCES clips (id) ON DELETE CASCADE,"
               "serverId INTEGER NOT NULL REFERENCES servers (id) ON DELETE CASCADE,"
               "output TEXT, "
               "createdAt TEXT NOT NULL"
               ")");
}
