#ifndef LOCALSTORAGEPROTOCOL_H
#define LOCALSTORAGEPROTOCOL_H

#include <QtCore>
#include "./base.h"


class LocalStorageUploader : public Uploader
{
    Q_OBJECT
    QVariantMap _settings;

public:
    LocalStorageUploader(const QVariantMap settings);
    void upload(QIODevice *stream, UploadJob &job) override;
};

class LocalStorageProtocol : public Protocol
{
    Q_OBJECT
    QList<ProtocolSettingInfo> _settingInfos;

public:
    LocalStorageProtocol();
    const QString getName();
    const QString getTitle();
    const QList<ProtocolSettingInfo> &getSettingInfos();
    Uploader *createUploader(const QVariantMap &settings);
};

#endif // LOCALSTORAGEPROTOCOL_H
