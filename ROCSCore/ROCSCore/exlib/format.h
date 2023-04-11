#pragma once

/** Provides the same functionality as sprintf, but stores the result in a string.  If
    the resulting string is longer than DEFAULT_FORMAT_LENGTH, a heap allocation will
    occur, otherwise, all work is done on the stack.

    To use,
    format(const std::string& fmt, ...) **/

#include "exlib/win32/declspec.h"

#include <string>
#include <cstdio>  // vsnprintf
#include <cstdarg> // va_list

namespace ex {

#define DEFAULT_FORMATTED_LENGTH 256

EXLIB_API std::string format(std::string fmt, ...);

EXLIB_API std::string format(const char *fmt, ...);

}
