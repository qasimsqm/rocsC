#pragma once

#include "exlib/win32/declspec.h"

#include <istream>
#include <ostream>
#include "exlib/binary_string_io.h"

namespace ex {

template< typename T >
T read(std::istream& is)
{
    T obj;
    is.read((char *)&obj, sizeof(T));
    return obj;
}

template< typename T >
void write(std::ostream& os, const T& obj)
{
    os.write((char *)&obj, sizeof(T));
}

}
