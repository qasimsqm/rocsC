#pragma once

#include "core/win32/declspec.h"

#include <vector>
#include <set>
#include <string>
#include <istream>
#include <ostream>
#include <algorithm> // equal
#include <stdexcept> // invalid argument
#include "exlib/xplatform_types.h"
#include "exlib/format.h"
#include "exlib/string_lib.h"
#include "core/rocs_midi/rocs_midi_exception.h"
#include "core/rocs_midi/voice_track.h"

namespace rocs_midi
{

class ROCS_CORE_API GroupsException : public ROCSMIDIException
{
public:
    GroupsException(const std::string& what): ROCSMIDIException(what) {}
};

class ROCS_CORE_API InvalidPort : public GroupsException
{
public:
    InvalidPort(const std::string& what): GroupsException(what) {}
};

class ROCS_CORE_API InvalidChannel : public GroupsException
{
public:
    InvalidChannel(const std::string& what): GroupsException(what) {}
};

class ROCS_CORE_API InvalidToken : public GroupsException
{
public:
    InvalidToken(const std::string& what): GroupsException(what) {}
};

ROCS_CORE_TEMPLATE_DECLSPEC(std::map<UInt8, std::string>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::map<std::string, UInt8>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::set<UInt16>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::map<UInt8, std::set<UInt16>>);

class ROCS_CORE_API Groups
{
protected:
    typedef std::map<UInt8, std::string> GroupNameByGroupIdT;
    typedef std::map<std::string, UInt8> GroupIdByGroupNameT;

public:
    typedef std::map<UInt8, std::set<UInt16>> GroupByGroupIdT;
    typedef GroupByGroupIdT::iterator iterator;
    typedef GroupByGroupIdT::const_iterator const_iterator;

    Groups(): lastGroupId_(0) {}

    Groups(std::istream &is, const cmn::FileVersion &);

    // Only used when loading tracks from a std_midi::MIDIFile
    void AddTrack(VoiceTrackPtrT voiceTrackPtr);
    
    bool operator==(const Groups &rhs) const;

    bool operator!=(const Groups &rhs) const { return !operator==(rhs); }
    
    std::set<std::string> GetNames() const;
    
    std::string GetGroupName(UInt8 group_id) const;
        
    UInt8 GetGroupId(const std::string &group_name) const;    
    
    const std::set<UInt16>& GetGroup(UInt8 group_id) const;
    
    GroupByGroupIdT & GetGroups() { return groups_; }

    void WriteBinary(std::ostream &os, const cmn::FileVersion &) const;

public:
    // plumbing to std::map
    iterator begin() { return groups_.begin(); }

    const_iterator begin() const { return groups_.begin(); }

    void clear();

    iterator end() { return groups_.end(); }

    const_iterator end() const { return groups_.end(); }

protected:
    UInt8 lastGroupId_;
    GroupNameByGroupIdT groupNameByGroupId_;
    GroupIdByGroupNameT groupIdByGroupName_;
    GroupByGroupIdT groups_;
};

} // namespace rocs_midi
