#pragma once

#include "core/win32/declspec.h"

#include <istream>
#include <ostream>
#include <map>
#include <list>
#include <set>

#include <memory>

#include <stdexcept> // out_of_range
#include <utility>
#include <algorithm>
#include "exlib/xplatform_types.h"
#include "exlib/binary_string_io.h"
#include "exlib/ptr_map.h"
#include "exlib/map_iters.h"
#include "core/rocs_midi/voice_track.h"
#include "core/rocs_midi/rocs_midi_exception.h"
#include "core/rocs_midi/groups.h"
#include "core/std_midi/midi_file.h"
#include <iostream> // cout for debugging

namespace rocs_midi
{

class ROCS_CORE_API VoiceDataError : public ROCSMIDIException
{
public:
    VoiceDataError(const std::string& what): ROCSMIDIException(what) {}
};

class ROCS_CORE_API DuplicateTrackID : public VoiceDataError
{
public:
    DuplicateTrackID(const std::string& what): VoiceDataError(what) {}
};

typedef ex::ptr_map<UInt16, VoiceTrack> VoiceTrackByTrackIdT;

// The map type wrapped by ex::ptr_map
ROCS_CORE_TEMPLATE_DECLSPEC(std::map <
    UInt16,	
	std::shared_ptr<VoiceTrack>,
	std::less<UInt16>,
	std::allocator < std::pair<const UInt16, VoiceTrack>>>);

ROCS_CORE_TEMPLATE_DECLSPEC(ex::ptr_map<
    UInt16,
    VoiceTrack,
    std::less<UInt16>,
	std::allocator < std::pair<const UInt16, VoiceTrack>>>);

// ROCS_CORE_TEMPLATE_DECLSPEC(EXLIB_PTR_MAP_CONTAINER_T(UInt16, VoiceTrack));
typedef ex::MapIters<VoiceTrackByTrackIdT>::key_iterator TrackIdIterator;
typedef ex::MapIters<VoiceTrackByTrackIdT>::value_iterator TrackIterator;

class ROCS_CORE_API VoiceData
{
public:
    VoiceData() {}
 
    VoiceData(const std_midi::MIDIFile &midiFile);
 
    VoiceData(std::istream &, const cmn::FileVersion &);
 
    ~VoiceData() {}
 
    void WriteBinary(std::ostream &os, const cmn::FileVersion &) const;
 
    void WriteString(std::ostream &os) const;
 
    void WriteStringTracks(std::ostream& os, int indent) const;
 
    void WriteStringTracksEvents(std::ostream& os, int indent) const;

    UInt16 GetTrackCount() const { return this->voiceTracksByTrackId_.size(); }
 
    VoiceTrack& GetTrack(UInt16 track_id);
 
    const VoiceTrack& GetTrack(UInt16 track_id) const;
 
    void AddTrack(VoiceTrackPtrT vt_ptr);
 
    // tracks are stored as a Map with track_id as key, and VoiceTrack as value
    const VoiceTrackByTrackIdT& GetTracks() const { return this->voiceTracksByTrackId_; }
 
    VoiceTrackByTrackIdT& GetTracks() { return this->voiceTracksByTrackId_; }

    // Iterate over just the track_ids
    TrackIdIterator GetTrackIdsBegin() const
    {
        return TrackIdIterator(this->voiceTracksByTrackId_.begin());
    }
 
    TrackIdIterator GetTrackIdsEnd() const
    {
        return TrackIdIterator(this->voiceTracksByTrackId_.end());
    }

    // Iterator over just the tracks
    TrackIterator GetTracksBegin() const
    {
        return TrackIterator(this->voiceTracksByTrackId_.begin());
    }
 
    TrackIterator GetTracksEnd() const
    {
        return TrackIterator(this->voiceTracksByTrackId_.end());
    }

    // Retrieve a vector of shared_ptr's to all of the tracks belonging to group_id
    VoiceTrackPtrVecT GetGroup(UInt8 group_id);
 
    const VoiceTrackPtrVecT GetGroup(UInt8 group_id) const;

    const Groups& GetGroups() const { return this->groups_; }
 
    Groups& GetGroups() { return this->groups_; }

protected:
    VoiceTrackByTrackIdT voiceTracksByTrackId_;
    Groups groups_;
};


ROCS_CORE_API bool operator==(const VoiceData &lhs, const VoiceData &rhs);

typedef std::shared_ptr<VoiceData> VoiceDataPtrT;
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<VoiceData>);

} // end namespace rocs_midi
