#include "core/rocs_midi/voice_track.h"

using namespace std;

namespace rocs_midi
{

/** Allowed CC messages are those that are allowed after time zero.  At time zero,
    RANGE_LOW_CC and RANGE_HIGH_CC are also allowed. **/

typedef set<UInt8> AllowedCCT;

ROCS_CORE_API AllowedCCT create_allowed_ccs()
{
    AllowedCCT allowed_ccs;
    allowed_ccs.insert(1); // Modulation Wheel
    allowed_ccs.insert(4); // Foot Pedal
    allowed_ccs.insert(7); // Volume
    allowed_ccs.insert(10); // Pan
    allowed_ccs.insert(11); // Expression
    allowed_ccs.insert(64); // Hold Pedal

    // Added for FillHarmonic
    allowed_ccs.insert(5); // Portamento Time
    allowed_ccs.insert(71); // Resonance
    allowed_ccs.insert(72); // Sound Release Time
    allowed_ccs.insert(73); // Sound Attack Time
    allowed_ccs.insert(84); // ?
    allowed_ccs.insert(90); // ?
    allowed_ccs.insert(91); // Reverb Level
    allowed_ccs.insert(93); // Chorus Level

    return allowed_ccs;
}

ROCS_CORE_API AllowedCCT allowed_ccs(create_allowed_ccs());

struct TransposeVoiceEvent
{
    TransposeVoiceEvent(SInt8 transpose, VoiceRangeT range)
        :
        transpose_(transpose),
        range_(range)
    {
        if (abs(transpose_) > 6) {
            throw std::range_error("transpose may be no more than + or - 6");
        }
    }

    void operator()(VoiceEvent& voiceEvent)
    {
        if ((this->range_.first <= voiceEvent.GetData1())
            && (voiceEvent.GetData1() <= this->range_.second))
        {
            voiceEvent.GetVoiceMessage().data1_ += this->transpose_;
            if (voiceEvent.GetData1() < this->range_.first)
            {
                voiceEvent.GetVoiceMessage().data1_ += 12;
            } else if (voiceEvent.GetData1() > range_.second)
            {
                voiceEvent.GetVoiceMessage().data1_ -= 12;
            }
        }
    }

private:
    SInt8 transpose_;
    VoiceRangeT range_;
};

VoiceTrack::VoiceTrack(std::istream &is, const cmn::FileVersion &)
{
    char vtrk[4];
    is.read(&vtrk[0], 4);
    if (string(&vtrk[0], 4) != "VTrk")
    {
        throw VoiceTrackException("Invalid track header: " + string(&vtrk[0], 4));
    }
    
    is.read((char *)&track_id_, sizeof(track_id_));
    this->SetRangeLow(is.get());
    this->SetRangeHigh(is.get());
    this->SetGroupId(is.get());
    this->track_name_ = ex::ReadString(is);
    
    UInt32 trackSize;
    is.read((char *)&trackSize, sizeof(trackSize));
    if (!trackSize)
    {
        return;
    }

    this->events_.resize(trackSize);
    is.read((char *)&events_[0], sizeof(VoiceEvent) * trackSize);

}

VoiceTrack::VoiceTrack(const std_midi::MIDITrack &midiTrack)
    :
    range_(make_pair(255, 255)),
    has_channel_number_(false)
{
    // Read the MetaMessages at time 0 first.  Meta messages in voice tracks
    // after time 0 will be ignored.
    if (midiTrack.GetPackets().at(0).delta_time() != 0)
    {
        throw MissingEvents(ex::format(
            "Required initial events not found for track_id %u",
            midiTrack.GetTrackId()));
    }
    
    UInt32 absTime = 0;

    auto meta_ptr = midiTrack.GetPackets().at(0).meta_filter(std_midi::sb::track_name);
    if (meta_ptr)
    {
        this->track_name_ = meta_ptr->message();
    } else
    {
        throw NoTrackName(ex::format(
            "Track name not found for track_id %u",
            midiTrack.GetTrackId()));
    }

    auto zeroth_msgs = midiTrack.GetPackets().at(0).voice_messages();
    for (auto it: zeroth_msgs)
    {
        if (it.GetCode() == std_midi::sb::control_change)
        {
            if (it.GetData1() == RANGE_LOW_CC)
            {
                if (this->GetRangeLow() != 255)
                {
                    warnings << "Track " << midiTrack.GetTrackName()
                        << " has multiple range_low CCs at time 0. Using first value = "
                        << static_cast<UInt16>(this->GetRangeLow()) << endl;
                } else
                {
                    this->SetRangeLow(it.GetData2());
                }
                continue;
            } else if (it.GetData1() == RANGE_HIGH_CC)
            {
                if (this->GetRangeHigh() != 255)
                {
                    warnings << "Track " << midiTrack.GetTrackName()
                        << " has multiple range_high CCs at time 0. Using first value = "
                        << static_cast<UInt16>(this->GetRangeHigh()) << endl;
                } else
                {
                    this->SetRangeHigh(it.GetData2());
                }
                
                continue;
            }
        }
        
        this->events_.push_back(VoiceEvent(absTime, it));
    }

    if (this->GetRange() == make_pair<UInt8, UInt8>(0, 0))
    {
        warnings << "Track \"" << midiTrack.GetTrackName() << "\" has range = (0, 0). "
                << "Assuming that this track is non-transposing." << endl;
        this->SetRangeLow(255);
        this->SetRangeHigh(255);
    }


    for (
        auto p_it = midiTrack.GetPackets().begin() + 1;
        p_it != midiTrack.GetPackets().end();
        p_it++)
    {
        absTime += p_it->delta_time();
        for (auto voiceIt : p_it->voice_messages())
        {
            if (voiceIt.GetCode() == std_midi::sb::control_change)
            {
                if (!allowed_ccs.count(voiceIt.GetData1()))
                {
                    if (voiceIt.GetData1() != 25)
                    {
                        // 25 is used to set audioStart/audioEnd 
                        warnings << "disallowed cc " 
                            << static_cast<UInt16>(voiceIt.GetData1()) 
                            << " found at "
                            << absTime << " in " << track_name_ << ". Ignoring."
                            << endl;
                    } else
                    {
                        warnings << "CC 25 found at " << absTime
                            << ", not in PDF track." << endl;
                    }

                    continue;
                }

                if (voiceIt.GetData1() == 64)
                {
                    if (!(voiceIt.GetData2() == 0 || voiceIt.GetData2() == 127 ))
                    {
                        warnings << "disallowed value for CC64: " 
                            << static_cast<UInt16>(voiceIt.GetData2())
                            << " found at " << absTime << " in " << this->track_name_
                            << ". Replacing with 0." << endl;
                        
                        events_.push_back(VoiceEvent(
                            absTime,
                            std_midi::VoiceMessage(
                                voiceIt.GetStatus(),
                                voiceIt.GetData1(),
                                0)));

                        continue;
                    }
                }
            }

            events_.push_back(VoiceEvent(absTime, voiceIt));
        }
    }

}


void VoiceTrack::WriteBinary(ostream &os, const cmn::FileVersion &) const
{
    UInt32 trackSize = events_.size();
    string vtrk("VTrk");
    os.write(vtrk.c_str(), 4);
    os.write((char *)&track_id_, sizeof(track_id_));
    os.put(this->GetRangeLow());
    os.put(this->GetRangeHigh());
    os.put(this->GetGroupId());
    ex::WriteString(os, this->track_name_);
    os.write((char *)&trackSize, sizeof(trackSize));
    os.write((char *)&events_[0], sizeof(VoiceEvent) * trackSize);
}


void VoiceTrack::WriteString(ostream &o) const
{
    o   << "ROCSVoiceTrack("
        << "track_name=" << track_name_
        << ", track_id=" << static_cast<UInt16>(track_id_)
        << ", track_size=" << events_.size()
        << ", range_low=" << static_cast<UInt16>(this->GetRangeLow())
        << ", range_high=" << static_cast<UInt16>(this->GetRangeHigh())
        << ")";
}


void VoiceTrack::WriteStringEvents(ostream &os, int indent) const
{
    map<UInt32, vector<VoiceEvent>> packets;
    for (auto it: this->events_)
    {
        packets[it.GetAbsTime()].push_back(it);
    }
    
    os << string(indent, '\t');
    this->WriteString(os);
    os << endl;
    string indent1(indent + 1, '\t');
    string indent2(indent + 2, '\t');
    
    for (auto it : packets)
    {
        os << indent1 << it.first << ": " << endl;
        for (auto eventIt: it.second)
        {
            os << indent2;
            eventIt.WriteStringMessage(os);
            os << endl;
        }
    }
}


void VoiceTrack::SetTrackId(UInt16 trackId)
{
    for (auto &it : this->events_)
    {
        it.SetChannel(static_cast<UInt8>(trackId));
        if (it.GetChannel() != (static_cast<UInt8>(trackId) & 0x0F))
        {
            throw std::runtime_error(ex::format(
                "VoiceTrack::track_id(%u), it.channel() = %u is not %u",
                trackId,
                it.GetChannel(),
                (trackId & 0x0F)));
        }
    }
    
    for (auto it: this->events_)
    {
        if (it.GetChannel() != (trackId & 0x0F))
        {
            throw std::runtime_error(ex::format(
                "VoiceTrack::track_id(%u) second pass, channel = %u is not %u",
                trackId,
                it.GetChannel(),
                (trackId & 0x0F)));
        }
    }
    
    this->track_id_ = trackId;
}


VoiceEvtVecT VoiceTrack::EventSlice(SInt64 start, SInt64 end, SInt8 transpose) const
{
    VoiceEvtVecT subRange;
    VoiceEvtVecT::const_iterator start_it, end_it;

    if (start < 0)
    {
        start_it = events_.begin();
    } else
    {
        start_it = lower_bound(
            this->events_.begin(),
            this->events_.end(),
            start,
            [](const VoiceEvent& evt, SInt64 start)->bool
            {
                return evt.GetAbsTime() < start;
            });
    }

    if (end < 0)
    {
        end_it = this->events_.end();
    } else
    {
        end_it = upper_bound(
            this->events_.begin(),
            this->events_.end(),
            end,
            [](SInt64 end, const VoiceEvent& evt)->bool
            {
                return (end < evt.GetAbsTime());
            });
    }

    copy(start_it, end_it, back_inserter(subRange));
    if (transpose && !(this->range_.first >= this->range_.second))
    {
        for_each(
            subRange.begin(),
            subRange.end(),
            TransposeVoiceEvent(transpose, this->range_));
    }

    return subRange;
}

ROCS_CORE_API bool operator==(const VoiceTrack &lhs, const VoiceTrack &rhs)
{
    if (lhs.GetTrackId() != rhs.GetTrackId()) return false;
    if (lhs.GetTrackName() != rhs.GetTrackName()) return false;
    if (lhs.GetRange() != rhs.GetRange()) return false;
    if (lhs.GetGroupId() != rhs.GetGroupId()) return false;
    if (lhs.GetEvents().size() != rhs.GetEvents().size()) return false;
    return equal(
        lhs.GetEvents().begin(),
        lhs.GetEvents().end(),
        rhs.GetEvents().begin(),
        [] (const rocs_midi::VoiceEvent& x, const rocs_midi::VoiceEvent& y)->bool
        {
            return x == y;
        });
}

} // end namespace rocs_midi
