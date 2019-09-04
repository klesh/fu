#include "ftpprotocol.h"
#include <qcurl.h>

FtpProtocol::FtpProtocol()
{
    _settingInfos.append({"proxy", tr("Proxy"), tr("socks5://localhost:1080"), Text, false, "", ""});
    _settingInfos.append({"host", tr("Host"), tr("FTP server host name"), Text, true, tr("Please enter server host name"), ""});
    _settingInfos.append({"port", tr("Port"), tr(""), Integer, true, tr("Please enter port number"), 21});
    _settingInfos.append({"user", tr("User name"), tr(""), Text, true, tr("Please enter login user name"), ""});
    _settingInfos.append({"pass", tr("Password"), tr(""), Text, true, tr("Please enter login password"), ""});
    _settingInfos.append({"root", tr("Root path"), tr("Root path on remote server, will not be part of output url"), Text, false, "", ""});
    _settingInfos.append({"folder", tr("Folder"), tr("%1 for Year, %2 for Month, %3 for Day Of Month, part of output url"), Text, false, "", "%1-%2-%3"});
    _settingInfos.append({"outputUrl", tr("Output Url"), tr("%1 for {Folder}/{File name}"), Text, false, "", "http://localhost/%1"});
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

// uploader
FtpUploader::FtpUploader(QVariantMap settings)
    : _curl(createQCurl(settings, "ftp")), _settings(settings)
{
}

void FtpUploader::upload(QIODevice *stream, UploadJob &job)
{
    QString path = formatPath(_settings["folder"].toString(), job.name);
    QString fullpath = joinPath(_settings["root"].toString(), path);

    if (job.overwrite == false) {
        int exists;
        auto re = _curl.exists(exists, fullpath);
        if (exists == 1) {
            job.status = Duplicated;
            return;
        }
        if (exists == -1) {
            job.status = Error;
            job.msg = re.message();
            return;
        }
    }

    auto res = _curl.put(fullpath, *stream);

    if (res.code()) {
        job.status = Error;
        job.msg = res.message();
    } else {
        job.status = Success;
        job.url = _settings["outputUrl"].toString().arg(path);
    }
}
