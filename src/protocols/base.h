#ifndef PROTOCOL_H
#define PROTOCOL_H

#include <QtCore>
#include <qcurl.h>
#include "../models/server.h"
#include "../models/clip.h"

enum ProtocolSettingDataType {
    Text,
    Integer,
    Directory,
    File
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
    QByteArray data;        // either memory bytes
    QString path;           // or local file to be uploaded
    QString name;           // file name
    UploadJobStatus status = Pending;
    int renameCounter = 0;
    bool overwrite = false;
    QString url;
    QString msg;
    bool saved = false;
    bool notified = false;
};

class Uploader : public QObject
{
    Q_OBJECT
public:
    virtual void upload(QIODevice *device, UploadJob &job) = 0;
};

class Protocol : public QObject
{
    Q_OBJECT
public:
    virtual ~Protocol() {}
    virtual const QString getName() = 0;
    virtual const QString getTitle() = 0;
    virtual const QList<ProtocolSettingInfo> &getSettingInfos() = 0;
    virtual Uploader *createUploader(const QVariantMap &settings) = 0;
};

inline QCurl createQCurl(const QVariantMap &settings, const QString scheme) {
    QUrl url;
    url.setScheme(scheme);
    url.setHost(settings["host"].toString());
    url.setPort(settings["port"].toInt());
    url.setUserName(settings["user"].toString());
    url.setPassword(settings["pass"].toString());
    auto path = settings["path"].toString();
    if (!path.endsWith('/'))
        path += '/';
    url.setPath(path);
    QCurl curl(url);
    curl.setProxyUrl(settings["proxy"].toString());
    return curl;
}

inline QString formatPath(QString folder, QString file) {
    if (folder.isEmpty())
        return file;

    QDate today = QDate::currentDate();
    QString path = folder
                .arg(today.year(), 4, 10, QChar('0'))
                .arg(today.month(), 2, 10, QChar('0'))
                .arg(today.day(), 2, 10, QChar('0'));
    if (!path.endsWith('/')) path.append('/');
    path.append(file);
    return path;
}

inline QString joinPath(QString root, QString path) {
    if (root.isEmpty())
        return path;
    if (!root.endsWith('/')) root.append('/');
    return root + path;
}
#endif // PROTOCOL_H
