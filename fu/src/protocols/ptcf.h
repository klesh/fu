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
#include "ftp/ftpp.cpp"

#define ThePtcFactory PtcFactory::Inst()

using namespace std;
class PtcFactory
{
private:
    map<wxString, PtcProvider*> _all;
    vector<wxString> _allKeys;

    PtcFactory()
    {
        AddProvider(new LocalProvider());
        AddProvider(new SftpProvider());
        AddProvider(new ImgurProvider());
        AddProvider(new SmmsProvider());
        AddProvider(new FtpProvider());
    }

    void AddProvider(PtcProvider *provider)
    {
        _all[provider->GetName()] = provider;
        _allKeys.push_back(provider->GetName());
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
