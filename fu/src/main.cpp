#include <wx/wx.h>
#include <wx/snglinst.h>
#include "ui/tray.cpp"
#include "os/os.h"

class App : public wxApp
{
private:
    Tray *_tray;
    wxSingleInstanceChecker _singleInstChecker;
    
public:
    virtual bool OnInit()
    {
        if (_singleInstChecker.IsAnotherRunning())
        {
            return false;
        }
        
        wxInitAllImageHandlers();
        _tray = new Tray();
        
        Init(_tray);
        return true;
    }
};

wxIMPLEMENT_APP(App);
