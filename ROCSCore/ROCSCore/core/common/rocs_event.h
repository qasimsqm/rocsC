#pragma once

/* ROCSEvent

    Base class for all ChangeLog and events, not for ROCSMIDI events.

*/

#include "core/win32/declspec.h"

#include <memory>

#include <ostream>
#include <vector>
#include <string>
#include <list>
#include <iostream> // for debugging only
#include "exlib/xplatform_types.h"

namespace cmn
{

class ROCS_CORE_API ROCSEvent;

typedef std::shared_ptr<ROCSEvent> ROCSEvtPtrT;
typedef std::pair<UInt32, UInt32> BoundsT;
typedef std::vector<cmn::ROCSEvent *> RocsEventSeqT;

/* Base class for Changelog Events.  rocs_midi::VoiceEvent and Timeline events do not
   inherit from this base class because vtables complicate direct serialization. */
class ROCS_CORE_API ROCSEvent
{
public:
    ROCSEvent() {}
    virtual ~ROCSEvent() {}
    virtual UInt32 abs_time() const = 0;
    virtual UInt8 code() const = 0;
    virtual bool operator<(const ROCSEvent &) const;
    
    virtual bool operator<(UInt32 tickIndex) const
    {
        return this->abs_time() < tickIndex;
    }

    virtual bool operator>(UInt32 tickIndex) const
    {
        return this->abs_time() > tickIndex;
    }

    virtual bool operator==(UInt32 tickIndex) const
    {
        return (this->abs_time() == tickIndex);
    }

    virtual bool operator>=(UInt32 tickIndex) const
    {
        return (!(this->operator<(tickIndex)));
    }

    virtual bool operator<=(UInt32 tickIndex) const
    {
        return (!(this->operator>(tickIndex)));
    }

    virtual bool operator==(const ROCSEvent &) const;
    virtual bool operator!=(const ROCSEvent &other) { return !(this->operator==(other)); }
    virtual void WriteString(std::ostream&) const;
    virtual UInt32 start() const { return this->abs_time(); }
    virtual void start(UInt32) {}
    virtual UInt32 end() const { return this->start(); }
    virtual void end(UInt32) {}
    virtual std::string value() const { return ""; }
    virtual void value(const std::string &) {}
    virtual SInt32 int_value() const { return 0; }
    virtual void int_value(SInt32) {}
    virtual float float_value() const { return 0; }
    virtual void float_value(float) {}
    virtual void float_value(double) {}
    virtual UInt32 VersionTarget() const { return 0; }
    virtual bool IsPairedEvent() const { return false; }
    virtual UInt32 GetLengthTicks() const { return 0; }
};

inline
bool operator<(UInt32 operand1, const ROCSEvent &operand2)
{
    return (operand2 >= operand1);
}

} // namespace cmn
