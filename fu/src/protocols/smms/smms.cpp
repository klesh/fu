#ifndef H_PROTOCOLS_SMMS
#define H_PROTOCOLS_SMMS

#include "../ptc.h"
#include "smmsos.cpp"
#include <curl/curl.h>
#include <map>


using namespace std;

class Smms : public Ptc
{
public:
    Smms(map<wxString, wxString> settings) : Ptc(settings)
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
        auto stream = new SmmsOutputStream(fileName, _settings, &_message, &_lastExtraInfo);
        return (wxOutputStream*)stream;
    }
};

#endif
