#pragma once

#include "exlib/win32/declspec.h"

#include <string>
#include <map>

namespace ex
{

struct EXLIB_API NumericStringCompare
{
    bool operator()(const std::string& first, const std::string& second) const;
};

typedef std::map<std::string, std::string, NumericStringCompare> SortedStringMapT;


} // namespace ex
