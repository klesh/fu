#ifndef H_SFTPWRAPPER_H
#define H_SFTPWRAPPER_H

/**
 * These wrapper classes are aim to resolve the lisssh2 and wxwin typdef conflict (SSIZE_T)
 */

class SftpWrapperStream
{
public:
    SftpWrapperStream(const char *path, void *sftp);
    size_t Write(const void *buffer, size_t bufsize);
    bool IsOk();
    void Close();
    const char *GetErrorMessage();
private:
    void *_stream;       /* LIBSSH2_SFTP_HANDLE  */
    char *_errorMessage;
};

class SftpWrapper
{
public:
    SftpWrapper();
    ~SftpWrapper();
    bool Connect(const char *host, const char *port, const char *user, const char *pass, const char *key, const char *path);
    void Disconnect();
    SftpWrapperStream *Open(const char *path);
    const char *GetErrorMessage();

private:
    int _sock;
    void* _ssh;          /* LIBSSH2_SESSION */
    void* _sftp;         /* LIBSSH2_SFTP */
    char *_errorMessage;
};
#endif