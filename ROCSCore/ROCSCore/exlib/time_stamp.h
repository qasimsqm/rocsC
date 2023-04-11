#pragma once

#include "exlib/win32/declspec.h"

#include <string>
#include <ctime>
#if defined(__linux) || defined(__APPLE__)
#include <time.h>
#endif

namespace ex {

EXLIB_API std::string current_time();

}
