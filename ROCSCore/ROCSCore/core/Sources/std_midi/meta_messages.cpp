#include <iostream>
#include "core/std_midi/meta_messages.h"

using namespace std;

namespace std_midi {

ROCS_CORE_API MetaMsgPtrT read_meta_message(istream& s) // throw(MetaMessageError)
{
#ifdef DEBUG
    UInt8 byte1 = s.get();
    assert(byte1 == 0xFF);
#else
    s.get();
#endif
    switch (s.peek()) {
        case sb::sequence_number:
            return MetaMsgPtrT(new MetaMessage(s));
        case sb::text:
            return MetaMsgPtrT(new StringMetaMessage(s));
        case sb::copyright:
            return MetaMsgPtrT(new StringMetaMessage(s));
        case sb::track_name:
            return MetaMsgPtrT(new TrackName(s));
        case sb::instrument:
            return MetaMsgPtrT(new StringMetaMessage(s));
        case sb::lyric:
            return MetaMsgPtrT(new StringMetaMessage(s));
        case sb::marker:
            return MetaMsgPtrT(new Marker(s));
        case sb::cue_point:
            return MetaMsgPtrT(new StringMetaMessage(s));
        case sb::program_name:
            return MetaMsgPtrT(new StringMetaMessage(s));
        case sb::device_port_name:
            return MetaMsgPtrT(new StringMetaMessage(s));
        case sb::midi_channel_prefix:
            return MetaMsgPtrT(new MetaMessage(s));
        case sb::end_of_track:
            return MetaMsgPtrT(new MetaMessage(s));
        case sb::tempo:
            return MetaMsgPtrT(new Tempo(s));
        case sb::smpte_offset:
            return MetaMsgPtrT(new MetaMessage(s));
        case sb::time_signature:
            return MetaMsgPtrT(new TimeSignature(s));
        case sb::key_signature:
            return MetaMsgPtrT(new KeySignature(s));
        case sb::proprietary_event:
            return MetaMsgPtrT(new MetaMessage(s));
        default:
            throw MetaMessageError(
                ex::format("Invalid Meta Message Type: %#X at %u", s.peek(), (UInt32)s.tellg())
            );

    }
}

ROCS_CORE_API MetaMsgPtrT create_meta_message(Byte status) // throw(MetaMessageError)
{
    switch (status) {
        case sb::sequence_number:   return MetaMsgPtrT(new MetaMessage(status));
        case sb::text:              return MetaMsgPtrT(new StringMetaMessage(status));
        case sb::copyright:         return MetaMsgPtrT(new StringMetaMessage(status));
        case sb::track_name:        return MetaMsgPtrT(new TrackName());
        case sb::instrument:        return MetaMsgPtrT(new StringMetaMessage(status));
        case sb::lyric:             return MetaMsgPtrT(new StringMetaMessage(status));
        case sb::marker:            return MetaMsgPtrT(new Marker());
        case sb::cue_point:         return MetaMsgPtrT(new StringMetaMessage(status));
        case sb::program_name:      return MetaMsgPtrT(new StringMetaMessage(status));
        case sb::device_port_name:  return MetaMsgPtrT(new StringMetaMessage(status));
        case sb::midi_channel_prefix:   return MetaMsgPtrT(new MetaMessage(status));
        case sb::end_of_track:      return MetaMsgPtrT(new MetaMessage(status));
        case sb::tempo:             return MetaMsgPtrT(new Tempo());
        case sb::smpte_offset:      return MetaMsgPtrT(new MetaMessage(status));
        case sb::time_signature:    return MetaMsgPtrT(new TimeSignature());
        case sb::key_signature:     return MetaMsgPtrT(new KeySignature());
        case sb::proprietary_event: return MetaMsgPtrT(new MetaMessage(status));
        default:
            throw MetaMessageError(ex::format("Invalid Meta Message Type: %#X", status));
    }
}



// FIXME: This should probably be a positive check for a preprocessor symbol
// that indicates the environment where these pragmas apply, but this will
// eliminate the warnings in the Win32 port.
#if !defined(_WIN32)
#pragma mark -
#pragma mark MetaMessage
#endif

MetaMessage::MetaMessage(const Byte status, UInt8 size): status_(status), length_(size)
{
    bytes_.resize(length_);
}

MetaMessage::MetaMessage(istream& is)
{
    status_ = is.get();
    length_ = is.get();
    bytes_.resize(length_);
    if (length_) is.read((char *)&bytes_.at(0), length_);
}


void MetaMessage::WriteString(ostream& o) const
{
    o << hex << showbase;
    o   << sb::status_names[status_] << "("
        << "status=" << (UInt16)status_ << ", "
        << "length=" << (UInt16)length_ << ", "
        << "bytes={";
    for (auto it : bytes_) {
        o << it << " ";
    }
    if (bytes_.size()) o << "\b\b";
    o << "}" << noshowbase << dec;
}

void MetaMessage::WriteBinary(ostream &os, const cmn::FileVersion &) const
{
    os.put(0xffu);
    os.put(status_);
    os.put(length_);
    if (length_)
    {
        os.write((char *)&bytes_.at(0), length_);
    }
}


// FIXME: This should probably be a positive check for a preprocessor symbol
// that indicates the environment where these pragmas apply, but this will
// eliminate the warnings in the Win32 port.
#if !defined(_WIN32)
#pragma mark -
#pragma mark StringMetaMessage
#endif

void StringMetaMessage::message(const string& s)
{
    bytes_.resize(s.size());
    length_ = s.size();
    copy(s.begin(), s.end(), bytes_.begin());
}

void StringMetaMessage::WriteString(std::ostream& o) const
{
    o << sb::status_names[status_]
        << "(status=" << hex << showbase << (int)status_ << dec << noshowbase << ", "
        << "length=" << (int)length_ << ", "
        << "string=" << message() << ")";
}

// FIXME: This should probably be a positive check for a preprocessor symbol
// that indicates the environment where these pragmas apply, but this will
// eliminate the warnings in the Win32 port.
#if !defined(_WIN32)
#pragma mark -
#pragma mark Tempo
#endif

union MIDITempo {
    UInt32 micro_seconds;
    char bytes[4];
};

UInt32 Tempo::microseconds() const
{
    MIDITempo t;
    t.micro_seconds = 0;

    // The tempo is stored as only 3 bytes, and they are big-endian!
    reverse_copy(bytes_.begin(), bytes_.begin() + 3, &t.bytes[0]);

    return t.micro_seconds;
}

void Tempo::microseconds(UInt32 tmpo)
{
    MIDITempo t;
    t.micro_seconds = tmpo;
    reverse_copy(t.bytes, t.bytes + 3, bytes_.begin());
}

void Tempo::WriteString(std::ostream& o) const
{
    o << sb::status_names[status_]
        << "(status=" << hex << showbase << (int)status_ << dec << noshowbase << ", "
        << "length=" << (int)length_ << ", "
        << "micro seconds=" << microseconds() << ")";
}

void TimeSignature::WriteString(std::ostream& o) const
{
    o << sb::status_names[status_]
        << "(status=" << hex << showbase << (int)status_ << dec << noshowbase << ", "
        << "length=" << (int)length_ << ", "
        << "numerator=" << (int)numerator() << ", "
        << "denominator=" << (int)denominator() << ", "
        << "clocksPerClick=" << (int)clocksPerClick() << ", "
        << "notated32nds=" << (int)notated32nds() << ")";
}


void KeySignature::WriteString(std::ostream& o) const
{
    o << sb::status_names[status_]
        << "(status=" << hex << showbase << (int)status_ << dec << noshowbase << ", "
        << "length=" << (int)length_ << ", "
        << "sharps=" << (int)sharps() << ", "
        << "mode=" << (int)mode() << ", "
        << "name=" << Keys::keyNameFromSig(mode(), sharps()) << ")";
}

} // End of namespace std_midi
