#include "core/common/rocs_event.h"

using namespace std;

namespace cmn {

void ROCSEvent::WriteString(ostream &os) const
{
    os  << "ROCSEvent("
        << "code=" << code() << ", "
        << "abs_time=" << abs_time()
        << ")";
}

bool ROCSEvent::operator<(const ROCSEvent &rhs) const
{
    if (abs_time() == rhs.abs_time()) {
        if (end() == rhs.end()) {
            return code() < rhs.code();
        } else {
            return end() < rhs.end();
        }
    } else {
        return abs_time() < rhs.abs_time();
    }
}

bool ROCSEvent::operator==(const ROCSEvent &other) const
{
    return ((this->code() == other.code())
        && (this->abs_time() == other.abs_time()));
}


} // namespace cmn

