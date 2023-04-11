#pragma once

#include "exlib/win32/declspec.h"
#include <ctime>
#include <string>
#include "exlib/xplatform_types.h"
#include "exlib/cout_buffer.h"

namespace ex
{

template<class FUNCTION_T>
Float64 Timer(FUNCTION_T functionToTime)
{
    std::clock_t start;
    Float64 duration;
    start = std::clock();

    functionToTime();

    duration = (std::clock() - start) / static_cast<double>(CLOCKS_PER_SEC);
    return duration;
}

template<class FUNCTION_T>
Float64 LogTimer(FUNCTION_T functionToTime, const std::string& description = "")
{
    Float64 execTime = Timer(functionToTime);
    ex::CoutBuffer() << "Time to " << description << ": " << execTime << ex::endl;
    return execTime;
}

}
