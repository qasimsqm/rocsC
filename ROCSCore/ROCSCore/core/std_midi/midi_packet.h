#pragma once

/**
    MIDIPacket wraps a delta time vectors of MIDIMessages that share the same delta time
**/

#include "core/win32/declspec.h"

#include <vector>
#include <ostream>
#include <string>
#include <algorithm> // find_if
#include <iostream> // for debug printing
#include "exlib/xplatform_types.h"
#include "core/std_midi/meta_messages.h"
#include "core/std_midi/voice_message.h"

namespace std_midi {

class ROCS_CORE_API MIDIPacket
{
public:
	MIDIPacket() : delta_time_(0) {}
    MIDIPacket(UInt32 _delta_time): delta_time_(_delta_time) {}

    void add_message(MetaMsgPtrT msg_ptr);
    void add_message(const VoiceMessage& voice_msg);

    UInt32
    delta_time() const { return delta_time_; }

    const MetaMsgPtrVecT&
    meta_messages() const { return meta_messages_; }

    const VoiceMsgVecT&
    voice_messages() const { return voice_messages_; }

    void WriteString(std::ostream& os, int indent=0) const;

    MetaMsgPtrT meta_filter(UInt8 status) const;
    VoiceMsgVecT voice_filter(UInt8 status) const;

private:
    UInt32              delta_time_;
    MetaMsgPtrVecT      meta_messages_;
    VoiceMsgVecT        voice_messages_;
};

typedef std::vector<MIDIPacket> MIDIPacketVecT;
ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<MIDIPacket>);

}
