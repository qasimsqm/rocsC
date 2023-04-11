#include "core/rocs_midi/song_data.h"

using namespace std;

namespace rocs_midi
{

SongData::SongData()
    :
    timeline_(new TL::Timeline()),
    voice_data_(new VoiceData())
{}

SongData::SongData(const SongData& other)
    :
    song_number_(other.song_number_),
    song_name_(other.song_name_),
    ppqn_(other.ppqn_),
    song_length_(other.song_length_),
    timeline_(other.timeline_),
    voice_data_(other.voice_data_)
{}

SongData& SongData::operator=(const SongData& other)
{
    if (this == &other)
    {
        throw logic_error("SongData::operator= Copy to self");
    } 
    
    this->song_number_ = other.song_number_;
    this->song_name_ = other.song_name_;
    this->ppqn_ = other.ppqn_;
    this->song_length_ = other.song_length_;
    this->timeline_.reset();
    this->voice_data_.reset();
    this->timeline_ = other.timeline_;
    this->voice_data_ = other.voice_data_;
    return *this;
}

SongData::SongData(istream& is, const cmn::FileVersion &fileVersion)
{
    this->song_number_ = ex::ReadString(is);
    this->song_name_ = ex::ReadString(is);
    is.read((char *)&this->ppqn_, sizeof(this->ppqn_));
    is.read((char *)&this->song_length_, sizeof(this->song_length_));
    this->timeline_ = TL::TimelinePtrT(new TL::Timeline(is, fileVersion));
    this->voice_data_ = VoiceDataPtrT(new VoiceData(is, fileVersion));
}

SongData::SongData(const std_midi::MIDIFile& midiFile)
{
    this->ppqn_ = midiFile.GetDivision();
    this->song_length_ = midiFile.GetLength();
    UInt8 attacca_placeholder;
    cmn::parse_filename(
        midiFile.GetFilename(), 
        this->song_number_, 
        this->song_name_, 
        attacca_placeholder);
    
    warnings << endl << "Warnings for " << midiFile.GetFilename() << ":" << endl;
    
    this->timeline_ = TL::TimelinePtrT(new TL::Timeline(midiFile));
    
    try
    {
        this->voice_data_ = VoiceDataPtrT(new VoiceData(midiFile));
    } catch (GroupsException &e)
    {
        throw GroupsException(ex::format(
            "%s: %s '%s'",
            this->GetSongNumber().c_str(),
            this->GetSongName().c_str(),
            e.what()));
    }
}


void SongData::WriteBinary(ostream &os, const cmn::FileVersion &fileVersion) const
{
    ex::WriteString(os, this->GetSongNumber());
    ex::WriteString(os, this->GetSongName());
    os.write((char *)&this->ppqn_, sizeof(this->ppqn_));
    os.write((char *)&this->song_length_, sizeof(this->song_length_));
    this->timeline_->WriteBinary(os, fileVersion);
    this->voice_data_->WriteBinary(os, fileVersion);
}


void SongData::WriteString(ostream &os) const
{
    os  << "SongData("
        << "song_number=" << this->GetSongNumber() << ", "
        << "song_name=" << this->GetSongName() << ", "
        << "ppqn=" << this->GetPulsesPerQuarterNote() << ", "
        << "song_length=" << this->GetSongLength() << ", "
        << "track_count=" << this->voice_data_->GetTrackCount()
        << ")";
}

void SongData::WriteStringTracks(ostream &os, int indent) const
{
    os << string(indent, '\t');
    this->WriteString(os);
    os << endl;
    this->voice_data_->WriteStringTracks(os, indent + 1);
}

void SongData::WriteStringTracksEvents(ostream &os, int indent) const
{
    os << string(indent, '\t');
    this->WriteString(os);
    os << endl;
    this->voice_data_->WriteStringTracksEvents(os, indent + 1);
}

void SongData::WriteStringTimeline(ostream &os, int indent) const
{
    os << string(indent, '\t');
    this->WriteString(os);
    os << endl;
    this->timeline_->WriteStringDetailed(os, indent + 1);
}

void SongData::SetTimeline(TL::TimelinePtrT timelinePtr)
{
    this->timeline_.reset();
    this->timeline_ = timelinePtr;
}

void SongData::SetVoiceData(VoiceDataPtrT voiceDataPtr)
{
    this->voice_data_.reset();
    this->voice_data_ = voiceDataPtr;
}

ROCS_CORE_API bool operator==(const SongData &lhs, const SongData &rhs)
{
    if (lhs.GetSongNumber() != rhs.GetSongNumber()) return false;
    if (lhs.GetSongName() != rhs.GetSongName()) return false;
    if (lhs.GetPulsesPerQuarterNote() != rhs.GetPulsesPerQuarterNote()) return false;
    if (lhs.GetSongLength() != rhs.GetSongLength()) return false;
    if (!(lhs.GetTimeline() == rhs.GetTimeline())) return false;
    return lhs.GetVoiceData() == lhs.GetVoiceData();
}

} // end namespace rocs_midi
