#pragma once

#include "core/win32/declspec.h"

#include <map>
#include <set>
#include <string>
#include "exlib/xplatform_types.h"

namespace std_midi {

namespace sb {
enum {
    note_off =          0x80,
    note_on =           0x90,
    aftertouch =        0xA0,
    control_change =    0xB0,
    program_change =    0xC0,
    channel_pressure =  0xD0,
    pitch_wheel =       0xE0,
    sequence_number =   0x00,
    text =              0x01,
    copyright =         0x02,
    track_name =        0x03,
    instrument =        0x04,
    lyric =             0x05,
    marker =            0x06,
    cue_point =         0x07,
    program_name =      0x08,
    device_port_name =  0x09,
    midi_channel_prefix = 0x20,
    end_of_track =      0x2F,
    tempo =             0x51,
    smpte_offset =      0x54,
    time_signature =    0x58,
    key_signature =     0x59,
    proprietary_event = 0x7F,

    // General Message Types
    voice =             0x60,
    meta =              0x61,
    sysex =             0x62,
    sysex_message =     0xF0,
    sysex_escape =      0xF7,
};

typedef std::map<Byte, std::string> StatusNameMapT;
typedef std::set<Byte> VoiceStatusSetT;

ROCS_CORE_API extern StatusNameMapT status_names;
ROCS_CORE_API extern VoiceStatusSetT voice_status_bytes;

} // End of namespace sb
} // End of namespace std_midi
