#ifndef H_PROTOCOLS_SFTP
#define H_PROTOCOLS_SFTP

#include <wx/wx.h>
#include "../ptc.h"
#include "sftpos.cpp"
#include "SftpWrapper.h"


class Sftp : public Ptc
{
private:
    int _sock;
    SftpWrapper *_sftp;

public:
    Sftp(map<wxString, wxString> settings) : Ptc(settings)
    {
        _sftp = new SftpWrapper();
    }

    bool Connect()
    {
        const char *host = _settings["host"];
        const char *port = _settings["port"];
        const char *user = _settings["user"];
        const char *pass = _settings["pass"];
        const char *key = _settings["key"];
        const char *path = _settings["path"];

        bool success = _sftp->Connect(host, port, user, pass, key, path);
        if (!success)
            _message = _sftp->GetErrorMessage();
        return success;
    }

    void Disconnect()
    {
        delete _sftp;
        _sftp = NULL;
    }
    

    virtual const wxString FormatUrl(const wxString &remoteName, map<wxString, wxString> &extraInfo)
    {
        // TODO: patch to use folder
        if (extraInfo.find("folder") != extraInfo.end()) {
            return wxString::Format(_settings["urlFormat"], extraInfo["folder"] + "/" + remoteName);
        }
        return wxString::Format(_settings["urlFormat"], remoteName);
    }


protected:

    virtual wxOutputStream *OnOpenStream(const wxString &fileName, const wxString &remoteName)
    {
        if (!_message.IsEmpty())
            return NULL;

        SftpOutputStream *stream = new SftpOutputStream(_settings, remoteName, &_message, _sftp, &_lastExtraInfo);
        return (wxOutputStream*)stream;
    }
};

#endif
