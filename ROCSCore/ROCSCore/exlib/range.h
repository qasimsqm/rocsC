#pragma once

#include "exlib/win32/declspec.h"

#include <iterator>
#include <cmath>
#include <cstdlib> // abs
#include <cstddef>

namespace ex {

template< class T >
class RangeIterator : public std::iterator<std::input_iterator_tag, T>
{
public:
    RangeIterator(T _current): step_(1), current_(_current) {}

    RangeIterator(T _current, T _step): step_(_step), current_(_current) {}

    RangeIterator& operator++() { current_ += step_; return *this; }

    RangeIterator& operator++(int) { RangeIterator tmp(*this); operator++(); return tmp; }
    
    bool operator==(const RangeIterator& rhs) const
    {
        return std::abs(current_ - rhs.current_) < std::abs(step_);
    }

    bool operator!=(const RangeIterator& rhs) const { return !(*this==rhs); }

    const T& operator*() const { return current_; }

    RangeIterator<T> operator+(ptrdiff_t rhs)
    {
        return RangeIterator<T>(current_ + rhs * step_, step_);
    }

    //RangeIterator& operator+(ptrdiff_t rhs) {
    //    return RangeIterator(current_ + rhs * step_, step_);
    //}
private:
    T step_;
    T current_;
};

template< class T >
class Range
{
public:
    typedef RangeIterator<T>    iterator;
    Range(T _end)
        :
        begin_(0),
        end_(_end),
        step_(1)
    {
        check_bounds();
    }

    Range(T _begin, T _end)
        :
        begin_(_begin),
        end_(_end),
        step_(1)
    {
        check_bounds();
    }
    
    Range(T _begin, T _end, T _step)
        :
        begin_(_begin),
        end_(_end),
        step_(_step)
    {
        check_bounds();
    }
    
    iterator begin() { return iterator(begin_, step_); }

    iterator end() { return iterator(end_, step_); }

private:
    T begin_;
    T end_;
    T step_;
    void check_bounds()
    {
        if (begin_ < end_)
        {
            if (step_ < 0)
            {
                begin_ = end_;
            }
        } else if (step_ > 0)
        {
            begin_ = end_;
        }
        
        if (step_ == 0)
        {
            begin_ = end_;
        }
    }
};


template< class T >
Range<T> range(T end) { return Range<T>(end); }

template< class T >
Range<T> range(T begin, T end) { return Range<T>(begin, end); }

template< class T >
Range<T> range(T begin, T end, T step) { return Range<T>(begin, end, step); }


}
