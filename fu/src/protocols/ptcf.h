#ifndef H_PROTOCOL_FACTORY
#define H_PROTOCOL_FACTORY

#include <wx/wx.h>
#include <vector>
#include <map>

#include "ptc.h"
#include "ptcp.h"
#include "sftp/sftp.cpp"
#include "sftp/sftpp.cpp"
#include "local/local.cpp"
#include "local/localp.cpp"

#define ThePtcFactory PtcFactory::Inst()

using namespace std;
class PtcFactory
{
private:
    map<wxString, PtcProvider*> _all;
    vector<wxString> _allKeys;
    
    PtcFactory()
    {
        _all.insert(pair<wxString, PtcProvider*>("local", (PtcProvider*)new LocalProvider()));
        _all.insert(pair<wxString, PtcProvider*>("sftp", (PtcProvider*)new SftpProvider()));
        _allKeys.push_back("local");
        _allKeys.push_back("sftp");
    }
    
public:
    const map<wxString, PtcProvider*> &All()
    {
        return _all;
    }
    
    const vector<wxString> &AllKeys()
    {
        return _allKeys;
    }
    
    PtcProvider* Get(wxString &name)
    {
        return _all[name];
    }
    
    static PtcFactory &Inst()
    {
        static PtcFactory factory;
        return factory;
    }
};

#endif
