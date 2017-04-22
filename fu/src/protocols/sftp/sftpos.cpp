#ifndef H_PROTOCOLS_SFTPOS
#define H_PROTOCOLS_SFTPOS

#include <wx/wx.h>
#include <wx/stream.h>
#include <map>
#include "SftpWrapper.h"

using namespace std;

class SftpOutputStream : wxOutputStream
{
private:
    SftpWrapperStream *_sftpStream = NULL;

protected:
    size_t OnSysWrite(const void *buffer, size_t bufsize)
    {
        return _sftpStream->Write(buffer, bufsize);
    }


public:
    SftpOutputStream(map<wxString, wxString> &settings, const wxString &remoteName, wxString *message, SftpWrapper *sftp, map<wxString, wxString> *extraInfo)
    {
        wxString remotePath = settings["path"];
        if (remotePath.Last() != '/') remotePath.Append('/');
        if (settings["dateFolder"] == "true") {
            (*extraInfo)["folder"] = wxDateTime::Today().Format("%F");
            remotePath += (*extraInfo)["folder"] + "/";
        }
        remotePath.Append(remoteName);

        _sftpStream = sftp->Open(remotePath.mb_str().data());
    }

    virtual bool IsOK()
    {
        return _sftpStream != NULL;
    }

    virtual bool Close()
    {
        _sftpStream->Close();
        delete _sftpStream;
        _sftpStream = NULL;
        return wxOutputStream::Close();
    }

};
#endif
