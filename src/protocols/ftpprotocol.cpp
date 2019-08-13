#include "ftpprotocol.h"
#include <qcurl.h>

FtpProtocol::FtpProtocol()
{
    _settingInfos.append({"host", tr("Host"), tr("FTP server host name"), Text, true, tr("Please pick a storage location"), ""});
    _settingInfos.append({"port", tr("Port"), tr(""), Integer, true, "", 22});
    _settingInfos.append({"user", tr("Username"), tr(""), Text, true, "", ""});
    _settingInfos.append({"pass", tr("Password"), tr(""), Text, true, "", ""});
    _settingInfos.append({"path", tr("Path"), tr(""), Text, true, "", ""});
    _settingInfos.append({"outputUrl", tr("Output Url"), tr(""), Text, true, "", "%1"});
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

Uploader *FtpProtocol::createUploader(const QVariantMap &settings)
{
    return new FtpUploader(settings);
}

QUrl createUrlFromSettings(const QVariantMap &settings) {
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
    : _ftpUrl(createUrlFromSettings(settings))
{
    _outputUrl = settings["outputUrl"].toString();
}

void FtpUploader::upload(QDataStream *stream, UploadJob &job)
{
    QCurl curl(_ftpUrl);

    if (curl.exists(job.name)) {
        job.status = Duplicated;
        return;
    }

    auto res = curl.put(job.name, *stream->device());

    if (res.code()) {
        job.status = Error;
        job.msg = res.message();
    } else {
        job.status = Success;
        job.url = _outputUrl.arg(job.name);
    }
}
