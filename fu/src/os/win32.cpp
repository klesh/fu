#ifndef H_OS_WIN32
#define H_OS_WIN32

#include "os.h"

void Init()
{

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
    wxMessageBox(message, title);
}

bool GetAutoStart()
{
    return false;
}

void SetAutoStart(bool enabled)
{

}

#endif // !H_OS_WIN32