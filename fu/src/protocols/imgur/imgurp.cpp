#ifndef H_PROTOCOLS_IMGURP
#define H_PROTOCOLS_IMGURP

#include "../ptc.h"
#include "../ptcp.h"
#include "imgur.cpp"

class ImgurProvider : public PtcProvider
{
public:
    virtual const wxString &GetName()
    {
        static wxString name = "imgur.com";
        return name;
    }

    virtual const vector<PtcSettingMeta*> &GetSettingMetas()
    {
        static vector<PtcSettingMeta*> metas =
        {
            new PtcSettingMeta("clientId", "Client-ID", true, PtcSettingMeta::TYPE_STRING, "obtain from http://api.imgur.com"),
            new PtcSettingMeta("proxy", "Proxy", false, PtcSettingMeta::TYPE_STRING, "http://localhost:8080 or socks5://localhost:1080")
        };
        return metas;
    }

    virtual Ptc *CreateInstance(map<wxString, wxString> &settings)
    {
        auto imgur = new Imgur(settings);
        return (Ptc*)imgur;
    }

    virtual const bool IsImageOnly()
    {
        return true;
    }
};

#endif
