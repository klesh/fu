#ifndef H_PROTOCOLS_SMMSOS
#define H_PROTOCOLS_SMMSOS

#include <wx/wx.h>
#include <wx/mstream.h>
#include <curl/curl.h>
#include <map>
#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include "../request.cpp"

using namespace std;

class SmmsOutputStream : wxMemoryOutputStream
{
private:
    wxString _fileName;
    wxString *_message;
    map<wxString, wxString> _settings;
    map<wxString, wxString> *_extraInfo;
    
public:
    SmmsOutputStream(const wxString &fileName, const map<wxString, wxString> &settings, wxString *message, map<wxString, wxString> *extraInfo)
    {
        _fileName = fileName;
        _settings = settings;
        _message = message;
        _extraInfo = extraInfo;
    }
    
    
    virtual bool Close()
    {
        *_message = "";
        auto multipart = TheRequestFactory.NewMultipart("https://sm.ms/api/upload");
        multipart->SetProxy(_settings["proxy"]);
        multipart->AddField("smfile", _fileName, GetOutputStreamBuffer());
        multipart->AddField("format", "xml");
        
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
                    if (name == "url")
                        (*_extraInfo)["url"] = child->GetNodeContent();
                    else if (name == "hash")
                        (*_extraInfo)["hash"] = child->GetNodeContent();
                    else if (name == "delete")
                        (*_extraInfo)["delete"] = child->GetNodeContent();
                    else if (name == "code" && child->GetNodeContent() == "success")
                        success = true;
                    else if (name == "msg")
                        *_message = child->GetNodeContent();
                    
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
