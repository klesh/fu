#ifndef H_PROTOCOLS_IMGUR
#define H_PROTOCOLS_IMGUR

#include "../ptc.h"
#include "imguros.cpp"
#include <curl/curl.h>
#include <map>


using namespace std;

class Imgur : public Ptc
{
public:
    Imgur(map<wxString, wxString> settings) : Ptc(settings)
    {
    }
    
    virtual const wxString FormatUrl(const wxString &remoteName, map<wxString, wxString> &extraInfo)
    {
        return extraInfo["url"];
    }
    
protected:
    virtual wxOutputStream *OnOpenStream(const wxString &fileName, const wxString &remoteName)
    {
        _lastExtraInfo.clear();
        auto stream = new ImgurOutputStream(fileName, _settings, &_message, &_lastExtraInfo);
        return (wxOutputStream*)stream;
    }
};

#endif
