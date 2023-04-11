#pragma once

#include "exlib/win32/declspec.h"

#include <vector>
#include <algorithm>
#include <cstring>

namespace ex {

template< class T >
T reverse_byte_order(T& val) {
    std::vector<char> bytes(sizeof(T), 0);
    memcpy((char *)&bytes[0], (char *)&val, sizeof(T));
    std::reverse(bytes.begin(), bytes.end());
    memcpy((char *)&val, (char *)&bytes[0], sizeof(T));
    return val;
}

template< class T >
T reverse_byte_order(const T& val) {
    T ret = 0;
    std::vector<char> bytes(sizeof(T), 0);
    memcpy((char *)&bytes[0], (char *)&val, sizeof(T));
    std::reverse(bytes.begin(), bytes.end());
    memcpy((char *)&ret, (char *)&bytes[0], sizeof(T));
    return ret;
}

template< class T >
T reverse_byte_order_const(const T& val) {
    T ret = 0;
    std::vector<char> bytes(sizeof(T), 0);
    memcpy((char *)&bytes[0], (char *)&val, sizeof(T));
    std::reverse(bytes.begin(), bytes.end());
    memcpy((char *)&ret, (char *)&bytes[0], sizeof(T));
    return ret;
}

}
