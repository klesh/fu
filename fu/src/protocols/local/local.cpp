#ifndef H_PROTOCOLS_LOCAL
#define H_PROTOCOLS_LOCAL

#include <wx/wx.h>
#include <wx/filename.h>
#include <wx/wfstream.h>
#include "../ptc.h"

class Local : public Ptc
{
private:
    wxString _path;
    wxFileOutputStream *_file = NULL;
    
    
public:
    void SetPath(wxString &path)
    {
        _path = path;
    }
    
    bool Connect()
    {
        return true;
    }
    
    void Disconnect()
    {
        
    }
    
    wxOutputStream *OpenStream(const wxString &remoteName)
    {
        wxFileName path(_path, remoteName);
        return new wxFileOutputStream(path.GetFullPath());
    }
};

#endif
