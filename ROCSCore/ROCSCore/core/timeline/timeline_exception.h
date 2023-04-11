#pragma once

#include "core/win32/declspec.h"

#include <string>
#include "core/common/rocs_exception.h"

namespace TL {

class ROCS_CORE_API TimelineException : public cmn::ROCSException
{
public:
    inline TimelineException(const std::string& what): cmn::ROCSException(what) {}
};

} // namespace TL
