#pragma once

#include "core/win32/declspec.h"

#include <string>
#include <vector>
#include <list>
#include <set>
#include <map>
#include <algorithm>
#include <stdexcept>

#include "exlib/cxx11.h"
#ifdef CXX11
    #include <memory>
#else
    #include <tr1/memory>
#endif

#include "exlib/string_lib.h"
#include "exlib/xplatform_types.h"
#include "exlib/format.h"
#include "core/common/warnings.h"
#include "core/common/codes.h"
#include "core/changelog/cl_events.h"
#include "core/changelog/change_log_exception.h"
#include "core/std_midi/meta_messages.h"


/*
Tokens read by this function
@b, @c, @m, @vs, @ve, @rs, @re, @f, @j

Revision 12/2013:
@c: applies to ShowReady, StageTracks, and FillHarmonic
@c1: applies to ShowReady
@c2: applies to StageTracks/FillHarmonic
@in: Applies to ShowReady and FillHarmonic
@in1: applies to ShowReady
@in2: applies to FillHarmonic
@out applies to ShowReady and FillHarmonic
@out1 applies to ShowReady.
@out2 applies to FillHarmonic
*/

namespace CL {
typedef cmn::ROCSEvtPtrT (*GenerateChangeLogEventFunc)(UInt32, const std::string&);
typedef std::map<std::string, GenerateChangeLogEventFunc> GenerateEventFuncByTokenT;
typedef std::map<UInt8, std::set<std::string>> TokensByEventCodeT;
ROCS_CORE_API extern GenerateEventFuncByTokenT generateEventFuncByToken;
extern std::set<std::string> validTokens;
ROCS_CORE_API extern TokensByEventCodeT tokensByEventCode;

class ROCS_CORE_API ReadMarkerException : public ChangeLogException
{
public:
    ReadMarkerException(const std::string& what): ChangeLogException(what) {}
};

class ROCS_CORE_API UnknownMarkerToken : public ReadMarkerException
{
public:
    UnknownMarkerToken(const std::string& what): ReadMarkerException(what) {}
};

class ROCS_CORE_API ArgError : public ReadMarkerException
{
public:
    ArgError(const std::string& what): ReadMarkerException(what) {}
};


ROCS_CORE_API 
std::list<cmn::ROCSEvtPtrT> read_marker(UInt32 abs_time, const std_midi::Marker& marker); 

template <class ROCSEventT>
std::shared_ptr<ROCSEventT> read_marker_single(
    UInt32 abs_time,
    const std_midi::Marker& marker)
{
    using namespace std;
    typedef shared_ptr<ROCSEventT> ReturnT;
    if (!tokensByEventCode.count(ROCSEventT::evt_code))
    {
        throw logic_error(ex::format(
            "markers do not contain events with evt_code=%d",
            ROCSEventT::evt_code));
    }

    ReturnT ret;

    auto parts = ex::tokenize_string(
        validTokens.begin(),
        validTokens.end(),
        marker.message());

    if (!count_if(
        parts.begin(),
        parts.end(),
        [](const std::string& s) -> bool
        {
            return tokensByEventCode[ROCSEventT::evt_code].count(s) != 0;
        }))
    {
        return ret;
    }

    while(parts.size())
    {
        if (tokensByEventCode[ROCSEventT::evt_code].count(parts[0]))
        {
            try
            {
                ret = std::dynamic_pointer_cast<ROCSEventT>(
                    generateEventFuncByToken[parts[0]](abs_time, parts[1]));
            } catch(ArgError& e)
            {
                throw ArgError(e.what() + ex::format(" at %u", abs_time));
            }

            return ret;
        }

        parts.pop_front();
        parts.pop_front();
    }

    #ifndef NDEBUG
        // This point of the code should not be reached...
        throw logic_error("read_marker_single fell through without finding event");
    #endif
    return ret;
}

} // End namespace CL
