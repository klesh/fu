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
    SftpOutputStream(map<wxString, wxString> &settings, const wxString &remoteName, wxString *message, SftpWrapper *sftp)
    {
        wxString path = settings["path"];
        wxString fullPath = path;
        if (fullPath.Last() != '/') fullPath.Append('/');
        fullPath.Append(remoteName);

        _sftpStream = sftp->Open(fullPath.mb_str().data());
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
