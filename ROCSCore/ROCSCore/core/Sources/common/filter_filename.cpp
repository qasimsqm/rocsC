#include "core/common/filter_filename.h"

using namespace std;

namespace cmn {

inline set<char> create_allowed_chars() {
    set<char> allowed_chars;
    allowed_chars.insert('(');
    allowed_chars.insert(')');
    // allowed_chars.insert('-');
    allowed_chars.insert('.');
    allowed_chars.insert('_');
    auto digits = ex::range<char>(48, 58);
    auto lowers = ex::range<char>(97, 123);
    allowed_chars.insert(digits.begin(), digits.end());
    allowed_chars.insert(lowers.begin(), lowers.end());
    return allowed_chars;
}

ROCS_CORE_API set<char> allowed_chars = create_allowed_chars();

ROCS_CORE_API string filter_filename(const string& filename)
{
    string filtered;
    transform(  filename.begin(), filename.end(), back_inserter(filtered),
                [] (char c) -> char
                {
                    if ((c == ' ') || (c == '-')) {
                        return '_';
                    } else {
                        return static_cast<char>(tolower(c));
                    }
                });

    auto it = remove_if(filtered.begin(), filtered.end(),
                        [] (char c) -> bool
                        {
                        if (allowed_chars.count(c)) return false;
                        else return true;
                        });
    return string(filtered.begin(), it);
}

}

