#include "SftpWrapper.h"

#include "libssh2_config_win32.h"
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


SftpWrapper::SftpWrapper()
{
    _sock = 0;
    _ssh = NULL;
    _sftp = NULL;
    _errorMessage = NULL;
}


SftpWrapper::~SftpWrapper()
{
    Disconnect();
}

bool SftpWrapper::Connect(const char *host, const char *port, const char *user, const char *pass, const char *key, const char *path)
{
#ifdef _WIN32
    WSADATA wsadata;
    int err;

    err = WSAStartup(MAKEWORD(2, 0), &wsadata);
    if (err != 0) {
        _errorMessage = "WSAStartup failed with error";
        return false;
    }
#endif
    char hostname[1024];
    strcpy(hostname, host);

    auto he = gethostbyname(hostname);
    if (he == NULL)
    {
        _errorMessage = "Unable to resolve hostname";
        return false;
    }

    auto addr_list = (struct in_addr **) he->h_addr_list;
    auto hostaddr = *addr_list[0];

    _sock = socket(he->h_addrtype, SOCK_STREAM, 0);

    struct sockaddr_in sin;
    sin.sin_family = he->h_addrtype;
    sin.sin_port = htons(atoi(port));
    sin.sin_addr.s_addr = hostaddr.s_addr;

    int code;
    code = connect(_sock, (const sockaddr*)(&sin), sizeof(struct sockaddr_in));
    if (code)
    {
        _errorMessage = "Unable to connect to server";
        return false;
    }

    LIBSSH2_SESSION* ssh = libssh2_session_init();

    _ssh = ssh;
    if (!ssh)
    {
        _errorMessage = "Unable to create SSH session";
        return false;
    }

    libssh2_session_set_blocking(ssh, 1);

    code = libssh2_session_handshake(ssh, _sock);
    if (code)
    {
        if (code == LIBSSH2_ERROR_KEX_FAILURE)
            _errorMessage = "Encryption key exchange with the remote host failed";
        else
            _errorMessage = "Unable to establish SSH session";
        Disconnect();
        return false;
    }

    if (strlen(key) == 0)
    {
        code = libssh2_userauth_password(ssh, user, pass);
    }
    else
    {
        char pubKey[1024];
        strcpy(pubKey, key);
        strcat(pubKey, ".pub");
        code = libssh2_userauth_publickey_fromfile(ssh, user, pubKey, key, pass);
    }

    if (code)
    {
        _errorMessage = "Authentication failed";
        Disconnect();
        return false;
    }

    _sftp = libssh2_sftp_init(ssh);

    if (!_sftp)
    {
        _errorMessage = "Unable to init SFTP session";
        Disconnect();
        return false;
    }

    return true;

}

void SftpWrapper::Disconnect()
{
    if (_sftp)
    {
        libssh2_sftp_shutdown((LIBSSH2_SFTP*)_sftp);
    }
    if (_ssh)
    {
        LIBSSH2_SESSION* ssh = (LIBSSH2_SESSION*)_ssh;
        libssh2_session_disconnect(ssh, "shut down ssh");
        libssh2_session_free(ssh);
        _ssh = NULL;
    }

#ifdef _WIN32
    closesocket(_sock);
#else
    close(_sock);
#endif
    libssh2_exit();
}

const char *SftpWrapper::GetErrorMessage()
{
    return _errorMessage;
}

SftpWrapperStream *SftpWrapper::Open(const char *path) 
{
    SftpWrapperStream *stream = new SftpWrapperStream(path, _sftp);
    if (!stream->IsOk())
    {
        delete stream;
        return NULL;
    }
    return stream;
}

SftpWrapperStream::SftpWrapperStream(const char *path, void *sftp)
{
    _stream = libssh2_sftp_open((LIBSSH2_SFTP*)sftp, path,
        LIBSSH2_FXF_WRITE | LIBSSH2_FXF_CREAT | LIBSSH2_FXF_TRUNC,
        LIBSSH2_SFTP_S_IRUSR | LIBSSH2_SFTP_S_IWUSR |
        LIBSSH2_SFTP_S_IRGRP | LIBSSH2_SFTP_S_IROTH);
    _errorMessage = NULL;
}

bool SftpWrapperStream::IsOk()
{
    return _stream != NULL;
}

size_t SftpWrapperStream::Write(const void *buffer, size_t bufsize)
{
    return libssh2_sftp_write((LIBSSH2_SFTP_HANDLE*)_stream, (const char*)buffer, bufsize);
}

void SftpWrapperStream::Close()
{
    libssh2_sftp_close((LIBSSH2_SFTP_HANDLE*)_stream);
}

const char *SftpWrapperStream::GetErrorMessage()
{
    return _errorMessage;
}
