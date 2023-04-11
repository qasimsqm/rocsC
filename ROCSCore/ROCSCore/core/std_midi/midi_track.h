#pragma once

#include "core/win32/declspec.h"

#include <istream>
#include <ostream>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm> // find_if
#include "exlib/reverse_byte_order.h"
#include "exlib/format.h"
#include "exlib/ptr_vector.h"
#include "core/common/rocs_exception.h"
#include "core/std_midi/utility.h"
#include "core/std_midi/status_bytes.h"
#include "core/std_midi/midi_packet.h"


namespace std_midi {

class ROCS_CORE_API MIDITrack;

typedef std::shared_ptr<MIDITrack> MIDITrackPtrT;
typedef ex::ptr_vector<MIDITrack> MIDITrackVecT;
ROCS_CORE_TEMPLATE_DECLSPEC(EXLIB_PTR_VECTOR_CONTAINER_T(MIDITrack));
ROCS_CORE_TEMPLATE_DECLSPEC(ex::ptr_vector<MIDITrack>);

// http://msdn.microsoft.com/en-us/library/3tdb471s.aspx
// C4275 can be ignored in Visual C++ if you are deriving from a type in the Standard C++ Library
MSC_DISABLE_WARNING(4275);
class ROCS_CORE_API MIDITrackError : public std::runtime_error
{
public:
    MIDITrackError(const std::string& what): std::runtime_error(what) {}
};
MSC_RESTORE_WARNING(4275);

class ROCS_CORE_API UnknownStatusByte : public MIDITrackError
{
public:
    UnknownStatusByte(const std::string& what): MIDITrackError(what) {}
};

class ROCS_CORE_API UnrecognizedTrackID : public MIDITrackError
{
public:
    UnrecognizedTrackID(const std::string& what): MIDITrackError(what) {}
};

class ROCS_CORE_API MIDITrack
{
public:
    MIDITrack(std::istream &is, UInt16 trackId);

    UInt16 GetTrackId() const { return track_id_; }

    void SetTrackId(const UInt8 trackId) { this->track_id_ = trackId; }

    std::string GetTrackName() const { return this->track_name_; }

    UInt32 GetLength() const { return this->length_; }

    const MIDIPacketVecT& GetPackets() const { return this->packets_; }

    void WriteString(std::ostream &os) const;

    void WriteStringPackets(std::ostream &os, int indent=0) const;

    bool operator<(MIDITrack &other) const { return this->track_id_ < other.track_id_; }

private:
    UInt16 track_id_;
    UInt32 length_;
	MSC_DISABLE_WARNING(4251);
	std::string track_name_;
	MSC_RESTORE_WARNING(4251);
    MIDIPacketVecT packets_;

};

} // End namepsace std_midi
