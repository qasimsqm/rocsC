#include "core/std_midi/midi_file.h"

using namespace std;

namespace std_midi
{

MIDIFile::MIDIFile(const string &midi_file_name)
    :
    filename_(midi_file_name)
{
    ifstream ifs;
    ifs.exceptions(ifstream::failbit | ifstream::badbit);
    ifs.open(filename_.c_str(), fstream::in | fstream::binary);
    ifs.read((char *)&this->header_, MIDI_HEADER_SIZE);
    
    if (this->header_.chunkType() != "MThd")
    {
        throw MIDIFileError("Not a valid midi file.");
    }

    ex::reverse_byte_order(this->header_.length);
    ex::reverse_byte_order(this->header_.format);
    ex::reverse_byte_order(this->header_.ntrks);
    ex::reverse_byte_order(this->header_.division);

    if (this->header_.division & 0x8000)
    {
        throw MIDIFileError("SMPTE division not supported.");
    }

    if (!header_.ntrks)
    {
        throw MIDIFileError(
            midi_file_name + 
            " appears to be corrupted because track count is set to zero.");
    }

    for (int i = 0; i < header_.ntrks; i++)
    {
        try
        {
            tracks_.push_back(MIDITrackPtrT(new MIDITrack(ifs, i)));
        } catch (UnrecognizedTrackID &e)
        {
            warnings << e.what() << endl;
            continue;
        }
    }

    this->length_ = 0;
    for (auto it: this->tracks_)
    {
        this->length_ = max(this->length_, it.GetLength());
    }
}

void MIDIFile::WriteString(ostream &os) const
{
    os  << "MIDIFile(filename=" << this->GetFilename()
        << ", format=" << this->GetFormat()
        << ", ntrks=" << this->GetTrackCount()
        << ", division=" << this->GetDivision() << ")";
}

void MIDIFile::WriteStringTracks(ostream &os, int indent) const
{
    os << string(indent, '\t');
    this->WriteString(os);
    os << endl;
    string indent_(indent + 1, '\t');
    
    for (auto it : tracks_)
    {
        os << indent_;
        it.WriteString(os);
        os << endl;
    }
    
    cout << endl;
}

void MIDIFile::WriteStringAll(ostream &os, int indent) const
{
    os << string(indent, '\t');
    this->WriteString(os);
    os << endl;
    for (auto it : tracks_)
    {
        it.WriteStringPackets(os, indent+1);
        os << endl;
    }
}

void MIDIFile::WriteBinary(ostream &os, const cmn::FileVersion &) const
{
    MIDIFileHeader big_endian_hdr(header_);
    ex::reverse_byte_order(big_endian_hdr.length);
    ex::reverse_byte_order(big_endian_hdr.format);
    ex::reverse_byte_order(big_endian_hdr.ntrks);
    ex::reverse_byte_order(big_endian_hdr.division);
    os.tellp();
    // Incomplete ...
}

/* Finds a track based on its track name.  Throws MIDIFileError if trackName
 * does not exist. */
const MIDITrack &MIDIFile::GetTrack(const std::string &trackName) const
{
    auto it = find_if(
        this->tracks_.begin(),
        this->tracks_.end(),
        [trackName] (const MIDITrack &track)
        {
            return track.GetTrackName() == trackName;
        });
    
    if (it != this->tracks_.end())
    {
        return *it;
    } else
    {
        throw MIDIFileError(trackName + " does not exist.");
    }
}


} // End namespace std_midi
