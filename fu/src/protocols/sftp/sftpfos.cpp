#ifndef H_PROTOCOLS_SFTPFOS
#define H_PROTOCOLS_SFTPFOS

#include <wx/wx.h>
#include <wx/stream.h>
#include <libssh/sftp.h>

class SftpFileOutputStream : wxOutputStream
{
private:
    sftp_file _file;
    
public:
    SftpFileOutputStream(sftp_file &file)
    {
        _file = file;
    }
    
    virtual bool Close()
    {
        return sftp_close(_file) == SSH_OK;
    }
    
protected:
    size_t OnSysWrite(const void *buffer, size_t bufsize)
    {
        return sftp_write(_file, buffer, bufsize);
    }
};
#endif
