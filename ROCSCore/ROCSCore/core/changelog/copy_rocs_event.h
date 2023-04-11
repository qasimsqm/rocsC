#pragma once

#include "core/win32/declspec.h"
#include "core/changelog/cl_events.h"

namespace CL
{

ROCS_CORE_API
cmn::ROCSEvtPtrT CopyRocsEvent(const cmn::ROCSEvent &evt);

} // end namespace CL
