#include "migrator.h"
#include "migrations/migrationv0.h"

#include <QDebug>
#include <QSqlQuery>
#include <QDateTime>

Migrator::Migrator()
{
    // construct migration list
    _migrations.append(new MigrationV0());

    // find out current database version
    do {
        QSqlQuery query;

        query.exec("SELECT name "
                   "FROM sqlite_master "
                   "WHERE type='table' AND name='_migration_history'");
        if (!query.next()) {
            query.exec("CREATE TABLE _migration_history ("
                       "version INTEGER PRIMARY KEY, "
                       "appliedAt TEXT"
                       ")");
            break;
        }

        query.exec("SELECT version "
                   "FROM _migration_history "
                   "ORDER BY version DESC "
                   "LIMIT 1");
        if (!query.next())
            break;

        _currentDbVersion = query.value(0).toInt();
    } while (false);

    qDebug() << "current database version : " << _currentDbVersion;

    // select out all migrations which version number greater than current database version
    for (Migration *migration : _migrations) {
        if (migration->getVersion() > _currentDbVersion)
            _pendingMigrations.append(migration);
    }
}

Migrator::~Migrator()
{
    for (Migration *migration : _migrations) {
        delete migration;
    }
}

int Migrator::totalPendingMigration()
{
    return _pendingMigrations.size();
}

void Migrator::run()
{
    int i = 1;
    QSqlQuery query;
    query.prepare("INSERT INTO _migration_history "
                  "(version, appliedAt) "
                  " VALUES (:version, :appliedAt)");
    for (Migration *migration : _pendingMigrations) {
        emit progressChanged(i, 0);
        connect(migration, &Migration::progressChanged, [this, i](double p){ this->progressChanged(i, p);});
        migration->run();
        emit progressChanged(i, 1);
        qDebug() << "finish migration : " << migration->getVersion();
        query.bindValue(":version", migration->getVersion());
        query.bindValue(":appliedAt", QDateTime::currentDateTime().toString(Qt::ISODate));
        assert(query.exec());
    }
}
