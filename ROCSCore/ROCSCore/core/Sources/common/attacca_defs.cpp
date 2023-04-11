#include "core/common/attacca_defs.h"

namespace attacca {

ROCS_CORE_API NamesT create_names()
{
    NamesT names;
    names[0] = "Play";
    names[1] = "Attacca";
    names[2] = "Skip";
    return names;
}

ROCS_CORE_API NamesT names(create_names());

} // namespace attacca
