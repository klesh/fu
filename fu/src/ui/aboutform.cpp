#ifndef UI_ABOUT
#define UI_ABOUT

#include <wx/wx.h>
#include <wx/hyperlink.h>
#include "common.cpp"

class AboutForm : public wxDialog
{
private:
    wxStaticBitmap* _icon;
    wxHyperlinkCtrl* _link;
    wxStaticText* _version;
    wxButton* _close;
    
public:
    AboutForm() : wxDialog(NULL, wxID_ANY, "About", wxPoint(-1, -1), wxSize(400, 300))
    {
        SetIcon(UICommon::Get().Icon());
        wxImage img = wxImage(TheConfig.GetIconPath("128x128", "icon.png"), wxBITMAP_TYPE_PNG);
        _icon = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);
        _icon->SetBitmap(wxBitmap(img));
        
        _link = new wxHyperlinkCtrl(this, wxID_ANY, "https://github.com/klesh/fu", "https://github.com/klesh/fu");
        _version = new wxStaticText(this, wxID_ANY, "version: 1.3.1");
        _close = new wxButton(this, wxID_ANY, "Close");
        Bind(wxEVT_BUTTON, &AboutForm::OnCloseClicked, this, wxID_ANY);

        
        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->AddStretchSpacer();
        sizer->Add(_icon, wxSizerFlags().Border(wxALL, 5).Center());
        sizer->Add(_link, wxSizerFlags().Border(wxALL, 5).Center());
        sizer->Add(_version, wxSizerFlags().Border(wxALL, 5).Center());
        sizer->Add(_close, wxSizerFlags().Border(wxALL, 5).Center());
        sizer->AddStretchSpacer();
        SetSizer(sizer);
    }

    void OnCloseClicked(wxCommandEvent &evt)
    {
        this->Hide();
    }
};

#endif
