#ifndef SQLSTORE_H
#define SQLSTORE_H

#include <QtCore>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QSqlRecord>

class SqlStore
{
    QSqlDatabase _db;
    QSqlQuery _query;

public:
    SqlStore(const QString &dbPath);
    ~SqlStore();

    bool open();
    void close();
    QSqlQuery &prepare(const QString &sql);
    QSqlQuery &exec(const QString &sql);
    QSqlQuery exec();
};


#endif // SQLSTORE_H
