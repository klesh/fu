#ifndef MIGRATOR_H
#define MIGRATOR_H

#include "migration.h"

class Migrator : public QObject
{
    Q_OBJECT

    int _currentDbVersion = -1;
    QList<Migration*> _migrations;
    QList<Migration*> _pendingMigrations;

public:
    Migrator();
    ~Migrator();

    int totalPendingMigration();
    void run();

signals:
    void progressChanged(int migrationIndex, double pencent);
};

#endif // MIGRATOR_H
