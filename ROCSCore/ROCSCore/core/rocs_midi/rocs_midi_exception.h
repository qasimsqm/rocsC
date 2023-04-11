#pragma once

#include "core/win32/declspec.h"

#include <string>
#include "core/common/rocs_exception.h"

namespace rocs_midi {

class ROCS_CORE_API ROCSMIDIException : public cmn::ROCSException
{
public:
    inline ROCSMIDIException(const std::string& what): cmn::ROCSException(what) {}
};

}
