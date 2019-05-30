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
    emit progressChanged(0.5);
}
