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
