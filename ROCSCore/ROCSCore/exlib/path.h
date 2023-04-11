#pragma once

#include "exlib/win32/declspec.h"

#include <string>
#include <vector>
#include <utility>
#include <functional>
#include "exlib/string_lib.h"
#include "exlib/ex_except.h"
#include <initializer_list>

#ifndef _WIN32
#include <sys/stat.h>
#include <errno.h>
#include <map>
#endif 

namespace ex {
namespace path {

#if defined(_WIN32)
    const char PATH_SEP = '\\';
#else
    const char PATH_SEP = '/';
#endif

class PathError : public ex::ExlibRuntimeError
{
public:
    PathError(const std::string &what): ex::ExlibRuntimeError(what) {}
};


// InputIterator must dereference to a std::string, or to an object that has
// rbegin, begin, and end.
template< class InputIterator >
std::string join(InputIterator first, InputIterator last)
{
    std::vector<std::string> result;
    while (first != last) {
        if (*first->rbegin() == PATH_SEP) {
            result.push_back(std::string(first->begin(), first->end() - 1));
        } else {
            result.push_back(*first);
        }
        first++;
    }

    return ex::join_string(result, std::string(1, PATH_SEP));
}

EXLIB_API std::string join(std::initializer_list<std::string> il);

EXLIB_API std::string join(const std::string& path1, const std::string& path2);

EXLIB_API std::string basename(const std::string& filename);

EXLIB_API std::string dirname(const std::string& filename);

EXLIB_API std::pair<std::string, std::string> split(const std::string& filename);

EXLIB_API std::pair<std::string, std::string> splitext(const std::string& filename);

EXLIB_API std::string basename_no_ext(const std::string& filename);

EXLIB_API std::string extension(const std::string& filename);

// This function should have the same result as calling `mkdir -p pathname`
#ifndef _WIN32
EXLIB_API void makedirs(const std::string &pathname);
#endif

// Use a lambda instead of ExtensionFilter.  It was created when lambdas were
// not as well supported.
// std::copy_if(iterator begin, iterator end, iterator dest, ExtensionFilter(".ext");
class EXLIB_API ExtensionFilter : public std::unary_function<const std::string&, bool>
{
private:
    MSC_DISABLE_WARNING(4251); 
    std::string ext;
    MSC_RESTORE_WARNING(4251);

public:
    ExtensionFilter(const std::string& ext) : ext(ext) {}
    bool operator()(const std::string& filename)
    {
        return extension(filename) == ext;
    }
};

} // namespace path
} // namespace ex
