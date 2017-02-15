#ifndef H_PROTOCOLS_SFTPOS
#define H_PROTOCOLS_SFTPOS

#include <wx/wx.h>
#include <wx/stream.h>
#include <map>


#include <libssh2.h>
#include <libssh2_sftp.h>

#ifdef _WIN32
#include <winsock2.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif
#include <string.h>


using namespace std;

class SftpOutputStream : wxOutputStream
{
private:
    LIBSSH2_SFTP_HANDLE *_handle = NULL;

protected:
    size_t OnSysWrite(const void *buffer, size_t bufsize)
    {
        return libssh2_sftp_write(_handle, (const char*)buffer, bufsize);
    }


public:
    SftpOutputStream(map<wxString, wxString> &settings, const wxString &remoteName, wxString *message, LIBSSH2_SFTP *sftp)
    {
        wxString path = settings["path"];
        wxString fullPath = path;
        if (fullPath.Last() != '/') fullPath.Append('/');
        fullPath.Append(remoteName);

        _handle = libssh2_sftp_open(sftp, fullPath.mb_str().data(),
                      LIBSSH2_FXF_WRITE|LIBSSH2_FXF_CREAT|LIBSSH2_FXF_TRUNC,
                      LIBSSH2_SFTP_S_IRUSR|LIBSSH2_SFTP_S_IWUSR|
                      LIBSSH2_SFTP_S_IRGRP|LIBSSH2_SFTP_S_IROTH);
    }

    virtual bool IsOK()
    {
        return _handle != NULL;
    }

    virtual bool Close()
    {
        libssh2_sftp_close(_handle);
        return wxOutputStream::Close();
    }

};
#endif
