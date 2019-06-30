#ifndef OUTPUTFORMATSERVICE_H
#define OUTPUTFORMATSERVICE_H

#include "../store/sqlstore.h"
#include "../models/outputformat.h"
#include "../models/clip.h"
#include "../models/upload.h"

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
    static QString format(const OutputFormat &outputFormat, const Clip &clip, const Upload &upload);
};

#endif // OUTPUTFORMATSERVICE_H
