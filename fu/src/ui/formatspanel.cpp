#ifndef H_UI_FORMATSPANEL
#define H_UI_FORMATSPANEL

#include <wx/wx.h>
#include "sizerhelper.cpp"
#include "../core/format.cpp"
#include "../core/config.cpp"

class FormatsPanel : public wxPanel, protected SizerHelper
{
private:
    Format *_format = NULL;
    wxPanel *_panel;
    wxListBox *_lsbFormats;
    wxButton *_btnSubmit, *_btnCreate, *_btnDelete;
    wxTextCtrl *_txtName, *_txtTemplate;
    enum
    {
        ctlID_SUBMIT,
        ctlID_CREATE,
        ctlID_DELETE,
        ctlID_FORMATS
    };
    
public:
    FormatsPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY)
    {
        _lsbFormats = new wxListBox(this, ctlID_FORMATS);
        _btnCreate = new wxButton(this, ctlID_CREATE, "Add New");
        _btnDelete = new wxButton(this, ctlID_DELETE, "Delete");
        
        _panel = new wxPanel(this, wxID_ANY);
        _btnSubmit = new wxButton(_panel, ctlID_SUBMIT, "Save");
        _txtName = new wxTextCtrl(_panel, wxID_ANY);
        _txtTemplate = new wxTextCtrl(_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 60), wxTE_MULTILINE);
        
        PopulateList();
        Populate();
        
        Bind(wxEVT_LISTBOX, &FormatsPanel::OnFormatsChanged, this, ctlID_FORMATS);
        Bind(wxEVT_BUTTON, &FormatsPanel::OnSubmitClicked, this, ctlID_SUBMIT);
        Bind(wxEVT_BUTTON, &FormatsPanel::OnCreateClicked, this, ctlID_CREATE);
        Bind(wxEVT_BUTTON, &FormatsPanel::OnDeleteClicked, this, ctlID_DELETE);
        
        // layout
        wxSizer *rightSizer = new wxBoxSizer(wxVERTICAL);
        AddRow(rightSizer, "Name", _txtName);
        AddRow(rightSizer, "Template", _txtTemplate);
        AddRow(rightSizer, wxEmptyString, _btnSubmit);
        _panel->SetSizer(rightSizer);
        
        wxSizer *leftSizer = new wxBoxSizer(wxVERTICAL);
        leftSizer->Add(_lsbFormats, wxSizerFlags().Expand().Proportion(1));
        
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
        _lsbFormats->Clear();
        for (auto const &format : TheConfig.Formats)
        {
            _lsbFormats->Append(format->GetName());
        }
    }
    
    void Populate()
    {
        if (_format == NULL)
        {
            _panel->Hide();
            return;
        }
        else
        {
            _panel->Show();
        }
        _txtName->SetValue(_format->GetName());
        _txtTemplate->SetValue(_format->GetTemplate());
        _txtName->SetFocus();
        Layout();
    }
    
    void OnFormatsChanged(wxCommandEvent &evt)
    {
        auto index = _lsbFormats->GetSelection();
        if (index >= 0)
            _format = TheConfig.Formats[index];
        
        Populate();
    }
    
    void OnSubmitClicked(wxCommandEvent &evt)
    {
        auto name = _txtName->GetValue();
        if (name.IsEmpty())
        {
            _txtName->SetFocus();
            return;
        }
        
        auto tpl = _txtTemplate->GetValue();
        if (tpl.IsEmpty())
        {
            _txtTemplate->SetFocus();
            return;
        }
        auto index = _lsbFormats->GetSelection();
        if (index == -1)
        {
            if (find(TheConfig.Formats.begin(), TheConfig.Formats.end(), _format) != TheConfig.Formats.end())
            {
                _format = new Format();
            }
        }
        
        _format->SetName(name);
        _format->SetTemplate(tpl);
        if (index == -1)
        {
            TheConfig.Formats.push_back(_format);
            index = (int)(_lsbFormats->GetStrings().GetCount());
        }
        TheConfig.FormatSelected = _format;
        TheConfig.Save();
        PopulateList();
        _lsbFormats->SetSelection(index);
    }
    
    void OnCreateClicked(wxCommandEvent &evt)
    {
        if (_lsbFormats->GetSelection() == -1 && _format != NULL)
            return;
        StartCreating();
    }
    
    void StartCreating()
    {
        _lsbFormats->SetSelection(-1);
        _format = new Format();
        Populate();
    }
    
    void OnDeleteClicked(wxCommandEvent &evt)
    {
        auto index = _lsbFormats->GetSelection();
        if (index == -1)
            return;
        TheConfig.RemoveFormatAt(index);
        PopulateList();
        _lsbFormats->SetSelection(-1);
        _format = NULL;
        Populate();
    }
};

#endif
