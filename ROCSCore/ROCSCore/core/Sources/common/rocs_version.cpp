#include "core/common/rocs_version.h"

namespace cmn {

ROCS_CORE_API
std::ostream& operator<<(std::ostream& os, const ROCSVersion& v)
{
    v.WriteString(os);
    return os;
}

} // end namespace cmn
