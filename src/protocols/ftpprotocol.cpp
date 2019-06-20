#include "ftpprotocol.h"

FtpProtocol::FtpProtocol()
{
    _settingInfos.append({"host", tr("Host"), tr("FTP server host name"), Text, true, tr("Please pick a storage location"), ""});
    _settingInfos.append({"port", tr("Port"), tr(""), Integer, true, "", 22});
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


// uploader
FtpUploader::FtpUploader(QVariantMap settings)
{

}

FtpUploader::~FtpUploader()
{

}

QString FtpUploader::upload(QDataStream *stream, const QString name)
{
    return "";
}
