#ifndef H_UI_IMGPRCPANEL
#define H_UI_IMGPRCPANEL

#include <wx/wx.h>
#include <wx/filepicker.h>
#include "../core/config.cpp"

class PreProcPanel : public wxPanel
{
private:
    wxCheckBox *_ckbReduceSize;
    wxCheckBox *_ckbWatermark;
    wxFilePickerCtrl *_fpcWatermarkImg;
    wxChoice *_chcWatermarkPos;
    
public:
    PreProcPanel(wxWindow *parent) : wxPanel(parent, wxID_ANY)
    {
        _ckbReduceSize = new wxCheckBox(this, wxID_ANY, "Reduce image file size before upload (jpg/png)");
        _ckbWatermark = new wxCheckBox(this, wxID_ANY, "Enable watermark");
        _fpcWatermarkImg = new wxFilePickerCtrl(this, wxID_ANY);
        _chcWatermarkPos = new wxChoice(this, wxID_ANY);
        
        _chcWatermarkPos->Append("Top Left");
        _chcWatermarkPos->Append("Top Right");
        _chcWatermarkPos->Append("Bottom Left");
        _chcWatermarkPos->Append("Bottom Right");
        
        Populate();
        
        Bind(wxEVT_CHECKBOX, &PreProcPanel::OnChange, this);
        Bind(wxEVT_CHOICE, &PreProcPanel::OnChange, this);
        Bind(wxEVT_FILEPICKER_CHANGED, &PreProcPanel::OnChange, this);
        
        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(_ckbReduceSize, wxSizerFlags().Expand());
        sizer->Add(_ckbWatermark, wxSizerFlags().Expand());
        sizer->Add(_fpcWatermarkImg, wxSizerFlags().Expand());
        sizer->Add(_chcWatermarkPos, wxSizerFlags().Expand());
        
        wxSizer *outer = new wxBoxSizer(wxHORIZONTAL);
        outer->Add(sizer, wxSizerFlags().Expand().Border(wxALL, 10));
        SetSizer(outer);
    }
    
    void Populate()
    {
        _ckbReduceSize->SetValue(TheConfig.EnableReduceSize);
        _ckbWatermark->SetValue(TheConfig.EnableWatermark);
        _fpcWatermarkImg->SetPath(TheConfig.WatermarkPath);
        _fpcWatermarkImg->Enable(TheConfig.EnableWatermark);
        _chcWatermarkPos->SetSelection(TheConfig.WatermarkPosition);
    }
    
    void Harvest()
    {
        TheConfig.EnableReduceSize = _ckbReduceSize->GetValue();
        TheConfig.EnableWatermark = _ckbWatermark->GetValue();
        TheConfig.WatermarkPath = _fpcWatermarkImg->GetPath();
        TheConfig.WatermarkPosition = _chcWatermarkPos->GetSelection();
        TheConfig.Save();
    }
    
    void OnChange(wxCommandEvent& evt)
    {
        Harvest();
        Populate();
    }
};

#endif
