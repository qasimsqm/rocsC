#pragma once

#include "exlib/win32/declspec.h"

#include <errno.h>
#include <cstdio>
#include <cstring>
#include <string>
#include <stdexcept>
#include "exlib/format.h"

namespace ex {

// http://msdn.microsoft.com/en-us/library/3tdb471s.aspx
MSC_DISABLE_WARNING(4275);

class EXLIB_API NOERRError : public std::runtime_error
{
public:
    NOERRError(const std::string& what)
        :
        std::runtime_error(what)
    {

    }
    
    NOERRError(int errnum, const std::string& what)
        :
        std::runtime_error(what),
        errnum_(errnum)
    {
    
    }
    
    int errnum()
    {
        return errnum_;
    }

private:
    int errnum_;
};

MSC_RESTORE_WARNING(4275);

}

#if defined(__linux__) || defined(__APPLE__)
#define NOERR(errnum, msg) \
    if (errnum != 0) { \
        char _temp_error_string[256]; \
        strerror_r(errnum, _temp_error_string, sizeof(_temp_error_string)); \
        throw ex::NOERRError(errnum, ex::format("%s: %d %s", msg, errnum, _temp_error_string)); \
    }
#elif defined(_WIN32)
#define NOERR(errnum, msg) \
    if (errnum != 0) { \
        char _temp_error_string[256]; \
        strerror_s(_temp_error_string, errnum); \
        throw ex::NOERRError(errnum, ex::format("%s: %d %s", msg, errnum, _temp_error_string)); \
    }
#endif // _WIN32

#define SOFTERR(errnum, msg) \
    try { NOERR(errnum, msg); } catch (ex::NOERRError& e) { cout << e.what() << endl; }

namespace ex {

EXLIB_API std::string errmsg(int errnum);

}
