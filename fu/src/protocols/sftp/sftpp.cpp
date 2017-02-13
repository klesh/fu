#ifndef H_PROTOCOLS_SFTPPROVIDER
#define H_PROTOCOLS_SFTPPROVIDER

#include <vector>
#include "../ptc.h"
#include "../ptcp.h"
#include "sftp.cpp"

using namespace std;

class SftpProvider : public PtcProvider
{
public:
    virtual const wxString &GetName()
    {
        static wxString name = "sftp";
        return name;
    }
    
    virtual const vector<PtcSettingMeta*> &GetSettingMetas()
    {
        static vector<PtcSettingMeta*> metas =
        {
            new PtcSettingMeta("host", "Host", true),
            new PtcSettingMeta("port", "Port", false, PtcSettingMeta::TYPE_PINT),
            new PtcSettingMeta("user", "User", true),
            new PtcSettingMeta("pass", "Password", false, PtcSettingMeta::TYPE_PASSWORD),
            new PtcSettingMeta("key", "Private Key", false, PtcSettingMeta::TYPE_FILE),
            new PtcSettingMeta("path", "Remote Path", false),
            new PtcSettingMeta("urlFormat", "URL format", true, PtcSettingMeta::TYPE_STRING, "http://example.com/%s (where %s will be replaced with URL)")
        };
        return metas;
    }
    
    virtual Ptc *CreateInstance(map<wxString, wxString> &settings)
    {
        auto sftp = new Sftp(settings);
        return (Ptc*)sftp;
    }
};

#endif
