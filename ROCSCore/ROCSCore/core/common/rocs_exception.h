#pragma once

#include "core/win32/declspec.h"

#include <stdexcept>
#include <string>
#include <iostream>
#include "exlib/format.h"

#define WHERESTR "[file %s, line %d]"
#define WHEREARG __FILE__, __LINE__

namespace cmn {

// http://msdn.microsoft.com/en-us/library/3tdb471s.aspx
MSC_DISABLE_WARNING(4275);

class ROCS_CORE_API ROCSException : public std::runtime_error {
public:
    inline ROCSException(const std::string& what): std::runtime_error(what) {}
};

MSC_RESTORE_WARNING(4275);

}
