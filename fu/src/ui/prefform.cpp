#ifndef H_UI_PREFFORM
#define H_UI_PREFFORM

#include <wx/wx.h>
#include <wx/notebook.h>
#include "sitespanel.cpp"
#include "formatspanel.cpp"
#include "preprocpanel.cpp"

class PrefForm : public wxDialog
{
private:
    wxNotebook *_tabs;
    SitesPanel *_pnlSites;
    FormatsPanel *_pnlFormats;
    PreProcPanel *_pnlPreproc;

    void OnClose(wxCloseEvent &evt)
    {
        TheConfig.Position = this->GetPosition();
        TheConfig.Size = this->GetSize();
        TheConfig.Save();
        Show(false);
    }

public:
    PrefForm(wxPoint &pos, wxSize &size) : wxDialog(NULL, wxID_ANY, "Preferences", pos, size, wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    {
        _tabs = new wxNotebook(this, wxID_ANY);
        _pnlSites = new SitesPanel(_tabs);
        _pnlPreproc = new PreProcPanel(_tabs);
        _pnlFormats = new FormatsPanel(_tabs);
        
        _tabs->AddPage(_pnlSites, "Servers");
        _tabs->AddPage(_pnlPreproc, "Preprocess");
        _tabs->AddPage(_pnlFormats, "Output Formats");

        Bind(wxEVT_CLOSE_WINDOW, &PrefForm::OnClose, this);

        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(_tabs, wxSizerFlags().Expand().Proportion(1));
        SetSizer(sizer);
        SetAutoLayout(true);
    }

    PrefForm *GoSiteCreating()
    {
        _tabs->SetSelection(0);
        _pnlSites->StartCreating();
        return this;
    }
    
    PrefForm *GoFormatCreating()
    {
        _tabs->SetSelection(2);
        _pnlFormats->StartCreating();
        return this;
    }
};

#endif
