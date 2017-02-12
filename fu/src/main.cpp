#include <wx/wx.h>
#include <wx/snglinst.h>
#include "ui/tray.cpp"
#include "os/os.h"

#include <wx/artprov.h>

class App : public wxApp
{
private:
    wxSingleInstanceChecker _singleInstChecker;
    
public:
    virtual bool OnInit()
    {
        if (_singleInstChecker.IsAnotherRunning())
        {
            return false;
        }
        
        if (!Tray::IsAvailable())
        {
            wxMessageBox("Your OS doesn't support Task Bar Icon.");
            return false;
        }
        
        wxInitAllImageHandlers();

        Init(&Tray::Inst());
        return true;
    }
};

wxIMPLEMENT_APP(App);
