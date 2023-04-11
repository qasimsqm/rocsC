#include "exlib/string_lib.h"

using namespace std;

namespace ex
{

EXLIB_API bool endswith(const string& to_test, const string& ending)
{
    size_t found = to_test.rfind(ending);
    if (found == string::npos) return false;
    if (found == to_test.size() - ending.size()) {
        return true;
    } else {
        return false;
    }
}


/*  The input str will be split on every occurrence of token.  The returned vector<string>
    does not contain any instance of token. */
EXLIB_API vector<string> split_string(const string& str, const string& token, int limit)
{
    string::size_type position = 0;
    int token_size = token.size();
    if (token_size == 0) {
        throw invalid_argument("empty token");
    }
    vector<string> result;
    string::size_type found_at;
    for (int i = 0; i != limit; i++)
    {
        found_at = str.find(token, position);
        if (found_at == string::npos) break;
        result.push_back(str.substr(position, found_at - position));
        position = found_at + token_size;
    }
    result.push_back(string(str, position));

    return result;

}


/*  Accepts a vector<string> and a token, then returns one string created from the vector
    of strings with the token inserted between them. This is the complementary function
    to split_string. */
EXLIB_API string join_string(const vector<string>& in, const string& token)
{
    stringstream ss;
    for (   vector<string>::const_iterator it = in.begin();
            it != in.end(); it++)
    {
        ss << *it << token;
    }
    if (!in.size()) return ss.str();
    return ss.str().substr(0, ss.str().size() - token.size());
}



/* Generic string test with any function pointer matching the signature */
static bool str_test(string& s, int (*test)(int c)) 
{
    for (   string::iterator it = s.begin();
            it != s.end(); it++)
    {
        if (!test(*it)) return false;
    }
    return true;
}

/* Test whether a string is all digits */
bool str_is_digit(string& s)
{
    return str_test(s, isdigit);
}

/* Test whether a string is all alpha numeric */
bool str_is_alnum(string& s)
{
    return str_test(s, isalnum);
}

/* Test whether a string is all alpha */
bool str_is_alpha(string& s)
{
    return str_test(s, isalpha);
}

/* Test whether a string can be converted to an int */
bool str_is_int(const string& s)
{
    string local(strip(s));
    string::const_iterator it = local.begin();
    while (it != local.end()) {
        if (!isdigit(*it)) {
            if (it != local.begin()) {
                return false;
            } else if (!((*it == '-') || (*it == '+'))) {
                return false;
            }
        }
        it++;
    }
    return true;
}

EXLIB_API bool str_is_float(const string& s)
{
    if (count(s.begin(), s.end(), '.') > 1) {
        return false;
    } else {
        string local(s);
        string::iterator it = remove(local.begin(), local.end(), '.');
        return str_is_int(string(local.begin(), it));
    }
}

EXLIB_API deque<string>
tokenize_string(const string& token, const string& to_tokenize) {
    vector<string> tokens;
    tokens.push_back(token);
    return tokenize_string(tokens.begin(), tokens.end(), to_tokenize);
}

EXLIB_API std::deque<std::string>
tokenize_string(std::initializer_list<std::string> il, const std::string& to_tokenize)
{
    return tokenize_string(il.begin(), il.end(), to_tokenize);
}

EXLIB_API string to_lower(const string& s)
{
    string result;
    transform(s.begin(), s.end(), back_inserter(result), ::tolower);
    return result;
}

EXLIB_API std::string to_upper(const std::string& s)
{
    string result;
    transform(s.begin(), s.end(), back_inserter(result), ::toupper);
    return result;
}

} // End namespace ex
