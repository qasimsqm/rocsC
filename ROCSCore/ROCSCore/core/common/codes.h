#pragma once

#include "core/win32/declspec.h"

#include <string>
#include <map>
#include <set>
#include "exlib/xplatform_types.h"

namespace codes {

enum {
    vamp =          0x40,
    repeat =        0x41,
    cut =           0x42,
    transpose =     0x43,
    tempo_scale =   0x44,
    marker =        0x45,
    bar =           0x4B,
    click =         0x4C,
    caesura =       0x4E,
    tempo =         0x46,
    bars_beats =    0x47,
    key =           0x48,
    page_num =      0x49,
    meter =         0x4A,
    fermata =       0x4D,
    inPoint =       0x4F,
    outPoint =      0x50
};

typedef std::map<Byte, std::string> CodeMapT;

#ifdef SWIG
extern CodeMapT code_names;
extern std::set<Byte> paired_event_codes;
#else
ROCS_CORE_API extern CodeMapT code_names;
ROCS_CORE_API extern std::set<Byte> paired_event_codes;
#endif
}
