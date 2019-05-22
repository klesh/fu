#ifndef MIGRATOR_H
#define MIGRATOR_H

#include "migration.h"
#include "sqlstore.h"

class Migrator : public QObject
{
    Q_OBJECT

    SqlStore &_store;
    int _currentDbVersion = -1;
    QList<Migration*> _migrations;
    QList<Migration*> _pendingMigrations;

public:
    Migrator(SqlStore &store);
    ~Migrator();

    int totalPendingMigration();
    void run();

signals:
    void progressChanged(int migrationIndex, double pencent);
};

#endif // MIGRATOR_H
