#include "core/std_midi/voice_message.h"

using namespace std;

namespace std_midi {

VoiceMessage::VoiceMessage(Byte status, Byte data1)
    :
    status_(status),
    data1_(data1),
    data2_(0)
{

}

VoiceMessage::VoiceMessage(Byte status, Byte data1, Byte data2)
    :
    status_(status),
    data1_(data1),
    data2_(data2)
{

}

VoiceMessage::VoiceMessage(istream &is)
    :
    status_(is.get() & 0xF0),
    data2_(0)
{
    this->read_message(is);
}

VoiceMessage::VoiceMessage(istream &is, Byte status)
    :
    status_(status),
    data2_(0)
{
    this->read_message(is);
}

void VoiceMessage::read_message(istream &is)
{
    this->data1_ = is.get();
    if (!((status_ == sb::program_change) || (status_ == sb::channel_pressure)))
    {
        this->data2_ = is.get();
    }
}

void VoiceMessage::WriteString(ostream &os) const
{
    os << sb::status_names[this->GetCode()] << "("
        << "status=" << showbase << hex << static_cast<UInt16>(this->status_)
        << dec << noshowbase << ", "
        << "data1=" << static_cast<UInt16>(this->data1_) << ", "
        << "data2=" << static_cast<UInt16>(this->data2_)
        << ")";
}


} // end namespace std_midi
