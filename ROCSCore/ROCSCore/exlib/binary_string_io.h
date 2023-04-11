#pragma once

#include "exlib/win32/declspec.h"

/**
    UInt8 show_name_size = is.get();
    char *c_show_name = new char[show_name_size];
    is.read(c_show_name, show_name_size);
    show_name_.assign(c_show_name, show_name_size);
    delete[] c_show_name;

**/

#include <string>
#include <istream>
#include <ostream>
#include <iostream>
#include <stdexcept>
#include "exlib/xplatform_types.h"

namespace ex {

EXLIB_API void WriteString(std::ostream &o, const std::string &s);
EXLIB_API std::string ReadString(std::istream &is);

}
