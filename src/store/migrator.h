#ifndef MIGRATOR_H
#define MIGRATOR_H

#include <QThread>

class Migrator : public QThread
{
    Q_OBJECT

public:
    Migrator();

    int totalPendingMigration();
    void run() override;

signals:
    void progressChanged(int migrationIndex, double pencent);
};

#endif // MIGRATOR_H
