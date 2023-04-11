#pragma once

#include "core/win32/declspec.h"

#include <set>
#include <string>
#include <cctype>
#include <algorithm>
#include <iterator>
#include "exlib/range.h"

namespace cmn {

ROCS_CORE_API std::string filter_filename(const std::string& filename);

}
