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
               "template TEXT NOT NULL, "
               ")");

    store.exec("CREATE TABLE servers ( "
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL UNIQUE, "
               "protocol TEXT NOT NULL, "
               "settingsJSON TEXT NOT NULL DEFAULT '{}', "
               "uploadEnabled INTEGER NOT NULL DEFAULT 1, "
               "outoutFormatId uint REFERENCE outputFormats (id) ON DELETE SET NULL, "
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

    store.exec("CREATE TABLE outputFormats ( "
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL UNIQUE, "
               "template TEXT NOT NULL, "
               "enabled INTEGER NOT NULL DEFAULT 1 "
               ")");

    QSqlQuery query = store.prepare("INSERT INTO outputFormats (name, template, enabled) VALUES (:name, :template, :enabled)");

    query.bindValue(":name", tr("Raw"));
    query.bindValue(":template", "%1");
    query.bindValue(":enabled", false);
    store.exec();

    query.bindValue(":name", tr("Markdown Link"));
    query.bindValue(":template", "[%2](%1)");
    query.bindValue(":enabled", false);
    store.exec();

    query.bindValue(":name", tr("Markdown Image"));
    query.bindValue(":template", "![%2](%1)");
    query.bindValue(":enabled", true);
    store.exec();

    query.bindValue(":name", tr("HTML Link"));
    query.bindValue(":template", "<a href=\"%1\">%2</a>");
    query.bindValue(":enabled", false);
    store.exec();

    query.bindValue(":name", tr("HTML Image"));
    query.bindValue(":template", "<img src=\"%1\" alt=\"%2\" />");
    query.bindValue(":enabled", false);
    store.exec();
}
