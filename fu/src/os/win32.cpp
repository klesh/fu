#ifndef H_OS_WIN32
#define H_OS_WIN32

#include "os.h"
#include <wx/notifmsg.h>

void Init(wxTaskBarIcon *tray)
{
    wxNotificationMessage::UseTaskBarIcon(tray);
}

bool IsClipboardChanged()
{
    static DWORD last = 0;
    auto current = GetClipboardSequenceNumber();
    bool isChanged = current != last;
    last = current;
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
    notification->Show(isError ? wxNotificationMessage::Timeout_Never : 3);
}

bool GetAutoStart()
{
    return false;
}

void SetAutoStart(bool enabled)
{

}

#endif // !H_OS_WIN32