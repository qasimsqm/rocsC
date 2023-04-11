#pragma once

#include "core/win32/declspec.h"

#include <algorithm>
#include <utility>
#include "exlib/xplatform_types.h"

namespace cmn {

/** Similar to std::equal_range, but the range does not contain equal events, it contains
    events bounded by start_evt and end_evt.

    All ROCS events implement operator< on abs_time, so start_evt and end_evt need only
    have abs_time set.  An event in [first, last) that has the same abs_time as end_evt
    will be pointed to by std::pair::second.

     **/
template< class ForwardIterator, class EvtT>
inline
std::pair<ForwardIterator, ForwardIterator>
range(  ForwardIterator first, ForwardIterator last,
        const EvtT& start_evt, const EvtT& end_evt)
{
    ForwardIterator begin_range = ::std::lower_bound(first, last, start_evt);
    return ::std::make_pair(begin_range, ::std::upper_bound(begin_range, last, end_evt));
}

}
