#ifndef H_PROTOCOL_FACTORY
#define H_PROTOCOL_FACTORY

#include <wx/wx.h>
#include <vector>
#include <map>

#include "ptc.h"
#include "ptcp.h"
#include "sftp/sftpp.cpp"
#include "local/localp.cpp"
#include "imgur/imgurp.cpp"
#include "smms/smmsp.cpp"

#define ThePtcFactory PtcFactory::Inst()

using namespace std;
class PtcFactory
{
private:
    map<wxString, PtcProvider*> _all;
    vector<wxString> _allKeys;
    
    PtcFactory()
    {
        _all["local"] = new LocalProvider();
        _all["sftp"] = new SftpProvider();
        _all["imgur.com"] = new ImgurProvider();
        _all["sm.ms"] = new SmmsProvider();
        
        for (auto const &it : _all)
            _allKeys.push_back(it.first);
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
