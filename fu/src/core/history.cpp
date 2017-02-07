#ifndef H_CORE_HISTORY
#define H_CORE_HISTORY

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>
#include <vector>
#include "file.cpp"

#define TheHistory History::Inst()

wxDEFINE_EVENT(fuEVT_HISTORY_CHANGED, wxCommandEvent);

using namespace std;

class History
{
private:
    wxString _path;
    wxString _thumbsPath;
    vector<File*> _all;
    vector<wxEvtHandler*> _listeners;
    
    History()
    {
        wxString folder = wxStandardPaths::Get().GetUserDataDir();
        if (!wxDirExists(folder))
            wxMkdir(folder);
        
        wxFileName thumbsPath(folder, "thumbnail");
        _thumbsPath = thumbsPath.GetFullPath();
        if (!wxDirExists(_thumbsPath))
            wxMkdir(_thumbsPath);
        
        wxFileName path(folder, "history.xml");
        
        _path = path.GetFullPath();
        
        wxLogDebug("history path: %s", _path);
        if (!wxFileExists(_path))
        {
            return;
        }
        
        wxXmlDocument _xml;
        _xml.Load(_path);
        
        wxXmlNode *child = _xml.GetRoot()->GetChildren();
        while (child) {
            wxString nodeName = child->GetName();
            if (nodeName == "file")
            {
                File* file = new File();
                file->SetOriginalPath(child->GetAttribute("path"));
                file->SetName(child->GetAttribute("name"));
                file->SetFileName(child->GetAttribute("filename"));
                file->SetRemoteName(child->GetAttribute("remoteName"));
                file->SetExt(child->GetAttribute("ext"));
                file->SetSiteId(child->GetAttribute("siteId"));
                file->GetUploadedAt().ParseISOCombined(child->GetAttribute("uploadedAt"));
                file->SetThumbnailPath(GetThumbFullPath(file->GetRemoteName()));
                file->IsImage(child->GetAttribute("isImage") == "true");
                
                auto *extra = child->GetChildren();
                while (extra)
                {
                    file->AddExtraInfo(extra->GetName(), extra->GetNodeContent());
                    extra = extra->GetNext();
                }
                
                _all.push_back(file);
            }
            
            child = child->GetNext();
        }
    }
    
    wxString GetThumbFullPath(const wxString &remoteName)
    {
        wxFileName f(_thumbsPath, remoteName);
        return f.GetFullPath();
    }
public:
    
    void Save()
    {
        wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, "root");
        for (auto const &file : _all)
        {
            wxXmlNode *fileNode = new wxXmlNode(wxXML_ELEMENT_NODE, "file");
            fileNode->AddAttribute("path", file->GetOriginalPath());
            fileNode->AddAttribute("name", file->GetName());
            fileNode->AddAttribute("filename", file->GetFileName());
            fileNode->AddAttribute("remoteName", file->GetRemoteName());
            fileNode->AddAttribute("ext", file->GetExt());
            fileNode->AddAttribute("siteId", file->GetSiteId());
            fileNode->AddAttribute("isImage", file->IsImage() ? "true" : "false");
            fileNode->AddAttribute("uploadedAt", file->GetUploadedAt().FormatISOCombined());
            
            for (auto const &extra : file->GetExtraInfo())
            {
                auto extraNode = new wxXmlNode(wxXML_ELEMENT_NODE, extra.first);
                auto extraTextNode = new wxXmlNode(wxXML_TEXT_NODE, "");
                extraTextNode->SetContent(extra.second);
                extraNode->AddChild(extraTextNode);
                
                fileNode->AddChild(extraNode);
            }
            
            root->AddChild(fileNode);
        }
        
        wxXmlDocument _xml;
        _xml.SetRoot(root);
        _xml.Save(_path);
        
        
        wxCommandEvent evt(fuEVT_HISTORY_CHANGED);
        for (auto const &listener : _listeners)
            wxPostEvent(listener, evt);
    }
    
    void Clear()
    {
        for (auto it = _all.begin(); it != _all.end(); ++it)
        {
            wxRemoveFile(GetThumbFullPath((*it)->GetRemoteName()));
        }
        
        _all.clear();
        Save();
    }
    
    void RemoveBySiteId(const wxString &siteId)
    {
        vector<File*> left;
        for (auto it = _all.begin(); it != _all.end(); ++it)
        {
            auto file = *it;
            if (file->GetSiteId() == siteId)
            {
                if (file->IsImage())
                    wxRemoveFile(GetThumbFullPath((*it)->GetRemoteName()));
            }
            else
            {
                left.push_back(file);
            }
        }
        
        _all = left;
        Save();
    }
    
    vector<File*> GetLatest()
    {
        vector<File*> latest;
        int i = TheConfig.MaxLatest;
        for (auto it = _all.rbegin(); it != _all.rend() && i-- > 0; ++it)
        {
            latest.push_back(*it);
        }
        return latest;
    }
    
    vector<File*> &GetAll()
    {
        return _all;
    }
    
    bool HasMore()
    {
        return _all.size() > TheConfig.MaxLatest;
    }
    
    void Push(const vector<File*> files)
    {
        for (auto it = files.rbegin(); it != files.rend(); ++it)
        {
            File *file = *it;
            if (file->IsImage())
            {
                wxString path = GetThumbFullPath(file->GetRemoteName());
                File::SaveImage(file->GetThumbnail(), file->GetType(), path);
            }
            _all.push_back(file);
        }
        Save();
    }
    
    void Subscribe(wxEvtHandler *listener)
    {
        _listeners.push_back(listener);
    }
    
    static History &Inst()
    {
        static History history;
        return history;
    }
};

#endif
