#ifndef H_UI_SIZERHELPER
#define H_UI_SIZERHELPER

#include <wx/wx.h>

#define ROW_BORDER 5

class SizerHelper
{
protected:
    void AddRow(wxSizer *mainSizer, const wxString &label, wxWindow *control)
    {

        wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->AddStretchSpacer();

        if (label != wxEmptyString)
        {
            wxStaticText *lbl = new wxStaticText(control->GetParent(), wxID_ANY, label, wxDefaultPosition, wxDefaultSize, wxALIGN_RIGHT);
            wxSizer *lblSizer = new wxBoxSizer(wxVERTICAL);
            lblSizer->AddStretchSpacer();
            lblSizer->Add(lbl, wxSizerFlags().Expand().Border(wxRIGHT, 10));
            lblSizer->AddStretchSpacer();
            sizer->Add(lblSizer, wxSizerFlags().Expand().Proportion(2));
            sizer->Add(control, wxSizerFlags().Expand().Proportion(5));
        }
        else
        {
            sizer->AddStretchSpacer(2);
            wxSizer *ctrlSizer = new wxBoxSizer(wxHORIZONTAL);
            ctrlSizer->Add(control);
            sizer->Add(ctrlSizer, wxSizerFlags().Proportion(5));
        }
        sizer->AddStretchSpacer();

        mainSizer->Add(sizer, wxSizerFlags().Expand().Border(wxALL, ROW_BORDER));
    }

    void AddRow(wxSizer *mainSizer, wxWindow *control)
    {
        mainSizer->Add(control, wxSizerFlags().Expand());
    }

    void AddRow(wxSizer *mainSizer, wxAlignment align, wxWindow *control)
    {
        wxSizer *sizer = new wxBoxSizer(wxHORIZONTAL);

        if (align != wxALIGN_LEFT)
            sizer->AddStretchSpacer();

        sizer->Add(control, wxSizerFlags().Border(wxALL, ROW_BORDER));

        if (align != wxALIGN_RIGHT)
            sizer->AddStretchSpacer();

        mainSizer->Add(sizer, wxSizerFlags().Expand());
    }
};

#endif
