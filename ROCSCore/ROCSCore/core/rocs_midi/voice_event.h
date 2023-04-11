#pragma once

#include "core/win32/declspec.h"

#include <ostream>
#include <cassert>

#include "exlib/xplatform_types.h"
#include "core/common/verify_align.h"
#include "core/std_midi/voice_message.h"

namespace rocs_midi
{

using std_midi::VoiceMessage;

class ROCS_CORE_API VoiceEvent
{
public:
    VoiceEvent() {}

    VoiceEvent(const VoiceEvent& voiceEvent)
        :
        abs_time_(voiceEvent.abs_time_),
        message_(voiceEvent.message_)
    {
    
    }

    VoiceEvent(UInt32 abs_time)
        :
        abs_time_(abs_time)
    {
    
    }

    VoiceEvent(UInt32 abs_time, const VoiceMessage &message)
        :
        abs_time_(abs_time),
        message_(message)
    {
    
    }

    VoiceEvent(UInt32 abs_time, UInt8 status, UInt8 data1, UInt8 data2)
        :
        abs_time_(abs_time),
        message_(VoiceMessage(status, data1, data2))
    {
        assert(std_midi::sb::voice_status_bytes.count(this->GetStatusType()) == 1);
        assert(data1 < 128);
        assert(data2 < 128);
    }

    ~VoiceEvent() {}

    void operator=(const VoiceEvent& voiceEvent)
    { 
        assert(this != &voiceEvent);
        this->abs_time_ = voiceEvent.abs_time_;
        this->message_ = voiceEvent.message_;
    }
    
    Byte GetStatus() const { return this->message_.status_; }
    
    UInt32 GetAbsTime() const { return this->abs_time_; }

    Byte GetStatusType() const { return this->message_.status_ & 0xF0; }
    
    UInt8 GetChannel() const { return this->message_.status_ & 0x0F; }

    void SetChannel(UInt8 channel)
    { 
        this->message_.status_ &= 0xF0;
        this->message_.status_ |= (channel & 0x0F);
    }
    
    UInt8 GetData1() const { return this->message_.data1_; }

    void SetData1(UInt8 val) { this->message_.data1_ = val; }
    
    UInt8 GetData2() const { return this->message_.data2_; }

    void SetData2(UInt8 val) { this->message_.data2_ = val; }
        
    const VoiceMessage& GetVoiceMessage() const { return this->message_; }

    VoiceMessage& GetVoiceMessage() { return this->message_; }
    
    void WriteString(std::ostream &os) const
    { 
        os << this->abs_time_ << ": ";
        this->message_.WriteString(os);
    }
    
    void WriteStringMessage(std::ostream &os) const
    {
        this->message_.WriteString(os);
    }

    bool operator<(const VoiceEvent &other) const
    {
        return this->abs_time_ < other.abs_time_;
    }

private:
    UInt32 abs_time_;
    VoiceMessage message_;

};

typedef std::vector<VoiceEvent> VoiceEvtVecT;
ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<VoiceEvent>);

ROCS_CORE_API bool operator==(const VoiceEvent &lhs, const VoiceEvent &rhs);
ROCS_CORE_API bool operator!=(const VoiceEvent &lhs, const VoiceEvent &rhs);

} // end namespace rocs_midi

ROCS_CORE_API
std::ostream& operator<<(std::ostream& os, const rocs_midi::VoiceEvent& evt);
