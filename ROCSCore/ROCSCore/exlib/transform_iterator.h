#pragma once

/** This header implements a simplified version of boost::transform_iterator for ARM
    compilations, where use of the Boost libraries may be more difficult. **/

#include "exlib/win32/declspec.h"

#if 0

#include "boost/iterator/transform_iterator.hpp"

namespace ex {

using boost::transform_iterator;

}

#else
#include <iostream>
#include <iterator>
#include <type_traits>
#include <functional>


namespace ex
{

template<class UnaryFunction, class Iterator>
class transform_iterator
{
public:
    typedef typename std::result_of<const UnaryFunction(
        typename std::iterator_traits<Iterator>::reference)>::type reference;

    typedef typename std::remove_cv<std::remove_reference<reference>>::type value_type;

    typedef typename std::iterator_traits<Iterator>::difference_type difference_type;

    transform_iterator() {}

    transform_iterator(const Iterator& x, UnaryFunction f = UnaryFunction())
        :
        m_iterator(x), m_f(f)
    {}

    template<class F2, class I2>
    transform_iterator(transform_iterator<F2, I2> const& t)
        :
        m_iterator(t.base()), m_f(t.functor())
    {}

    UnaryFunction functor() const { return m_f; }
    
    const Iterator& base() const { return m_iterator; }

    reference operator*() const { return m_f(*m_iterator); }

    transform_iterator& operator++() { m_iterator++; return *this; }

    transform_iterator operator++(int)
    {
        transform_iterator<UnaryFunction, Iterator> temp(*this);
        this->operator++();
        return temp;
    }

    transform_iterator& operator--() { m_iterator--; return *this; }

private:
    Iterator m_iterator;
    UnaryFunction m_f;
};

template<class UnaryFunction, class Iterator>
bool operator==(
    const transform_iterator<UnaryFunction, Iterator>& x,
    const transform_iterator<UnaryFunction, Iterator>& y)
{
    return x.base() == y.base();
}

template<class UnaryFunction, class Iterator>
bool operator!=(
    const transform_iterator<UnaryFunction, Iterator>& x,
    const transform_iterator<UnaryFunction, Iterator>& y)
{
    return x.base() != y.base();
}

} // namespace ex
#endif // #ifdef __arm__
