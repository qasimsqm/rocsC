#pragma once

#include "midi/voice_event.h"
#include "changelog/cl_events.h"
#include "timeline/tl_events.h"

namespace rocs_testing {

bool operator==(cmn::ROCSEvtPtrT lhs, cmn::ROCSEvtPtrT rhs);

}
