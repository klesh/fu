#include <wx/wx.h>
#include <wx/snglinst.h>
#include "ui/tray.cpp"
#include "os/os.h"

#include <wx/artprov.h>

class App : public wxApp
{
private:
    wxSingleInstanceChecker _singleInstChecker;
    Tray *_tray = NULL;

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
        _tray = new Tray();


        wxLogDebug("app start");
        Init(_tray);
        return true;
    }

    virtual int OnExit()
    {
        if (_tray != NULL)
            delete _tray;
        return 0;
    }

};

wxIMPLEMENT_APP(App);
