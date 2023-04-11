#pragma once

#include "core/win32/declspec.h"

#include <string>
#include "core/common/rocs_exception.h"

namespace CL {

class ROCS_CORE_API ChangeLogException : public cmn::ROCSException
{
public:
    inline ChangeLogException(const std::string& what): cmn::ROCSException(what) {}
};

}
