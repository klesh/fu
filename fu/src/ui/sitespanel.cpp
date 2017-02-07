#ifndef H_UI_SITEFORM
#define H_UI_SITEFORM

#include <map>
#include <wx/wx.h>
#include <wx/valnum.h>
#include <wx/filepicker.h>
#include "../core/config.cpp"
#include "../core/site.cpp"
#include "sizerhelper.cpp"
#include "../core/history.cpp"

using namespace std;

class SitesPanel : public wxPanel, protected SizerHelper
{
private:
    Site* _site = NULL;
    wxPanel *_panel;
    wxListBox *_lsbSites;
    wxTextCtrl *_txtName;
    wxChoice *_chcProtocol;
    map<wxString, wxControl*> _settings;
    wxPanel *_pnlSettings = NULL;
    wxButton *_btnSubmit, *_btnCreate, *_btnDelete;
    PtcProvider *_provider;
    enum
    {
        ctlID_SUBMIT,
        ctlID_CREATE,
        ctlID_DELETE,
        ctlID_PROTOCOL,
        ctlID_SITES
    };
    
public:
    SitesPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY)
    {
        _lsbSites = new wxListBox(this, ctlID_SITES);
        _btnCreate = new wxButton(this, ctlID_CREATE, "Add New");
        _btnDelete = new wxButton(this, ctlID_DELETE, "Delete");
        
        _panel = new wxPanel(this, wxID_ANY);
        _txtName = new wxTextCtrl(_panel, wxID_ANY);
        _chcProtocol = new wxChoice(_panel, ctlID_PROTOCOL);
        _pnlSettings = new wxPanel(_panel, wxID_ANY);
        _btnSubmit = new wxButton(_panel, ctlID_SUBMIT, "Save");
        
        for (wxString pn : PtcFactory::Inst().AllKeys())
        {
            _chcProtocol->Append(pn);
        }
        
        PopulateList();
        Populate();
        
        Bind(wxEVT_CHOICE, &SitesPanel::OnProtocolChanged, this, ctlID_PROTOCOL);
        Bind(wxEVT_LISTBOX, &SitesPanel::OnSitesChanged, this, ctlID_SITES);
        Bind(wxEVT_BUTTON, &SitesPanel::OnSubmitClicked, this, ctlID_SUBMIT);
        Bind(wxEVT_BUTTON, &SitesPanel::OnCreateClicked, this, ctlID_CREATE);
        Bind(wxEVT_BUTTON, &SitesPanel::OnDeleteClicked, this, ctlID_DELETE);
        
        wxSizer *rightSizer = new wxBoxSizer(wxVERTICAL);
        AddRow(rightSizer, "Name", _txtName);
        AddRow(rightSizer, "Protocol", _chcProtocol);
        AddRow(rightSizer, _pnlSettings);
        AddRow(rightSizer, wxEmptyString, _btnSubmit);
        _panel->SetSizer(rightSizer);
        
        wxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
        leftSizer->Add(_lsbSites, wxSizerFlags().Expand().Proportion(1));
        
        wxSizer *leftBtns = new wxBoxSizer(wxHORIZONTAL);
        leftBtns->Add(_btnCreate);
        leftBtns->AddStretchSpacer();
        leftBtns->Add(_btnDelete);
        leftSizer->Add(leftBtns, wxSizerFlags().Expand().Border(wxTOP, 5));
        
        wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(leftSizer, wxSizerFlags().Expand().Proportion(3));
        sizer->Add(_panel, wxSizerFlags().Expand().Proportion(8));
        
        wxSizer *outer = new wxBoxSizer(wxHORIZONTAL);
        outer->Add(sizer, wxSizerFlags().Expand().Border(wxALL, 10));
        SetSizer(outer);
    }
    
    void PopulateList()
    {
        _lsbSites->Clear();
        for (auto const &site : TheConfig.GetSites())
        {
            _lsbSites->Append(site->GetName());
        }
    }
    
    void OnProtocolChanged(wxCommandEvent &evt)
    {
        RecreateSettingPanel();
    }
    
    void OnSitesChanged(wxCommandEvent &evt)
    {
        auto index = _lsbSites->GetSelection();
        if (index >= 0)
            _site = TheConfig.GetSite(index);
        else
            _site = new Site();
        
        
        Populate();
    }
    
    void OnSubmitClicked(wxCommandEvent &evt)
    {
        Harvest();
    }
    
    void OnCreateClicked(wxCommandEvent &evt)
    {
        if (_lsbSites->GetSelection() == -1 && _site != NULL)
            return;
        
        _lsbSites->SetSelection(-1);
        _site = new Site();
        Populate();
    }
    
    void OnDeleteClicked(wxCommandEvent &evt)
    {
        auto index = _lsbSites->GetSelection();
        if (index == -1)
        {
            return;
        }
        auto site = TheConfig.GetSite(index);
        if (site == NULL)
            return;
            
        TheHistory.RemoveBySiteId(site->GetId());
        TheConfig.RemoveSiteAt(index);
        PopulateList();
        _lsbSites->SetSelection(-1);
        _site = NULL;
        Populate();
    }
    
    void RecreateSettingPanel()
    {
        _settings.clear();
        _pnlSettings->DestroyChildren();

        if (_chcProtocol->GetSelection() < 0)
            _chcProtocol->SetSelection(0);

        auto protocol = _chcProtocol->GetStringSelection();
        _provider = PtcFactory::Inst().Get(protocol);
        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        for (auto const &meta : _provider->GetSettingMetas())
        {
            wxString name = meta->GetName();
            wxString caption = meta->GetCaption();
            wxString value = wxEmptyString;
            
            wxControl *control = NULL;
            if (_site)
            {
                value = _site->GetSetting(name);
            }
            
            switch (meta->GetValueType())
            {
                case PtcSettingMeta::TYPE_PINT:
                    control = CreatePIntCtrl(value, meta);
                    break;
                case PtcSettingMeta::TYPE_PASSWORD:
                    control = CreatePasswordCtrl(value, meta);
                    break;
                case PtcSettingMeta::TYPE_FILE:
                    control = CreateFileCtrl(value, meta);
                    break;
                case PtcSettingMeta::TYPE_DIR:
                    control = CreateDirCtrl(value, meta);
                    break;
                default:
                    control = CreateStringCtrl(value, meta);
                    break;
            }
            
            
            _settings.insert(pair<wxString, wxControl*>(name, control));
            AddRow(sizer, caption, control);
        }
        _pnlSettings->SetSizer(sizer);
        Layout();
    }
    
    wxControl *CreateStringCtrl(wxString &value, PtcSettingMeta *meta)
    {
        auto ctrl = new wxTextCtrl(_pnlSettings, wxID_ANY, value);
        ctrl->SetHint(meta->GetHint());
        return ctrl;
    }
    
    wxControl *CreatePasswordCtrl(wxString &value, PtcSettingMeta *meta)
    {
        auto ctrl = new wxTextCtrl(_pnlSettings, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, wxTE_PASSWORD);
        ctrl->SetHint(meta->GetHint());
        return ctrl;
    }
    
    wxControl *CreatePIntCtrl(wxString &value, PtcSettingMeta *meta)
    {
        wxIntegerValidator<unsigned int> validator;
        validator.SetMin(1);
        auto ctrl = new wxTextCtrl(_pnlSettings, wxID_ANY, value, wxDefaultPosition, wxDefaultSize, 0, validator);
        ctrl->SetHint(meta->GetHint());
        return ctrl;
    }
    
    wxControl *CreateFileCtrl(wxString &value, PtcSettingMeta *meta)
    {
        auto ctrl = new wxFilePickerCtrl(_pnlSettings, wxID_ANY, value);
        return ctrl;
    }
    
    wxControl *CreateDirCtrl(wxString &value, PtcSettingMeta *meta)
    {
        auto ctrl = new wxDirPickerCtrl(_pnlSettings, wxID_ANY, value);
        return ctrl;
    }
    
    void Populate()
    {
        if (_site == NULL)
        {
            _panel->Hide();
            return;
        }
        
        _panel->Show();
        _txtName->SetValue(_site->GetName());
        _chcProtocol->SetStringSelection(_site->GetProtocol());
        RecreateSettingPanel();
    }
    
    void Harvest()
    {
        auto name = _txtName->GetValue();
        if (name.IsEmpty())
        {
            _txtName->SetFocus();
            return;
        }
        
        map<wxString, wxString> settings;
        for (auto const &meta : _provider->GetSettingMetas())
        {
            auto name = meta->GetName();
            auto control = _settings[name];
            
            wxString value;
            
            int valueType = meta->GetValueType();
            if (valueType == PtcSettingMeta::TYPE_FILE)
            {
                value = ((wxFilePickerCtrl*)control)->GetPath();
            }
            else if (valueType == PtcSettingMeta::TYPE_DIR)
            {
                value = ((wxDirPickerCtrl*)control)->GetPath();
            }
            else
            {
                value = ((wxTextCtrl*)control)->GetValue();
            }
            
            if (meta->IsRequired() && value.IsEmpty())
            {
                control->SetFocus();
                return;
            }
            settings.insert(pair<wxString, wxString>(name, value));
        }
        
        int index = _lsbSites->GetSelection();
        if (index == -1)
        {
            auto sites = TheConfig.GetSites();
            if (find(sites.begin(), sites.end(), _site) != sites.end())
            {
                _site = new Site();
            }
        }
        
        _site->SetName(name);
        _site->SetProtocol(_chcProtocol->GetStringSelection());
        _site->SetSettings(settings);
        
        if (index == -1)
        {
            TheConfig.AddSite(_site);
            index = (int)(_lsbSites->GetStrings().GetCount());
        }
        else
        {
            TheConfig.UpdateSite(_site);
        }
        TheConfig.SiteSelected = _site;
        PopulateList();
        _lsbSites->Select(index);
    }
    
    void StartCreating()
    {
        _site = new Site();
        RecreateSettingPanel();
    }
};

#endif
