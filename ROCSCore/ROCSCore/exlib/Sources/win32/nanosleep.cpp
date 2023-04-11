#include "exlib/win32/nanosleep.h"

#ifdef _WIN32

#include <Windows.h>
#include <winbase.h>

#define NANOSECONDS_PER_MILLISECOND 1000000
#define MILLISECONDS_PER_SECOND 1000

EXLIB_API int nanosleep(timespec *req, timespec *rem)
{
    DWORD durationMilliseconds = static_cast<DWORD>(
        req->tv_sec * MILLISECONDS_PER_SECOND
        + req->tv_nsec / NANOSECONDS_PER_MILLISECOND);
    Sleep(durationMilliseconds);
    return 0;
}

#endif // _WIN32

