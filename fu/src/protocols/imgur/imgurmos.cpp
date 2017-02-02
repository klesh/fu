#ifndef H_PROTOCOLS_IMGURFOS
#define H_PROTOCOLS_IMGURFOS

#include <wx/wx.h>
#include <wx/mstream.h>
#include <curl/curl.h>
#include <map>
#include <wx/xml/xml.h>
#include <wx/sstream.h>

using namespace std;

class ImgurMemoryOutputStream : wxMemoryOutputStream
{
private:
    CURL *_curl = NULL;
    wxString _fileName;
    wxString *_message;
    map<wxString, wxString> _settings;
    map<wxString, wxString> *_extraInfo;
    
public:
    ImgurMemoryOutputStream(CURL* curl, const wxString &fileName, const map<wxString, wxString> &settings, wxString *message, map<wxString, wxString> *extraInfo)
    {
        _curl = curl;
        _fileName = fileName;
        _settings = settings;
        _message = message;
        _extraInfo = extraInfo;
    }
    
    
    static size_t writeResponse(char *buf, size_t size, size_t nmemb, void *pBody) {
        size_t total = size * nmemb;
        
        wxString *body = (wxString*)pBody;
        body->append(wxString::FromUTF8(buf, total));
        
        return total;
    }
    
    virtual bool Close()
    {
        // start post
        struct curl_httppost* post = NULL;
        struct curl_httppost* last = NULL;
        
        auto buf = GetOutputStreamBuffer();
        
        wxString proxy = _settings["proxy"];
        if (!proxy.IsEmpty())
        {
            curl_easy_setopt(_curl, CURLOPT_PROXY, proxy.mb_str().data());
        }
        
        
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, wxString::Format("Authorization: Client-ID %s", _settings["clientId"]).mb_str().data());
        headers = curl_slist_append(headers, "Content-Type: multipart/form-data");
        
        curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, headers);
        
        curl_formadd(&post,
                     &last,
                     CURLFORM_COPYNAME, "image",
                     CURLFORM_BUFFER, _fileName.mb_str().data(),
                     CURLFORM_BUFFERPTR, buf->GetBufferStart(),
                     CURLFORM_BUFFERLENGTH, buf->GetBufferSize(),
                     CURLFORM_END);
        
        curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(_curl, CURLOPT_HTTPPOST, post);
        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &writeResponse);
        
        wxString res;
        curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &res);
        
        *_message = "";
        CURLcode code = curl_easy_perform(_curl);
        if (code != CURLE_OK) {
            *_message = wxString::Format("upload to imgur.com fail, curl code: %d", code);
        } else {
            if (res[0] != '<')
            {
                *_message = "Seems like your ip has been banned! You may need to use a Proxy. ";
            }
            else
            {
                wxStringInputStream stream(res);
                wxXmlDocument xml(stream);
                if (xml.IsOk())
                {
                    auto child = xml.GetRoot()->GetChildren();
                    while (child)
                    {
                        auto name = child->GetName();
                        if (name == "link")
                            (*_extraInfo)["url"] = child->GetNodeContent();
                        else if (name == "id")
                            (*_extraInfo)["id"] = child->GetNodeContent();
                        else if (name == "deletehash")
                            (*_extraInfo)["deletehash"] = child->GetNodeContent();
                        
                        child = child->GetNext();
                    }
                }
                else
                {
                    *_message = "Fail to parse response";
                }
            }
        }
        
//        curl_slist_free_all(headers);
        curl_easy_cleanup(_curl);
        _curl = NULL;
        return wxMemoryOutputStream::Close();
    }
};
#endif
