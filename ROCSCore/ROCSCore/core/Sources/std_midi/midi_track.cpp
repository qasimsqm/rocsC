#include "core/std_midi/midi_track.h"

using namespace std;

namespace std_midi
{

struct MIDITrackHeader
{
    char chunk_id[4];
    UInt32 size;
};


/* ReadMessageFunctor is a function that gets called multiple times from the
 * MIDITrack constructor.  running_status and require_status are persistent
 * values between function calls. Other private variables only need to be
 * created on the stack once, when the functor is constructed. */
struct ReadMessageFunctor
{
    ReadMessageFunctor(): require_status(true) {}
    
    bool operator()(istream& is, MIDIPacket *p)
    {
        next_byte = is.peek();
        next_status = next_byte & 0xF0;
        if (next_byte == 0xFF)
        {
            require_status = true;
            p->add_message(read_meta_message(is));
            if (p->meta_messages().back()->status() == sb::end_of_track)
            {
                return true;
            }

        } else if ((next_byte == sb::sysex) || (next_byte == sb::sysex_escape))
        {
            // Ignore all sysex messages
            require_status = true;
            any_var_length = read_variable_int(is);
            is.seekg(any_var_length, istream::cur);
        } else if (sb::voice_status_bytes.count(next_status))
        {
            // This is a voice message.
            running_status = next_status;
            require_status = false;
            p->add_message(VoiceMessage(is));
        } else if (!(next_byte & 0x80) & !require_status)
        {
            // This is a value between 0 and 127
            p->add_message(VoiceMessage(is, running_status));
        } else
        {
            throw UnknownStatusByte(ex::format(
                "Unknown status byte: %#X at %u",
                next_byte,
                is.tellg()));
        }

        return false;
    }

private:
    bool require_status;
    Byte running_status;
    Byte next_byte, next_status;
    UInt32 any_var_length;
};

MIDITrack::MIDITrack(istream &is, UInt16 trackId)
    :
    track_id_(trackId)
{
    MIDITrackHeader hdr;
    is.read((char *)&hdr, sizeof(hdr));
    ex::reverse_byte_order(hdr.size);

    if (string(hdr.chunk_id, 4) != "MTrk")
    {
        /*  According to MIDI specification, unrecognized chunks should be
         *  skipped. Advance the get pointer on the istream to the end of this
         *  unknown chunk, then throw the error to indicate that this is not a
         *  MIDITrack. */
        is.seekg(hdr.size, istream::cur);
        throw UnrecognizedTrackID(ex::format(
            "Unrecognized track_id " + string(hdr.chunk_id, 4) + "at %u",
            is.tellg()));
    }

    UInt32 end_g = (UInt32)is.tellg() + hdr.size;

    MIDIPacket *p;
    ReadMessageFunctor read_message;
    UInt32 delta_time;

    // The first delta time will always create a new packet.
    p = new MIDIPacket(read_variable_int(is));
    read_message(is, p);
    length_ = p->delta_time();
    while (static_cast<UInt32>(is.tellg()) < end_g)
    {
        delta_time = read_variable_int(is);

        if (delta_time == 0)
        {
            if (read_message(is, p))
            {
                packets_.push_back(*p);
                break;
            }

        } else
        {
            // Create a new packet with the new delta time.
            length_ += delta_time;
            packets_.push_back(*p);
            delete p;
            p = new MIDIPacket(delta_time);
            if (read_message(is, p)) {
                packets_.push_back(*p);
                break;
            }
        }
    }

    // Determine track name 
    auto tname = packets_.at(0).meta_filter(sb::track_name);
    if (tname)
    {
        track_name_ = tname->message();
    }
};

void MIDITrack::WriteString(ostream& os) const
{
    os << dec;
    os  << "MIDITrack("
        << "track_id=" << static_cast<UInt16>(this->track_id_) << ", "
        << "packet_count=" << this->packets_.size()
        << ")";
}

void MIDITrack::WriteStringPackets(ostream &os, int indent) const
{
    os << string(indent, '\t');
    this->WriteString(os);
    os << endl;
    for (auto it: this->packets_)
    {
        it.WriteString(os, indent + 1);
        os << endl;
    }
}

} // End namespace std_midi
