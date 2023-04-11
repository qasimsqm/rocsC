#pragma once

#include "core/win32/declspec.h"

#include <string>
#include <vector>

#include <memory>

#include <istream>
#include <ostream>
#include <fstream>
#include <stdexcept>
#include <algorithm> // find_if, max

#include "exlib/reverse_byte_order.h"
#include "core/common/rocs_exception.h"
#include "core/common/warnings.h"
#include "core/std_midi/midi_track.h"
#include "core/std_midi/midi_packet.h"

#define MIDI_HEADER_SIZE 14

#include "core/win32/declspec.h"

namespace std_midi {

// http://msdn.microsoft.com/en-us/library/3tdb471s.aspx
// C4275 can be ignored in Visual C++ if you are deriving from a type in the Standard C++ Library
MSC_DISABLE_WARNING(4275);
class ROCS_CORE_API MIDIFileError : public std::runtime_error
{
public:
    MIDIFileError(const std::string& what): std::runtime_error(what) {}
};
MSC_RESTORE_WARNING(4275);

struct ROCS_CORE_API MIDIFileHeader
{
    char    chunk_type[4];
    UInt32  length;
    UInt16  format;
    UInt16  ntrks;
    UInt16  division;
    
    std::string chunkType() const { return std::string(this->chunk_type, 4); }
    
    void WriteString(std::ostream& os) const
    {
        os << std::showbase << std::hex;
        os << "MIDIFileHeader( ";
        char *hdr_ptr = (char *)this;
        for (int i = 0; i < MIDI_HEADER_SIZE; i++)
        {
            os << SInt16(*(hdr_ptr + i)) << " ";
        }

        os << ")" << std::dec << std::noshowbase;
    }
};

class ROCS_CORE_API MIDIFile
{
public:
    MIDIFile(const std::string &midi_file_name); 
 
    virtual ~MIDIFile() {}
 
    void WriteString(std::ostream &os) const;
 
    void WriteBinary(std::ostream &, const cmn::FileVersion &fileVersion) const;
 
    void WriteStringTracks(std::ostream &os, int indent=0) const;
 
    void WriteStringAll(std::ostream &os, int indent=0) const;

    std::string GetFilename() const { return this->filename_; }
 
    UInt32 GetLength() const { return this->length_; }
 
    UInt16 GetFormat() const { return this->header_.format; }
 
    UInt16 GetTrackCount() const { return this->header_.ntrks; }
 
    UInt16 GetDivision() const { return this->header_.division; }

    const MIDITrackVecT & GetTracks() const { return this->tracks_; }

    const MIDIPacketVecT & GetConductorPackets() const
    {
        return this->tracks_.at(0).GetPackets();
    }

    /* Finds a track based on its track name.  Throws MIDIFileError if
     * trackName does not exist. */
    const MIDITrack & GetTrack(const std::string &trackName) const;
 
private:
	MSC_DISABLE_WARNING(4251);
	std::string filename_;
	MSC_RESTORE_WARNING(4251);
    MIDIFileHeader header_;
    UInt32 length_;
    MIDITrackVecT tracks_;

};

typedef std::shared_ptr<MIDIFile> MIDIFilePtrT;

} // End namespace std_midi
