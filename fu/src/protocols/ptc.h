#ifndef H_PROTOCOL
#define H_PROTOCOL

#include <vector>
#include <wx/wx.h>
#include <wx/stream.h>

//#include "../core/file.cpp"

class Ptc
{
public:
    virtual bool Connect() = 0;
    virtual void Disconnect() = 0;
    virtual wxOutputStream *OpenStream(const wxString &remoteName) = 0;
    virtual ~Ptc() {}
    virtual const wxString &GetErrorMessage() { return wxEmptyString; };
};

#endif
