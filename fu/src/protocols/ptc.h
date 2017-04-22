#ifndef H_PROTOCOL
#define H_PROTOCOL

#include <vector>
#include <wx/wx.h>
#include <wx/stream.h>
#include <map>

using namespace std;

class Ptc
{
protected:
    map<wxString, wxString> _settings;
    wxString _message;
    map<wxString, wxString> _lastExtraInfo;
    virtual wxOutputStream *OnOpenStream(const wxString &fileName, const wxString &remoteName) = 0;

public:
    Ptc(map<wxString, wxString> &settings)
    {
        _settings = settings;
    }

    virtual ~Ptc()
    {
        Disconnect();
    }

    virtual bool Connect()
    {
        return true;
    }

    virtual void Disconnect()
    {
    }


    virtual const wxString &GetErrorMessage()
    {
        return _message;
    };

    virtual const wxString FormatUrl(const wxString &remoteName, map<wxString, wxString> &extraInfo)
    {
        if (extraInfo.find("folder") != extraInfo.end()) {
            return wxString::Format(_settings["urlFormat"], extraInfo["folder"] + "/" + remoteName);
        }
        return wxString::Format(_settings["urlFormat"], remoteName);
    }

    virtual const map<wxString, wxString> &GetLastExtraInfo()
    {
        return _lastExtraInfo;
    }

    wxOutputStream *OpenStream(const wxString &fileName, const wxString &remoteName)
    {
        return OnOpenStream(fileName, remoteName);
    }
};

#endif
