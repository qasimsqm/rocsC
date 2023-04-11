#pragma once

#ifdef _WIN32

#include "exlib/win32/declspec.h"

#include <pthread.h>

// A rough approximation of nanosleep using millisecond resolution.
EXLIB_API int nanosleep(timespec *req, timespec *rem);

#endif // _WIN32
