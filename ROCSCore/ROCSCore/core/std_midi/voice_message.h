#pragma once

#include "core/win32/declspec.h"

#include <ostream>
#include <istream>
#include <ios>
#include <vector>
#include "exlib/xplatform_types.h"
#include "core/std_midi/status_bytes.h"


namespace std_midi {

class ROCS_CORE_API VoiceMessage;
typedef std::vector<VoiceMessage> VoiceMsgVecT;
ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<VoiceMessage>);

/* VoiceMessage
   Byte GetStatus() --> hi nibble is type, lo nibble is channel
   Byte GetData1()  --> the note to be played
   Byte GetData2()  --> the velocity
*/
class ROCS_CORE_API VoiceMessage
{
public:
    VoiceMessage() {}

    VoiceMessage(std::istream &is);

    VoiceMessage(std::istream &is, Byte status);

    VoiceMessage(Byte status, Byte data1);

    VoiceMessage(Byte status, Byte data1, Byte data2);

    void WriteString(std::ostream &os) const;

    Byte GetStatus() const { return status_; }

    Byte GetCode() const { return status_ & 0xF0; }

    UInt8 GetChannel() const { return status_ & 0x0F; }

    UInt8 GetData1() const { return data1_; }

    UInt8 GetData2() const { return data2_; }

public:
    Byte status_;
    Byte data1_;
    Byte data2_;

private:
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wunused-private-field"
    Byte padding_;
    #pragma GCC diagnostic pop
    void read_message(std::istream &is);
};

} // end namespace standard_midi
