#include "core/rocs_midi/voice_event.h"

namespace rocs_midi {

ROCS_CORE_API bool operator==(const VoiceEvent &lhs, const VoiceEvent &rhs)
{
    if (lhs.GetAbsTime() != rhs.GetAbsTime()) return false;
    if (lhs.GetStatus() != rhs.GetStatus()) return false;
    if (lhs.GetData1() != rhs.GetData1()) return false;
    if (lhs.GetData2() != rhs.GetData2()) return false;
    return true;
}

ROCS_CORE_API bool operator!=(const VoiceEvent &lhs, const VoiceEvent &rhs)
{
    return !(lhs == rhs);
}

}

ROCS_CORE_API
std::ostream & operator<<(std::ostream &os, const rocs_midi::VoiceEvent &voiceEvent)
{
    voiceEvent.WriteString(os);
    return os;
}
