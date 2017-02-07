#ifndef H_CORE_FILE
#define H_CORE_FILE

#include <wx/wx.h>
#include <wx/regex.h>
#include <wx/wfstream.h>
#include <wx/quantize.h>
#include <map>
#include "config.cpp"
#include "func.cpp"

#define MAX_FILE_ID 4000

using namespace std;

const static wxRegEx IMG_PATTERN("\\.(png|jpg|jpeg|gif)$", wxRE_ICASE);

enum FileStatus
{
    PENDING,
    UPLOADING,
    UPLOADED
};

/**
 * Entity to represent a pending/uploaded file/data
 */
class File
{
private:
    wxImage _imgOriginal;
    wxBitmap _bmpThumbnail;
    wxString _originalPath, _thumbnailPath, _name, _remoteName, _ext, _siteId,  _fileName;
    wxDateTime _uploadedAt;
    FileStatus _status;
    map<wxString, wxString> _extraInfo;
    bool _isImage = false;
    wxBitmapType _type;
    
public:
    File() {}
    // initialized by Bitmap from Clipboard
    File(wxBitmap &original)
    {
        _imgOriginal = original.ConvertToImage();
        _type = wxBITMAP_TYPE_JPEG;
        _fileName = _name = "Copied";
        _ext = "jpg";
        _isImage = true;
        _imgOriginal.SetType(wxBITMAP_TYPE_JPEG);
    }
    
    // initialized by Files from Clipboard or History
    File(wxString &path)
    {
        _originalPath = path;
        _fileName = _name = wxFileNameFromPath(path);
        _ext = path.AfterLast('.').Lower();
        _isImage = IMG_PATTERN.Matches(path);
#ifndef _WIN32
        if (_name.Length() > 10)
        {
            _name = _name.SubString(0, 5) + "...." + _ext;
        }
#endif
    }
    
    wxImage &GetOriginal()
    {
        if (!_imgOriginal.IsOk() && _isImage && !_originalPath.IsEmpty())
        {
            wxLogNull suppress;
            _imgOriginal.LoadFile(_originalPath);
            _type = _imgOriginal.GetType();
        }
        return _imgOriginal;
    }
    
    void SetOriginal(const wxImage &img)
    {
        _imgOriginal = img;
    }
    
    wxBitmapType GetType()
    {
        return _type;
    }
    
    wxBitmap &GetThumbnail()
    {
        if (_bmpThumbnail.IsOk() || !_isImage)
            return _bmpThumbnail;
        
        // load cached thumbnail
        if (!_thumbnailPath.IsEmpty())
        {
            _bmpThumbnail = wxBitmap(wxImage(_thumbnailPath));
            return _bmpThumbnail;
        }
        
        wxImage thumbnail = Thumbnailize(GetOriginal(), TheConfig.MaxPreviewWidth, TheConfig.MaxPreviewHeight);
        _bmpThumbnail = wxBitmap(thumbnail);
        return _bmpThumbnail;
    }
    
    static wxImage Thumbnailize(const wxImage &original, const int maxWidth, const int maxHeight)
    {
        int oldWidth = original.GetWidth();
        int oldHeight = original.GetHeight();
        
        if (oldWidth <= maxWidth && oldHeight <= maxHeight)
        {
            return original;
        }
        
        float maxRatio = (float)maxWidth / (float)maxHeight;
        float oldRatio = (float)oldWidth / (float)oldHeight;
        
        int newWidth, newHeight;
        if (oldRatio > maxRatio)
        {
            newWidth = maxWidth;
            newHeight = (int)(newWidth / oldRatio);
        }
        else
        {
            newHeight = maxHeight;
            newWidth = (int)(newHeight * oldRatio);
        }
        return original.Scale(newWidth, newHeight, wxIMAGE_QUALITY_BOX_AVERAGE);
    }
    
    void SetThumbnail(const wxBitmap &bitmap)
    {
        _bmpThumbnail = bitmap;
    }
    
    wxString& GetOriginalPath()
    {
        return _originalPath;
    }
    
    void SetOriginalPath(const wxString &path)
    {
        _originalPath = path;
    }
    
    wxString &GetThumbnailPath()
    {
        return _thumbnailPath;
    }
    
    void SetThumbnailPath(const wxString &path)
    {
        _thumbnailPath = path;
    }
    
    wxString &GetName()
    {
        return _name;
    }
    
    void SetName(const wxString &name)
    {
        _name = name;
    }
    
    wxString &GetFileName()
    {
        return _fileName;
    }
    
    void SetFileName(const wxString &fileName)
    {
        _fileName = fileName;
    }
    
    wxString GetLongName()
    {
        if (!_fileName.IsEmpty())
            return _fileName;
        
        return "[Clipboard]";
    }
    
    wxString &GetExt()
    {
        return _ext;
    }
    
    void SetExt(const wxString &ext)
    {
        _ext = ext;
    }
    
    bool HasThumbnail()
    {
        return _isImage;
    }
    
    bool IsImage()
    {
        return _isImage;
    }
    
    void IsImage(bool isImage)
    {
        _isImage = isImage;
    }
    
    void SetRemoteName(const wxString &remoteName)
    {
        _remoteName = remoteName;
    }
    
    wxString &GetRemoteName()
    {
        if (_remoteName == wxEmptyString)
        {
            _remoteName = Uuid() + "." + _ext;
        }
        
        return _remoteName;
    }
    
    void SetSiteId(const wxString &siteId)
    {
        _siteId = siteId;
    }
    
    wxString &GetSiteId()
    {
        return _siteId;
    }
    
    void SetStatus(const FileStatus status)
    {
        if (status == UPLOADED)
            _uploadedAt = wxDateTime::Now();
        _status = status;
    }
    
    FileStatus GetStatus()
    {
        return _status;
    }
    
    wxDateTime &GetUploadedAt()
    {
        return _uploadedAt;
    }
    
    void SetUploadedAt(const wxDateTime &uploadedAt)
    {
        _uploadedAt = uploadedAt;
    }
    
    map<wxString, wxString> &GetExtraInfo()
    {
        return _extraInfo;
    }
    
    void SetExtraInfo(map<wxString, wxString> extraInfo)
    {
        _extraInfo = extraInfo;
    }
    
    void AddExtraInfo(const wxString &name, const wxString &value)
    {
        _extraInfo[name] = value;
    }
    
    static void SaveImage(const wxBitmap &bmp, const wxBitmapType type, const wxString &path)
    {
        wxOutputStream *stream = new wxFileOutputStream(path);
        SaveImage(bmp.ConvertToImage(), type, stream);
        stream->Close();
        delete stream;
    }
    
    static void SaveImage(const wxImage &img, const wxBitmapType type, wxOutputStream *oStream)
    {
        if (type == wxBITMAP_TYPE_GIF && !img.HasPalette())
        {
            wxImage img2(img.GetSize());
            img2.InitAlpha();
            unsigned char *alpha = img2.GetAlpha();
            memset(alpha, wxIMAGE_ALPHA_TRANSPARENT, img.GetWidth() * img.GetHeight());
            wxQuantize::Quantize(img, img2);
            img2.SaveFile(*oStream, type);
        }
        else
        {
            img.SaveFile(*oStream, type);
        }
    }
};

#endif
