#ifndef SETTINGSERVICE_H
#define SETTINGSERVICE_H

#include "../store/sqlstore.h"

class SettingService
{
    SqlStore &_store;

public:
    SettingService(SqlStore &store);

    QVariant get(const QString &settingKey);
    void set(const QString &settingKey, const QString &settingValue);

    bool imageCompressionEnabled();
    void setImageCompressionEnabled(bool enabled);

    bool imageWatermarkEnabled();
    void setImageWatermarkEnabled(bool enabled);

    QString imageWatermarkPath();
    void setImageWatermarkPath(const QString &path);

    QString imageWatermarkPosition();
    void setImageWatermarkPosition(const QString &pos);
};

#endif // SETTINGSERVICE_H
