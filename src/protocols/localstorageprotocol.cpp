#include "localstorageprotocol.h"

LocalStorageProtocol::LocalStorageProtocol()
{
    _settingInfos.append({"folder", tr("Storage folder"), tr("Click 'Browse' button to select a folder"), Directory, true, tr("Please pick a storage location"), ""});
    _settingInfos.append({"output", tr("Output template"), tr("%1 will be replace with your file name"), Text, false, "", "http://localhost/%1"});
}

const QString LocalStorageProtocol::getName()
{
    return "local";
}

const QString LocalStorageProtocol::getTitle()
{
    return tr("Local Storage");
}

const QList<ProtocolSettingInfo> &LocalStorageProtocol::getSettingInfos()
{
    return _settingInfos;
}

Uploader *LocalStorageProtocol::createUploader(QVariantMap &settings)
{
    return new LocalStorageUploader(settings);
}


// uploader
LocalStorageUploader::LocalStorageUploader(QVariantMap settings)
{
    _folder = settings["folder"].toString();
    _output = settings["output"].toString();
}

LocalStorageUploader::~LocalStorageUploader()
{

}

QString LocalStorageUploader::upload(QDataStream *stream, const QString name)
{
    QThread::sleep(3);
    QDir dir(_folder);
    QFile outputFile(dir.absoluteFilePath(name));
    outputFile.open(QIODevice::WriteOnly);
    QDataStream outputStream(&outputFile);
    char *buffer = new char[1024];
    while (!stream->atEnd()) {
        int len = stream->readRawData(buffer, 1024);
        outputFile.write(buffer, len);
    }
    outputFile.close();

    delete[] buffer;
    return _output.arg(name);
}
