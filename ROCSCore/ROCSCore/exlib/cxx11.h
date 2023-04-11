#pragma once

#include "exlib/win32/declspec.h"

#ifdef __cplusplus
    // The current preview of Visual Studio 2013 still defines __cplusplus to be 199711L.
    #if __cplusplus >= 201103L || defined(_MSC_VER)
        #define CXX11
    #else
        #define nullptr NULL
    #endif
#endif
