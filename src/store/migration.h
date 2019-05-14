#ifndef MIGRATION_H
#define MIGRATION_H

#include <QtCore>

class Migration
{
public:
    virtual QString getVersion() = 0;
    virtual void run() = 0;
};

#endif // MIGRATION_H
