#pragma once

#include "exlib/win32/declspec.h"

#include <type_traits>

namespace ex {

template<template<typename> class X, typename T>
struct is_specialization_of : public std::false_type {};

template<template<typename> class X, typename Y>
struct is_specialization_of<X, X<Y>> : public std::true_type {};

} // namespace ex
