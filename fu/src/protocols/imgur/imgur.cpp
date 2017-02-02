#ifndef H_PROTOCOLS_IMGUR
#define H_PROTOCOLS_IMGUR

#include "../ptc.h"
#include "imgurmos.cpp"
#include <curl/curl.h>
#include <map>

#define UPLOAD_URL "https://api.imgur.com/3/image.xml"

using namespace std;

class Imgur : public Ptc
{
public:
    Imgur(map<wxString, wxString> settings) : Ptc(settings)
    {
    }
    
    virtual bool Connect()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        
        return true;
    }
    
    virtual void Disconnect()
    {
        curl_global_cleanup();
    }
    
    virtual const wxString FormatUrl(const wxString &remoteName, map<wxString, wxString> &extraInfo)
    {
        return extraInfo["url"];
    }
    
protected:
    virtual wxOutputStream *OnOpenStream(const wxString &fileName, const wxString &remoteName)
    {
        CURL *curl = curl_easy_init();
        if (curl == NULL)
            return NULL;
        
        curl_easy_setopt(curl, CURLOPT_URL, UPLOAD_URL);
        
        _lastExtraInfo.clear();
        auto stream = new ImgurMemoryOutputStream(curl, fileName, _settings, &_message, &_lastExtraInfo);
        return (wxOutputStream*)stream;
    }
};

#endif
