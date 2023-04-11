#include "core/std_midi/status_bytes.h"

using namespace std;


namespace std_midi {
namespace sb {

inline StatusNameMapT create_map() {
    StatusNameMapT name_map;

    name_map[note_off] = "NoteOff";
    name_map[note_on] = "NoteOn";
    name_map[aftertouch] = "Aftertouch";
    name_map[control_change] = "ControlChange";
    name_map[program_change] = "ProgramChange";
    name_map[channel_pressure] = "ChannelPressure";
    name_map[pitch_wheel] = "PitchWheel";
    name_map[sequence_number] = "SequenceNumber";
    name_map[text] = "Text";
    name_map[copyright] = "Copyright";
    name_map[track_name] = "TrackName";
    name_map[instrument] = "Instrument";
    name_map[lyric] = "Lyric";
    name_map[marker] = "Marker";
    name_map[cue_point] = "CuePoint";
    name_map[program_name] = "ProgramName";
    name_map[device_port_name] = "DevicePortName";
    name_map[midi_channel_prefix] = "MIDIChannelPrefix";
    name_map[end_of_track] = "EndOfTrack";
    name_map[tempo] = "Tempo";
    name_map[smpte_offset] = "SMPTEOffset";
    name_map[time_signature] = "TimeSignature";
    name_map[key_signature] = "KeySignature";
    name_map[proprietary_event] = "ProprietaryEvent";
    name_map[sysex] = "Sysex";
    name_map[sysex_escape] = "SysexEscape";
    return name_map;
}

inline VoiceStatusSetT create_voice_status_bytes() {
    VoiceStatusSetT voice_status_bytes;

    voice_status_bytes.insert(note_off);
    voice_status_bytes.insert(note_on);
    voice_status_bytes.insert(aftertouch);
    voice_status_bytes.insert(control_change);
    voice_status_bytes.insert(program_change);
    voice_status_bytes.insert(channel_pressure);
    voice_status_bytes.insert(pitch_wheel);

    return voice_status_bytes;
}

ROCS_CORE_API StatusNameMapT status_names = create_map();
ROCS_CORE_API VoiceStatusSetT voice_status_bytes = create_voice_status_bytes();

} // End namespace sb
} // End namespace std_midi
