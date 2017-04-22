#ifndef H_PROTOCOLS_FTP
#define H_PROTOCOLS_FTP

#include "../ptc.h"
#include "ftpos.cpp"
#include <map>


using namespace std;

class Ftp : public Ptc
{
public:
    Ftp(map<wxString, wxString> settings) : Ptc(settings)
    {
    }


protected:
    virtual wxOutputStream *OnOpenStream(const wxString &fileName, const wxString &remoteName)
    {
        _lastExtraInfo.clear();
        auto stream = new FtpOutputStream(remoteName, _settings, &_message, &_lastExtraInfo);
        return (wxOutputStream*)stream;
    }
};

#endif
