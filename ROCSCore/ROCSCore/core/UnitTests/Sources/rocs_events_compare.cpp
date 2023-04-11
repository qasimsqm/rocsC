#include "events_compare.h"
#include <iostream>
using namespace std;
using std::dynamic_pointer_cast;

namespace rocs_testing {

template< class ROCSEventT >
bool compare(cmn::ROCSEvtPtrT lhs, cmn::ROCSEvtPtrT rhs)
{
    auto p1 = dynamic_pointer_cast<ROCSEventT>(lhs);
    auto p2 = dynamic_pointer_cast<ROCSEventT>(rhs);
    return *p1 == *p2;
}

bool operator==(cmn::ROCSEvtPtrT lhs, cmn::ROCSEvtPtrT rhs)
{
    if (lhs->code() != rhs->code())
    {
        return false;
    }

    switch (lhs->code())
    {
        case codes::vamp:
            return compare<CL::Vamp>(lhs, rhs);
            break;
        case codes::repeat:
            return compare<CL::Repeat>(lhs, rhs);
            break;
        case codes::cut:
            return compare<CL::Cut>(lhs, rhs);
            break;
        case codes::transpose:
            return compare<CL::Transpose>(lhs, rhs);
            break;
        case codes::tempo_scale:
            return compare<CL::TempoScale>(lhs, rhs);
            break;
        case codes::marker:
            return compare<CL::Marker>(lhs, rhs);
            break;
        case codes::bar:
            return compare<CL::CustomBar>(lhs, rhs);
            break;
        case codes::click:
            return compare<CL::Click>(lhs, rhs);
            break;
        case codes::caesura:
            return compare<CL::Caesura>(lhs, rhs);
            break;
        default:
            return false;
    }
}

}
