#ifndef MIGRATION_H
#define MIGRATION_H

#include "sqlstore.h"

#include <QObject>

class Migration : public QObject
{
    Q_OBJECT

public:
    virtual int getVersion() = 0;
    virtual void run(SqlStore &store) = 0;

signals:
    void progressChanged(double percent);
};

#endif // MIGRATION_H
