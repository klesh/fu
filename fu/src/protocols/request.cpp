#ifndef H_PROTOCOLS_REQUEST
#define H_PROTOCOLS_REQUEST

#include <wx/wx.h>
#include <curl/curl.h>
#include <wx/xml/xml.h>
#include <wx/sstream.h>
#include <map>

#ifdef _WIN32
#include <wx/stdpaths.h>
#include <wx/filename.h>
#endif

#define TheRequestFactory RequestFactory::Inst()

using namespace std;

class Request
{
protected:
    CURL* _curl = NULL;
    wxString _url;
    struct curl_slist *_headers = NULL;
    wxString _responseText;
    wxString _errorMessage;
    int _statusCode;
    
public:
    Request()
    {
        _curl = curl_easy_init();
    }

    Request(const wxString &url) : Request()
    {
        _url = url;

#ifdef _WIN32
        static wxString executeDir = wxFileName(wxStandardPaths::Get().GetExecutablePath()).GetPath();
        static wxString caPath = wxFileName(executeDir, "cacert.pem").GetFullPath();

        if (_url.StartsWith("https://"))
        {
            SetCAPath(caPath);
        }
#endif // _WIN32
    }
    
    ~Request()
    {
        if (_headers != NULL)
        {
            curl_slist_free_all(_headers);
            _headers = NULL;
        }
        
        if (_curl != NULL)
        {
            curl_easy_cleanup(_curl);
            _curl = NULL;
        }
    }
    
    virtual bool IsOk()
    {
        return _curl != NULL;
    }
    
    virtual void SetProxy(const wxString &proxy)
    {
        if (proxy.IsEmpty())
            return;
        
        curl_easy_setopt(_curl, CURLOPT_PROXY, proxy.mb_str().data());
    }
    
    virtual void AddHeader(const wxString &name, const wxString &value)
    {
        _headers = curl_slist_append(_headers, wxString::Format("%s: %s", name, value).mb_str().data());
    }
    
    virtual void SetCAPath(const wxString &caPath)
    {
        curl_easy_setopt(_curl, CURLOPT_CAINFO, caPath.mb_str().data());
    }
    
    static size_t writeResponse(char *buf, size_t size, size_t nmemb, void *pBody) {
        size_t total = size * nmemb;
        
        wxString *body = (wxString*)pBody;
        body->append(wxString::FromUTF8(buf, total));
        
        return total;
    }
    
    virtual const bool Execute()
    {
        _responseText = "";
        _errorMessage = "";
        _statusCode = 0;
        
        if (_headers != NULL)
            curl_easy_setopt(_curl, CURLOPT_HTTPHEADER, _headers);

        curl_easy_setopt(_curl, CURLOPT_URL, _url.mb_str().data());

        
        curl_easy_setopt(_curl, CURLOPT_WRITEFUNCTION, &writeResponse);
        curl_easy_setopt(_curl, CURLOPT_VERBOSE, 1L);
        
        wxString res;
        curl_easy_setopt(_curl, CURLOPT_WRITEDATA, &res);
        CURLcode rc = curl_easy_perform(_curl);
        _responseText = res;
        
        if (rc == CURLE_OK)
        {
            curl_easy_getinfo(_curl, CURLINFO_RESPONSE_CODE, &_statusCode);
            return true;
        }
        else
        {
            _errorMessage = curl_easy_strerror(rc);
        }
        return false;
    }
    
    virtual const int GetStatusCode()
    {
        return _statusCode;
    }
    
    virtual const wxString &GetResponseText()
    {
        return _responseText;
    }
    
    virtual const wxString &GetLastError()
    {
        return _errorMessage;
    }
    
    virtual bool FillXml(wxXmlDocument &xml)
    {
        if (_responseText.IsEmpty())
        {
            _errorMessage = "response body is empty";
            return false;
        }
        
        if (_responseText[0] != '<')
        {
            _errorMessage = "invalid xml format";
            return false;
        }
        
        wxStringInputStream stream(_responseText);
        xml.Load(stream);
        
        if (!xml.IsOk())
        {
            _errorMessage = "empty xml";
            return false;
        }
        
        return true;
    }
};

class PostRequest : public Request
{
public:
    PostRequest(const wxString &url) : Request(url) {}
    
    virtual void SetJSON(const wxString &json)
    {
        AddHeader("Content-Type", "application/json");
        curl_easy_setopt(_curl, CURLOPT_POSTFIELDS, json.mb_str().data());
    }
};

class MultipartRequest : public Request
{
protected:
    struct curl_httppost* _post = NULL;
    struct curl_httppost* _last = NULL;
    
public:
    MultipartRequest(const wxString &url) : Request(url) {}
    
    virtual void AddField(const wxString &fieldName, const wxString &fileName, wxStreamBuffer *buf)
    {
        curl_formadd(&_post,
                     &_last,
                     CURLFORM_COPYNAME, fieldName.mb_str().data(),
                     CURLFORM_BUFFER, fileName.mb_str().data(),
                     CURLFORM_BUFFERPTR, buf->GetBufferStart(),
                     CURLFORM_BUFFERLENGTH, buf->GetBufferSize(),
                     CURLFORM_END);
    }
    
    virtual void AddField(const wxString &fieldName, const wxString &fieldValue)
    {
        curl_formadd(&_post,
                     &_last,
                     CURLFORM_COPYNAME, fieldName.mb_str().data(),
                     CURLFORM_COPYCONTENTS, fieldValue.mb_str().data(),
                     CURLFORM_END);
    }
    
    virtual const bool Execute()
    {
        if (_post == NULL)
        {
            _errorMessage = "nothing to post";
            return false;
        }
        
        curl_easy_setopt(_curl, CURLOPT_HTTPPOST, _post);
        return Request::Execute();
    }
};

class SftpPutRequest : public Request
{
private:
    wxStreamBuffer *_buffer = NULL;

public:
    SftpPutRequest(const wxString &url) : Request(url) {}

    virtual void AuthByKey()
    {
        curl_easy_setopt(_curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PUBLICKEY);
    }

    virtual void AuthByPass()
    {
        curl_easy_setopt(_curl, CURLOPT_SSH_AUTH_TYPES, CURLSSH_AUTH_PASSWORD);
    }

    virtual void SetPrivateKeyPath(const wxString &path)
    {
        curl_easy_setopt(_curl, CURLOPT_SSH_PRIVATE_KEYFILE, path.mb_str().data());
    }

    virtual void SetPublicKeyPath(const wxString &path)
    {
        curl_easy_setopt(_curl, CURLOPT_SSH_PUBLIC_KEYFILE, path.mb_str().data());
    }

    virtual void SetPrivateKeyPass(const wxString &pass)
    {
        curl_easy_setopt(_curl, CURLOPT_KEYPASSWD, pass.mb_str().data());
    }

    virtual void SetBuffer(wxStreamBuffer *buf)
    {
        _buffer = buf;
    }

    static size_t readData(char *buffer, size_t size, size_t nitems, void *instream)
    {
        auto buf = (wxStreamBuffer*)instream;
        return buf->Read(buffer, size * nitems);
    }

    virtual const bool Execute()
    {
        if (_buffer == NULL)
        {
            _errorMessage = "nothing to upload";
            return false;
        }
            

        curl_easy_setopt(_curl, CURLOPT_PROTOCOLS, CURLPROTO_SFTP);
        curl_easy_setopt(_curl, CURLOPT_UPLOAD, 1L);
        curl_easy_setopt(_curl, CURLOPT_READFUNCTION, &readData);
        curl_easy_setopt(_curl, CURLOPT_READDATA, _buffer);
        return Request::Execute();
    }
};

class RequestFactory
{
private:
    RequestFactory()
    {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    }
    
    ~RequestFactory()
    {
        curl_global_cleanup();
    }
    
public:
    Request *NewGet(const wxString &url)
    {
        return new Request(url);
    }
    
    PostRequest *NewPost(const wxString &url)
    {
        return new PostRequest(url);
    }
    
    MultipartRequest *NewMultipart(const wxString &url)
    {
        return new MultipartRequest(url);
    }

    SftpPutRequest *NewSftpPut(const wxString &url)
    {
        return new SftpPutRequest(url);
    }
    
    static RequestFactory &Inst()
    {
        static RequestFactory client;
        return client;
    }
};

#endif
