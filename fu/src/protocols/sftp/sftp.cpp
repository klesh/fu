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
    ssh_session _session = NULL;
    sftp_session _sftp = NULL;
    bool _connected = false;
    
public:
    Sftp(map<wxString, wxString> settings) : Ptc(settings)
    {
    }
    
    bool Connect()
    {
        _session = ssh_new();
        int rc;
        
        if (_session == NULL)
            return false;
        
        unsigned int port = wxAtoi(_settings["port"]);
        const char *puser = _settings["user"].mb_str().data();
        const char *ppass = _settings["pass"].mb_str().data();
        ssh_options_set(_session, SSH_OPTIONS_HOST, _settings["host"].mb_str().data());
        ssh_options_set(_session, SSH_OPTIONS_PORT, &port);
        ssh_options_set(_session, SSH_OPTIONS_USER, puser);
        
        wxString keyPath = _settings["key"];
        
        bool authByPass = keyPath.IsEmpty();
        
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
            rc = ssh_userauth_password(_session, NULL, ppass);
            
            if (rc != SSH_AUTH_SUCCESS)
            {
                _message = wxString::Format("Error authenticating with password: %s\n", ssh_get_error(_session));
                Disconnect();
                return false;
            }
        }
        else
        {
            rc = ssh_userauth_privatekey_file(_session, puser, keyPath.mb_str().data(), ppass);
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
    
protected:
    
    virtual wxOutputStream *OnOpenStream(const wxString &fileName, const wxString &remoteName)
    {
        int accessType = O_WRONLY | O_CREAT | O_TRUNC;
        
        wxString path = _settings["path"];
        
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
};

#endif
