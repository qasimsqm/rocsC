#pragma once

#include "exlib/win32/declspec.h"

/** Use this to create an ostream that writes to void * data

std::ostream(iostreambuf&);

or to create an istream

std::istream(iostreambuf&);

Thanks to Steve Guidi
http://stackoverflow.com/questions/1494182/setting-the-internal-buffer-used-by-a-standard-stream-pubsetbuf

**/
#include <streambuf>
#include <sstream>


namespace ex {

class iostreambuf : public std::stringbuf
{
public:
    iostreambuf(const char_type* buffer, std::streamsize buffer_length)
        :
        buffer(const_cast<char_type *>(buffer)),
        buffer_length(buffer_length)
    {
        this->reset_pointers();
    }

    void reset_pointers()
    {
        // set the "put" and "get" pointer to the start of the buffer and
        // record its length.
        this->setp(buffer, buffer + buffer_length);
        this->setg(buffer, buffer, buffer + buffer_length);
    }

private:
    char_type *buffer;
    std::streamsize buffer_length;
};



class EXLIB_API char_array_buffer : public std::streambuf
{
public:
    char_array_buffer(const char *begin, const char *end);
    explicit char_array_buffer(const char *str);

private:
    int_type underflow();
    int_type uflow();
    int_type pbackfail(int_type ch);
    std::streamsize showmanyc();

    // copy ctor and assignment not implemented;
    // copying not allowed
    char_array_buffer(const char_array_buffer &);
    char_array_buffer &operator= (const char_array_buffer &);
    std::streampos seekoff(std::streamoff off, std::ios_base::seekdir way,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);
    std::streampos seekpos(std::streampos sp,
        std::ios_base::openmode which = std::ios_base::in | std::ios_base::out);

private:
    const char * const begin_;
    const char * const end_;
    const char * current_;
};

}
