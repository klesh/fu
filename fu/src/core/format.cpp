#ifndef H_CORE_FORMAT
#define H_CORE_FORMAT

#include <wx/wx.h>

/**
 * Entity to store customize url format;
 */
class Format
{
private:
    wxString _name;
    wxString _tpl;
    
public:
    Format(){}
    Format(const wxString &name, const wxString &tpl)
    {
        SetName(name);
        SetTemplate(tpl);
    }
    
    wxString &GetName()
    {
        return _name;
    }
    
    void SetName(const wxString &name)
    {
        _name = name;
    }
    
    wxString &GetTemplate()
    {
        return _tpl;
    }
    
    void SetTemplate(const wxString &tpl)
    {
        _tpl = tpl;
    }
    
    wxString Execute(const wxString &url)
    {
        return wxString::Format(_tpl, url);
    }
};

#endif
