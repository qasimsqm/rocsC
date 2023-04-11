#pragma once

#include "core/win32/declspec.h"

#include "exlib/cxx11.h"
#ifdef CXX11
    #include <memory>
#else
    #include <tr1/memory>
#endif

#include <istream>
#include <ostream>
#include <cassert>
#include <stdexcept>
#include "core/common/rocs_sequence.h"
#include "core/timeline/tl_events.h"

namespace TL {

template< class EventT >
class TimelineSequence : public cmn::SequenceTemplate< EventT >
{
public:
    TimelineSequence() {}

    TimelineSequence(std::istream &is, const cmn::FileVersion &)
    {
        UInt8 event_type = is.get();
        if (event_type != this->code())
        {
            throw std::logic_error("event_type does not match container type.");
        }
        
        UInt32 obj_count;
        is.read((char *)&obj_count, sizeof(UInt32));
        if (!obj_count) return;

        this->events_.resize(obj_count);
        is.read((char *)&(this->events_[0]), sizeof(EventT) * obj_count);
    }

    void WriteBinary(std::ostream& os, const cmn::FileVersion &) const
    {
        os.put(this->code());
        UInt32 obj_count = this->events_.size();
        os.write((char *)&obj_count, sizeof(UInt32));
        os.write((char *)&(this->events_[0]), sizeof(EventT) * obj_count);
    }
};


typedef TimelineSequence<Tempo> TempoSeqT;
typedef TimelineSequence<BarsBeats> BarsBeatsSeqT;
typedef TimelineSequence<Key> KeySeqT;
typedef TimelineSequence<PageNum> PageNumSeqT;
typedef TimelineSequence<Meter> MeterSeqT;

typedef std::vector<Tempo>      stdTempoSeqT;
typedef std::vector<BarsBeats>  stdBarsBeatsSeqT;
typedef std::vector<Key>        stdKeySeqT;
typedef std::vector<PageNum>    stdPageNumSeqT;
typedef std::vector<Meter>      stdMeterSeqT;

typedef std::shared_ptr<TempoSeqT> TempoSeqPtrT;
typedef std::shared_ptr<BarsBeatsSeqT> BarsBeatsSeqPtrT;
typedef std::shared_ptr<KeySeqT> KeySeqPtrT;
typedef std::shared_ptr<PageNumSeqT> PageNumSeqPtrT;
typedef std::shared_ptr<MeterSeqT> MeterSeqPtrT;

/* Still a work in progress. rocs_sequence.h:156 'no overloaded function takes two arguments'
ROCS_SEQUENCE_TEMPLATE(Tempo);
ROCS_SEQUENCE_TEMPLATE(BarsBeats);
ROCS_SEQUENCE_TEMPLATE(Key);
ROCS_SEQUENCE_TEMPLATE(PageNum);
ROCS_SEQUENCE_TEMPLATE(Meter);

ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<Tempo>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<BarsBeats>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<Key>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<PageNum>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<Meter>);

ROCS_CORE_TEMPLATE_DECLSPEC(TimelineSequence<Tempo>);
ROCS_CORE_TEMPLATE_DECLSPEC(TimelineSequence<BarsBeats>);
ROCS_CORE_TEMPLATE_DECLSPEC(TimelineSequence<Key>);
ROCS_CORE_TEMPLATE_DECLSPEC(TimelineSequence<PageNum>);
ROCS_CORE_TEMPLATE_DECLSPEC(TimelineSequence<Meter>);

ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<Tempo>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<BarsBeats>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<Key>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<PageNum>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::vector<Meter>);

ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<TempoSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<BarsBeatsSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<KeySeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<PageNumSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<MeterSeqT>);
*/
}// namespace TL
