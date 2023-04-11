#pragma once

#include "exlib/win32/declspec.h"

#include <functional>
#include "exlib/transform_iterator.h"
#include "exlib/type_traits.h"

namespace ex {

template< class MapT, class Enable = void >
struct GetKey
	:
	public std::unary_function< typename MapT::value_type const&, typename MapT::key_type const& >
{
    typename MapT::key_type const&
    operator()(typename MapT::value_type const& p) const
    {
        return p.first;
    }
};

/** template for shared_ptr keys.  A return by reference could cause the use_count to
    drop to zero, so this specialization returns a copy of shared_ptr. **/
template< class MapT >
struct GetKey<
    MapT,
    typename std::enable_if<ex::is_specialization_of<
	    std::shared_ptr,
	    typename MapT::key_type>::value>::type>
    :
    public std::unary_function<
		typename MapT::value_type const&,
        typename MapT::key_type::element_type const&>
{
    typename MapT::key_type::element_type const&
    operator()(typename MapT::value_type const& p) const
    {
        return *p.first;
    }
};


template< class MapT, class Enable = void >
struct GetValue
	:
	public std::unary_function<
	    typename MapT::value_type const&,
        typename MapT::mapped_type const&>
{
    typename MapT::mapped_type const&
    operator()(typename MapT::value_type const& p) const
    {
        return p.second;
    }
};

/** Template Specialization for a map that has a shared_ptr as its mapped_type. See
    comment at GetKey specialization above. **/
template< class MapT >
struct GetValue
<
    MapT,
    typename std::enable_if<
        ex::is_specialization_of<std::shared_ptr, typename MapT::mapped_type>::value>::type>
    :
    public std::unary_function<
		typename MapT::value_type const&,
        typename MapT::mapped_type::element_type const&>
{
    typename MapT::mapped_type::element_type const&
    operator()(typename MapT::value_type const& p) const
    {
        return *p.second;
    }
};

template< class MapT >
struct MapIters
{
    typedef ex::transform_iterator<
	    GetKey<MapT>,
        typename MapT::const_iterator> key_iterator;

    typedef ex::transform_iterator<
		GetValue<MapT>,
        typename MapT::const_iterator> value_iterator;

    typedef ex::transform_iterator<
        GetKey<MapT>,
        typename MapT::const_reverse_iterator> reverse_key_iterator;

    typedef ex::transform_iterator<
        GetValue<MapT>,
        typename MapT::const_reverse_iterator> reverse_value_iterator;
};

} // namespace ex
