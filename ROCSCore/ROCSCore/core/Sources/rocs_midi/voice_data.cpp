#include <iostream>
#include <iterator>

#include "core/rocs_midi/voice_data.h"

using namespace std;

namespace rocs_midi
{

VoiceData::VoiceData(const std_midi::MIDIFile &midiFile)
{
    // Read all tracks except the conductor track and the pdf track
    list<std_midi::MIDITrackPtrT> tracks(
        midiFile.GetTracks().std_begin(),
        midiFile.GetTracks().std_end());

    // The tracks should be sorted already, but for good measure...
    tracks.sort(
        [] (std_midi::MIDITrackPtrT first, std_midi::MIDITrackPtrT second)
        {
            return first->GetTrackId() < second->GetTrackId();
        });

    // Remove the conductor track;
    tracks.pop_front();

    auto it_end = remove_if(
        tracks.begin(),
        tracks.end(),
        [](std_midi::MIDITrackPtrT t)
        {
            return ((t->GetTrackName() == "PDF") || (t->GetTrackName() == "pdf"));
        });

    UInt16 trackId = 0;
    VoiceTrackPtrT voiceTrackPtr;
    
    for (auto it = tracks.begin(); it != it_end; it++)
    {
        voiceTrackPtr = VoiceTrackPtrT(new VoiceTrack(**it));
        voiceTrackPtr->SetTrackId(trackId);
        this->groups_.AddTrack(voiceTrackPtr);
        this->voiceTracksByTrackId_.insert(trackId, voiceTrackPtr);
        voiceTrackPtr.reset();
        trackId++;
    }
}

VoiceData::VoiceData(istream &is, const cmn::FileVersion &fileVersion)
{
    char vdat[4];
    is.read(&vdat[0], 4);
    
    if (string(&vdat[0], 4) != "VDat")
    {
        throw VoiceDataError("Invalid VoiceData header: " + string(&vdat[0], 4));
    }

    this->groups_ = Groups(is, fileVersion);
    UInt16 trackCount;
    is.read((char *)&trackCount, sizeof(trackCount));
    VoiceTrackPtrT voiceTrackPtr;
    while (trackCount > 0) {
        voiceTrackPtr = VoiceTrackPtrT(new VoiceTrack(is, fileVersion));
        this->voiceTracksByTrackId_.insert(voiceTrackPtr->GetTrackId(), voiceTrackPtr);
        voiceTrackPtr.reset();
        trackCount--;
    }
}

void VoiceData::WriteBinary(ostream &os, const cmn::FileVersion &fileVersion) const
{
    string vdat("VDat");
    os.write(vdat.c_str(), 4);
    this->groups_.WriteBinary(os, fileVersion);
    UInt16 trackCount = this->GetTrackCount();
    os.write((char *)&trackCount, sizeof(trackCount));
    
    for (auto it: this->voiceTracksByTrackId_)
    {
        it.second->WriteBinary(os, fileVersion);
    }
}

void VoiceData::WriteString(ostream &os) const
{
    os  << "VoiceData("
        << "track_count=" << this->GetTrackCount()
        << ")";
}

void VoiceData::WriteStringTracks(ostream &os, int indent) const
{
    os << string(indent, '\t');
    this->WriteString(os);
    os << endl;
    string indentString(indent + 1, '\t');
    for (auto it: this->voiceTracksByTrackId_)
    {
        os << indentString;
        it.second->WriteString(os);
        os << endl;
    }
}

void VoiceData::WriteStringTracksEvents(ostream &os, int indent) const
{
    os << string(indent, '\t');
    this->WriteString(os);
    os << endl;
    for (auto it: this->voiceTracksByTrackId_)
    {
        it.second->WriteStringEvents(os, indent + 1);
    }
}

#ifndef NDEBUG
    #define CHECK_TRACK_ID(trackId) \
        if (!this->voiceTracksByTrackId_.count(trackId)) \
            throw out_of_range(ex::format("%u not found!", trackId))
#else
    #define CHECK_TRACK_ID(trackId)
#endif

VoiceTrack& VoiceData::GetTrack(UInt16 trackID)
{
    CHECK_TRACK_ID(trackID);
    return this->voiceTracksByTrackId_.at(trackID);
}

const VoiceTrack& VoiceData::GetTrack(UInt16 trackID) const
{
    CHECK_TRACK_ID(trackID);
    return this->voiceTracksByTrackId_.at(trackID);
}

void VoiceData::AddTrack(VoiceTrackPtrT voiceTrackPtr)
{
    this->voiceTracksByTrackId_.erase(voiceTrackPtr->GetTrackId());
    this->voiceTracksByTrackId_.insert(voiceTrackPtr->GetTrackId(), voiceTrackPtr);
}

VoiceTrackPtrVecT VoiceData::GetGroup(UInt8 group_id)
{
    VoiceTrackPtrVecT group_v;
    for (auto it: groups_.GetGroup(group_id))
    {
        /** Use find rather than operator[] so that the shared_ptr to the track can
            be retrieved. operator[] on a ptr_map returns the reference. **/
        CHECK_TRACK_ID(it);
        auto found = this->voiceTracksByTrackId_.find(it);
        group_v.push_back(found->second);
    }
    return group_v;
}

const VoiceTrackPtrVecT VoiceData::GetGroup(UInt8 groupId) const
{
    return const_cast<VoiceData *>(this)->GetGroup(groupId);
}

#undef CHECK_TRACK_ID

ROCS_CORE_API bool operator==(const VoiceData& lhs, const VoiceData& rhs)
{
    if (lhs.GetTrackCount() != rhs.GetTrackCount()) return false;
    if (lhs.GetGroups() != rhs.GetGroups()) return false;
    return equal(
        lhs.GetTracksBegin(),
        lhs.GetTracksEnd(),
        rhs.GetTracksBegin(),
        [] (const VoiceTrack& x, const VoiceTrack& y)->bool
        {
            return x == y;
        });
}

} // end namespace rocs_midi
