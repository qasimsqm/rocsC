#pragma once

#include "exlib/win32/declspec.h"

#include <sys/types.h>

#if defined(__APPLE__) || defined(__linux)
#include <dirent.h>

#if !(defined(DT_DIR) && defined(DT_REG))
    #error missing macros DT_DIR and DT_REG.
#endif
#elif defined(_WIN32)
#include "boost/filesystem/operations.hpp"
#include "boost/filesystem/path.hpp"
#endif

#include <string>
#include <vector>
#include <stdexcept>
#include "exlib/cxx11.h"
#include "exlib/ex_errno.h"

namespace ex {

// http://msdn.microsoft.com/en-us/library/3tdb471s.aspx
MSC_DISABLE_WARNING(4275);
class EXLIB_API ListDirError
    :
    public std::runtime_error
{
public:
    ListDirError(const std::string& what): std::runtime_error(what) {}
};

class EXLIB_API BadDir
    :
    public ListDirError
{
public:
    BadDir(const std::string& what) : ListDirError(what) {}
};

MSC_RESTORE_WARNING(4275);

EXLIB_API std::vector<std::string> listdir(const std::string& path);

}
