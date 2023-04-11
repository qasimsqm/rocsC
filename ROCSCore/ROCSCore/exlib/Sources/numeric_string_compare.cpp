#include "exlib/numeric_string_compare.h"
#include <vector>
#include <algorithm>
#include <locale>
#include "exlib/string_lib.h"

using namespace std;

namespace ex {

class SubString
{
public:
    SubString(const string& s, bool is_digit): str_(s), is_digit(is_digit)
    {
         if (is_digit) val = ex::str_to_num<int>(s);
    }

    bool operator<(const SubString& rhs) const {
        if (is_digit && rhs.is_digit) {
            return val < rhs.val;
        } else {
            return str_ < rhs.str_;
        }
    }

    bool operator>(const SubString& rhs) const {
        if (is_digit && rhs.is_digit) {
            return val > rhs.val;
        } else {
            return str_ > rhs.str_;
        }
    }

    friend ostream& operator<<(ostream& os, const SubString& rhs)
    {
        if (rhs.is_digit) os << rhs.val;
        else os << rhs.str_;
        return os;
    }


private:
    string str_;
    bool is_digit;
    int val;
};


class NumericStringParser
{
public:
    NumericStringParser(const string& s)
    {
        locale active_locale;
        string sub;
        sub.push_back(s[0]);
        bool sub_is_digit = std::isdigit(s[0], active_locale);

        for (auto it: s.substr(1))
        {
            if (sub_is_digit == std::isdigit(it, active_locale)) {
                sub.push_back(it);
            } else {
                subs.push_back(SubString(sub, sub_is_digit));
                sub.assign(&it, 1);
                sub_is_digit = std::isdigit(it, active_locale);
            }
        }


        subs.push_back(SubString(sub, sub_is_digit));
    }

    bool operator<(const NumericStringParser& rhs) const
    {
        size_t truncate = min(subs.size(), rhs.subs.size());
        vector<SubString>::const_iterator this_it, rhs_it;

        this_it = subs.begin();
        rhs_it = rhs.subs.begin();
        for (unsigned int i = 0; i < truncate; i++) {
            if (*this_it < *rhs_it) {
                return true;
            } else if (*this_it > *rhs_it) {
                return false;
            }
            this_it++;
            rhs_it++;
        }

        if (subs.size() < rhs.subs.size()) {
            return true;
        } else {
            return false;
        }
    }

private:
    vector<SubString> subs;
};


bool NumericStringCompare::operator()(const string& first, const string& second) const
{
    NumericStringParser f1(first);
    NumericStringParser s1(second);
    return f1 < s1;
}


} // namespace ex
