#ifndef H_PROTOCOLS_IMGUROS
#define H_PROTOCOLS_IMGUROS

#include <wx/wx.h>
#include <wx/mstream.h>
#include <wx/xml/xml.h>
#include <curl/curl.h>
#include <map>
#include "../request.cpp"

using namespace std;

class ImgurOutputStream : wxMemoryOutputStream
{
private:
    wxString _fileName;
    wxString *_message;
    map<wxString, wxString> _settings;
    map<wxString, wxString> *_extraInfo;
    
public:
    ImgurOutputStream(const wxString &fileName, const map<wxString, wxString> &settings, wxString *message, map<wxString, wxString> *extraInfo)
    {
        _fileName = fileName;
        _settings = settings;
        _message = message;
        _extraInfo = extraInfo;
    }
    
    
    virtual bool Close()
    {
        *_message = "";
        auto multipart = TheRequestFactory.NewMultipart("https://api.imgur.com/3/image.xml");
        multipart->SetProxy(_settings["proxy"]);
        multipart->AddHeader("Authorization", "Client-ID " + _settings["clientId"]);
        multipart->AddField("image", _fileName, GetOutputStreamBuffer());
        
        if (multipart->Execute())
        {
            wxXmlDocument xml;
            if (multipart->FillXml(xml))
            {
                auto child = xml.GetRoot()->GetChildren();
                bool success = false;
                while (child)
                {
                    auto name = child->GetName();
                    if (name == "link")
                    {
                        success = true;
                        (*_extraInfo)["url"] = child->GetNodeContent();
                    }
                    else if (name == "id")
                        (*_extraInfo)["id"] = child->GetNodeContent();
                    else if (name == "deletehash")
                        (*_extraInfo)["deletehash"] = child->GetNodeContent();
                    
                    child = child->GetNext();
                }
                if (!success)
                    *_message = "Upload to imgur.com fail: \n" + multipart->GetResponseText();
            }
            else
            {
                *_message = "Seems like your ip has been banned! You may need to use a Proxy. ";
            }
        }
        else
        {
            *_message = multipart->GetLastError();
        }
        
        delete multipart;
        
        return wxMemoryOutputStream::Close();
    }
};
#endif
