#include "exlib/iostreambuf.h"

#include <functional>
#include <cassert>
#include <cstring>

namespace ex {

EXLIB_API char_array_buffer::char_array_buffer(const char *begin, const char *end)
    :
    begin_(begin),
    end_(end),
    current_(begin_)
{
    assert(std::less_equal<const char *>()(begin_, end_));
}

EXLIB_API char_array_buffer::char_array_buffer(const char *str)
    :
    begin_(str),
    end_(begin_ + std::strlen(str)),
    current_(begin_)
{
}

char_array_buffer::int_type char_array_buffer::underflow()
{
    if (current_ == end_)
    {
        return traits_type::eof();
    }

    return traits_type::to_int_type(*current_);
}

char_array_buffer::int_type char_array_buffer::uflow()
{
    if (current_ == end_)
    {
        return traits_type::eof();
    }

    return traits_type::to_int_type(*current_++);
}

char_array_buffer::int_type char_array_buffer::pbackfail(int_type ch)
{
    if (current_ == begin_ || (ch != traits_type::eof() && ch != current_[-1]))
    {
        return traits_type::eof();
    }

    return traits_type::to_int_type(*--current_);
}

std::streamsize char_array_buffer::showmanyc()
{
    assert(std::less_equal<const char *>()(current_, end_));
    return end_ - current_;
}

std::streampos char_array_buffer::seekoff(
    std::streamoff off,
    std::ios_base::seekdir way,
    std::ios_base::openmode)
{
    const char *startptr;
    if (way == std::ios_base::beg)
    {
        startptr = begin_;
    } else if (way == std::ios_base::end)
    {
        startptr = end_;
    } else if (way == std::ios_base::cur)
    {
        startptr = current_;
    } else 
        // Woody, I don't remember writing this code. Was this you? I added
        // this else return -1 to silence a warning about uninitialized use of
        // startptr. Does this look like reasonable behavior to you? --Jonathan
    {
        return -1;
    }

    const char *resultpos = startptr + off;
    if ((resultpos < begin_) || (resultpos > end_)) {
        return -1;
    } else {
        current_ = resultpos;
        return current_ - begin_;
    }
}

std::streampos char_array_buffer::seekpos(std::streampos sp, std::ios_base::openmode)
{
    const char *resultpos = begin_ + static_cast<int>(sp);
    if ((resultpos < begin_) || (resultpos > end_))
    {
        return -1;
    } else
    {
        current_ = resultpos;
        return current_ - begin_;
    }
}

}
