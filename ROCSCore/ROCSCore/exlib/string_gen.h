#pragma once

#include "exlib/win32/declspec.h"

#include <string>
#include <ctype.h>
#include <algorithm>
#include <iterator>

namespace ex {

class EXLIB_API Pred;

class EXLIB_API StrGen {
public:
    static std::string alnum();
    static std::string alpha();
    static std::string cntrl();
    static std::string digit();
    static std::string graph();
    static std::string lower();
    static std::string print();
    static std::string punct();
    static std::string space();
    static std::string upper();
    static std::string xdigit();
    static std::string ascii_chars();
    static std::string upper_or_digit();
    static std::string lower_or_digit();

private:
    MSC_DISABLE_WARNING(4251);
    static std::string ascii_chars_;
    static std::string subset(const Pred& filter);
    MSC_RESTORE_WARNING(4251);
    static int is_upper_or_digit(int c);
    static int is_lower_or_digit(int c);
};

}
