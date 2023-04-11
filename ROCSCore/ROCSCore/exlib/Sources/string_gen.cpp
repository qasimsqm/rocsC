#include "exlib/string_gen.h"

using namespace std;

namespace ex
{

class EXLIB_API Pred
{
public:
    Pred(int (*func)(int c)): func(func) {}
    bool operator()(char c) { return (func(c) == 0); }
private:
    int (*func)(int c);
};


string StrGen::subset(const Pred& filter) {
    string result;
    remove_copy_if(ascii_chars_.begin(), ascii_chars_.end(), back_inserter(result), filter);
    return result;
}

string StrGen::alnum() { return subset(Pred(isalnum)); }
string StrGen::alpha() { return subset(Pred(isalpha)); }
string StrGen::cntrl() { return subset(Pred(iscntrl)); }
string StrGen::digit() { return subset(Pred(isdigit)); }
string StrGen::graph() { return subset(Pred(isgraph)); }
string StrGen::lower() { return subset(Pred(islower)); }
string StrGen::print() { return subset(Pred(isprint)); }
string StrGen::punct() { return subset(Pred(ispunct)); }
string StrGen::space() { return subset(Pred(isspace)); }
string StrGen::upper() { return subset(Pred(isupper)); }
string StrGen::xdigit() { return subset(Pred(isxdigit)); }
string StrGen::ascii_chars() { return ascii_chars_; }

int StrGen::is_upper_or_digit(int c) { return (isupper(c) || isdigit(c)); }
int StrGen::is_lower_or_digit(int c) { return (islower(c) || isdigit(c)); }
string StrGen::upper_or_digit() { return subset(Pred(is_upper_or_digit)); }
string StrGen::lower_or_digit() { return subset(Pred(is_lower_or_digit)); }

string create_ascii_chars()
{
    char chars[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87, 88, 89, 90, 91, 92, 93, 94, 95, 96, 97, 98, 99, 100, 101, 102, 103, 104, 105, 106, 107, 108, 109, 110, 111, 112, 113, 114, 115, 116, 117, 118, 119, 120, 121, 122, 123, 124, 125, 126, 127};
    string ascii_chars(chars, 128);
    return ascii_chars;
}

string StrGen::ascii_chars_ = create_ascii_chars();

}
