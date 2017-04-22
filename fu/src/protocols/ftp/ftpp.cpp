#ifndef H_PROTOCOLS_FTPP
#define H_PROTOCOLS_FTPP

#include "../ptc.h"
#include "../ptcp.h"
#include "ftp.cpp"

class FtpProvider : public PtcProvider
{
public:
    virtual const wxString &GetName()
    {
        static wxString name = "ftp";
        return name;
    }

    virtual const vector<PtcSettingMeta*> &GetSettingMetas()
    {
        static vector<PtcSettingMeta*> metas =
        {
            new PtcSettingMeta("host", "Host", true),
            new PtcSettingMeta("port", "Port", false, PtcSettingMeta::TYPE_PINT),
            new PtcSettingMeta("user", "User", true),
            new PtcSettingMeta("pass", "Password", true, PtcSettingMeta::TYPE_PASSWORD),
            new PtcSettingMeta("path", "Remote Path", false),
            new PtcSettingMeta("dateFolder", "Misc", false, PtcSettingMeta::TYPE_BOOL, "Organize By Date As Folder"),
            new PtcSettingMeta("urlFormat", "URL format", true, PtcSettingMeta::TYPE_STRING, "http://example.com/%s (where %s will be replaced with URL)")
        };
        return metas;
    }

    virtual Ptc *CreateInstance(map<wxString, wxString> &settings)
    {
        auto ftp = new Ftp(settings);
        return (Ptc*)ftp;
    }

    virtual const bool IsImageOnly()
    {
        return false;
    }
};

#endif
