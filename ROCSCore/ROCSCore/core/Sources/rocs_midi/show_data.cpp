#include "core/rocs_midi/show_data.h"

using namespace std;

namespace rocs_midi
{

ShowData::ShowData(istream &is)
{
    cmn::FileVersion fileVersion;
    is.exceptions(istream::failbit|istream::badbit);
    if (!cmn::check_file_header(
        is,
        "ROCS",
        "SDAT",
        MinimumFileVersion,
        fileVersion))
    {
        // This input is less than the minimum supported version
        throw ShowDataVersionError("Unsupported ShowData Version");
    }

    this->show_name_ = ex::ReadString(is);
    UInt16 song_count;
    is.read((char *)&song_count, 2);

    SongDataPtrT song;

    for (; song_count > 0; song_count--)
    {
        song = SongDataPtrT(new SongData(is, fileVersion));
        this->songDataBySongNumber_.insert(song->GetSongNumber(), song);
        this->songNameBySongNumber_[song->GetSongNumber()] = song->GetSongName();
        song.reset();
    }
}


void ShowData::WriteBinary(ostream &os, const cmn::FileVersion &fileVersion) const
{
    cmn::ROCSFileHeader hdr(
        "ROCS",
        "SDAT",
        fileVersion.GetMajorVersion(),
        fileVersion.GetMinorVersion());

    os.write((char *)&hdr, sizeof(hdr));
    ex::WriteString(os, this->show_name_);
    UInt16 song_count = this->songDataBySongNumber_.size();
    os.write((char *)&song_count, sizeof(song_count));
    
    for (auto it: this->songDataBySongNumber_)
    {
        it.second->WriteBinary(os, fileVersion);
    }
}

void ShowData::WriteString(ostream& o) const
{
    o << "ShowData(" << this->show_name_ << ")";
    for ( auto it: this->songNameBySongNumber_ ) {
        o << '\t' << it.first << ": " << it.second << endl;
    }
}

const SongData& ShowData::GetSongData(const string& song_num) const
{
    return this->songDataBySongNumber_.at(song_num);
}

void ShowData::AddSongData(SongDataPtrT song_data_ptr)
{
    /*  If GetSongName() already exists in the map, remove it first so that the insert
        command is guaranteed to work.  */
    string song_num(song_data_ptr->GetSongNumber());
    this->songDataBySongNumber_.erase(song_num);
    this->songDataBySongNumber_.insert(song_num, song_data_ptr);
    this->songNameBySongNumber_.erase(song_num);
    this->songNameBySongNumber_[song_num] = song_data_ptr->GetSongName();
    song_data_ptr.reset();
}

string ShowData::sr_file_name() const
{
    return cmn::filter_filename(this->show_name_) + ".showdata";
}

string ShowData::fh_file_name() const
{
    return cmn::filter_filename(this->show_name_) + "-fh" + ".showdata";
}

ROCS_CORE_API bool operator==(const ShowData& lhs, const ShowData& rhs)
{
    if (lhs.GetShowName() != rhs.GetShowName()) return false;
    if (lhs.GetSongs().size() != rhs.GetSongs().size()) return false;
    return equal(
        lhs.GetSongDataBegin(),
        lhs.GetSongDataEnd(),
        rhs.GetSongDataBegin(),
        [] (const SongData& sd1, const SongData& sd2) -> bool
        {
            return sd1 == sd2;
        });
}


} // End namespace rocs_midi
