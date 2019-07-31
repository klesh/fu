#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtCore>
#include "../models/server.h"
#include "../models/clip.h"

enum ProtocolSettingDataType {
    Text,
    Integer,
    Directory
};

struct ProtocolSettingInfo
{
    QString name;
    QString label;
    QString description;
    ProtocolSettingDataType type;
    bool required;
    QString hint;
    QVariant defaultValue;
};

enum UploadJobStatus {
    Pending = 0,
    Duplicated,
    Assigned,
    Success,
    Error,
    Skipped,
};

struct UploadJob {
    Server server;
    Clip clip;
    QByteArray data;
    QString name;
    UploadJobStatus status;
    int counter;
    bool overwrite = false;
    QString message;
    QString output;
};

class Uploader : public QObject
{
    Q_OBJECT
public:
    virtual QString upload(QDataStream *stream, const QString name, bool overwrite = false) = 0;
    virtual ~Uploader() {}

signals:
    void fileExists();
};

class Protocol : public QObject
{
    Q_OBJECT
public:
    virtual ~Protocol() {}
    virtual const QString getName() = 0;
    virtual const QString getTitle() = 0;
    virtual const QList<ProtocolSettingInfo> &getSettingInfos() = 0;
    virtual Uploader *createUploader(QVariantMap &settings) = 0;
};

#endif // PROTOCOL_H
