#include "migrationv0.h"

#include <QThread>

MigrationV0::MigrationV0()
{

}

int MigrationV0::getVersion()
{
    return 0;
}

void MigrationV0::run()
{
    execute("CREATE TABLE tags ( "
               "id INTEGER PRIMARY KEY AUTOINCREMENT, "
               "name TEXT NOT NULL, "
               "lastUsedTimestamp INTEGER"
               ")");
    QThread::sleep(1);
    emit progressChanged(0.5);
    execute("CREATE INDEX idx_tags_lastUsedTimestamp ON tags (lastUsedTimestamp DESC);");
    emit progressChanged(1);
}
