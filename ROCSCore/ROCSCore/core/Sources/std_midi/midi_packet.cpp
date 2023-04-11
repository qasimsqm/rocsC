#include "core/std_midi/midi_packet.h"
using namespace std;

namespace std_midi
{

void MIDIPacket::add_message(MetaMsgPtrT msg_ptr)
{
    meta_messages_.push_back(msg_ptr);
}

void MIDIPacket::add_message(const VoiceMessage& voice_msg)
{
    voice_messages_.push_back(voice_msg);
}

void MIDIPacket::WriteString(std::ostream& os, int indent) const
{
    os  << string(indent, '\t') << "MIDIPacket(delta_time="
        << delta_time() << ")" << endl;
    string indent_(indent + 1, '\t');
    for (auto m_it : meta_messages_) {
        os << indent_;
        m_it->WriteString(os);
        os << endl;
    }

    for (auto v_it : voice_messages_) {
        os << indent_;
        v_it.WriteString(os);
        os << endl;
    }
}

MetaMsgPtrT MIDIPacket::meta_filter(UInt8 status) const
{
    auto it = find_if(  meta_messages_.begin(),
                        meta_messages_.end(),
                        [status] (MetaMsgPtrT msg) { return msg->status() == status; } );
    if (it != meta_messages_.end()) {
        return *it;
    } else {
        return MetaMsgPtrT();
    }
}

VoiceMsgVecT MIDIPacket::voice_filter(UInt8 status) const
{
    VoiceMsgVecT ret;
    auto it = this->voice_messages_.begin();
    while (it != this->voice_messages_.end()) {

        it = find_if(
            it,
            this->voice_messages_.end(),
            [&status] (VoiceMessage msg)
            {
                return msg.GetStatus() == status;
            });
        
        if (it != voice_messages_.end())
        {
            ret.push_back(*it);
            it++;
        }
    }

    return ret;
}

} // end namespace std_midi
