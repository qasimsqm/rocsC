#pragma once

/** 

Floating Point Compare
    
Comparison of two floating point numbers may not compare equal because of rounding
errors. Use equal_to to compare floating point numbers down to the limits of the machine. 

Requires C++11 

**/

#include "exlib/win32/declspec.h"

#include <cmath>
#include <limits>
#include <functional>
#include <type_traits>

namespace ex {

template<
    typename T,
    typename TMustBeFloatingPoint = typename std::enable_if<std::is_floating_point<T>::value>::type
>
struct equal_to : std::binary_function<T, T, bool>
{
    bool operator()(const T& first, const T& second)
    {
        return fabs(first - second) < std::numeric_limits<T>::epsilon();
    }
};

template<
    typename T,
    typename TMustBeFloatingPoint = typename std::enable_if<std::is_floating_point<T>::value>::type
>
struct not_equal_to : std::binary_function<T, T, bool>
{
    bool operator()(const T& first, const T& second)
    {
        return !(equal_to<T>()(first, second));
    }
};


template<typename T>
bool equal(const T& first, const T& second)
{
    return equal_to<T>()(first, second);
}

template<typename T>
bool not_equal(const T& first, const T& second)
{
    return not_equal_to<T>()(first, second);
}

}
