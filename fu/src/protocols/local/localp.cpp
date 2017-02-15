#ifndef H_PROTOCOLS_LOCALP
#define H_PROTOCOLS_LOCALP

#include "../ptcp.h"
#include "local.cpp"

class LocalProvider : public PtcProvider
{
public:
    virtual const wxString &GetName()
    {
        static wxString name = "local";
        return name;
    }

    virtual const vector<PtcSettingMeta*> &GetSettingMetas()
    {
        static vector<PtcSettingMeta*> metas = {
            new PtcSettingMeta("path", "Target", true, PtcSettingMeta::TYPE_DIR),
            new PtcSettingMeta("urlFormat", "URL format", true, PtcSettingMeta::TYPE_STRING, "http://localhost/%s (where %s will be replaced with URL)")
        };
        return metas;
    }

    virtual Ptc *CreateInstance(map<wxString, wxString> &settings)
    {
        auto local = new Local(settings);
        return (Ptc*)local;
    }
};

#endif
