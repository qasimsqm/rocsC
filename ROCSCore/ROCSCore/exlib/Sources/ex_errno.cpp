#include "exlib/ex_errno.h"

using namespace std;

namespace ex {

EXLIB_API string errmsg(int errnum)
{
    char buffer[256];

#if defined(_WIN32)
    auto result = strerror_s<sizeof buffer>(buffer, errnum);
#else
    char *buf_ptr = &buffer[0];
    auto result = strerror_r(errnum, buf_ptr, sizeof buffer);
#endif

    if (result == 0) {
        return string(buffer, sizeof buffer);
    } else {
        return string();
    }
}


}
