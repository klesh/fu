#include "error.h"
#include "settingservice.h"

SettingService::SettingService(SqlStore &store)
    : _store(store)
{
}

QVariant SettingService::get(const QString &settingKey)
{
    auto query = _store.prepare("SELECT settingValue FROM settings WHERE settingKey=:settingKey");
    query.bindValue(":settingKey", settingKey);

    auto result = _store.exec();
    if (result.next()) {
        return result.value(0);
    }
    throw_error(QString("Requested setting `%1` not found").arg(settingKey));
}

void SettingService::set(const QString &settingKey, const QString &settingValue)
{
    auto query = _store.prepare("UPDATE settings SET settingValue=:settingValue WHERE settingKey=:settingKey");
    query.bindValue(":settingKey", settingKey);
    query.bindValue(":settingValue", settingValue);

    _store.exec();
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
