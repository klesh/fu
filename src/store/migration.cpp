#include "migration.h"

#include <QSqlQuery>
#include <QSqlError>

void Migration::execute(const QString &sql)
{
    QSqlQuery query;
    if (!query.exec(sql))
    {
        throw query.lastError();
    }
}
