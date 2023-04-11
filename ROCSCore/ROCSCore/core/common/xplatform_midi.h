#pragma once

#include "core/win32/declspec.h"

#if defined(__APPLE__)
    #include <CoreMIDI/MIDIServices.h>
#elseif defined(_WIN32)
    #include "ExLib/xplatform_types.h"

    inline struct MIDIPacket {
        ROCS_CORE_API UInt64 timeStamp;
        ROCS_CORE_API UInt16 length;
        ROCS_CORE_API Byte data[256];
    };

    inline struct MIDIPacketList {
        ROCS_CORE_API UInt32 numPackets;
        ROCS_CORE_API MIDIPacket packet[1];
    };

#endif
