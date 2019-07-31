#ifndef OUTPUTFORMATSERVICE_H
#define OUTPUTFORMATSERVICE_H

#include "../models/format.h"
#include "../models/clip.h"
#include "../models/upload.h"

class FormatService
{
public:
    QList<Format> getAll();
    Format findById(uint id);
    bool append(Format &outputFormat);
    bool update(Format &outputFormat);
    bool save(Format &outputFormat);
    void remove(uint id);
};

#endif // OUTPUTFORMATSERVICE_H
