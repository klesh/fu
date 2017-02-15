#ifndef UI_TRAY
#define UI_TRAY


#include <wx/wx.h>
#include <wx/icon.h>
#include <wx/taskbar.h>
#include <map>
#include <vector>

#include "../core/config.cpp"
#include "../core/file.cpp"
#include "../core/clip.cpp"
#include "../core/site.cpp"
#include "../core/uploader.cpp"
#include "../core/format.cpp"
#include "../core/history.cpp"
#include "../os/os.h"
#include "prefform.cpp"
#include "browseform.cpp"

#ifdef HAVE_APPINDICATOR
extern "C"
{
#include <libappindicator/app-indicator.h>
}
#endif

using namespace std;

class Tray : public wxTaskBarIcon
{
private:
    enum {
        itemID_HINT = 100,
        itemID_UPLOAD_ALL,
        itemID_BROWSE_HISTORY,
        itemID_PREFERENCES,
        itemID_EXIT,
        itemID_ADD_SITE,
        itemID_ADD_FORMAT,
        itemID_PENDING_START = 1000,
        itemID_UPLOADING_START = 2000,
        itemID_UPLOADED_START = 3000,
        itemID_FORMAT_START = 5000,
        itemID_SITE_START = 7000
    };

    PrefForm *_pref;
    BrowseForm *_browser = NULL;

    vector<File*> _pending, _uploading, _uploaded;
    int uploadingCount = 0;

#ifdef HAVE_APPINDICATOR
    AppIndicator* _indicator;
    wxMenu *_menu = NULL;

    static void gtk_menu_item_selected(GtkMenuItem *menu_item, gpointer user_data)
    {
        //wxLogDebug("item id : %d", GPOINTER_TO_INT(user_data));
        wxCommandEvent evt(wxEVT_MENU);
        evt.SetId(GPOINTER_TO_INT(user_data));
        wxPostEvent(&Tray::Inst(), evt);
    }

    static void g_connect(wxMenuItem *item)
    {
        g_signal_connect((GObject*)item->GetMenuItem(), "activate", G_CALLBACK(gtk_menu_item_selected), GINT_TO_POINTER(item->GetId()));
    }
#else
    wxString _iconPath;
    wxString _iconUploadingPath;
    wxIcon _icon;
    wxIcon _iconUploading;
#endif

    Tray()
    {
        wxLogDebug("tray icon ready");



        _pref = new PrefForm(TheConfig.Position, TheConfig.Size);

        Bind(wxEVT_COMMAND_MENU_SELECTED, &Tray::OnMenuItemSelected, this);
        Bind(EVT_UPLOAD_ERROR, &Tray::OnError, this);
        Bind(EVT_UPLOAD_SUCCESS, &Tray::OnUploadSuccess, this);
        Bind(EVT_UPLOAD_START, &Tray::OnUploadStart, this);
        Bind(EVT_UPLOAD_END, &Tray::OnUploadEnd, this);

#ifdef HAVE_APPINDICATOR
        _indicator = app_indicator_new("fu", "indicator-messages", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
        app_indicator_set_status(_indicator, APP_INDICATOR_STATUS_ACTIVE);
        CreateIndicatorMenu();
        Bind(wxEVT_UPDATE_UI, &Tray::OnUpdateMenu, this);
#else
        _iconPath = TheConfig.GetIconPath("16x16", "icon.png");
        _iconUploadingPath = TheConfig.GetIconPath("16x16", "icon_uploading.png");
        
        wxLogDebug("_iconPath: %s", _iconPath);
        _icon.LoadFile(_iconPath, wxBITMAP_TYPE_PNG);
        _iconUploading.LoadFile(_iconUploadingPath, wxBITMAP_TYPE_PNG);
#endif
        NormalIcon();
        wxLogDebug("tray icon ready");
    }

public:


#ifdef HAVE_APPINDICATOR
    void CreateIndicatorMenu()
    {
        if (_menu)
            delete _menu;
        _menu = CreatePopupMenu();
        app_indicator_set_menu(_indicator, GTK_MENU(_menu->m_menu));
    }

    void OnUpdateMenu(wxUpdateUIEvent &evt)
    {
        CreateIndicatorMenu();
    }

    void NormalIcon()
    {
        app_indicator_set_icon(_indicator, "fu");
    }

    void UploadingIcon()
    {
        app_indicator_set_icon(_indicator, "fu_uploading");
    }
#else

    void NormalIcon()
    {
        wxLogDebug("set icon: %s", _iconPath);
        SetIcon(_icon);
    }

    void UploadingIcon()
    {
        SetIcon(_iconUploading);
    }

#endif

    void FixMenuItem(wxMenuItem *item)
    {
#ifdef HAVE_APPINDICATOR
        g_connect(item);
#endif
    }

    void RefreshMenu()
    {
#ifdef HAVE_APPINDICATOR
        CreateIndicatorMenu();
#endif
    }

    void OnUploadStart(wxCommandEvent &evt)
    {
        uploadingCount++;
        RefreshIcon();
    }

    void OnUploadEnd(wxCommandEvent &evt)
    {
        uploadingCount--;
        RefreshIcon();
    }

    void RefreshIcon()
    {
        if (uploadingCount)
            UploadingIcon();
        else
            NormalIcon();
    }

    void CreateFileMenuItems(wxMenu *menu, const wxString &hintText, vector<File*> &files, int start)
    {
        wxMenuItem *hint = new wxMenuItem(menu, itemID_HINT, hintText);
        menu->Append(hint);
        hint->Enable(false);

        for (File *file : files)
        {
            wxMenuItem *item = new wxMenuItem(menu, start++, file->GetName());
            if (file->HasThumbnail())
            {
                item->SetBitmap(file->GetThumbnail());
            }

            menu->Append(item);
            FixMenuItem(item);
        }
    }

    wxMenu *CreatePopupMenu()
    {
        wxMenu *menu = new wxMenu();

        // pending
        _pending = TheClip.GetFiles();
        if (!_pending.empty())
        {
            CreateFileMenuItems(menu, "Pending", _pending, itemID_PENDING_START);
            if (_pending.size() > 1)
            {
                FixMenuItem(menu->Append(itemID_UPLOAD_ALL, "[Upload all]"));
            }
        }

        if (!_uploading.empty())
        {
            menu->AppendSeparator();
            CreateFileMenuItems(menu, "Uploading", _uploading, itemID_UPLOADING_START);
        }

        _uploaded = TheHistory.GetLatest();
        if (!_uploaded.empty())
        {
            menu->AppendSeparator();
            CreateFileMenuItems(menu, "Uploaded", _uploaded, itemID_UPLOADED_START);
            if (TheHistory.HasMore())
                FixMenuItem(menu->Append(itemID_BROWSE_HISTORY, "[More...]"));
        }

        // build sites;
        menu->AppendSeparator();
        wxMenuItem *siteHint = new wxMenuItem(menu, itemID_HINT, "Server");
        menu->Append(siteHint);
        siteHint->Enable(false);
        vector<Site*> sites = TheConfig.GetSites();
        if (sites.empty())
        {
            FixMenuItem(menu->Append(itemID_ADD_SITE, "[ADD A UPLOAD SERVER]"));
        }
        else
        {
            wxLogDebug("sites size: %zd", sites.size());
            int siteMenuId = itemID_SITE_START;
            for (auto const &site : sites)
            {
                wxMenuItem *item = new wxMenuItem(menu, siteMenuId++, site->GetName(), "", wxITEM_CHECK);
                menu->Append(item);
                item->Check(site == TheConfig.SiteSelected);
                FixMenuItem(item);
            }
        }

        // build up output format menuitems;
        menu->AppendSeparator();
        wxMenuItem *formatHint = new wxMenuItem(menu, itemID_HINT, "Output format");
        menu->Append(formatHint);
        formatHint->Enable(false);

        vector<Format*> formats = TheConfig.Formats;
        if (formats.empty())
        {
            FixMenuItem(menu->Append(itemID_ADD_FORMAT, "[ADD A NEW FORMAT]"));
        }
        else
        {
            int formatMenuId = itemID_FORMAT_START;
            for (auto const &format : formats)
            {
                wxMenuItem *item = new wxMenuItem(menu, formatMenuId++, format->GetName(), "", wxITEM_CHECK);
                menu->Append(item);
                item->Check(format == TheConfig.FormatSelected);
                FixMenuItem(item);
            }
        }

        menu->AppendSeparator();
        FixMenuItem(menu->Append(itemID_PREFERENCES, "Preferences"));
        FixMenuItem(menu->Append(itemID_EXIT, "Exit"));

        return menu;
    }

    void ShowPref()
    {
        _pref->Show();
        BringAppToTop();
    }

    void OnMenuItemSelected(wxCommandEvent &evt)
    {
        int itemId = evt.GetId();

        if (itemId == itemID_UPLOAD_ALL)
        {
            TheClip.Remove(_pending);
            auto uploader = new Uploader(this, _pending);
            uploader->Run();

            for (auto const &tmp : _pending)
                _uploading.push_back(tmp);
        }
        else if (itemId == itemID_BROWSE_HISTORY)
        {
            if (_browser == NULL)
                _browser = new BrowseForm(TheConfig.BrowseFormPosition, TheConfig.BrowseFormSize);

            _browser->Show();
            BringAppToTop();
        }
        else if (itemId == itemID_PREFERENCES)
        {
            ShowPref();
        }
        else if (itemId == itemID_EXIT)
        {
            TheConfig.Position = _pref->GetPosition();
            TheConfig.Size = _pref->GetSize();
            if (_browser != NULL)
            {
                TheConfig.BrowseFormPosition = _browser->GetPosition();
                TheConfig.BrowseFormSize = _browser->GetSize();
            }
            TheConfig.Save();
            wxExit();
        }
        else if (itemId == itemID_ADD_SITE)
        {
            _pref->GoSiteCreating();
            ShowPref();
        }
        else if (itemId == itemID_ADD_FORMAT)
        {
            _pref->GoFormatCreating();
            ShowPref();
        }
        else if (itemId >= itemID_SITE_START)
        {
            TheConfig.SiteSelected = TheConfig.GetSite(itemId - itemID_SITE_START);
            TheConfig.Save();
        }
        else if (itemId >= itemID_FORMAT_START)
        {
            TheConfig.FormatSelected = TheConfig.Formats[itemId - itemID_FORMAT_START];
            TheConfig.Save();
        }
        else if (itemId >= itemID_UPLOADED_START)
        {
            auto file = _uploaded[itemId - itemID_UPLOADED_START];
            TheClip.SetText({file});
        }
        else if (itemId >= itemID_UPLOADING_START)
        {
            wxLogDebug("uploading menu id : %d", itemId);
        }
        else if (itemId >= itemID_PENDING_START)
        {
            auto file = TheClip.Remove(itemId - itemID_PENDING_START);
            auto uploader = new Uploader(this, { file });
            uploader->Run();
            _uploading.push_back(file);
        }
        else
        {
            wxLogDebug("unknown menu id : %d", itemId);
        }
        RefreshMenu();
    }

    void OnError(wxCommandEvent &evt)
    {
        Toast("Error", evt.GetString(), true);

        vector<File*> uploading;
        for (auto const &file : _uploading)
        {
            if (file->GetStatus() == PENDING)
            {
                TheClip.Add(file);
            }
            else if (file->GetStatus() == UPLOADING)
            {
                uploading.push_back(file);
            }
        }
        _uploading = uploading;
        RefreshMenu();
    }

    void OnUploadSuccess(wxCommandEvent &evt)
    {
        TheClip.SetText(evt.GetString());
        vector<File*> uploaded, uploading;
        for (auto it = _uploading.begin(); it != _uploading.end(); ++it)
        {
            File *file = *it;
            if (file->GetStatus() == UPLOADED)
            {
                uploaded.push_back(file);
            }
            else if (file->GetStatus() == UPLOADING)
            {
                uploading.push_back(file);
            }
        }
        TheHistory.Push(uploaded);
        _uploading = uploading;
        Toast("Uploaded successfully", wxString::Format("Total %zd files uploaded", uploaded.size()));
        RefreshMenu();
    }

    static Tray &Inst()
    {
        static Tray inst;
        return inst;
    }
};

#endif
