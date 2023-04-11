#include "exlib/format.h"

#include <memory>

using namespace std;

namespace ex {

// vsnprintf is considered unsafe, but we're using it in a safe way and
// there's not a better alternative.
// Disable the vsnprintf warning in MS Visual C++.
int vformat(char *targetBuffer, size_t targetBufferSize, const char *fmt, va_list args)
{
MSC_DISABLE_WARNING(4996);
    return vsnprintf(targetBuffer, targetBufferSize, fmt, args);
MSC_RESTORE_WARNING(4996);
}

// According to http://www.cplusplus.com/reference/cstdarg/va_start/ the named
// argument passed to va_start cannot be a reference. Apparently Visual C++ is
// sensitive to this. We should probably switch this to a const string * or a
// const char *. -- Woody
EXLIB_API string format(string fmt, ...)
{
    char cstr_stack[DEFAULT_FORMATTED_LENGTH];
    char *cstr_ptr = cstr_stack;

    va_list args;
    va_start(args, fmt);
    
    MSC_DISABLE_WARNING(4996); 
    int formatted_length =
        vsnprintf(cstr_ptr, DEFAULT_FORMATTED_LENGTH, fmt.c_str(), args);
    va_end(args);
    
    int required_buffer_size = formatted_length + 1;

    std::unique_ptr<char> cstr_heap;
    if (required_buffer_size > DEFAULT_FORMATTED_LENGTH)
    {
        // cstr_stack wasn't big enough. Allocate one on the heap.
        cstr_heap = std::unique_ptr<char>(new char[required_buffer_size]);
        cstr_ptr = cstr_heap.get();

        va_start(args, fmt);
        vsnprintf(cstr_ptr, required_buffer_size, fmt.c_str(), args);
        va_end(args);
    }
    
    MSC_RESTORE_WARNING(4996);

    string formatted(cstr_ptr, formatted_length);

    return formatted;
}

EXLIB_API string format(const char *fmt, ...)
{
    char cstr_stack[DEFAULT_FORMATTED_LENGTH];
    char *cstr_ptr = cstr_stack;

    va_list args;
    va_start(args, fmt);
    int formatted_length =
        vformat(cstr_ptr, DEFAULT_FORMATTED_LENGTH, fmt, args);
    va_end(args);

    int required_buffer_size = formatted_length + 1;

    std::unique_ptr<char> cstr_heap;
    if (required_buffer_size > DEFAULT_FORMATTED_LENGTH)
    {
        // cstr_stack wasn't big enough. Allocate one on the heap.
        cstr_heap = std::unique_ptr<char>(new char[required_buffer_size]);
        cstr_ptr = cstr_heap.get();

        va_start(args, fmt);
        vformat(cstr_ptr, required_buffer_size, fmt, args);
        va_end(args);
    }

    string formatted(cstr_ptr, formatted_length);

    return formatted;
}

}
