#ifndef MIGRATOR_H
#define MIGRATOR_H

#include "migration.h"

class Migrator : public QObject
{
    Q_OBJECT

public:
    Migrator();
    ~Migrator();

    int totalPendingMigration();
    void run();

signals:
    void progressChanged(int migrationIndex, double pencent);

private:
    int currentDbVersion = -1;
    QList<Migration*> migrations;
    QList<Migration*> pendingMigrations;
};

#endif // MIGRATOR_H
