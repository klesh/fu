#ifndef H_PROTOCOLS_LOCALP
#define H_PROTOCOLS_LOCALP

#include "../ptcp.h"
#include "local.cpp"

class LocalProvider : public PtcProvider
{
public:
    virtual wxString &GetName()
    {
        static wxString name = "local";
        return name;
    }
    
    virtual const vector<PtcSettingMeta*> &GetSettingMetas()
    {
        static vector<PtcSettingMeta*> metas = {
            new PtcSettingMeta("path", "Target", true, PtcSettingMeta::TYPE_DIR)
        };
        return metas;
    }
    
    virtual Ptc *CreateInstance(map<wxString, wxString> &settings)
    {
        auto local = new Local();
        local->SetPath(settings["path"]);
        return (Ptc*)local;
    }
};

#endif
