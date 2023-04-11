#pragma once

/* string_lib.h */

#include "exlib/win32/declspec.h"

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <list>
#include <stdexcept>
#include <ostream>
#include <sstream>
#include <iomanip> // setw, setfill
#include <algorithm>
#include <iterator>
#include <cctype> // isalpha, isdigit, isalnum, tolower

#include "exlib/cxx11.h"
#include <initializer_list>

namespace ex
{

template <size_t char_count>
void strcpy_safe(char (&destination)[char_count], const char* source)
{
#if defined(_WIN32)
    strncpy_s(destination, source, char_count);
#else
    strncpy(destination, source, char_count);

    // Ensure null-termination.
    destination[char_count - 1] = '\0';
#endif
}

EXLIB_API bool endswith(const std::string& to_test, const std::string& ending);

/*  The input str will be split on every occurrence of token.  The returned vector<string>
    does not contain any instance of token. */
EXLIB_API std::vector<std::string> split_string(
    const std::string& str,
    const std::string& token = " ",
    int limit = -1);


/*  Accepts a vector<string> and a token, then returns one string created from the vector
    of strings with the token inserted between them. This is the complementary function
    to split_string. */
EXLIB_API std::string join_string(const std::vector<std::string>& in, const std::string& token);

/** InputIterator must dereference to a std::string or to any object that can be a
    right hand operand to ostream::operator<< **/
template< class InputIterator >
std::string join_string(InputIterator first, InputIterator last, const std::string& token)
{
    std::stringstream ss;
    while (first != last) {
        ss << *first << token;
        first++;
    }
    if (!ss.str().size()) return ss.str();
    return ss.str().substr(0, ss.str().size() - token.size());
}

inline
std::string join_string(
    std::initializer_list<std::string> il,
    const std::string& token=" ")
{
    return join_string(il.begin(), il.end(), token);
}

/*  string& strip(const string& s)
    Removes all leading and trailing white space characters, but leaves any white space
    characters that may occur between other characters. */
// EXLIB_API std::string strip(const std::string& s);
template< class stringT > 
stringT strip(const stringT& s)
{
    int inputLength = s.length();

    // Advance to the character position that follows any leading space characters.
    int firstNonSpaceIndex = 0;
    while (firstNonSpaceIndex < inputLength && s.at(firstNonSpaceIndex) == ' ')
    {
        firstNonSpaceIndex++;
    }

    // Reduce the content length until it reaches 0 or the the last character
    // is a non-space.
    int contentLength = inputLength - firstNonSpaceIndex;
    while (contentLength > 0 && s.at(firstNonSpaceIndex + contentLength - 1) == ' ')
    {
        contentLength--;
    }

    if (contentLength == 0)
    {
        // There is no content.
        // Return the empty string.
        return std::string();
    } else 
    {
        // There is content.
        // Return a substring that omits leading/trailing whitespace.
        return s.substr(firstNonSpaceIndex, contentLength);
    }
}


/* Test whether a string is all digits */
EXLIB_API bool str_is_digit(std::string& s);

/* Test whether a string is all alpha numeric */
EXLIB_API bool str_is_alnum(std::string& s);

/* Test whether a string is all alpha */
EXLIB_API bool str_is_alpha(std::string& s);

/* Test whether a string can be converted to an integral number */
EXLIB_API bool str_is_int(const std::string& s);

/* Test whether a string can be converted to a floating point number.*/
EXLIB_API bool str_is_float(const std::string& s);

template <typename T>
bool str_to_num(const std::string& s, T& num)
{
    if (str_is_int(s) || str_is_float(s))
    {
        std::stringstream ss(strip(s));
        ss >> num;
        return true;
    } else
    {
        return false;
    }
}

template <typename T>
T str_to_num(const std::string& s)
{
    T num;
    std::stringstream ss(strip(s));
    ss >> num;
    return num;
}

template <typename T>
T hex_str_to_num(const std::string& s)
{
    T num;
    std::stringstream ss(strip(s));
    ss >> std::hex >> num;
    return num;
}

/** tokenize_string splits the string on the given tokens, returning a list<string>
     e.g. "@b 6 @m some text @rs 7", where '@b', '@m',and '@rs' results in:
        list<string> {"@b", " 6 ", "@m", " some text ", "@rx", " 7"}
    The function is space agnostic, and only relies on the positions of the tokens.
    If more than one occurrence of the same token is found, both will be preserved. **/


template <class InputIterator>
std::deque<std::string>
tokenize_string(
    InputIterator tokens_first,
    InputIterator tokens_last,
    const std::string& to_tokenize)
{
    std::list<std::string>::iterator list_it;
    std::list<std::string> result;
    
    // Begin with to_tokenize as one substring. Iterate through all of the
    // tokens, dividing each substring at the token location.
    result.push_back(to_tokenize);
    
    for (; tokens_first != tokens_last; tokens_first++)
    {
        // Each time through this for loop, result changes size as new
        // substrings are added. Each time a token is found, divide the
        // current substring at the token location.
        for (list_it = result.begin(); list_it != result.end(); list_it++)
        {
            while (true)
            {
                // Find the position of every occurrence of this token in
                // each substring
                size_t find_pos = list_it->find(*tokens_first);
                if (find_pos != list_it->npos)
                {
                    if (find_pos != 0)
                    {
                        // Insert the part of the substring that precedes the
                        // token.
                        result.insert(list_it, list_it->substr(0, find_pos));
                    }
                    
                    // Insert the token.
                    result.insert(list_it, *tokens_first);
                    
                    // Set the current substring to the part of the substring
                    // after the token.
                    *list_it = list_it->substr(
                        find_pos + tokens_first->size(),
                        list_it->npos);
                } else
                {
                    // no further occurrences of *tokens_first have been found.
                    break;
                }
            }
        }
    }

    std::deque<std::string> ret;
    std::transform(
        result.begin(),
        result.end(),
        back_inserter(ret),
        &strip<std::string>);
    
    return ret;
}

// Overloaded for convenience when there is only one token string
EXLIB_API std::deque<std::string>
tokenize_string(const std::string& token, const std::string& to_tokenize);

EXLIB_API std::deque<std::string>
tokenize_string(std::initializer_list<std::string> il, const std::string& to_tokenize);

EXLIB_API std::string to_lower(const std::string& s);
EXLIB_API std::string to_upper(const std::string& s);

} // End namespace ex
