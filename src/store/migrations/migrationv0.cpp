#include "migrationv0.h"

#include <QThread>

MigrationV0::MigrationV0()
{

}

int MigrationV0::getVersion()
{
    return 0;
}

void MigrationV0::run(SqlStore &store)
{
    store.exec("CREATE TABLE tags ( "
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL UNIQUE, "
               "createdAt TEXT NOT NULL,"
               "lastUsedTimestamp INTEGER"
               ")");
    store.exec("CREATE INDEX idx_tags_lastUsedTimestamp ON tags (lastUsedTimestamp DESC);");
    emit progressChanged(0.3);

    store.exec("CREATE TABLE outputFormats ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL UNIQUE, "
               "template TEXT NOT NULL "
               ")");

    store.exec("CREATE TABLE servers ( "
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL UNIQUE, "
               "protocol TEXT NOT NULL, "
               "settingsJSON TEXT NOT NULL DEFAULT '{}', "
               "uploadEnabled INTEGER NOT NULL DEFAULT 1, "
               "outputFormatId INTEGER REFERENCES outputFormats (id) ON DELETE SET NULL, "
               "createdAt TEXT NOT NULL"
               ")");

    store.exec("CREATE TABLE settings ( "
               "settingKey TEXT NOT NULL PRIMARY KEY, "
               "settingValue TEXT DEFAULT ''"
               ")");

    store.exec("INSERT INTO settings (settingKey, settingValue) VALUES ('imageCompressionEnabled', 'true')");
    store.exec("INSERT INTO settings (settingKey, settingValue) VALUES ('imageWatermarkEnabled', 'false')");
    store.exec("INSERT INTO settings (settingKey, settingValue) VALUES ('imageWatermarkPath', '')");
    store.exec("INSERT INTO settings (settingKey, settingValue) VALUES ('imageWatermarkPosition', 'MiddleCenter')");

    emit progressChanged(0.6);

    QSqlQuery query = store.prepare("INSERT INTO outputFormats (name, template) VALUES (:name, :template)");

    query.bindValue(":name", tr("Raw"));
    query.bindValue(":template", "%1");
    store.exec();

    query.bindValue(":name", tr("Markdown Link"));
    query.bindValue(":template", "[%2](%1)");
    store.exec();

    query.bindValue(":name", tr("Markdown Image"));
    query.bindValue(":template", "![%2](%1)");
    store.exec();

    query.bindValue(":name", tr("HTML Link"));
    query.bindValue(":template", "<a href=\"%1\">%2</a>");
    store.exec();

    query.bindValue(":name", tr("HTML Image"));
    query.bindValue(":template", "<img src=\"%1\" alt=\"%2\" />");
    store.exec();

    store.exec("CREATE TABLE clips ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "isImage INTEGER NOT NULL,"
               "isFile INTEGER NOT NULL,"
               "thumbnail BLOB,"
               "description TEXT,"
               "phash SQLITE3_UINT64,"
               "createdAt TEXT NOT NULL"
               ")");

    store.exec("CREATE TABLE clips_tags ("
               "clipId INTEGER NOT NULL REFERENCES clips (id) ON DELETE CASCADE,"
               "tagId INTEGER NOT NULL REFERENCES tags (id) ON DELETE CASCADE,"
               "PRIMARY KEY (clipId, tagId)"
               ")");

    store.exec("CREATE TABLE uploads ("
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "clipId INTEGER NOT NULL REFERENCES clips (id) ON DELETE CASCADE,"
               "serverId INTEGER NOT NULL REFERENCES servers (id) ON DELETE CASCADE,"
               "output TEXT, "
               "createdAt TEXT NOT NULL"
               ")");
}
