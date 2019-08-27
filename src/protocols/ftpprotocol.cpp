#include "ftpprotocol.h"
#include <qcurl.h>

FtpProtocol::FtpProtocol()
{
    _settingInfos.append({"proxy", tr("Proxy"), tr("socks5://localhost:1080"), Text, false, "", ""});
    _settingInfos.append({"host", tr("Host"), tr("FTP server host name"), Text, true, tr("Please pick a storage location"), ""});
    _settingInfos.append({"port", tr("Port"), tr(""), Integer, true, tr("Please enter port number"), 21});
    _settingInfos.append({"user", tr("Username"), tr(""), Text, true, tr("Please enter login user name"), ""});
    _settingInfos.append({"pass", tr("Password"), tr(""), Text, true, tr("Please enter login password"), ""});
    _settingInfos.append({"path", tr("Remote Path"), tr(""), Text, false, "", ""});
    _settingInfos.append({"outputUrl", tr("Output Url"), tr(""), Text, false, "", "%1"});
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
    : _curl(createQCurl(settings, "ftp"))
{
    _outputUrl = settings["outputUrl"].toString();
}

void FtpUploader::upload(QIODevice *stream, UploadJob &job)
{
    if (job.overwrite == false) {
        int exists;
        auto re = _curl.exists(exists, job.name);
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

    auto res = _curl.put(job.name, *stream);

    if (res.code()) {
        job.status = Error;
        job.msg = res.message();
    } else {
        job.status = Success;
        job.url = _outputUrl.arg(job.name);
    }
}
