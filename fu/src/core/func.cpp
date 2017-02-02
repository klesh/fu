#ifndef H_CORE_FUNC
#define H_CORE_FUNC

#include <wx/wx.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>

inline wxString Uuid()
{
    boost::uuids::uuid uuid = boost::uuids::random_generator()();
    return wxString(boost::uuids::to_string(uuid).c_str(), wxConvUTF8);
}

#endif
