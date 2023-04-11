#pragma once

#include "core/win32/declspec.h"

#include <iostream> // debug only
#include <utility>
#include <string>
#include <ostream>
#include <sstream>

#include <memory>

#include "exlib/xplatform_types.h"
#include "exlib/binary_string_io.h"
#include "exlib/binary_io.h"
#include "core/common/verify_align.h"
#include "core/common/rocs_event.h"
#include "core/common/codes.h"
#include "core/common/file_version.h"

namespace CL
{

/* CustomBar is a temporary event that never gets stored in the change log.  It
 * is used by the timeline to rename bar numbers.  It is provided here for
 * convenience when reading the marker message in 'changelog/read_marker.h' */

class ROCS_CORE_API CustomBar
    :
    public cmn::ROCSEvent
{
public:
    static const UInt8 evt_code;
 
    CustomBar() {}
 
    CustomBar(std::istream &, const cmn::FileVersion &) {}
 
    CustomBar(UInt32 _abs_time): abs_time_(_abs_time) {}
 
    CustomBar(UInt32 abs_time, const std::string& val)
        :
        abs_time_(abs_time),
        value_(val)
    {}

    virtual UInt8 code() const { return evt_code; }
 
    virtual UInt32 abs_time() const { return abs_time_; }
 
    virtual std::string value() const { return value_; }
 
    void WriteBinary(std::ostream &, const cmn::FileVersion &) const {}
 
    virtual void WriteString(std::ostream& os) const;

    virtual bool operator<(const ROCSEvent &other) const
    {
        return abs_time() < other.abs_time();
    }

    virtual bool operator==(const ROCSEvent &other) const;

private:
    UInt32 abs_time_;
	MSC_DISABLE_WARNING(4251);
    std::string value_;
	MSC_RESTORE_WARNING(4251);
};


class ROCS_CORE_API Marker
    :
    public cmn::ROCSEvent
{
public:
    static const UInt8 evt_code;
    
    Marker() {}
    
    Marker(const cmn::ROCSEvent &event)
        :
        abs_time_(event.abs_time()),
        value_(event.value())
    {}

    Marker(UInt32 _abs_time): abs_time_(_abs_time) {}
    
    Marker(UInt32 abs_time, const std::string& value)
        :
        abs_time_(abs_time),
        value_(value)
    {}
    
    Marker(std::istream &, const cmn::FileVersion &);
    
    virtual UInt8 code() const { return evt_code; }
    
    virtual UInt32 abs_time() const { return abs_time_; }
    
    virtual UInt32 start() const { return this->abs_time_; }

    virtual void start(UInt32 val) { abs_time_ = val; }
    
    void abs_time(UInt32 val) { abs_time_ = val; }
 
    virtual std::string value() const { return value_; }
 
    virtual void value(const std::string& val);
 
    virtual void WriteString(std::ostream& os) const;
 
    void WriteBinary(std::ostream& os, const cmn::FileVersion &) const;
 
    virtual bool operator<(const ROCSEvent &other) const;

    virtual bool operator==(const ROCSEvent &) const;

private:
    UInt32 abs_time_;
	MSC_DISABLE_WARNING(4251);
    std::string value_;
	MSC_RESTORE_WARNING(4251);
};

#define CAESURA_TARGETS_ALL 0
#define CAESURA_TARGETS_SHOWREADY 1
#define CAESURA_TARGETS_STAGETRACKS_FILLHARMONIC 2

extern UInt32 caesuraVersionTarget_;
extern UInt32 caesuraIgnoredVersionTarget_;

ROCS_CORE_API
UInt32 GetCaesuraVersionTarget();

ROCS_CORE_API
UInt32 GetCaesuraIgnoredVersionTarget();

ROCS_CORE_API
void SetCaesuraVersionTarget(UInt32 caesuraVersionTarget);


class ROCS_CORE_API Caesura
    : 
    public cmn::ROCSEvent
{
public:
    static const UInt8 evt_code;
     
    Caesura()
        :
        abs_time_(0),
        versionTarget_(CAESURA_TARGETS_ALL)
    {}
    
    Caesura(const cmn::ROCSEvent &event)
        :
        abs_time_(event.abs_time()),
        versionTarget_(event.VersionTarget())
    {}

    Caesura(std::istream &, const cmn::FileVersion &);
 
    Caesura(UInt32 abs_time, UInt32 versionTarget = CAESURA_TARGETS_ALL)
        :
        abs_time_(abs_time),
        versionTarget_(versionTarget)
    {}

    virtual UInt8 code() const { return evt_code; }

    virtual UInt32 abs_time() const { return abs_time_; }
 
    virtual UInt32 start() const { return this->abs_time_; }
    
    virtual void start(UInt32 val) { abs_time_ = val; }
    
    void abs_time(UInt32 val) { abs_time_ = val; }

    void WriteBinary(std::ostream &, const cmn::FileVersion &) const;
 
    virtual void WriteString(std::ostream& os) const;

    virtual bool operator<(const ROCSEvent &other) const
    {
        return abs_time() < other.abs_time();
    }
    
    virtual UInt32 VersionTarget() const { return this->versionTarget_; }

private:
    UInt32 abs_time_;
    UInt32 versionTarget_;
};

#define INOUT_TARGETS_ALL_MIDI 0
#define INOUT_TARGETS_SHOWREADY 1
#define INOUT_TARGETS_FILLHARMONIC 2

class ROCS_CORE_API PackagerInOut
    :
    public cmn::ROCSEvent
{
public:
    PackagerInOut(UInt32 absTime, UInt32 versionTarget)
        :
        absTime_(absTime),
        versionTarget_(versionTarget)
    {}

    virtual UInt32 abs_time() const { return this->absTime_; }
 
    void abs_time(UInt32 val) { this->absTime_ = val; }
    
    UInt32 VersionTarget() const { return this->versionTarget_; }

protected:
    UInt32 absTime_;
    UInt32 versionTarget_;
};

class ROCS_CORE_API InPoint
    :
    public PackagerInOut 
{
public:
    static const UInt8 evtCode;
    
    InPoint(UInt32 absTime, UInt32 versionTarget = INOUT_TARGETS_ALL_MIDI)
        :
        PackagerInOut(absTime, versionTarget)
    {}

    virtual UInt8 code() const { return evtCode; }
};

class ROCS_CORE_API OutPoint
    :
    public PackagerInOut 
{
public:
    static const UInt8 evtCode;
    
    OutPoint(UInt32 absTime, UInt32 versionTarget = INOUT_TARGETS_ALL_MIDI)
        :
        PackagerInOut(absTime, versionTarget)
    {}

    virtual UInt8 code() const { return evtCode; }
};


class ROCS_CORE_API PairedEvent
    :
    public cmn::ROCSEvent
{
public:
    PairedEvent()
        :
        start_(0),
        end_(0),
        int_value_(0)
    {}
    
    PairedEvent(std::istream &, const cmn::FileVersion &);
    
    PairedEvent(UInt32 _start, UInt32 _end, SInt32 _value)
        :
        start_(_start),
        end_(_end),
        int_value_(_value)
    {}
    
    PairedEvent(UInt32 _start, UInt32 _end, double _value)
        :
        start_(_start),
        end_(_end),
        float_value_(float(_value))
    {}
    
    virtual ~PairedEvent() {}
    
    virtual UInt32 abs_time() const { return start_; }
    
    cmn::BoundsT bounds() const { return std::make_pair(start_, end_); }
    
    virtual UInt32 start() const { return start_; }
    
    virtual void start(UInt32 val) { start_ = val; }
    
    virtual UInt32 end() const { return end_; }
    
    virtual void end(UInt32 val) { end_ = val; }
    
    virtual std::string value() const;

    virtual SInt32 int_value() const { return int_value_; }

    virtual float float_value() const { return float_value_; } 

    virtual void int_value(SInt32 val) { int_value_ = val; }

    virtual void float_value(float val) { float_value_ = val; }

    virtual void float_value(double val) { float_value_ = float(val); }
    
    void WriteBinary(std::ostream &, const cmn::FileVersion &) const;

    virtual void WriteString(std::ostream& os) const;
    
    // For completely specified paired events, both is_start() and is_end()
    // will return false.  These functions are useful when building
    // PairedEvents from a midi file.
    bool is_start() const { return end() == 0xFFFFFFFF; }

    bool is_end() const { return start() == 0xFFFFFFFF; }

    void reset() { start(0); end(0); }

    /** bounded(UInt32 t) returns true if t falls in the interval [start(), end()) **/
    bool bounded(UInt32 t) const { return (start_ <= t) && (end_ > t); }

    virtual bool operator<(const ROCSEvent &) const;

    virtual bool operator==(const ROCSEvent &) const;
    
    virtual bool IsPairedEvent() const { return true; } 

    virtual UInt32 GetLengthTicks() const
    {
        if (this->end() <= this->start())
        {
            return 0;
        } else
        {
            return this->end() - this->start();
        }
    }

private:
    UInt32 start_;
    UInt32 end_;
    union
    {
        SInt32 int_value_;
        float float_value_;
    };
};


class ROCS_CORE_API Cut
    :
    public PairedEvent
{
public:
    static const UInt8 evt_code;

    Cut(): PairedEvent() {}

    Cut(const cmn::ROCSEvent &event)
        :
        PairedEvent(event.start(), event.end(), event.int_value())
    {}
    
    Cut(std::istream &is, const cmn::FileVersion &fileVersion)
        :
        PairedEvent(is, fileVersion)
    {}
    
    Cut(UInt32 start, UInt32 end, SInt32 value=0)
        :
        PairedEvent(start, end, value)
    {}
    
    virtual UInt8 code() const { return evt_code; }
};


class ROCS_CORE_API Vamp
    :
    public PairedEvent
{
public:
    static const UInt8 evt_code;

    Vamp(): PairedEvent() {}

    Vamp(const cmn::ROCSEvent &event)
        :
        PairedEvent(event.start(), event.end(), event.int_value())
    {}
    
    Vamp(std::istream &is, const cmn::FileVersion &fileVersion)
        :
        PairedEvent(is, fileVersion)
    {}

    Vamp(UInt32 start, UInt32 end, SInt32 value=0)
        :
        PairedEvent(start, end, value)
    {}

    virtual UInt8 code() const { return evt_code; }
};


class ROCS_CORE_API Repeat
    :
    public PairedEvent
{
public:
    static const UInt8 evt_code;
    
    Repeat(): PairedEvent() {}
    
    Repeat(const cmn::ROCSEvent &event)
        :
        PairedEvent(event.start(), event.end(), event.int_value())
    {}
    
    Repeat(std::istream &is, const cmn::FileVersion &fileVersion)
        :
        PairedEvent(is, fileVersion)
    {}
    
    Repeat(UInt32 start, UInt32 end, SInt32 value=2)
        :
        PairedEvent(start, end, value)
    {}
    
    virtual UInt8 code() const { return evt_code; }
};


class ROCS_CORE_API Transpose
    : 
    public PairedEvent
{
public:
    static const UInt8 evt_code;
    Transpose()
        :
        PairedEvent()
    {}
   
    Transpose(const cmn::ROCSEvent &event)
        :
        PairedEvent(event.start(), event.end(), event.int_value())
    {}
    
    Transpose(std::istream &is, const cmn::FileVersion &fileVersion)
        : PairedEvent(is, fileVersion)
    {}
    
    Transpose(UInt32 start, UInt32 end, SInt32 value=0)
        :
        PairedEvent(start, end, value) {}
    
    virtual UInt8 code() const { return evt_code; }
    virtual std::string value() const;
    // virtual void int_value(int val) { PairedEvent::value(val); }
};


class ROCS_CORE_API TempoScale
    :
    public PairedEvent
{
public:
    static const UInt8 evt_code;
    TempoScale(): PairedEvent() {}
    
    TempoScale(const cmn::ROCSEvent &event)
        :
        PairedEvent(event.start(), event.end(), event.int_value())
    {}
    
    TempoScale(std::istream &is, const cmn::FileVersion &fileVersion)
        :
        PairedEvent(is, fileVersion)
    {}
    
    TempoScale(UInt32 start, UInt32 end, double value=1.0)
        :
        PairedEvent(start, end, value)
    {}
    
    virtual std::string value() const;
    
    // virtual void value(float val) { PairedEvent::value(val); }
    
    virtual void WriteString(std::ostream& os) const;
    
    virtual UInt8 code() const { return evt_code; }
    
    virtual bool operator<(const ROCSEvent &other) const;
};

/** Click is interpreted as notated-32nd-notes-per-click
    use TL::Meter to convert to clocks per click
    TL::Meter::notated_32_per_quarter

     n32     qn      24 clocks     clocks
    ----- x ----- x ----------- = ---------
    click    n32        qn          click

    Click::int_value() / TL::Meter::notated_32_per_click() * 24.0

    ----
    There appears to be a mistake here.
    
    Click::int_value is notated32ndPerClick, and the math is right, but the
    method name I have used is not the right one.
    
    Should have been notated_32_per_quarter()

    clocksPerClick = notated32ndPerClick / notated32ndPerQuarter * clocksPerQuarter
    
    I am adding a method to Click to handle this conversion.

    --- Jonathan, 13 Feb 2014

**/
class ROCS_CORE_API Click
    :
    public PairedEvent
{
public:
    static const UInt8 evt_code;
    
    Click(): PairedEvent() {}
    
    Click(const cmn::ROCSEvent &event)
        :
        PairedEvent(event.start(), event.end(), event.int_value())
    {}
    
    Click(std::istream& is, const cmn::FileVersion &fileVersion)
        : 
        PairedEvent(is, fileVersion)
    {}
    
    Click(UInt32 start, UInt32 end, SInt32 value=8)
        :
        PairedEvent(start, end, value)
    {}
    
    virtual UInt8 code() const { return evt_code; }

    UInt32 ClocksPerClick(UInt8 notated32ndPerQuarter)
    {
        // I changed the order of operation to avoid rounding errors.
        UInt32 clocksPerQuarter = 24;
        UInt32 notated32ndPerClick = this->int_value();
        UInt32 notated32ndClocksPerClickQuarter = notated32ndPerClick * clocksPerQuarter;
        UInt32 clocksPerClick = notated32ndClocksPerClickQuarter / notated32ndPerQuarter;
        return clocksPerClick; 
    }
};

class ROCS_CORE_API Fermata
    :
    public PairedEvent
{
public:
    static const UInt8 evt_code;

    Fermata(): PairedEvent() {}
    
    Fermata(const cmn::ROCSEvent &event)
        :
        PairedEvent(event.start(), event.end(), event.int_value())
    {}

    Fermata(std::istream &is, const cmn::FileVersion &fileVersion)
        :
        PairedEvent(is, fileVersion)
    {}

    Fermata(UInt32 start, UInt32 end, SInt32 value=0)
        :
        PairedEvent(start, end, value)
    {}

    virtual UInt8 code() const  { return evt_code; } 
};

typedef std::shared_ptr<CustomBar> CustomBarPtrT;
typedef std::shared_ptr<Marker> MarkerPtrT;
typedef std::shared_ptr<Caesura> CaesuraPtrT;
typedef std::shared_ptr<Vamp> VampPtrT;
typedef std::shared_ptr<Repeat> RepeatPtrT;
typedef std::shared_ptr<Cut> CutPtrT;
typedef std::shared_ptr<Transpose> TransposePtrT;
typedef std::shared_ptr<TempoScale> TempoScalePtrT;
typedef std::shared_ptr<Click> ClickPtrT;
typedef std::shared_ptr<Fermata> FermataPtrT;
/*
ROCS_CORE_API bool operator==(const CustomBar& lhs, const CustomBar& rhs);
ROCS_CORE_API bool operator==(const Marker& lhs, const Marker& rhs);
ROCS_CORE_API bool operator==(const Caesura& lhs, const Caesura& rhs);
ROCS_CORE_API bool operator==(const PairedEvent& lhs, const PairedEvent& rhs);
ROCS_CORE_API bool operator!=(const CustomBar& lhs, const CustomBar& rhs);
ROCS_CORE_API bool operator!=(const Marker& lhs, const Marker& rhs);
ROCS_CORE_API bool operator!=(const Caesura& lhs, const Caesura& rhs);
ROCS_CORE_API bool operator!=(const PairedEvent& lhs, const PairedEvent& rhs);
*/
} // End namespace CL
