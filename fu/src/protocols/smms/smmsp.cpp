#ifndef H_PROTOCOLS_SMMSP
#define H_PROTOCOLS_SMMSP

#include "../ptc.h"
#include "../ptcp.h"
#include "smms.cpp"

class SmmsProvider : public PtcProvider
{
public:
    virtual const wxString &GetName()
    {
        static wxString name = "sm.ms";
        return name;
    }
    
    virtual const vector<PtcSettingMeta*> &GetSettingMetas()
    {
        static vector<PtcSettingMeta*> metas =
        {
            new PtcSettingMeta("proxy", "Proxy", false, PtcSettingMeta::TYPE_STRING, "http://localhost:8080 or socks5://localhost:1080")
        };
        return metas;
    }
    
    virtual Ptc *CreateInstance(map<wxString, wxString> &settings)
    {
        auto smms = new Smms(settings);
        return (Ptc*)smms;
    }
    
    virtual const bool IsImageOnly()
    {
        return true;
    }
};

#endif
