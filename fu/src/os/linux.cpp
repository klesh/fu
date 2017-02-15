#include <gtk/gtk.h>
#include "os.h"

static bool clipChanged = true;
static wxTaskBarIcon *_tray = NULL;

void handle_owner_change(GtkClipboard *clipboard,
                         GdkEvent *event,
                         gpointer data)
{
    // Avoid 'unused arg' warnings.
    (void)event;
    (void)data;

    wxLogDebug("==== clipboard changed");
    clipChanged = true;

    wxUpdateUIEvent evt;
    wxPostEvent(_tray, evt);
}


void Init(wxTaskBarIcon *tray)
{
    GtkClipboard* clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
    g_signal_connect(clipboard, "owner-change", G_CALLBACK(handle_owner_change), NULL);
    _tray = tray;
}

bool IsClipboardChanged()
{
    bool isChanged = clipChanged;
    clipChanged = !clipChanged;
    return isChanged;
}

void BringAppToTop()
{

}

void Toast(const wxString &title, const wxString& message, bool isError)
{
    auto notification = new wxNotificationMessage(title, message);
    if (isError)
    {
        notification->SetFlags(wxICON_ERROR);
    }
    notification->Show(isError ? wxNotificationMessage::Timeout_Never : 3);}

bool GetAutoStart()
{
    return false;
}

void SetAutoStart(bool enabled)
{
}
