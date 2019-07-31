#include "settingservice.h"

#include <QSqlQuery>
#include <QSqlRecord>

QVariant SettingService::get(const QString &settingKey)
{
    QSqlQuery query;
    query.prepare("SELECT settingValue FROM settings WHERE settingKey=:settingKey");
    query.bindValue(":settingKey", settingKey);
    assert(query.exec());
    assert(query.next());
    return query.value(0);
}

void SettingService::set(const QString &settingKey, const QString &settingValue)
{
    QSqlQuery query;
    query.prepare("UPDATE settings SET settingValue=:settingValue WHERE settingKey=:settingKey");
    query.bindValue(":settingKey", settingKey);
    query.bindValue(":settingValue", settingValue);
    assert(query.exec());
}

bool SettingService::imageCompressionEnabled()
{
    return get("imageCompressionEnabled").toBool();
}

void SettingService::setImageCompressionEnabled(bool enabled)
{
    set("imageCompressionEnabled", enabled ? "true" : "false");
}

bool SettingService::imageWatermarkEnabled()
{
    return get("imageWatermarkEnabled").toBool();
}

void SettingService::setImageWatermarkEnabled(bool enabled)
{
    set("imageWatermarkEnabled", enabled ? "true" : "false");
}

QString SettingService::imageWatermarkPath()
{
    return get("imageWatermarkPath").toString();
}

void SettingService::setImageWatermarkPath(const QString &path)
{
    set("imageWatermarkPath", path);
}

QString SettingService::imageWatermarkPosition()
{
    return get("imageWatermarkPosition").toString();
}

void SettingService::setImageWatermarkPosition(const QString &pos)
{
    set("imageWatermarkPosition", pos);
}
