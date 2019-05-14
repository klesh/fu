#include "migrator.h"

Migrator::Migrator()
{

}

int Migrator::totalPendingMigration()
{
    return 4;
}

void Migrator::run()
{
    QThread::sleep(1);
    emit progressChanged(1, 0.1);
    QThread::sleep(1);
    emit progressChanged(1, 0.3);
    QThread::sleep(1);
    emit progressChanged(1, 0.8);
    QThread::sleep(1);
    emit progressChanged(1, 1);
}
