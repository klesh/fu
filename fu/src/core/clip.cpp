#ifndef H_CORE_CLIP
#define H_CORE_CLIP


#include <wx/wx.h>
#include <wx/clipbrd.h>
#include <vector>
#include "../os/os.h"
#include "file.cpp"
#include "config.cpp"

#define TheClip Clip::Inst()

using namespace std;

class Clip
{
private:
    vector<File*> _files;
    
    void Clear()
    {
        for (File* file : _files)
            delete file;
        _files.clear();
    }
    
public:
    
    vector<File*> &GetFiles()
    {
        if (!IsClipboardChanged())
            return _files;
        
        if (!wxTheClipboard->Open())
            Toast("Error", "Fail to access Clipbaord", true);
        
        if (wxTheClipboard->IsSupported(wxDF_FILENAME))
        {
            wxLogDebug("clipboard data format: wxDF_FILENAME");
            Clear();
            wxFileDataObject data;
            if (wxTheClipboard->GetData(data))
            {
                wxArrayString fileNames = data.GetFilenames();
                for (wxString fileName : fileNames)
                {
                    File *file = new File(fileName);
                    _files.push_back(file);
                }
            }
            else
            {
                Toast("Error", "Fail to fetch Clipboard data", true);
            }
        } 
        else if (wxTheClipboard->IsSupported(wxDF_BITMAP))
        {
            wxLogDebug("clipboard data format: wxDF_BITMAP");
            Clear();
            wxBitmapDataObject data;
            if (wxTheClipboard->GetData(data))
            {
                wxBitmap bitmap = data.GetBitmap();
                File *file = new File(bitmap);
                _files.push_back(file);
            }
            else
            {
                Toast("Error", "Fail to fetch Clipboard data", true);
            }
        }
        wxTheClipboard->Close();
       
        return _files;
    }
    
    // prevent deleted by GetFiles
    File* Remove(int index)
    {
        File *file = _files[index];
        _files.erase(remove(_files.begin(), _files.end(), file), _files.end());
        return file;
    }
    
    void Remove(vector<File*> files)
    {
        if (files[0] != _files[0]) // new copying happened
            return;
        
        _files.clear();
    }
    
    void Add(File* file)
    {
        _files.push_back(file);
    }
    
    void SetText(const wxString &text)
    {
        if (!wxTheClipboard->Open())
        {
            Toast("Error", "Fail to open Clipboard", true);
            return;
        }
        wxTheClipboard->SetData(new wxTextDataObject(text));
        wxTheClipboard->Close();
    }
    
    bool SetText(const vector<File*> files)
    {
        wxString formatted = FormatFiles(files);
        if (formatted.IsEmpty())
            return false;
        
        SetText(formatted);
        return true;
    }
    
    wxString FormatFiles(const vector<File*> files)
    {
        auto format = TheConfig.FormatSelected;
        if (format == NULL)
        {
            return wxEmptyString;
        }
        wxString output;
        for (auto const &file : files)
        {
            if (!output.IsEmpty())
                output.Append('\n');
            
            wxString url = GetFileUrl(file);
            output.Append(format->Execute(url));
        }
        return output;
    }
    
    wxString GetFileUrl(File* file)
    {
        Site *site = TheConfig.FindSiteById(file->GetSiteId());
        PtcProvider *provider = ThePtcFactory.Get(site->GetProtocol());
        Ptc *ptc = provider->CreateInstance(site->GetSettings());
        auto url = ptc->FormatUrl(file->GetRemoteName(), file->GetExtraInfo());
        delete ptc;
        return url;
    }
    
    static Clip &Inst()
    {
        static Clip clip;
        return clip;
    }
};

#endif
