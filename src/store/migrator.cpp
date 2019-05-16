#include "migrator.h"
#include "migrations/migrationv0.h"

#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>

Migrator::Migrator()
{
    // construct migration list
    migrations.append(new MigrationV0);

    // find out current database version
    do {
        QSqlQuery query;
        query.exec("SELECT name FROM sqlite_aster WHERE type='table' AND name='_migration_history");
        if (!query.next()) {
            query.exec("CREATE TABLE _migration_history ("
                      "version INTEGER PRIMARY KEY, "
                      "appliedAt TEXT"
                      ")");
            break;
        }

        query.exec("SELECT version FROM _migration_history ORDER BY version DESC LIMIT 1");
        if (!query.next()) {
            break;
        }

        currentDbVersion = query.value(0).toInt();
    } while (false);

    // select out all migrations which version number greater than current database version
    for (Migration *migration : migrations) {
        if (migration->getVersion() > currentDbVersion)
            pendingMigrations.append(migration);
    }
}

Migrator::~Migrator()
{
    for (Migration *migration : migrations) {
        delete migration;
    }
}

int Migrator::totalPendingMigration()
{
    return pendingMigrations.size();
}

void Migrator::run()
{
    int i = 1;
    for (Migration *migration : pendingMigrations) {
        emit progressChanged(i, 0);
        connect(migration, &Migration::progressChanged, [this, i](double p){ this->progressChanged(i, p);});
        migration->run();
        emit progressChanged(i, 1);
        qDebug() << "finish migration : " << migration->getVersion();
        QSqlQuery query;
        query.prepare("INSERT INTO _migration_history "
                   "(version, appliedAt) "
                   " VALUES (:version, :appliedAt)");
        query.bindValue(":version", migration->getVersion());
        query.bindValue(":appliedAt", QDateTime::currentDateTime().toString(Qt::ISODate));
        query.exec();
    }
}
