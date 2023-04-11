#pragma once

#include "exlib/win32/declspec.h"

/** Include this header to insure that the same data types are
    available on both Windows and Mac.

    Windows Data Types
    http://msdn.microsoft.com/en-us/library/windows/desktop/aa383751(v=vs.85).aspx

**/

#ifdef __APPLE__

#include <MacTypes.h>

#elif defined(_WIN32)
#include <BaseTsd.h>

typedef UINT8	    UInt8;
typedef UINT16 	    UInt16;
typedef UINT32      UInt32;
typedef UINT64      UInt64;

typedef INT8 	    SInt8;
typedef INT16 	    SInt16;
typedef INT32       SInt32;
typedef INT64       SInt64;
typedef long double Float64;

#else
#include <cstdint>

typedef uint8_t     UInt8;
typedef uint16_t    UInt16;
typedef uint32_t    UInt32;
typedef uint64_t    UInt64;

typedef int8_t      SInt8;
typedef int16_t     SInt16;
typedef int32_t     SInt32;
typedef int64_t     SInt64;


#endif

typedef UInt8 Byte;

#include <climits>

const UInt64 MAX_UINT64 = ULLONG_MAX;
const UInt64 MAX_UINT32 = 4294967295;
const UInt64 MAX_UINT16 = 65535;
const UInt64 MAX_UINT8 = 255;
const SInt64 MAX_SINT64 = 9223372036854775807;
const SInt64 MIN_SINT64 = -9223372036854775807;
const SInt64 MAX_SINT32 = 2147483647;
const SInt64 MIN_SINT32 = -2147483647;
const SInt64 MAX_SINT16 = 32767;
const SInt64 MIN_SINT16 = -32767;
const SInt64 MAX_SINT8 = 127;
const SInt64 MIN_SINT8 = -127;
