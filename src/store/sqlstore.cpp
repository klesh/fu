#include "../application.h"
#include "sqlstore.h"
#include "../core/error.h"

#include <QErrorMessage>

SqlStore::SqlStore(const QString &dbPath)
    : _db(QSqlDatabase::addDatabase("QSQLITE")), _query(QSqlQuery(_db))
{
    _db.setDatabaseName(dbPath);
}

SqlStore::~SqlStore()
{
}

bool SqlStore::open()
{
    return _db.open();
}

void SqlStore::close()
{
    _db.close();
}

QSqlQuery &SqlStore::prepare(const QString &sql)
{
    if (!_db.isOpen())
        open();

    _query.prepare(sql);
    return _query;
}

bool SqlStore::exec()
{
    bool result = _query.exec();
    if (!result) {
        auto detail = QString("  SQL: %1\n  ERR: %2").arg(_query.executedQuery()).arg(_query.lastError().text());
        qDebug() << detail;
        throw_error(detail);
    }
    return result;
}

QSqlQuery &SqlStore::exec(const QString &sql)
{
    prepare(sql);
    exec();
    return _query;
}
