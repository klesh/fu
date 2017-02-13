#ifndef H_PROTOCOLS_SFTP
#define H_PROTOCOLS_SFTP

#include <wx/wx.h>
#include "../ptc.h"
#include "sftpos.cpp"


class Sftp : public Ptc
{
private:
    int _sock;
    LIBSSH2_SESSION *_ssh = NULL;
    LIBSSH2_SFTP *_sftp = NULL;

public:
    Sftp(map<wxString, wxString> settings) : Ptc(settings)
    {
    }
    
    bool Connect() 
    {
        wxString host = _settings["host"];
        wxString port = _settings["port"];
        wxString user = _settings["user"];
        wxString pass = _settings["pass"];
        wxString key = _settings["key"];
        wxString path = _settings["path"];
       
        
#ifdef WIN32
        WSADATA wsadata;
        int err;
     
        err = WSAStartup(MAKEWORD(2,0), &wsadata);
        if (err != 0) {
            fprintf(stderr, "WSAStartup failed with error: %d\n", err);
            return 1;
        }
#endif


        char hostname[host.size() + 1];
        strcpy(hostname, host.mb_str().data());
        auto he = gethostbyname(hostname);
        if (he == NULL) 
        {
            _message = wxString::Format("Unable to resolve hostname: %s", host);
            return false;
        }

        auto addr_list = (struct in_addr **) he->h_addr_list;
        auto hostaddr = *addr_list[0];
        wxLogDebug("hostaddr: %s", inet_ntoa(hostaddr));

        _sock = socket(he->h_addrtype, SOCK_STREAM, 0);

        struct sockaddr_in sin;
        sin.sin_family = he->h_addrtype;
        sin.sin_port = htons(wxAtoi(port));
        sin.sin_addr.s_addr = hostaddr.s_addr;
        
        int code;
        code = connect(_sock, (const sockaddr*)(&sin), sizeof(struct sockaddr_in));
        if (code)
        {
            _message = "Unable to connect to server";
            return false;
        }
        
        _ssh = libssh2_session_init();
        
        if (!_ssh)
        {
            _message = "Unable to create SSH session";
            return false;
        }
        
        libssh2_session_set_blocking(_ssh, 1);
        
        code = libssh2_session_handshake(_ssh, _sock);
        if (code)
        {
            _message = "Unable to establish SSH session";
            return false;
        }
        
        if (key.IsEmpty())
        {
            code = libssh2_userauth_password(_ssh, user.mb_str().data(), pass.mb_str().data());
        }
        else
        {
            wxString pubKey = key + ".pub";
            code = libssh2_userauth_publickey_fromfile(_ssh, user.mb_str().data(), pubKey.mb_str().data(), key.mb_str().data(), pass.mb_str().data());
        }
        
        if (code)
        {
            _message = "Authentication failed";
            Disconnect();
            return false;
        }
        
        _sftp = libssh2_sftp_init(_ssh);
        
        if (!_sftp)
        {
            _message = "Unable to init SFTP session";
            Disconnect();
            return false;
        }
        
        return true;
    }
    
    void Disconnect()
    {
        if (_sftp)
        {
            libssh2_sftp_shutdown(_sftp);
        }
        if (_ssh)
        {
            libssh2_session_disconnect(_ssh, "shut down ssh");
            libssh2_session_free(_ssh);
        }

#ifdef WIN32
        closesocket(_sock);
#else
        close(_sock);
#endif
        libssh2_exit();
    }
        
protected:
    
    virtual wxOutputStream *OnOpenStream(const wxString &fileName, const wxString &remoteName)
    {
        if (!_message.IsEmpty())
            return NULL;
                        
        SftpOutputStream *stream = new SftpOutputStream(_settings, remoteName, &_message, _sftp);
        return (wxOutputStream*)stream;
    }
};

#endif
