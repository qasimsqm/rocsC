#pragma once

#include "core/win32/declspec.h"

#include <istream>
#include <ostream>
#include <stdexcept>
#include <cassert>

#include <memory>

#include "exlib/xplatform_types.h"
#include "core/common/rocs_sequence.h"
#include "core/common/codes.h"
#include "core/changelog/cl_events.h"

namespace CL {

ROCS_SEQUENCE_TEMPLATE(Marker);

class ROCS_CORE_API MarkerSequence
    :
    public cmn::SequenceTemplate<Marker>
{
public:
    MarkerSequence() {}
    MarkerSequence(std::istream &, const cmn::FileVersion &);
    ~MarkerSequence() {}
    void WriteBinary(std::ostream &, const cmn::FileVersion &) const;
};

template< class EventT >
class ChangeLogSequence : public cmn::SequenceTemplate<EventT>
{
public:
    ChangeLogSequence() {}
    ChangeLogSequence(std::istream &is, const cmn::FileVersion &fileVersion)
        :
        cmn::SequenceTemplate<EventT>(is, fileVersion) {}

    // Because the events contained by a ChangeLog sequence have a virtual base
    // class, each must be read individually.  Timeline events have no such
    // restriction, and the entire vector can be serialized in one operation.
    void WriteBinary(std::ostream& os, const cmn::FileVersion &version) const
    {
        os.put(this->code());
        UInt32 obj_count = this->events_.size();
        os.write((char *)&obj_count, sizeof(UInt32));
        for (auto it: this->events_)
        {
            it.WriteBinary(os, version);
        }
    }
};
/*
ROCS_SEQUENCE_TEMPLATE(CustomBar);
ROCS_SEQUENCE_TEMPLATE(Caesura);
ROCS_SEQUENCE_TEMPLATE(Cut);
ROCS_SEQUENCE_TEMPLATE(Vamp);
ROCS_SEQUENCE_TEMPLATE(Repeat);
ROCS_SEQUENCE_TEMPLATE(Transpose);
ROCS_SEQUENCE_TEMPLATE(Click);
ROCS_SEQUENCE_TEMPLATE(TempoScale);
ROCS_SEQUENCE_TEMPLATE(Fermata);
*/
typedef ChangeLogSequence<CustomBar> CustomBarSeqT;
typedef ChangeLogSequence<Caesura> CaesuraSeqT;
typedef MarkerSequence MarkerSeqT;
typedef ChangeLogSequence<Cut> CutSeqT;
typedef ChangeLogSequence<Vamp> VampSeqT;
typedef ChangeLogSequence<Repeat> RepeatSeqT;
typedef ChangeLogSequence<Transpose> TransposeSeqT;
typedef ChangeLogSequence<Click> ClickSeqT;
typedef ChangeLogSequence<TempoScale> TempoScaleSeqT;
typedef ChangeLogSequence<Fermata> FermataSeqT;
/*
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<CustomBar>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<Caesura>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<Cut>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<Vamp>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<Repeat>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<Transpose>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<Click>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<TempoScale>);
ROCS_CORE_TEMPLATE_DECLSPEC(cmn::SequenceTemplate<Fermata>);

ROCS_CORE_TEMPLATE_DECLSPEC(ChangeLogSequence<CustomBar>);
ROCS_CORE_TEMPLATE_DECLSPEC(ChangeLogSequence<Caesura>);
ROCS_CORE_TEMPLATE_DECLSPEC(ChangeLogSequence<Cut>);
ROCS_CORE_TEMPLATE_DECLSPEC(ChangeLogSequence<Vamp>);
ROCS_CORE_TEMPLATE_DECLSPEC(ChangeLogSequence<Repeat>);
ROCS_CORE_TEMPLATE_DECLSPEC(ChangeLogSequence<Transpose>);
ROCS_CORE_TEMPLATE_DECLSPEC(ChangeLogSequence<Click>);
ROCS_CORE_TEMPLATE_DECLSPEC(ChangeLogSequence<TempoScale>);
ROCS_CORE_TEMPLATE_DECLSPEC(ChangeLogSequence<Fermata>);
*/
typedef std::shared_ptr<CaesuraSeqT> CaesuraSeqPtrT;
typedef std::shared_ptr<MarkerSeqT> MarkerSeqPtrT;
typedef std::shared_ptr<CutSeqT> CutSeqPtrT;
typedef std::shared_ptr<VampSeqT> VampSeqPtrT;
typedef std::shared_ptr<RepeatSeqT> RepeatSeqPtrT;
typedef std::shared_ptr<TransposeSeqT> TransposeSeqPtrT;
typedef std::shared_ptr<ClickSeqT> ClickSeqPtrT;
typedef std::shared_ptr<TempoScaleSeqT> TempoScaleSeqPtrT;
typedef std::shared_ptr<FermataSeqT> FermataSeqPtrT;
/*
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<CaesuraSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<MarkerSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<CutSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<VampSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<RepeatSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<TransposeSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<ClickSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<TempoScaleSeqT>);
ROCS_CORE_TEMPLATE_DECLSPEC(std::shared_ptr<FermataSeqT>);
*/
} // End namespace CL
