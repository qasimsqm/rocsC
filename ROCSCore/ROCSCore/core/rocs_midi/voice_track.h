#pragma once

#include "core/win32/declspec.h"

#include <vector>
#include <string>
#include <sstream>
#include <set>
#include <map>
#include <istream>
#include <ostream>
#include <iostream>
#include <utility>
#include <algorithm>
#include <stdexcept>
#include <iterator>

#include <memory>

#include <cstdlib> // abs
#include "exlib/binary_string_io.h"
#include "exlib/ptr_vector.h"
#include "core/common/warnings.h"
#include "core/rocs_midi/voice_event.h"

#include "core/rocs_midi/rocs_midi_exception.h"
#include "core/std_midi/midi_track.h"
#include "core/std_midi/midi_packet.h"
#include "core/std_midi/voice_message.h"
#include "core/std_midi/status_bytes.h"

/** RANGE_LOW_CC and RANGE_HIGH_CC only affect the packager. **/
// #define OLD_RANGE_CCS
#ifdef OLD_RANGE_CCS
const UInt8 RANGE_LOW_CC = 30;
const UInt8 RANGE_HIGH_CC = 31;
#else
const UInt8 RANGE_LOW_CC = 28;
const UInt8 RANGE_HIGH_CC = 29;
#endif


namespace rocs_midi
{

class ROCS_CORE_API VoiceTrackException : public ROCSMIDIException
{
public:
    VoiceTrackException(const std::string& what): ROCSMIDIException(what) {}
};

class ROCS_CORE_API NoTrackName : public VoiceTrackException
{
public:
    NoTrackName(const std::string& what): VoiceTrackException(what) {}
};

class ROCS_CORE_API MissingEvents : public VoiceTrackException
{
public:
    MissingEvents(const std::string& what): VoiceTrackException(what) {}
};

typedef std::pair<UInt8, UInt8> VoiceRangeT;
ROCS_CORE_STRUCT_TEMPLATE_DECLSPEC(std::pair<UInt8, UInt8>);

class ROCS_CORE_API VoiceTrack
{
public:
    VoiceTrack() {}

    VoiceTrack(UInt16 track_id_, const std::string &track_name_)
        :
        track_id_(track_id_),
        track_name_(track_name_),
        range_(std::make_pair(255, 255)),
        has_channel_number_(false)
    {}
    
    VoiceTrack(std::istream &is, const cmn::FileVersion &);

    // Only used for packaging a show from MIDI files
    VoiceTrack(const std_midi::MIDITrack &t);

    void WriteBinary(std::ostream &o, const cmn::FileVersion &) const;

    void WriteString(std::ostream &o) const;

    void WriteStringEvents(std::ostream &o, int indent) const;

    UInt16 GetTrackId() const { return this->track_id_; }

    void SetTrackId(UInt16 id);

    std::string GetTrackName() const { return this->track_name_; }

    void SetTrackName(const std::string& track_name) { this->track_name_ = track_name; }

    VoiceRangeT GetRange() const { return this->range_; }

    void SetRange(const VoiceRangeT &r) { this->range_ = r; }

    UInt8 GetRangeLow() const { return this->range_.first; }

    void SetRangeLow(UInt8 lo) { this->range_.first = lo; }

    UInt8 GetRangeHigh() const { return this->range_.second; }

    void SetRangeHigh(UInt8 hi) { this->range_.second = hi; }

    UInt8 GetPort() const { return track_id_ >> 4; }

    UInt8 GetGroupId() const { return this->group_id_; }

    void SetGroupId(UInt8 id) { this->group_id_ = id; }

    VoiceEvtVecT EventSlice(SInt64 start=-1, SInt64 end=-1, SInt8 transpose=0) const;

    const VoiceEvtVecT& GetEvents() const { return this->events_; }

    VoiceEvtVecT& GetEvents() { return this->events_; }

    // Only used for packaging a show from MIDI files
    bool HasChannelNumber() const { return this->has_channel_number_; }

    UInt16 GetChannelNumber() const { return this->channel_number_; }

    void SetChannelNumber(UInt16 channelNumber)
    {
        this->has_channel_number_ = true;
        this->channel_number_ = channelNumber;
    }

private:
    UInt16 track_id_;
	MSC_DISABLE_WARNING(4251);
	std::string track_name_;
	MSC_RESTORE_WARNING(4251);
    VoiceRangeT range_;
    UInt8 group_id_;
    VoiceEvtVecT events_;
    bool has_channel_number_;
    UInt16 channel_number_;
};

ROCS_CORE_API bool operator==(const VoiceTrack& lhs, const VoiceTrack& rhs);

typedef std::shared_ptr<VoiceTrack> VoiceTrackPtrT;
typedef ex::ptr_vector<VoiceTrack> VoiceTrackPtrVecT;

} // End namespace rocs_midi
