#ifndef H_PROTOCOLS_SFTP
#define H_PROTOCOLS_SFTP

#include <wx/wx.h>
#include "../ptc.h"
#include "sftpfos.cpp"

#define LIBSSH_STATIC 1
#include <libssh/libssh.h>
#include <libssh/sftp.h>

class Sftp : public Ptc
{
private:
    wxString _host;
    wxString _port;
    wxString _user;
    wxString _pass;
    wxString _keyPath;
    wxString _remotePath;
    ssh_session _session = NULL;
    sftp_session _sftp = NULL;
    bool _connected = false;
    wxString _message;
    
public:
    
    void SetHost(const wxString &host)
    {
        _host = host;
    }
    
    void SetPort(const wxString &port)
    {
        _port = port;
    }
    
    void SetUser(const wxString &user)
    {
        _user = user;
    }
    
    void SetPass(const wxString &pass)
    {
        _pass = pass;
    }
    
    void SetKeyPath(const wxString &keyPath)
    {
        _keyPath = keyPath;
    }
    
    void SetRemotePath(const wxString &remotePath)
    {
        _remotePath = remotePath;
    }
    
    bool Connect()
    {
        _session = ssh_new();
        int rc;
        
        if (_session == NULL)
            return false;
        
        unsigned int port = wxAtoi(_port);
        ssh_options_set(_session, SSH_OPTIONS_HOST, _host.mb_str().data());
        ssh_options_set(_session, SSH_OPTIONS_PORT, &port);
        ssh_options_set(_session, SSH_OPTIONS_USER, _user.mb_str().data());
        
        bool authByPass = _keyPath.IsEmpty();
        
        rc = ssh_connect(_session);
        if (rc != SSH_OK)
        {
            _message = wxString::Format("Error connecting to localhost: %s\n", ssh_get_error(_session));
            Disconnect();
            return false;
        }
        
        _connected = true;
        
        if (authByPass)
        {
            rc = ssh_userauth_password(_session, NULL, _pass.mb_str().data());
            
            if (rc != SSH_AUTH_SUCCESS)
            {
                _message = wxString::Format("Error authenticating with password: %s\n", ssh_get_error(_session));
                Disconnect();
                return false;
            }
        }
        else
        {
            rc = ssh_userauth_privatekey_file(_session, _user.mb_str().data(), _keyPath.mb_str().data(), _pass.mb_str().data());
            if (rc != SSH_AUTH_SUCCESS)
            {
                _message = wxString::Format("Error public key authentication: %d\n", ssh_get_error(_session));
                Disconnect();
                return false;
            }
        }
        
        _sftp = sftp_new(_session);
        if (_sftp == NULL)
        {
            _message = wxString::Format("Error allocating SFTP session: %s\n", ssh_get_error(_session));
            Disconnect();
            return false;
        }
        
        rc = sftp_init(_sftp);
        if (rc != SSH_OK)
        {
            _message = wxString::Format("Error initializing SFTP session: %s.\n", ssh_get_error(_session));
            Disconnect();
            return false;
        }
        
        return true;
    }
    
    void Disconnect()
    {
        if (_sftp != NULL)
        {
            sftp_free(_sftp);
            _sftp = NULL;
        }
        if (_connected)
        {
            ssh_disconnect(_session);
            _connected = false;
        }
        if (_session != NULL)
        {
            ssh_free(_session);
            _session = NULL;
        }
    }
    
    ~Sftp()
    {
        Disconnect();
    }
    
    wxOutputStream *OpenStream(const wxString &remoteName)
    {
        int accessType = O_WRONLY | O_CREAT | O_TRUNC;
        
        wxString path = _remotePath;
        
        if (!path.IsEmpty())
            if (path.Last() != '/') path = path + "/";
        
        path = path + remoteName;
        sftp_file file = sftp_open(_sftp, path, accessType, S_IRWXU);
        if (file == NULL)
        {
            _message = wxString::Format( "Can't open file for writing: %s\n", ssh_get_error(_session));
            return NULL;
        }
        
        SftpFileOutputStream *stream = new SftpFileOutputStream(file);
        return (wxOutputStream*)stream;
    }
    
    const wxString &GetErrorMessage()
    {
        return _message;
    }
};

#endif
