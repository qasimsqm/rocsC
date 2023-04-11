#include "exlib/binary_string_io.h"

using namespace std;

namespace ex
{

EXLIB_API void WriteString(ostream &o, const string &s)
{
    if (s.size() > 256)
    {
        throw length_error("String length is limited to 256 characters");
    }
    
    UInt8 string_size = s.size();
    o.write(reinterpret_cast<char *>(&string_size), sizeof(string_size));
    o.write(s.c_str(), string_size);
}


EXLIB_API string ReadString(istream &is)
{
    UInt8 string_size = is.get();
    char *c_str = new char[string_size];
    is.read(c_str, string_size);
    string s(c_str, string_size);
    delete[] c_str;
    return s;
}



}
