#include "ftpprotocol.h"

FtpProtocol::FtpProtocol()
{
    _settingInfos.append({"host", tr("Host"), tr("FTP server host name"), Text, true, tr("Please pick a storage location"), ""});
    _settingInfos.append({"port", tr("Port"), tr(""), Integer, true, "", 22});
    _settingInfos.append({"user", tr("Username"), tr(""), Text, true, "", ""});
    _settingInfos.append({"pass", tr("Password"), tr(""), Text, true, "", ""});
    _settingInfos.append({"path", tr("Path"), tr(""), Text, true, "", ""});
}

const QString FtpProtocol::getName()
{
    return "ftp";
}

const QString FtpProtocol::getTitle()
{
    return tr("FTP");
}

const QList<ProtocolSettingInfo> &FtpProtocol::getSettingInfos()
{
    return _settingInfos;
}

Uploader *FtpProtocol::createUploader(QVariantMap &settings)
{
    return new FtpUploader(settings);
}

QUrl createUrlFromSettings(QVariantMap &settings) {
    QUrl url;
    url.setScheme("ftp");
    url.setHost(settings["host"].toString());
    url.setPort(settings["port"].toInt());
    url.setUserName(settings["user"].toString());
    url.setPassword(settings["pass"].toString());
    auto path = settings["path"].toString();
    if (!path.endsWith('/'))
        path += '/';
    url.setPath(path);
    return url;
}

// uploader
FtpUploader::FtpUploader(QVariantMap settings)
    : url(createUrlFromSettings(settings))
{
}

FtpUploader::~FtpUploader()
{

}

QString FtpUploader::upload(QDataStream *stream, const QString name, bool overwrite = false)
{
    QUrl ftpUrl(url);
    ftpUrl.setPath(url.path() + name);
    QNetworkRequest req(url);
    auto res = network.get(req);
    connect(res, &QNetworkReply::readyRead, [res](){
        res->close();
    }, );
    QObject::connect(repl,QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),[repl]()->void{
         qDebug() << "Error: " + repl->errorString() << (repl->error() == QNetworkReply::OperationCanceledError);
     });
    path.dj
    QUrl ftpUrl(url);
    ftpUrl.setPath();
    QNetworkRequest req(url);
    network.put(req, stream->device());
    return name;
}
