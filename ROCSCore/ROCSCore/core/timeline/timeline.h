#pragma once

#include "core/win32/declspec.h"

#include <ostream>
#include <istream>
#include <string>
#include <vector>
#include <algorithm>

#include "exlib/xplatform_types.h"
#include "exlib/binary_string_io.h"
#include "exlib/format.h"
#include "exlib/string_lib.h"
#include "core/common/warnings.h"
#include "core/timeline/timeline_exception.h"
#include "core/timeline/tl_sequences.h"
#include "core/timeline/tl_events.h"
#include "core/timeline/midi_loaders.h"
#include "core/std_midi/midi_file.h"
#include "core/rocs_midi/show_data_version.h"

namespace TL
{

class ROCS_CORE_API UnknownSequenceType : public TimelineException
{
public:
    inline UnknownSequenceType(const std::string& what): TimelineException(what) {}
};

class ROCS_CORE_API MissingEvents : public TimelineException
{
public:
    inline MissingEvents(const std::string& what): TimelineException(what) {}
};

class ROCS_CORE_API Timeline
{
public:
    Timeline();

    Timeline(const Timeline& other);

    Timeline(const std_midi::MIDIFile&);

    Timeline(std::istream &, const cmn::FileVersion &);

    Timeline& operator=(const Timeline& other);
    
    void WriteString(std::ostream&) const;

    void WriteStringDetailed(std::ostream&, int indent=0) const;

    void WriteBinary(std::ostream &, const cmn::FileVersion &fileVersion) const;
    
    UInt32 GetBarOne(unsigned int pulsesPerQuarterNote) const;
 
    UInt32 GetMidiStartTicks() const { return this->midiStartTicks_; }
 
    UInt32 GetMidiEndTicks() const { return this->midiEndTicks_; }
 
    UInt32 GetAudioStartTicks() const { return this->audioStartTicks_; }
 
    UInt32 GetAudioEndTicks() const { return this->audioEndTicks_; }
    
    void SetMidiStartTicks(UInt32 midiStartTicks)
    {
        this->midiStartTicks_ = midiStartTicks;
    }

    void SetMidiEndTicks(UInt32 midiEndTicks)
    {
        this->midiEndTicks_ = midiEndTicks;
    }
    
    void SetAudioStartTicks(UInt32 audioStartTicks)
    {
        this->audioStartTicks_ = audioStartTicks;
    }
    
    void SetAudioEndTicks(UInt32 audioEndTicks)
    {
        this->audioEndTicks_ = audioEndTicks;
    }
    
    TempoSeqT& GetTempos() { return *tempos_; }
 
    BarsBeatsSeqT& GetBarsBeats() { return *bars_beats_; }
 
    KeySeqT& GetKeys() { return *keys_; }
 
    PageNumSeqT& GetPageNums() { return *page_nums_; }
 
    MeterSeqT& GetMeters() { return *meters_; }
 
    const TempoSeqT& GetTempos() const { return *tempos_; }
 
    const BarsBeatsSeqT& GetBarsBeats() const { return *bars_beats_; }
 
    const KeySeqT& GetKeys() const { return *keys_; }
 
    const PageNumSeqT& GetPageNums() const { return *page_nums_; }
 
    const MeterSeqT& GetMeters() const { return *meters_; }
 
    const cmn::ROCSSeqPtrVecT& GetAllSequences() const { return all_seqs_; }
               
private:
    UInt32 midiStartTicks_;
    UInt32 midiEndTicks_;
    UInt32 audioStartTicks_;
    UInt32 audioEndTicks_;
    TempoSeqPtrT tempos_;
    BarsBeatsSeqPtrT bars_beats_;
    KeySeqPtrT keys_;
    PageNumSeqPtrT page_nums_;
    MeterSeqPtrT meters_;
    cmn::ROCSSeqPtrVecT all_seqs_;
    void make_all_seqs();
    void process_meta_messages(const std_midi::MetaMsgPtrVecT&, UInt32);
};

ROCS_CORE_API bool operator==(const Timeline& lhs, const Timeline& rhs);

typedef std::shared_ptr<Timeline> TimelinePtrT;
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<Timeline>);

} // end namespace TL
