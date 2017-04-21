#ifndef H_UI_COMMON
#define H_UI_COMMON

#ifdef NDEBUG
#define ICONS_PREFIX "/home/klesh/Projects/Study/fu/fu/resources"
#endif

#include <wx/wx.h>
#include <wx/icon.h>


class UICommon
{
protected:
    wxString _iconPath;
    wxString _iconUploadingPath;
    wxIcon _icon;
    wxIcon _iconUploading;

public:
    static UICommon &Get()
    {
        static UICommon inst;
        return inst;
    }
    
    wxString &IconPath()
    {
        return _iconPath;
    }
     
    wxString &IconUploadingPath()
    {
        return _iconUploadingPath;
    }
    
    wxIcon &Icon()
    {
        return _icon;
    }
    
    wxIcon &IconUploading()
    {
        return _iconUploading;
    }
   

private:
    UICommon() 
    {
#ifndef HAVE_APPINDICATOR
#ifdef __WXGTK__
        _iconPath = wxString(ICONS_PREFIX) + "/fu.png";
        _iconUploadingPath = wxString(ICONS_PREFIX) + "/fu_uploading.png";
        wxLogDebug(_iconPath);
#else
        _iconPath = TheConfig.GetIconPath("16x16", "icon.png");
        _iconUploadingPath = TheConfig.GetIconPath("16x16", "icon_uploading.png");
#endif
        _icon.LoadFile(_iconPath, wxBITMAP_TYPE_PNG);
        _iconUploading.LoadFile(_iconUploadingPath, wxBITMAP_TYPE_PNG);
#endif
    }
};

#endif
