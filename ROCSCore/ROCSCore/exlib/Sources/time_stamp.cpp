#include "exlib/time_stamp.h"

using namespace std;

namespace ex {

EXLIB_API string current_time()
{
    time_t rawtime;
    struct tm timeinfo;
    char time_buffer[26];

    time(&rawtime);

#if defined(_WIN32)
    localtime_s(&timeinfo, &rawtime);
    asctime_s(time_buffer, sizeof(time_buffer), &timeinfo);
    return string(time_buffer);
#elif defined(__APPLE__) || defined(__linux)
    localtime_r(&rawtime, &timeinfo);
    asctime_r(&timeinfo, time_buffer);
    return string(time_buffer);
#else
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    return string(asctime(timeinfo));
#endif
}

}
