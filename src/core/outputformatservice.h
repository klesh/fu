#ifndef OUTPUTFORMATSERVICE_H
#define OUTPUTFORMATSERVICE_H

#include "../store/sqlstore.h"
#include "../models/outputformat.h"

class OutputFormatService
{
    SqlStore &_store;
public:
    OutputFormatService(SqlStore &store);

    QList<OutputFormat> getAll();
    OutputFormat findById(uint id);
    void append(OutputFormat &outputFormat);
    void update(OutputFormat &outputFormat);
    void save(OutputFormat &outputFormat);
    void remove(uint id);
};

#endif // OUTPUTFORMATSERVICE_H
