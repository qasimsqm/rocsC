#include "core/rocs_midi/groups.h"

using namespace std;

namespace rocs_midi
{

Groups::Groups(istream &is, const cmn::FileVersion &)
{
    UInt8 groupId;
    string gname;
    UInt16 track_id_count;
    vector<UInt16> track_ids;
    UInt8 grp_count = is.get();
    
    while (grp_count > 0)
    {
        groupId = is.get();
        gname = ex::ReadString(is);
        this->groupNameByGroupId_[groupId] = gname;
        this->groupIdByGroupName_[gname] = groupId;
        is.read((char *)&track_id_count, sizeof(track_id_count));
        track_ids.resize(track_id_count);
        is.read((char *)&track_ids[0], sizeof(UInt16) * track_id_count);
        this->groups_[groupId].insert(track_ids.begin(), track_ids.end());
        grp_count--;
    }
}

void Groups::WriteBinary(ostream &os, const cmn::FileVersion &) const
{
    UInt16 track_id_count;
    vector<UInt16> track_ids;
    UInt8 group_count = this->groups_.size();
    os.put(group_count);

    for (auto it: this->groups_)
    {   
        // write the group_id
        os.put(it.first);
        
        // write the group_name
        ex::WriteString(os, this->groupNameByGroupId_.at(it.first)); 
        
        track_id_count = this->groups_.at(it.first).size();
        track_ids.assign(
            this->groups_.at(it.first).begin(),
            this->groups_.at(it.first).end());
        
        os.write((char *)&track_id_count, sizeof(track_id_count));
        os.write((char *)&track_ids[0], sizeof(UInt16) * track_id_count);
    }
}

void Groups::AddTrack(VoiceTrackPtrT voiceTrackPtr)
{
    string groupName;
    UInt8 groupId;
    string originalTrackName = voiceTrackPtr->GetTrackName();
    std::initializer_list<const std::string> tokens = { "@g", "@p", "@ch" };
    
    auto trackNameParts = ex::tokenize_string(
        tokens.begin(),
        tokens.end(),
        voiceTrackPtr->GetTrackName());
    
    voiceTrackPtr->SetTrackName(trackNameParts.front());
    trackNameParts.pop_front();
    UInt16 port, channel;
    bool hasPort = false;
    bool hasChannel = false;
    
    while (!trackNameParts.empty())
    {
        if (trackNameParts.front() == "@g")
        {
            trackNameParts.pop_front();
            groupName = trackNameParts.front();
            trackNameParts.pop_front();
            if (this->groupIdByGroupName_.count(groupName))
            {
                groupId = this->groupIdByGroupName_[groupName];
                voiceTrackPtr->SetGroupId(groupId);
                this->groups_[groupId].insert(voiceTrackPtr->GetTrackId());
            } else
            {
                this->lastGroupId_ += 1;
                this->groupNameByGroupId_[this->lastGroupId_] = groupName;
                this->groupIdByGroupName_[groupName] = this->lastGroupId_;
                voiceTrackPtr->SetGroupId(this->lastGroupId_);
                this->groups_[this->lastGroupId_].insert(voiceTrackPtr->GetTrackId());
            }
        } else if (trackNameParts.front() == "@p")
        {
            trackNameParts.pop_front();
            if (!ex::str_to_num(trackNameParts.front(), port))
            {
                throw InvalidPort(
                    trackNameParts.front() +
                    " is not a valid port number. track name: \"" + originalTrackName
                    + "\"");
            } else
            {
                hasPort = true;
                trackNameParts.pop_front();
            }
        } else if (trackNameParts.front() == "@ch")
        {
            trackNameParts.pop_front();
            if (!ex::str_to_num(trackNameParts.front(), channel))
            {
                throw InvalidChannel(trackNameParts.front() +
                " is not a valid channel number. track name: \"" + originalTrackName
                + "\"");
            } else
            {
                hasChannel = true;
                trackNameParts.pop_front();
            }
        } else
        {
            throw InvalidToken(trackNameParts.front() +
                " is not a valid track name token. track name: \"" + originalTrackName
                + "\"");
        }
    }

    if (hasPort && hasChannel)
    {
        voiceTrackPtr->SetChannelNumber(
            static_cast<UInt16>(((port - 1) << 4) | (channel - 1)));
    }
}


bool Groups::operator==(const Groups& rhs) const
{
    if (this->groups_.size() != rhs.groups_.size())
    {
        return false;
    }

    for (auto it: this->groups_)
    {
        try
        {
            if (!equal(
                it.second.begin(),
                it.second.end(),
                rhs.GetGroup(it.first).begin()))
            {
                cout << "Groups compares != for group " << UInt16(it.first) << endl;
                return false;
            }
        } catch (invalid_argument &)
        {
            return false;
        }
    }

    return true;
}

set<string> Groups::GetNames() const
{
    set<string> names;
    for (auto it: this->groupIdByGroupName_)
    {
        names.insert(it.first);
    }

    return names;
}

#ifndef NDEBUG
    #define CHECK_GID(groupId) \
        if (!this->groupNameByGroupId_.count(groupId)) \
            throw invalid_argument(ex::format("groupId %u does not exist!", groupId))
    #define CHECK_NAME(name) \
        if (!this->groupIdByGroupName_.count(name)) \
            throw invalid_argument("group name " + name + " does not exist!")
#else
    #define CHECK_GID(groupId)
    #define CHECK_NAME(name)
#endif


string Groups::GetGroupName(UInt8 groupId) const
{
    CHECK_GID(groupId);
    return this->groupNameByGroupId_.at(groupId);
}

UInt8 Groups::GetGroupId(const string& groupName) const
{
    CHECK_NAME(groupName);
    return this->groupIdByGroupName_.at(groupName);
}

const set<UInt16>& Groups::GetGroup(UInt8 groupId) const
{
    CHECK_GID(groupId);
    return this->groups_.at(groupId);
}

#undef CHECK_GID
#undef CHECK_NAME

void Groups::clear()
{
    this->lastGroupId_ = 0;
    this->groupNameByGroupId_.clear();
    this->groupIdByGroupName_.clear();
    this->groups_.clear();
}

} // namespace rocs_midi
