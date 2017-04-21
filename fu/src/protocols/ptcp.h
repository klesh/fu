#ifndef H_PROTOCOL_PROVIDER
#define H_PROTOCOL_PROVIDER

#include <wx/wx.h>
#include <vector>
#include <map>
#include "ptc.h"

using namespace std;
/**
 * describe setting information for a protocol
 */
class PtcSettingMeta
{
protected:
    wxString _name;
    wxString _caption;
    bool _isRequired;
    int _type;
    wxString _hint;

public:
    enum {
        TYPE_STRING,
        TYPE_PASSWORD,
        TYPE_PINT,
        TYPE_FILE,
        TYPE_DIR,
        TYPE_BOOL
    };

    PtcSettingMeta(const wxString &name, const wxString &caption, bool isRequired = false, int type = TYPE_STRING, const wxString hint = wxEmptyString)
    {
        _name = name;
        _caption = caption;
        _isRequired = isRequired;
        _type = type;
        _hint = hint;
    }

    wxString &GetName() { return _name; }
    wxString &GetCaption() { return _caption; }
    bool IsRequired() { return _isRequired; }
    int GetValueType() { return _type; }
    wxString &GetHint() { return _hint; }
};

/**
 * protocol provider interface
 */
class PtcProvider
{
public:
    virtual const wxString &GetName() = 0;
    virtual const vector<PtcSettingMeta*> &GetSettingMetas() = 0;
    virtual Ptc *CreateInstance(map<wxString, wxString> &settings) = 0;
    virtual const bool IsImageOnly() { return false; }
};


#endif
