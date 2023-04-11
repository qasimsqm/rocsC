#pragma once

#include "core/win32/declspec.h"

#include <istream>
#include <ostream>
#include <iostream>
#include <vector>
#include <deque>
#include <string>
#include <algorithm> // find_if
#include <sstream>
#include <stdexcept> // out_of_range

#include "exlib/cxx11.h"
#ifdef CXX11
    #include <memory>
#else
    #include <tr1/memory>
#endif

#include "exlib/format.h"
#include "exlib/string_lib.h"

#include "core/common/warnings.h"
#include "core/common/parse_filename.h"
#include "core/rocs_midi/voice_track.h"
#include "core/std_midi/midi_file.h"
#include "core/timeline/timeline.h"
#include "core/rocs_midi/voice_data.h"


namespace rocs_midi
{

class ROCS_CORE_API SongData
{
public:
    SongData();
 
    SongData(const SongData& other);
 
    SongData(
        const std::string& song_number,
        const std::string song_name,
        UInt16 ppqn,
        UInt32 song_length)
        :
        song_number_(song_number),
        song_name_(song_name),
        ppqn_(ppqn),
        song_length_(song_length)
    {
        this->timeline_ = TL::TimelinePtrT(new TL::Timeline());
        this->voice_data_ = VoiceDataPtrT(new VoiceData());
    }

    SongData(std::istream &is, const cmn::FileVersion &);
 
    SongData(const std_midi::MIDIFile& mf);

    SongData& operator=(const SongData& other);

    void WriteString(std::ostream&) const;
 
    void WriteBinary(std::ostream &, const cmn::FileVersion &) const;
 
    void WriteStringTracks(std::ostream& os, int indent) const;
 
    void WriteStringTracksEvents(std::ostream& os, int indent) const;
 
    void WriteStringTimeline(std::ostream& os, int indent) const;

    std::string GetSongNumber() const { return this->song_number_; }
 
    std::string GetSongName() const { return this->song_name_; }
 
    UInt16 GetPulsesPerQuarterNote() const { return this->ppqn_; }
 
    UInt32 GetSongLength() const { return this->song_length_; }
    
    const TL::Timeline& GetTimeline() const { return *this->timeline_; }
 
    const VoiceData& GetVoiceData() const { return *this->voice_data_; }

    TL::Timeline& GetTimeline() { return *this->timeline_; }
 
    VoiceData& GetVoiceData() { return *this->voice_data_; }

    void SetTimeline(TL::TimelinePtrT timeline_ptr);
 
    void SetVoiceData(VoiceDataPtrT voice_data_ptr);
 
    UInt32 GetBarOne() const { return this->timeline_->GetBarOne(this->ppqn_); }
 
private:
	MSC_DISABLE_WARNING(4251);
	std::string song_number_;
    std::string song_name_;
	MSC_RESTORE_WARNING(4251);
    UInt16 ppqn_;
    UInt32 song_length_;
    TL::TimelinePtrT timeline_;
    VoiceDataPtrT voice_data_;
};

ROCS_CORE_API bool operator==(const SongData& lhs, const SongData& rhs);

typedef std::shared_ptr<SongData> SongDataPtrT;

} // end namespace rocs_midi
