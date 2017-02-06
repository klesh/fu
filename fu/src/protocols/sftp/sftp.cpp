#ifndef H_PROTOCOLS_SFTP
#define H_PROTOCOLS_SFTP

#include <wx/wx.h>
#include "../ptc.h"
#include "sftpos.cpp"


class Sftp : public Ptc
{    
public:
    Sftp(map<wxString, wxString> settings) : Ptc(settings)
    {
    }
        
protected:
    
    virtual wxOutputStream *OnOpenStream(const wxString &fileName, const wxString &remoteName)
    {        
        SftpOutputStream *stream = new SftpOutputStream(_settings, remoteName, &_message);
        return (wxOutputStream*)stream;
    }
};

#endif
