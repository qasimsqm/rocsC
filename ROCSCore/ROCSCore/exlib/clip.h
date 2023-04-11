#pragma once

#include <algorithm>

#ifdef min
#undef min
#endif

#ifdef max
#undef max
#endif

namespace ex
{

template <typename T>
T clip(T in, T low, T high)
{
    return std::min(std::max(in, low), high);
}

}
