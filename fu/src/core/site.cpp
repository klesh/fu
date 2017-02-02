#ifndef H_CORE_SITE
#define H_CORE_SITE

#include <wx/wx.h>
#include <map>
#include "func.cpp"
using namespace std;

class Site
{
private:
    wxString _id;
    wxString _name;
    wxString _protocol;
    wxString _urlFormat;
    map<wxString, wxString> _settings;
    
public:
    Site(){}
    
    void SetId(wxString &id)
    {
        _id = id;
    }
    
    wxString &GetId()
    {
        if (_id == wxEmptyString)
        {
            _id = Uuid();
        }
        
        return _id;
    }
    
    wxString &GetName()
    {
        return _name;
    }
    
    void SetName(const wxString &name)
    {
        _name = name;
    }
    
    wxString &GetUrlFormat()
    {
        return _urlFormat;
    }
    
    void SetUrlFormat(wxString &urlFormat)
    {
        _urlFormat = urlFormat;
    }
    
    wxString &GetProtocol()
    {
        return _protocol;
    }
    
    map<wxString, wxString> &GetSettings()
    {
        return _settings;
    }
    
    void SetSettings(map<wxString, wxString> settings)
    {
        _settings = settings;
    }
    
    void ClearSettings()
    {
        _settings.clear();
    }
    
    void SetProtocol(const wxString &protocol)
    {
        _protocol = protocol;
    }
    
    wxString &GetSetting(const wxString &key)
    {
        return _settings[key];
    }
    
    void SetSetting(const wxString &key, const wxString &value)
    {
        _settings[key] = value;
    }
};

#endif
