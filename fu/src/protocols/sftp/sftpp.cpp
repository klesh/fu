#ifndef H_PROTOCOLS_SFTPPROVIDER
#define H_PROTOCOLS_SFTPPROVIDER

#include <vector>
#include "../ptc.h"
#include "../ptcp.h"
#include "sftp.cpp"

using namespace std;

class SftpProvider : public PtcProvider
{
private:
    vector<PtcSettingMeta*> _settingMetas;
    
public:
    SftpProvider()
    {
        _settingMetas.push_back(new PtcSettingMeta("host", "Host", true));
        _settingMetas.push_back(new PtcSettingMeta("port", "Port", false, PtcSettingMeta::TYPE_PINT));
        _settingMetas.push_back(new PtcSettingMeta("user", "User", true));
        _settingMetas.push_back(new PtcSettingMeta("pass", "Password", false, PtcSettingMeta::TYPE_PASSWORD));
        _settingMetas.push_back(new PtcSettingMeta("key", "Private Key", false, PtcSettingMeta::TYPE_FILE));
        _settingMetas.push_back(new PtcSettingMeta("path", "Remote Path", false));
    }
    
    virtual const wxString &GetName()
    {
        static wxString name = "sftp";
        return name;
    }
    
    virtual const vector<PtcSettingMeta*> &GetSettingMetas()
    {
        return _settingMetas;
    }
    
    virtual Ptc *CreateInstance(map<wxString, wxString> &settings)
    {
        Sftp *sftp = new Sftp();
        for (auto const &pair : settings)
        {
            if (pair.first == wxT("host"))
                sftp->SetHost(pair.second);
            else if (pair.first == wxT("port"))
                sftp->SetPort(pair.second);
            else if (pair.first == wxT("user"))
                sftp->SetUser(pair.second);
            else if (pair.first == wxT("pass"))
                sftp->SetPass(pair.second);
            else if (pair.first == wxT("key"))
                sftp->SetKeyPath(pair.second);
            else if (pair.first == wxT("path"))
                sftp->SetRemotePath(pair.second);
        }
        return (Ptc*)sftp;
    }
};

#endif
