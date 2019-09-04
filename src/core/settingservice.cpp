#include "settingservice.h"

#include <QSqlQuery>
#include <QSqlRecord>

QVariant SettingService::get(const QString &settingKey)
{
    QSqlQuery query;
    query.prepare("SELECT settingValue FROM settings WHERE settingKey=:settingKey");
    query.bindValue(":settingKey", settingKey);
    assert(query.exec());
    if (query.next()) {
        return query.value(0);
    }
    return QString();
}

void SettingService::set(const QString &settingKey, const QString &settingValue)
{
    QSqlQuery query;
    query.prepare("UPDATE settings SET settingValue=:settingValue WHERE settingKey=:settingKey");
    query.bindValue(":settingKey", settingKey);
    query.bindValue(":settingValue", settingValue);
    assert(query.exec());
    if (query.numRowsAffected() == 0) {
        query.prepare("INSERT INTO settings (settingKey, settingValue) VALUES (:settingKey, :settingValue)");
        query.bindValue(":settingKey", settingKey);
        query.bindValue(":settingValue", settingValue);
        query.exec();
    }
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

QString SettingService::lang()
{
    return get("lang").toString();
}

void SettingService::setLang(const QString &lang)
{
    set("lang", lang);
}
