#pragma once

#include "core/win32/declspec.h"

#include <string>
#include <cstring>
#include <sstream>
#include <istream>
#include <ostream>
#include <iterator>
#include <cmath> // round, abs
#include <cstring> // memcpy
#include "exlib/xplatform_types.h"
#include "exlib/string_lib.h"
#include "exlib/binary_io.h"
#include "core/common/verify_align.h"
#include "core/common/key_sigs.h"
#include "core/common/codes.h"
#include "core/timeline/timeline_exception.h"

namespace TL {

class ROCS_CORE_API Tempo
{
public:
    static const UInt8 evt_code;
    Tempo() {}
    Tempo(UInt32 _abs_time): abs_time_(_abs_time) {}
    Tempo(UInt32 _abs_time, UInt32 _microseconds) :
            abs_time_(_abs_time), microseconds_(_microseconds) {}
    
    void WriteString(std::ostream&) const;
    
    UInt8 code() const         { return evt_code; }
    
    UInt32 abs_time() const    { return abs_time_; }
    void abs_time(UInt32 val)  { abs_time_ = val; }
    
    /** microseconds is microseconds per midi quarter note (always 24 clocks) **/
    UInt32 microseconds() const    { return microseconds_; }
    void microseconds(UInt32 val)  { microseconds_ = val; }
    
    UInt32 nanos_per_tick(UInt16 ppqn) const;
    UInt32 micros_per_clock(UInt16 ppqn) const;
    double seconds_per_clock() const;
    double tick_time(UInt16 ppqn) const;
    double bpm(UInt8 clocks_per_click) const;
    bool operator<(const Tempo& other) const
    {
        std::cout << "Tempo::operator<(Tempo)" << std::endl;
        return abs_time() < other.abs_time();
    }    
    
private:
    UInt32 abs_time_;
    UInt32 microseconds_;
};


class ROCS_CORE_API BarNameLengthError : public TimelineException
{
public:
    BarNameLengthError(const std::string& what): TimelineException(what) {}
};

class ROCS_CORE_API BarsBeats
{
public:
    static const UInt8 evt_code;
    BarsBeats() {}
    BarsBeats(UInt32 _abs_time): abs_time_(_abs_time) {}
    BarsBeats(UInt32 _abs_time, const std::string& _bar_number, UInt32 _beat_number):
            abs_time_(_abs_time), beat_number_(_beat_number)
    {
        bar_number(_bar_number);
    }
    BarsBeats(UInt32 _abs_time, SInt32 _bar_number, UInt32 _beat_number):
            abs_time_(_abs_time), beat_number_(_beat_number)
    {
        bar_number(_bar_number);
    }

    void WriteString(std::ostream &os) const;

    UInt8 code() const                  { return evt_code; }

    UInt32 abs_time() const             { return abs_time_; }
    void abs_time(UInt32 val)           { abs_time_ = val; }

    std::string bar_number() const      { return std::string(bar_number_); }
    bool bar_number_is_int() const      { return ex::str_is_int(bar_number()); }
    SInt32 bar_number_int() const       { return ex::str_to_num<SInt32>(bar_number()); }
    void bar_number(const std::string& val);
    void bar_number(const SInt32&);

    UInt32 beat_number() const          { return beat_number_; }
    void beat_number(UInt32 val)        { beat_number_ = val; }

    bool operator<(const BarsBeats& other) const { return abs_time() < other.abs_time(); }

private:
    UInt32 abs_time_;
    char bar_number_[8];
    UInt32 beat_number_;
};


class ROCS_CORE_API Key
{
public:
    static const UInt8 evt_code;
    Key() {}
    Key(UInt32 _abs_time): abs_time_(_abs_time) {}
    Key(UInt32 _abs_time, SInt16 _sharps, SInt16 _mode):
            abs_time_(_abs_time), sharps_(_sharps), mode_(_mode) {}

    void WriteString(std::ostream &os) const;

    UInt8 code() const                        { return evt_code; }

    UInt32 abs_time() const                   { return abs_time_; }
    void abs_time(const UInt32& _abs_time)    { abs_time_ = _abs_time; }

    SInt16 sharps() const			   { return sharps_; }
    void sharps(const SInt16& _sharps)        { sharps_ = _sharps; }

    SInt16 mode() const                       { return mode_; }
    void mode(const SInt16& _mode)            { mode_ = _mode; }
    
    std::string name() const                  { return Keys::keyNameFromSig(mode(), sharps()); }
    std::string name(SInt16 transposition) const;

    bool operator<(const Key& other) const    { return abs_time() < other.abs_time(); }

private:
    UInt32 abs_time_;
    SInt16 sharps_;
    SInt16 mode_;

};

class ROCS_CORE_API PageNum
{
public:
    static const UInt8 evt_code;
    PageNum(): abs_time_(0), page_number_(0) {}
    PageNum(UInt32 _abs_time): abs_time_(_abs_time), page_number_(0) {}
    PageNum(UInt32 _abs_time, UInt32 _page_number):
            abs_time_(_abs_time), page_number_(_page_number) {}

    void WriteString(std::ostream &os) const;

    UInt8 code() const                              { return evt_code; }

    UInt32 abs_time() const                         { return abs_time_; }
    void abs_time(const UInt32& _abs_time)          { abs_time_ = _abs_time; }

    UInt32 page_number() const                      { return page_number_; }
    void page_number(const UInt32& _page_number)    { page_number_ = _page_number; }

    bool operator<(const PageNum& other) const { return abs_time() < other.abs_time(); }

private:
    UInt32 abs_time_;
    UInt32 page_number_;

};

class ROCS_CORE_API Meter
{
public:
    static const UInt8 evt_code;

    Meter()
        :
        abs_time_(0),
        numerator_(0),
        denominator_(0),
        clocks_per_click_(0),
        notated_32_per_quarter_(0)
    {}

    Meter(UInt32 _abs_time): abs_time_(_abs_time) {}

    Meter(
        UInt32 _abs_time,
        UInt8 _numerator,
        UInt8 _denominator,
        UInt8 _clocks_per_click,
        UInt8 _notated_32)
        :
        abs_time_(_abs_time),
        numerator_(_numerator),
        denominator_(_denominator),
        clocks_per_click_(_clocks_per_click),
        notated_32_per_quarter_(_notated_32)
    {}
    
    Meter(UInt32 _abs_time, const Byte* byte_ptr)
        :
        abs_time_(_abs_time)
    {
        memcpy(&numerator_, byte_ptr, 4);
        denominator_ = 1 << denominator_; // denominator_ = 2^denominator_
    }

    void WriteString(std::ostream &os) const;

    UInt8 code() const                              { return evt_code; }

    UInt32 abs_time() const                         { return abs_time_; }
    void abs_time(const UInt32& _abs_time)          { abs_time_ = _abs_time; }

    UInt8 numerator() const                         { return numerator_; }
    void numerator(const UInt8& _numerator)         { numerator_ = _numerator; }

    UInt8 denominator() const                       { return denominator_; }
    void denominator(const UInt8& _denominator)     { denominator_ = _denominator; }

    UInt8 clocks_per_click() const                  { return clocks_per_click_; }
    void clocks_per_click(const UInt8& val)         { clocks_per_click_ = val; }

    UInt8 notated_32_per_quarter() const            { return notated_32_per_quarter_; }
    void notated_32_per_quarter(const UInt8& val)   { notated_32_per_quarter_ = val; }

    double beats_per_measure() const; // May be fractional for irregular meters like 5/8
    UInt32 ticks_per_beat(UInt16 ppqn) const;
    UInt8 notated_32_per_click() const;
    std::vector<int> get_click_values() const;

    bool operator<(const Meter& other) const { return abs_time() < other.abs_time(); }

private:
    UInt32 abs_time_;
    UInt8 numerator_;
    UInt8 denominator_;
    UInt8 clocks_per_click_;
    UInt8 notated_32_per_quarter_;
};

ROCS_CORE_API bool operator==(const Tempo &lhs, const Tempo &rhs);
ROCS_CORE_API bool operator==(const BarsBeats &lhs, const BarsBeats &rhs);
ROCS_CORE_API bool operator==(const Key &lhs, const Key &rhs);
ROCS_CORE_API bool operator==(const PageNum &lhs, const PageNum &rhs);
ROCS_CORE_API bool operator==(const Meter &lhs, const Meter &rhs);
ROCS_CORE_API bool operator!=(const Tempo &lhs, const Tempo &rhs);
ROCS_CORE_API bool operator!=(const BarsBeats &lhs, const BarsBeats &rhs);
ROCS_CORE_API bool operator!=(const Key &lhs, const Key &rhs);
ROCS_CORE_API bool operator!=(const PageNum &lhs, const PageNum &rhs);
ROCS_CORE_API bool operator!=(const Meter &lhs, const Meter &rhs);

ROCS_CORE_API inline bool operator==(const Tempo &lhs, UInt32 tickIndex)
{
    return lhs.abs_time() == tickIndex;
}

ROCS_CORE_API inline bool operator==(const BarsBeats &lhs, UInt32 tickIndex)
{
    return lhs.abs_time() == tickIndex;
}

ROCS_CORE_API inline bool operator==(const Key &lhs, UInt32 tickIndex)
{
    return lhs.abs_time() == tickIndex;
}

ROCS_CORE_API inline bool operator==(const PageNum &lhs, UInt32 tickIndex)
{
    return lhs.abs_time() == tickIndex;
}

ROCS_CORE_API inline bool operator==(const Meter &lhs, UInt32 tickIndex)
{
    return lhs.abs_time() == tickIndex;
}

ROCS_CORE_API inline bool operator!=(const Tempo &lhs, UInt32 tickIndex)
{
    return lhs.abs_time() != tickIndex;
}

ROCS_CORE_API inline bool operator!=(const BarsBeats &lhs, UInt32 tickIndex)
{
    return lhs.abs_time() != tickIndex;
}

ROCS_CORE_API inline bool operator!=(const Key &lhs, UInt32 tickIndex)
{
    return lhs.abs_time() != tickIndex;
}

ROCS_CORE_API inline bool operator!=(const PageNum &lhs, UInt32 tickIndex)
{
    return lhs.abs_time() != tickIndex;
}

ROCS_CORE_API inline bool operator!=(const Meter &lhs, UInt32 tickIndex)
{
    return lhs.abs_time() != tickIndex;
}

ROCS_CORE_API inline bool operator<(const Tempo &lhs, UInt32 tickIndex)
{
    return (lhs.abs_time() < tickIndex);
}

ROCS_CORE_API inline bool operator<(const BarsBeats &lhs, UInt32 tickIndex)
{
    return (lhs.abs_time() < tickIndex);
}

ROCS_CORE_API inline bool operator<(const Key &lhs, UInt32 tickIndex)
{
    return (lhs.abs_time() < tickIndex);
}

ROCS_CORE_API inline bool operator<(const PageNum &lhs, UInt32 tickIndex)
{
    return (lhs.abs_time() < tickIndex);
}

ROCS_CORE_API inline bool operator<(const Meter &lhs, UInt32 tickIndex)
{
    return (lhs.abs_time() < tickIndex);
}

ROCS_CORE_API inline bool operator>(const Tempo &lhs, UInt32 tickIndex)
{
    return (lhs.abs_time() > tickIndex);
}

ROCS_CORE_API inline bool operator>(const BarsBeats &lhs, UInt32 tickIndex)
{
    return (lhs.abs_time() > tickIndex);
}

ROCS_CORE_API inline bool operator>(const Key &lhs, UInt32 tickIndex)
{
    return (lhs.abs_time() > tickIndex);
}

ROCS_CORE_API inline bool operator>(const PageNum &lhs, UInt32 tickIndex)
{
    return (lhs.abs_time() > tickIndex);
}

ROCS_CORE_API inline bool operator>(const Meter &lhs, UInt32 tickIndex)
{
    return (lhs.abs_time() > tickIndex);
}

ROCS_CORE_API inline bool operator<=(const Tempo &lhs, UInt32 tickIndex)
{
    return (!(lhs > tickIndex));
}

ROCS_CORE_API inline bool operator<=(const BarsBeats &lhs, UInt32 tickIndex)
{
    return (!(lhs > tickIndex));
}

ROCS_CORE_API inline bool operator<=(const Key &lhs, UInt32 tickIndex)
{
    return (!(lhs > tickIndex));
}

ROCS_CORE_API inline bool operator<=(const PageNum &lhs, UInt32 tickIndex)
{
    return (!(lhs > tickIndex));
}

ROCS_CORE_API inline bool operator<=(const Meter &lhs, UInt32 tickIndex)
{
    return (!(lhs > tickIndex));
}

ROCS_CORE_API inline bool operator>=(const Tempo &lhs, UInt32 tickIndex)
{
    return (!(lhs < tickIndex));
}

ROCS_CORE_API inline bool operator>=(const BarsBeats &lhs, UInt32 tickIndex)
{
    return (!(lhs < tickIndex));
}

ROCS_CORE_API inline bool operator>=(const Key &lhs, UInt32 tickIndex)
{
    return (!(lhs < tickIndex));
}

ROCS_CORE_API inline bool operator>=(const PageNum &lhs, UInt32 tickIndex)
{
    return (!(lhs < tickIndex));
}

ROCS_CORE_API inline bool operator>=(const Meter &lhs, UInt32 tickIndex)
{
    return (!(lhs < tickIndex));
}

ROCS_CORE_API inline bool operator<(UInt32 tickIndex, const Tempo &other)
{
    return (other > tickIndex);
}

ROCS_CORE_API inline bool operator<(UInt32 tickIndex, const BarsBeats &other)
{
    return (other > tickIndex);
}

ROCS_CORE_API inline bool operator<(UInt32 tickIndex, const Key &other)
{
    return (other > tickIndex);
}

ROCS_CORE_API inline bool operator<(UInt32 tickIndex, const PageNum &other)
{
    return (other > tickIndex);
}

ROCS_CORE_API inline bool operator<(UInt32 tickIndex, const Meter &other)
{
    return (other > tickIndex);
}

} // end namespace TL
