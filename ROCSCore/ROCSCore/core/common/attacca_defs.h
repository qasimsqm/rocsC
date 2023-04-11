#pragma once

#include "core/win32/declspec.h"

#include <string>
#include <map>
#include "exlib/xplatform_types.h"

namespace attacca {

const UInt8 PLAY = 0;
const UInt8 ATTACCA = 1;
const UInt8 SKIPS = 2;

typedef std::map<UInt8, std::string> NamesT;
ROCS_CORE_API extern NamesT names;

} // namespace attacca
