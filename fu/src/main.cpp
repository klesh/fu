#include <wx/wx.h>
#include <wx/snglinst.h>
#include "ui/tray.cpp"

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
        
        return true;
    }
};

wxIMPLEMENT_APP(App);
