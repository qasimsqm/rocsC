#pragma once

#include "core/win32/declspec.h"

#include <iostream> // for debug only
#include <ostream>
#include <istream>
#include <string>
#include <algorithm>

#include <mutex>
#include <memory>

#include "exlib/xplatform_types.h"
#include "exlib/binary_string_io.h"
#include "exlib/format.h"
#include "exlib/ptr_vector.h"
#include "core/common/parse_filename.h"
#include "core/common/attacca_defs.h"
#include "core/changelog/change_log_exception.h"
#include "core/changelog/cl_sequences.h"
#include "core/changelog/read_marker.h"
#include "core/changelog/cl_events.h"
#include "core/std_midi/midi_file.h"


// A ChangeLog is made up of individual SongLogs, one for each song of the
// show.

namespace CL
{

class ROCS_CORE_API SongLogException : public ChangeLogException
{
public:
    SongLogException(const std::string& what): ChangeLogException(what) {}
};

class ROCS_CORE_API UnknownSequenceType : public SongLogException
{
public:
    UnknownSequenceType(const std::string& what): SongLogException(what) {}
};

class ROCS_CORE_API MIDIFileLoopError : public SongLogException
{
public:
    MIDIFileLoopError(const std::string& what): SongLogException(what) {}
};

class ROCS_CORE_API SongLog
{
public:
    SongLog();

    SongLog(const SongLog& other);

    SongLog(
        const std::string &song_number_,
        const std::string &song_name_,
        UInt16 ppqn_,
        UInt32 song_length_);

    SongLog(std::istream &, const cmn::FileVersion &);

    SongLog(const std_midi::MIDIFile &);

    ~SongLog() {}

    SongLog& operator=(const SongLog& other);

    void WriteString(std::ostream&) const;

    void WriteStringDetailed(std::ostream&, int indent=0) const;

    void WriteBinary(std::ostream &, const cmn::FileVersion &) const;

    std::string GetSongNumber() const { return this->song_number_; }

    std::string GetSongName() const { return this->song_name_; }

    void SetSongNumber(const std::string& num) { this->song_number_ = num; }

    void SetSongName(const std::string& name) { this->song_name_ = name; }

    UInt16 GetPulsesPerQuarterNote() const { return this->ppqn_; }

    void SetPulsesPerQuarterNote(UInt16 val) { this->ppqn_ = val; }

    UInt32 GetSongLength() const { return this->song_length_; }

    UInt8 GetAttaccaState() const { return this->attacca_state_; }

    void SetAttaccaState(UInt8 attaccaState)
    {
        this->attacca_state_ = attaccaState;
    }

    const VampSeqT& GetVamps() const { return *vamps_; }

    const RepeatSeqT& GetRepeats() const { return *repeats_; }

    const CutSeqT& GetCuts() const { return *cuts_; }

    const TransposeSeqT& GetTranspositions() const { return *transpositions_; }

    const TempoScaleSeqT& GetTempoScales() const { return *tempo_scales_; }

    const MarkerSeqT& GetMarkers() const { return *markers_; }

    const ClickSeqT& GetClicks() const { return *clicks_; }

    const CaesuraSeqT& GetCaesuras() const { return *caesuras_; }

    const FermataSeqT& GetFermatas() const { return *fermatas_; }

    VampSeqT& GetVamps() { return *vamps_; }

    RepeatSeqT& GetRepeats() { return *repeats_; }

    CutSeqT& GetCuts() { return *cuts_; }

    TransposeSeqT& GetTranspositions() { return *transpositions_; }

    TempoScaleSeqT& GetTempoScales() { return *tempo_scales_; }

    MarkerSeqT& GetMarkers() { return *markers_; }

    ClickSeqT& GetClicks() { return *clicks_; }

    CaesuraSeqT& GetCaesuras() { return *caesuras_; }
    
    /* Return the caesuras for the current software version. */
    CaesuraSeqT GetDisplayCaesuras();

    FermataSeqT& GetFermatas() { return *fermatas_; }

    const cmn::ROCSSeqPtrVecT& GetAllSequences() const { return this->all_seqs_; }
    
    std::recursive_mutex &GetLock() { return this->lock_; }

private:
    std::string song_number_;
    std::string song_name_;
    UInt16 ppqn_;
    UInt32 song_length_;
    UInt8 attacca_state_;
    VampSeqPtrT vamps_;
    RepeatSeqPtrT repeats_;
    CutSeqPtrT cuts_;
    TransposeSeqPtrT transpositions_;
    TempoScaleSeqPtrT tempo_scales_;
    MarkerSeqPtrT markers_;
    ClickSeqPtrT clicks_;
    FermataSeqPtrT fermatas_;
    CaesuraSeqPtrT caesuras_;
    cmn::ROCSSeqPtrVecT all_seqs_;
    void make_all_seqs();

private:
    UInt32 showReadyStartTicks_;
    UInt32 showReadyEndTicks_;
    UInt32 fillHarmonicStartTicks_;
    UInt32 fillHarmonicEndTicks_;
    bool hasShowReadyStartTicks_;
    bool hasShowReadyEndTicks_;
    bool hasFillHarmonicStartTicks_;
    bool hasFillHarmonicEndTicks_;
    std::recursive_mutex lock_;

public: 
    UInt32 GetShowReadyStartTicks() { return this->showReadyStartTicks_; }

    UInt32 GetShowReadyEndTicks() { return this->showReadyEndTicks_; }

    UInt32 GetFillHarmonicStartTicks() { return this->fillHarmonicStartTicks_; }
    
    UInt32 GetFillHarmonicEndTicks() { return this->fillHarmonicEndTicks_; }

    bool HasShowReadyStartTicks() { return this->hasShowReadyStartTicks_; }
 
    bool HasShowReadyEndTicks() { return this->hasShowReadyEndTicks_; }
 
    bool HasFillHarmonicStartTicks() { return this->hasFillHarmonicStartTicks_; }
 
    bool HasFillHarmonicEndTicks() { return this->hasFillHarmonicEndTicks_; }
};

ROCS_CORE_API bool operator==(const SongLog& lhs, const SongLog& rhs);

typedef std::shared_ptr<SongLog> SongLogPtrT;
typedef ex::ptr_vector<SongLog> SongLogVecT;
// ROCS_CORE_TEMPLATE_DECLSPEC(EXLIB_PTR_VECTOR_CONTAINER_T(SongLog));
// ROCS_CORE_TEMPLATE_DECLSPEC(ex::ptr_vector<SongLog>);

} // End namespace CL
