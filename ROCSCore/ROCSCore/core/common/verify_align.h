#pragma once

#include "core/win32/declspec.h"

/*  Verify the alignment of critical data types
    This library uses direct serialization of data, so data alignment is critical.
    To use this library, your target architecture must pass these checks.

For a write up of this technique see:
http://www.eetimes.com/discussion/programming-pointers/4025549/Catching-errors-early-with-compile-time-assertions


*/



#define compile_time_assert(cond, msg) \
    typedef char msg[(cond) ? 1:-1]

ROCS_CORE_API compile_time_assert(sizeof(char) == 1, sizeof_char_must_be_1);
ROCS_CORE_API compile_time_assert(__alignof(char) == 1, alignof_char_must_be_1);

ROCS_CORE_API compile_time_assert(sizeof(short) == 2, sizeof_short_must_be_2);
ROCS_CORE_API compile_time_assert(__alignof(short) == 2, alignof_short_must_be_2);

ROCS_CORE_API compile_time_assert(sizeof(int) == 4, sizeof_int_must_be_4);
ROCS_CORE_API compile_time_assert(__alignof(int) == 4, alignof_int_must_be_4);

ROCS_CORE_API compile_time_assert(sizeof(float) == 4, sizeof_float_must_be_4);
ROCS_CORE_API compile_time_assert(__alignof(float) == 4, align_of_float_must_be_4);
