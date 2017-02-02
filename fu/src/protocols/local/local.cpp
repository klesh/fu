#ifndef H_PROTOCOLS_LOCAL
#define H_PROTOCOLS_LOCAL

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include "../ptc.h"

class Local : public Ptc
{
public:
    Local(map<wxString, wxString> settings) : Ptc(settings)
    {
    }
    
protected:
    virtual wxOutputStream *OnOpenStream(const wxString &fileName, const wxString &remoteName)
    {
        wxFileName path(_settings["path"], remoteName);
        return new wxFileOutputStream(path.GetFullPath());
    }
};

#endif
