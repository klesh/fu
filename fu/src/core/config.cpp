#ifndef H_CORE_CONFIG
#define H_CORE_CONFIG 

#include <wx/stdpaths.h>
#include <wx/filename.h>
#include <wx/xml/xml.h>
#include <vector>

#include "format.cpp"
#include "site.cpp"

#include "../protocols/ptcp.h"
#include "../protocols/ptcf.h"

#define TheConfig Config::Inst()

wxDEFINE_EVENT(fuEVT_SITES_CHANGED, wxCommandEvent);

using namespace std;

class Config
{
private:
    wxString _path;
    vector<Site*> _sites;
    vector<wxEvtHandler*> _listeners;
    wxString _executeDir;
    
    Config()
    {
        wxString folder = wxStandardPaths::Get().GetUserDataDir();
        if (!wxDirExists(folder))
            wxMkdir(folder);
        
        wxFileName path(folder, "config.xml");
        _path = path.GetFullPath();
        
        Format *plain = new Format("Plain URL", "%s");
        Format *imgTag = new Format("HTML img Tag", "<img src=\"%s\" />");
        Format *markdown = new Format("Markdown", "![](%s)");
        Formats.push_back(plain);
        Formats.push_back(imgTag);
        Formats.push_back(markdown);
        FormatSelected = plain;
        
        if (!wxFileExists(_path))
        {
            return;
        }
        
        wxXmlDocument _xml;
        _xml.Load(_path);
        
        wxXmlNode *child = _xml.GetRoot()->GetChildren();
        while (child) {
            wxString nodeName = child->GetName();
            if (nodeName == "preview")
            {
                wxString maxWidth = child->GetAttribute("maxWidth");
                if (!maxWidth.IsEmpty())
                    MaxPreviewWidth = wxAtoi(maxWidth);
                
                wxString maxHeight = child->GetAttribute("maxHeight");
                if (!maxHeight.IsEmpty())
                    MaxPreviewHeight = wxAtoi(maxHeight);
            }
            else if (nodeName == "ui")
            {
                Position = wxPoint(
                    wxAtoi(child->GetAttribute("x")),
                    wxAtoi(child->GetAttribute("y"))
                );
                
                Size = wxSize(
                    wxAtoi(child->GetAttribute("width")),
                    wxAtoi(child->GetAttribute("height"))
                );
            }
            else if (nodeName == "browseform")
            {
                BrowseFormPosition = wxPoint(
                    wxAtoi(child->GetAttribute("x")),
                    wxAtoi(child->GetAttribute("y"))
                );
                
                BrowseFormSize = wxSize(
                    wxAtoi(child->GetAttribute("width")),
                    wxAtoi(child->GetAttribute("height"))
                );
            }
            else if (nodeName == "formats")
            {
                Formats.clear();
                wxString selected = child->GetAttribute("selected");
                wxXmlNode *formatNode = child->GetChildren();
                while (formatNode)
                {
                    if (formatNode->GetName() != "format")
                        continue;
                    
                    Format *format = new Format(formatNode->GetAttribute("name"), formatNode->GetAttribute("template"));
                    Formats.push_back(format);
                    if (format->GetName() == selected || FormatSelected == NULL)
                        FormatSelected = format;
                    
                    formatNode = formatNode->GetNext();
                }
            }
            else if (nodeName == "sites")
            {
                wxString selected = child->GetAttribute("selected");
                wxXmlNode *siteNode = child->GetChildren();
                while (siteNode)
                {
                    if (siteNode->GetName() != "site")
                        continue;
                    
                    wxString id = siteNode->GetAttribute("id");
                    wxString name = siteNode->GetAttribute("name");
                    wxString protocol = siteNode->GetAttribute("protocol");
                    PtcProvider *provider = PtcFactory::Inst().Get(protocol);
                    map<wxString, wxString> settings;
                    for (PtcSettingMeta *settingMeta : provider->GetSettingMetas())
                    {
                        wxString settingName = settingMeta->GetName();
                        wxString settingValue = siteNode->GetAttribute(settingName);
                        settings.insert(pair<wxString, wxString>(settingName, settingValue));
                    }
                    
                    Site *site = new Site();
                    site->SetId(id);
                    site->SetName(name);
                    site->SetProtocol(protocol);
                    site->SetSettings(settings);
                    
                    _sites.push_back(site);
                    if (name == selected || SiteSelected == NULL)
                        SiteSelected = site;
                    
                    siteNode = siteNode->GetNext();
                }
            }
            else if (nodeName == "preprocess")
            {
                EnableReduceSize = child->GetAttribute("reduceSize") == "true";
                EnableWatermark = child->GetAttribute("watermark") == "true";
                WatermarkPath = child->GetAttribute("watermarkPath");
            }
            
            child = child->GetNext();
        }
    }
    
    ~Config()
    {
        Save();
    }
    
    void EmitEvent(wxCommandEvent &evt)
    {
        for (auto const &listener : _listeners)
        {
            wxPostEvent(listener, evt);
        }
    }
    
public:
    int MaxPreviewWidth = 200;
    int MaxPreviewHeight = 150;
    wxPoint Position = wxDefaultPosition;
    wxSize Size = wxSize(600, 450);
    wxPoint BrowseFormPosition = wxDefaultPosition;
    wxSize BrowseFormSize = wxSize(600, 450);
    
    vector<Format*> Formats;
    Format* FormatSelected = NULL;
    
    Site* SiteSelected;
    
    bool EnableReduceSize = true;
    bool EnableWatermark = false;
    int JPGQuality = 75;
    int PNGDepth = 8;
    int PNGCompressionLevel = 9;
    size_t MaxLatest = 3;
    wxString WatermarkPath = wxEmptyString;
    int WatermarkPosition = 0;
    
    void Subscribe(wxEvtHandler *listener)
    {
        _listeners.push_back(listener);
    }
    
    void AddSite(Site *site)
    {
        _sites.push_back(site);
        Save();
        
        wxCommandEvent evt(fuEVT_SITES_CHANGED);
        EmitEvent(evt);
    }
    
    void UpdateSite(Site* site)
    {
        Save();
        wxCommandEvent evt(fuEVT_SITES_CHANGED);
        EmitEvent(evt);
    }
    
    const vector<Site*> GetSites()
    {
        if (_sites.empty())
        {
            auto smms = new Site();
            smms->SetName("sm.ms");
            smms->SetProtocol("sm.ms");
            _sites.push_back(smms);
            SiteSelected = smms;
        }
        return _sites;
    }
    
    Site *GetSite(int index)
    {
        return _sites[index];
    }
    
    Site *FindSiteById(const wxString &siteId)
    {
        for (auto const &site : _sites)
        {
            if (site->GetId() == siteId)
            {
                return site;
            }
        }
        return NULL;
    }
    
    void RemoveSiteAt(int index)
    {
        auto site = _sites[index];
        if (site == SiteSelected)
        {
            SiteSelected = NULL;
        }
        delete site;
        _sites.erase(_sites.begin() + index);
        if (SiteSelected == NULL && !_sites.empty())
            SiteSelected = _sites[0];
        Save();
        wxCommandEvent evt(fuEVT_SITES_CHANGED);
        EmitEvent(evt);
    }
    
    void RemoveFormatAt(int index)
    {
        auto format = Formats[index];
        if (format == FormatSelected)
        {
            FormatSelected = NULL;
        }
        delete format;
        Formats.erase(Formats.begin() + index);
        Save();
    }
    
    void Save()
    {
        wxXmlNode *preview = new wxXmlNode(wxXML_ELEMENT_NODE, "preview");
        preview->AddAttribute("maxWidth", wxString::Format("%d", MaxPreviewWidth));
        preview->AddAttribute("maxHeight", wxString::Format("%d", MaxPreviewHeight));
        
        wxXmlNode *ui = new wxXmlNode(wxXML_ELEMENT_NODE, "ui");
        ui->AddAttribute("x", wxString::Format("%d", Position.x));
        ui->AddAttribute("y", wxString::Format("%d", Position.y));
        ui->AddAttribute("width", wxString::Format("%d", Size.GetWidth()));
        ui->AddAttribute("height", wxString::Format("%d", Size.GetHeight()));
        
        wxXmlNode *browseform = new wxXmlNode(wxXML_ELEMENT_NODE, "browseform");
        browseform->AddAttribute("x", wxString::Format("%d", BrowseFormPosition.x));
        browseform->AddAttribute("y", wxString::Format("%d", BrowseFormPosition.y));
        browseform->AddAttribute("width", wxString::Format("%d", BrowseFormSize.GetWidth()));
        browseform->AddAttribute("height", wxString::Format("%d", BrowseFormSize.GetHeight()));
        
        wxXmlNode *formats = new wxXmlNode(wxXML_ELEMENT_NODE, "formats");
        if (FormatSelected != NULL)
            formats->AddAttribute("selected", FormatSelected->GetName());
        
        for (Format *format : Formats)
        {
            wxXmlNode *formatNode = new wxXmlNode(wxXML_ELEMENT_NODE, "format");
            formatNode->AddAttribute("name", format->GetName());
            formatNode->AddAttribute("template", format->GetTemplate());
            
            formats->AddChild(formatNode);
        }
        
        wxXmlNode *sites = new wxXmlNode(wxXML_ELEMENT_NODE, "sites");
        if (SiteSelected != NULL)
            sites->AddAttribute("selected", SiteSelected->GetName());
        
        for (Site *site : _sites)
        {
            wxXmlNode *siteNode = new wxXmlNode(wxXML_ELEMENT_NODE, "site");
            siteNode->AddAttribute("id", site->GetId());
            siteNode->AddAttribute("name", site->GetName());
            siteNode->AddAttribute("protocol", site->GetProtocol());
            
            for (auto const &setting : site->GetSettings())
            {
                siteNode->AddAttribute(setting.first, setting.second);
            }
            
            sites->AddChild(siteNode);
        }
        
        wxXmlNode *preprocess = new wxXmlNode(wxXML_ELEMENT_NODE, "preprocess");
        preprocess->AddAttribute("reduceSize", EnableReduceSize ? "true": "false");
        preprocess->AddAttribute("watermark", EnableWatermark ? "true" : "false");
        preprocess->AddAttribute("watermarkPath", WatermarkPath);
        
        wxXmlNode *root = new wxXmlNode(wxXML_ELEMENT_NODE, "root");
        root->SetChildren(preview);
        root->AddChild(ui);
        root->AddChild(browseform);
        root->AddChild(formats);
        root->AddChild(sites);
        root->AddChild(preprocess);
        
        wxXmlDocument _xml;
        _xml.SetRoot(root);
        _xml.Save(_path);
    }
    
    static Config &Inst()
    {
        static Config config;
        return config;
    }
};

#endif
