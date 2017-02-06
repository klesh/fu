#ifndef H_PROTOCOLS_SFTPOS
#define H_PROTOCOLS_SFTPOS

#include <wx/wx.h>
#include <wx/mstream.h>
#include <map>
#include "../request.cpp"

using namespace std;

class SftpOutputStream : wxMemoryOutputStream
{
private:
    map<wxString, wxString> _settings;
    wxString _remoteName;
    wxString *_message;

    
public:
    SftpOutputStream(map<wxString, wxString> &settings, const wxString &remoteName, wxString *message)
    {
        _settings = settings;
        _remoteName = remoteName;
        _message = message;
    }
    
    virtual bool Close()
    {
        *_message = "";

        wxString host = _settings["host"];
        wxString port = _settings["port"];
        wxString user = _settings["user"];
        wxString pass = _settings["pass"];
        wxString key = _settings["key"];
        wxString path = _settings["path"];

        wxString url = "sftp://" + user;
        if (!pass.IsEmpty() && key.IsEmpty())
        {
            url.Append(':').Append(pass);
        }
        url.Append("@").Append(host);
        if (!port.IsEmpty() && port != "22")
        {
            url.Append(":").Append(port);
        }

        wxString fullPath = path;
        if (fullPath.Last() != '/') fullPath.Append('/');
        fullPath.Append(_remoteName);

        if (fullPath[0] != '/') fullPath.Prepend('/');
        url.Append(fullPath);

        wxLogDebug("url: %s", url);
        auto sftpPut = TheRequestFactory.NewSftpPut(url);
        if (!key.IsEmpty())
        {
            sftpPut->AuthByKey();
            sftpPut->SetPrivateKeyPath(key);
            sftpPut->SetPublicKeyPath(key + ".pub");

            if (!pass.IsEmpty())
                sftpPut->SetPrivateKeyPass(pass);
        }
        else
        {
            sftpPut->AuthByPass();
        }
        sftpPut->SetBuffer(GetOutputStreamBuffer());

        

        if (!sftpPut->Execute())
        {
            *_message = sftpPut->GetLastError();
        }

        delete sftpPut;

        return wxMemoryOutputStream::Close();
    }

};
#endif
