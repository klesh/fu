#ifndef H_CORE_UPLOADER
#define H_CORE_UPLOADER
#include <wx/wx.h>
#include <vector>
#include <map>
#include <wx/graphics.h>
#include "../protocols/ptc.h"
#include "../protocols/ptcp.h"
#include "../protocols/ptcf.h"
#include "file.cpp"
#include "site.cpp"
#include "format.cpp"
#include "clip.cpp"

wxDEFINE_EVENT(EVT_UPLOAD_SUCCESS, wxCommandEvent);
wxDEFINE_EVENT(EVT_UPLOAD_ERROR, wxCommandEvent);
wxDEFINE_EVENT(EVT_UPLOAD_START, wxCommandEvent);
wxDEFINE_EVENT(EVT_UPLOAD_END, wxCommandEvent);

// TODO: remove after development.
//#include "../protocols/local/local.cpp"


using namespace std;

class Uploader : public wxThread
{
private:
    wxEvtHandler *_handler;
    vector<File*> _files;
    
public:
    Uploader(wxEvtHandler *handler, vector<File*> files)
    {
        _handler = handler;
        _files = files;
    }
    
    ~Uploader()
    {
    }
    
protected:
    
    virtual ExitCode Entry()
    {
        Site *site = TheConfig.SiteSelected;
        if (site == NULL)
            return EmitError("None of site is selected");
        
        wxString siteId = site->GetId();
        
        EmitStart();
        
        auto ptcProvider = ThePtcFactory.Get(site->GetProtocol());
        
        if (ptcProvider->IsImageOnly())
        {
            for (auto &file : _files)
            {
                if (!file->IsImage())
                    return EmitError("Selected site support image format only.");
            }
        }
        auto ptc = ptcProvider->CreateInstance(site->GetSettings());
        if (ptc->Connect() == false)
        {
            return EmitError(wxString::Format("Connect to server fail: %s", ptc->GetErrorMessage()));
        }
        
        wxImage watermark;
        if (TheConfig.EnableWatermark && !TheConfig.WatermarkPath.IsEmpty())
        {
            watermark.LoadFile(TheConfig.WatermarkPath);
        }
        bool imgPreproc = watermark.IsOk() || TheConfig.EnableReduceSize;
        
        for (auto &file : _files)
        {
            auto oStream = ptc->OpenStream(file->GetFileName(), file->GetRemoteName());
            if (oStream == NULL)
                return EmitError(wxString::Format("Fail to open remote file.\n%s", ptc->GetErrorMessage()));
            
            file->SetStatus(UPLOADING);
            
            ProcessFile(file, imgPreproc, watermark, oStream);
 
            oStream->Close();
            
            delete oStream;
            
            auto message = ptc->GetErrorMessage();
            if (!message.IsEmpty())
            {
                file->SetStatus(PENDING);
                EmitError(wxString::Format("Upload fail: %s", message));
                ptc->Disconnect();
                delete ptc;
                return (ExitCode)1;
            }
            else
            {
                file->SetSiteId(siteId);
                file->SetStatus(UPLOADED);
                file->SetExtraInfo(ptc->GetLastExtraInfo());
            }
        }
        
        ptc->Disconnect();
        delete ptc;
        
        wxString output = TheClip.FormatFiles(_files);
        
        return EmitSuccess(output);
    }
    
    void ProcessFile(File *file, bool imgPreproc, wxImage &watermark, wxOutputStream *oStream)
    {
        if (file->IsImage() && imgPreproc)
        {
            wxImage bg = file->GetOriginal();
            wxBitmapType type = bg.GetType();
            if (watermark.IsOk())
            {
                wxImage wm = File::Thumbnailize(watermark, bg.GetWidth() / 2, bg.GetHeight() / 2);
                int pos = TheConfig.WatermarkPosition;
                int x = pos == 3 || pos == 1 ? bg.GetWidth() - wm.GetWidth() : 0;
                int y = pos == 3 || pos == 2 ? bg.GetHeight() - wm.GetHeight() : 0;
                bool isGif = type == wxBITMAP_TYPE_GIF;
                
                if (isGif && wxImage::GetImageCount(file->GetOriginalPath()) > 1)
                {
                    wxLogDebug("== animated gif");
                    return;
                }
                
                bg = Composite(bg, wm, x, y);
            }
            if (TheConfig.EnableReduceSize)
            {
                if (type == wxBITMAP_TYPE_JPEG)
                {
                    bg.SetOption(wxIMAGE_OPTION_QUALITY, TheConfig.JPGQuality);
                }
                else if (type == wxBITMAP_TYPE_PNG)
                {
                    bg.SetOption(wxIMAGE_OPTION_PNG_BITDEPTH, TheConfig.PNGDepth);
                    bg.SetOption(wxIMAGE_OPTION_PNG_COMPRESSION_LEVEL, TheConfig.PNGCompressionLevel);
                }
            }
            File::SaveImage(bg, type, oStream);
            file->GetOriginal().Destroy();
            bg.Destroy();
        }
        else
        {
            bool isDiskfile = !file->GetOriginalPath().IsEmpty();
            if (isDiskfile)
            {
                wxFileInputStream iStream(file->GetOriginalPath());
                iStream.Read(*oStream);
            }
            else
            {
                wxImage bg = file->GetOriginal();
                bg.SaveFile(*oStream, wxBITMAP_TYPE_JPEG); // clipboard bitmap should always be saved as jpg
                bg.Destroy();
            }
        }
    }
    
    wxImage Composite(const wxImage &bg, const wxImage &wm, const int x, const int y)
    {
        wxImage img(bg.GetSize());
        img.InitAlpha();
        unsigned char *alpha = img.GetAlpha();
        memset(alpha, wxIMAGE_ALPHA_TRANSPARENT, bg.GetWidth() * bg.GetHeight());
        
        wxGraphicsContext *gc = wxGraphicsContext::Create(img);
        gc->DrawBitmap(wxBitmap(bg), 0, 0, bg.GetWidth(), bg.GetHeight());
        gc->DrawBitmap(wxBitmap(wm), x, y, wm.GetWidth(), wm.GetHeight());
        delete gc;
 
        return img;
    }
    
    ExitCode EmitError(const wxString &message)
    {
        wxCommandEvent evt(EVT_UPLOAD_ERROR);
        evt.SetString(message);
        wxPostEvent(_handler, evt);
        EmitEnd();
        return (ExitCode)1;
    }
    
    ExitCode EmitSuccess(const wxString &text)
    {
        wxCommandEvent evt(EVT_UPLOAD_SUCCESS);
        evt.SetString(text);
        wxPostEvent(_handler, evt);
        
        EmitEnd();
        return 0;
    }
    
    void EmitStart()
    {
        wxCommandEvent start(EVT_UPLOAD_START);
        wxPostEvent(_handler, start);
    }
    
    void EmitEnd()
    {
        wxCommandEvent end(EVT_UPLOAD_END);
        wxPostEvent(_handler, end);
    }
};

#endif
