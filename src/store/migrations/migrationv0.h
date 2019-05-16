#ifndef MIGRATIONV0_H
#define MIGRATIONV0_H

#include "../migration.h"

class MigrationV0 : public Migration
{
    Q_OBJECT

public:
    MigrationV0();

    int getVersion();
    void run();
};

#endif // MIGRATIONV0_H
