#ifndef H_PROTOCOLS_FTPOS
#define H_PROTOCOLS_FTPOS

#include <wx/wx.h>
#include <wx/mstream.h>
#include <wx/xml/xml.h>
#include <curl/curl.h>
#include <map>
#include "../request.cpp"

using namespace std;

class FtpOutputStream : wxMemoryOutputStream
{
private:
    wxString _remoteName;
    wxString *_message;
    map<wxString, wxString> _settings;
    map<wxString, wxString> *_extraInfo;

public:
    FtpOutputStream(const wxString &remoteName, const map<wxString, wxString> &settings, wxString *message, map<wxString, wxString> *extraInfo)
    {
        _remoteName = remoteName;
        _settings = settings;
        _message = message;
        _extraInfo = extraInfo;
    }


    virtual bool Close()
    {
        *_message = "";
        
        
        /* BEGIN comprise remotePath */
        wxString remotePath = _settings["path"];
        if (remotePath.Last() != '/') remotePath.Append('/');
        if (_settings["dateFolder"] == "true") {
            (*_extraInfo)["folder"] = wxDateTime::Today().Format("%F");
            remotePath += (*_extraInfo)["folder"] + "/";
        }
        remotePath.Append(_remoteName);
        /* END comprise remotePath */

        
        wxString url = wxString::Format("ftp://%s:%s@%s:%s/%s",
                                        _settings["user"],
                                        _settings["pass"],
                                        _settings["host"],
                                        _settings["port"],
                                        remotePath);
        wxLogDebug(url);
        auto ftpput = TheRequestFactory.NewFtpPut(url);
        
        ftpput->SetBuffer(GetOutputStreamBuffer());

        int code = ftpput->Execute();
        if (!code)
        {
            *_message = ftpput->GetLastError();
        }

        delete ftpput;

        return wxMemoryOutputStream::Close();
    }
};
#endif
