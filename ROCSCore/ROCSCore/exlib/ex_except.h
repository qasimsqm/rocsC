#pragma once

#include "exlib/win32/declspec.h"

#include <stdexcept>

namespace ex {

class ExlibRuntimeError : public std::runtime_error
{
public:
    ExlibRuntimeError(const std::string& what): std::runtime_error(what) {}
};

class NotImplementedError : public std::logic_error
{
public:
    NotImplementedError(const std::string& what): std::logic_error(what) {}
};

}
