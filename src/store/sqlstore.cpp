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
    bool r = true;
    if (!_db.isOpen()) {
        r = _db.open();
        if (r)
            exec("PRAGMA foreign_keys=ON");
    }
    return r;
}

void SqlStore::close()
{
    _db.close();
}

QSqlQuery &SqlStore::prepare(const QString &sql)
{
    open();

    //_query = QSqlQuery(_db);
    _query.prepare(sql);
    return _query;
}

QSqlQuery SqlStore::exec()
{
    bool result = _query.exec();
    if (!result) {
        auto detail = QString("  SQL: %1\n  ERR: %2").arg(_query.executedQuery()).arg(_query.lastError().text());
        qDebug() << detail;
        throw_error(detail);
    } else {
        /*
        auto rec = _query.record();
        QStringList fields;
        for (int i = 0; i < rec.count(); i++) {
            fields.append(rec.fieldName(i));
        }
        qDebug() << _query.executedQuery() << fields.join(", ");
        */
        qDebug() << _query.executedQuery() << _query.boundValues();
    }
    return _query;
}

QSqlQuery &SqlStore::exec(const QString &sql)
{
    prepare(sql);
    exec();
    return _query;
}
