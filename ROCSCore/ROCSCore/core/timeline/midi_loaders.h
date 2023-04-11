#pragma once

#include "core/win32/declspec.h"

#include <map>
#include <string>
#include <cmath> // fmod

#include "exlib/format.h"
#include "exlib/string_lib.h"
#include "core/std_midi/midi_file.h"
#include "core/std_midi/status_bytes.h"
#include "core/timeline/timeline_exception.h"
#include "core/timeline/tl_sequences.h"
#include "core/timeline/tl_events.h"
#include "core/changelog/read_marker.h"
#include "core/changelog/cl_sequences.h"
#include "core/changelog/cl_events.h"

namespace TL {

class ROCS_CORE_API PageNumberError : public TimelineException
{
public:
    inline PageNumberError(const std::string& what): TimelineException(what) {}
};

class ROCS_CORE_API CustomBarError : public TimelineException
{
public:
    inline CustomBarError(const std::string& what): TimelineException(what) {}
};

ROCS_CORE_API void read_pdf_track(
    const std_midi::MIDIFile&,
    PageNumSeqPtrT seqPtr,
    UInt32 &audioStartTicks,
    UInt32 &audioEndTicks);

ROCS_CORE_API void create_bars_beats(
    const std_midi::MIDIFile& mf,
    MeterSeqPtrT meterSeqPtr,
    BarsBeatsSeqPtrT barsBeatsSeqPtr);




} // namespace TL
