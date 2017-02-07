#ifndef H_UI_BROWSEFORM
#define H_UI_BROWSEFORM

#include <wx/wx.h>
#include <wx/hyperlink.h>
#include "../core/config.cpp"
#include "../core/history.cpp"
#include "../core/clip.cpp"

class bfCell : public wxPanel
{
private:
    wxWindow *_parent;
    bool _isSelected = false;
    wxColour _normalBgColor;
    wxColour _selectedBgColor;
    wxStaticBitmap *_sbmThumbnail;
    bool _thumbnailLoaded = false;
    
    File *_file;
    
    void OnMouseLeftClicked(wxMouseEvent &evt)
    {
        _isSelected = !_isSelected;
        
        
        SetBackgroundColour(_isSelected ? _selectedBgColor : _normalBgColor);
        
        Refresh();
    }
    
    void OnPaint(wxPaintEvent &evt)
    {
        if (_thumbnailLoaded == false)
        {
            _thumbnailLoaded = true;
            _sbmThumbnail->SetBitmap(_file->GetThumbnail());
            Layout();
        }
    }
    
public:
    bfCell(wxWindow *parent, File *file) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 200))
    {
        _parent = parent;
        _file = file;
        
        Site *site = TheConfig.FindSiteById(file->GetSiteId());
        auto sttSite = new wxStaticText(this, wxID_ANY, site->GetName());
        auto sttName = new wxStaticText(this, wxID_ANY, file->GetLongName());
        sttSite->SetForegroundColour("dark grey");
        sttSite->GetFont().Scale(0.8f);
        auto sttDate = new wxStaticText(this, wxID_ANY, file->GetUploadedAt().Format());
        sttDate->SetForegroundColour("dark grey");
        sttDate->GetFont().Scale(0.8f);
        
        _normalBgColor.Set("white");
        _selectedBgColor.Set("sky blue");
        
        _sbmThumbnail = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);
        
        SetBackgroundColour(_normalBgColor);
        Bind(wxEVT_LEFT_DOWN, &bfCell::OnMouseLeftClicked, this);
        _sbmThumbnail->Bind(wxEVT_LEFT_DOWN, &bfCell::OnMouseLeftClicked, this);
        Bind(wxEVT_PAINT, &bfCell::OnPaint, this);
        
        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        
        sizer->AddStretchSpacer();
        sizer->Add(_sbmThumbnail, wxSizerFlags().Border(wxALL, 5).Center());
        sizer->AddStretchSpacer();
        sizer->Add(sttSite, wxSizerFlags().Center());
        sizer->Add(sttDate, wxSizerFlags().Center());
        sizer->Add(sttName, wxSizerFlags().Center().Border(wxALL, 5));
        
        SetSizer(sizer);
    }
    
    bool IsSelected()
    {
        return _isSelected;
    }
    
    File* GetFile()
    {
        wxLogDebug("file site id : %s", _file->GetSiteId());
        return _file;
    }
};

class bfScrolled : public wxScrolledWindow
{
private:
    wxGridSizer *_gridSizer;
    vector<bfCell*> _cells;
    
    void OnSize(wxSizeEvent &evt)
    {
        int width = GetClientSize().GetWidth();
        int cols = width / 210;
        if (cols <=0 ) cols = 1;
        _gridSizer->SetCols(cols);
        FitInside();
    }
    
public:
    bfScrolled(wxWindow *parent) : wxScrolledWindow(parent, wxID_ANY)
    {
        _gridSizer = new wxGridSizer(3, 0, 0);
        
        SetSizer(_gridSizer);
        
        // this part makes the scrollbars show up
        this->SetScrollRate(5, 5);
        
        Bind(wxEVT_SIZE, &bfScrolled::OnSize, this);
    }
    
    void Populate(const vector<File*> files)
    {
        _gridSizer->Clear(true);
        _cells.clear();
        for (auto const & file : files)
        {
            bfCell *cell = new bfCell(this, file);
            _gridSizer->Add(cell, wxSizerFlags().Border(wxALL, 5).Center());
            _cells.push_back(cell);
        }
        this->FitInside(); // ask the sizer about the needed size
    }
    
    vector <File*> GetSelected()
    {
        vector<File*> selected;
        for (auto const &child :  _cells)
        {
            auto cell = (bfCell*)child;
            if (cell->IsSelected())
                selected.push_back(cell->GetFile());
        }
        
        return selected;
    }
};

class BrowseForm : public wxDialog
{
    wxChoice *_chcSite;
    wxTextCtrl *_txtKeyword;
    wxButton *_btnSearch, *_btnCopy, *_btnDelete, *_btnClear;
    bfScrolled *_scwFiles;
    
    enum {
        ctlID_SEARCH,
        ctlID_COPY,
        ctlID_CLEAR,
        ctlID_SITE
    };
    
    void OnSitesChanged(wxCommandEvent &evt)
    {
        PopulateSites();
    }
    
    void OnFilterChanged(wxCommandEvent &evt)
    {
        PopulateCells();
    }
    
    void PopulateSites()
    {
        _chcSite->Clear();
        _chcSite->Append("[All Servers]");
        for (auto const &site : TheConfig.GetSites())
            _chcSite->Append(site->GetName());
    }
    
    void PopulateCells()
    {
        wxString siteId, keyword;
        
        int siteIndex = _chcSite->GetSelection();
        if (siteIndex > 0)
        {
            auto site = TheConfig.GetSite(siteIndex - 1);
            if (site != NULL)
                siteId = site->GetId();
        }
        keyword = _txtKeyword->GetValue();
        
        vector<File*> files = TheHistory.GetAll();
        
        if (!siteId.IsEmpty() || !keyword.IsEmpty())
        {
            vector<File*> filtered;
            for (auto const &file : files)
            {
                if (!siteId.IsEmpty() && siteId != file->GetSiteId())
                    continue;
               
                if (!keyword.IsEmpty()
                    && file->GetFileName().Find(keyword) == wxNOT_FOUND
                    && file->GetRemoteName().Find(keyword) == wxNOT_FOUND)
                    continue;
                
                filtered.push_back(file);
            }
            files = filtered;
        }
        
        _scwFiles->Populate(files);
    }
    
    void OnClearClicked(wxCommandEvent &evt)
    {
        TheHistory.Clear();
        PopulateCells();
    }
    
    void OnCopyClicked(wxCommandEvent &evt)
    {
        vector<File*> selected = _scwFiles->GetSelected();
        
        if (!selected.empty())
            TheClip.SetText(selected);
        else
            wxMessageBox("Please click to select items you want to copy", "Error", wxOK, this);
    }



    void OnClose(wxCloseEvent &evt)
    {
        TheConfig.BrowseFormPosition = this->GetPosition();
        TheConfig.BrowseFormSize = this->GetSize();
        TheConfig.Save();
        Show(false);
    }

    
public:
    BrowseForm(wxPoint &pos, wxSize &size) : wxDialog(NULL, wxID_ANY, "History", pos, size, wxCAPTION | wxMAXIMIZE_BOX | wxRESIZE_BORDER | wxCLOSE_BOX)
    {
        _chcSite = new wxChoice(this, ctlID_SITE);
        _txtKeyword = new wxTextCtrl(this, wxID_ANY);
        _txtKeyword->SetHint("Keyword");
        _btnSearch = new wxButton(this, ctlID_SEARCH, "Go", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
        _btnSearch->SetDefault();
        
        _scwFiles = new bfScrolled(this);
        _scwFiles->SetBackgroundColour("white");
        
        _btnCopy = new wxButton(this, ctlID_COPY, "Copy To Clipboard");
        _btnClear = new wxButton(this, ctlID_CLEAR, "Clear All");
        
        PopulateSites();
        PopulateCells();
        
        Bind(wxEVT_BUTTON, &BrowseForm::OnClearClicked, this, ctlID_CLEAR);
        Bind(wxEVT_CHOICE, &BrowseForm::OnFilterChanged, this, ctlID_SITE);
        Bind(wxEVT_BUTTON, &BrowseForm::OnFilterChanged, this, ctlID_SEARCH);
        Bind(wxEVT_BUTTON, &BrowseForm::OnCopyClicked, this, ctlID_COPY);
        Bind(fuEVT_SITES_CHANGED, &BrowseForm::OnSitesChanged, this);
        Bind(fuEVT_HISTORY_CHANGED, &BrowseForm::OnFilterChanged, this);
        Bind(wxEVT_CLOSE_WINDOW, &BrowseForm::OnClose, this);

        TheConfig.Subscribe(this);
        TheHistory.Subscribe(this);
        
        wxSizer *toolbarSizer = new wxBoxSizer(wxHORIZONTAL);
        toolbarSizer->Add(_chcSite);
        toolbarSizer->AddStretchSpacer();
        toolbarSizer->Add(_txtKeyword, wxSizerFlags().Expand());
        toolbarSizer->Add(_btnSearch, wxSizerFlags().Expand().Border(wxLEFT, 5));
        
        wxSizer *operationSizer = new wxBoxSizer(wxHORIZONTAL);
        operationSizer->Add(_btnClear, wxSizerFlags().Border(wxLEFT, 5));
        operationSizer->AddStretchSpacer();
        operationSizer->Add(_btnCopy, wxSizerFlags().Border(wxLEFT, 5));
        
        wxSizer *sizer = new wxBoxSizer(wxVERTICAL);
        sizer->Add(toolbarSizer, wxSizerFlags().Expand().Border(wxALL, 5));
        sizer->Add(_scwFiles, wxSizerFlags().Expand().Border(wxALL, 5).Proportion(1));
        sizer->Add(operationSizer, wxSizerFlags().Expand().Border(wxALL, 5));
        
        wxSizer *outer = new wxBoxSizer(wxVERTICAL);
        outer->Add(sizer, wxSizerFlags().Expand().Proportion(1).Border(wxALL, 10));
        SetSizer(outer);
    }
};

#endif
