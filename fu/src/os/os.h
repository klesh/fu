#ifndef H_OS_OS
#define H_OS_OS 

#include <wx/wx.h>

void Init();

bool IsClipboardChanged();

void BringAppToTop();

void Toast(const wxString &title, const wxString& message, bool isError = false);

bool GetAutoStart();

void SetAutoStart(bool enabled);

#endif
