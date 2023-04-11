#pragma once

#include "core/win32/declspec.h"

#include <istream>
#include <stdexcept>
#include <string>
#include <sstream>
#include "exlib/xplatform_types.h"
#include "exlib/format.h"

namespace std_midi {

// http://msdn.microsoft.com/en-us/library/3tdb471s.aspx
// C4275 can be ignored in Visual C++ if you are deriving from a type in the Standard C++ Library
MSC_DISABLE_WARNING(4275);
class ROCS_CORE_API StdMIDIError : public std::runtime_error
{
public:
    StdMIDIError(const std::string& what): std::runtime_error(what) {}
};
MSC_RESTORE_WARNING(4275);

/*  Reads numbers stored as a VARIABLE-LENGTH QUANTITY.
    Each byte has bit 7 set, except for the last byte.  */
ROCS_CORE_API UInt32 read_variable_int(std::istream& is);



}
