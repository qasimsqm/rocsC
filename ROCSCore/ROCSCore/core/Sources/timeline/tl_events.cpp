#include "core/timeline/tl_events.h"

using namespace std;

namespace TL {

const UInt8 Tempo::evt_code = codes::tempo;
const UInt8 BarsBeats::evt_code = codes::bars_beats;
const UInt8 Key::evt_code = codes::key;
const UInt8 PageNum::evt_code = codes::page_num;
const UInt8 Meter::evt_code = codes::meter;

void Tempo::WriteString(ostream &os) const
{
    os  << "Tempo("
        << "abs_time=" << abs_time() << ", "
        << "microseconds=" << microseconds()
        << ")";
}


UInt32 Tempo::nanos_per_tick(UInt16 ppqn) const
{
    double n =
        round(static_cast<double>(microseconds()) / static_cast<double>(ppqn) * 1000.0);
    return static_cast<UInt32>(n);
}

UInt32 Tempo::micros_per_clock(UInt16 ppqn) const
{
    double m =
        round(24.0 * static_cast<double>(microseconds()) / static_cast<double>(ppqn));
    return static_cast<UInt32>(m);
}

double Tempo::seconds_per_clock() const
{
/**
        secs         quarter      microseconds() us        sec
      ---------  = ----------- * ------------------- * -------------
        clock       24 clocks          quarter           1.0e6 us
**/
    
    return static_cast<double>(microseconds()) / 24.0 / 1.0e6;
}

double Tempo::bpm(UInt8 clocks_per_click) const
{
    /**
    1e6 * 60 * 24 == 1.440e9 -->

    1e6 us   60 sec   24 clock    1.440e9 us * clock
    ------ * ------ * -------- = --------------------
     sec       min     quarter       quarter * min


     clock       us          clock * us
    ------- * --------- = -----------------
     click     quarter     click * quarter


     1.440e9 us * clock    /     clock * us         1.440e9 us * clock     click * quarter     click
    --------------------  /   ----------------- =  -------------------- * ----------------- = -------
       quarter * min     /     click * quarter        quarter * min           clock * us        min
    
    
    ** easier **
    
     clicks    60 secs     clocks     click
    ------- = --------- * -------- * --------
      min        min        secs      clocks
    
        60 secs      24 clocks           quarter           1.0e6 us       click
    = ----------- * ----------- * ------------------- * ------------- * --------
          min          quarter      microseconds() us       sec          clocks
    
    
    
    **/
    
    double result = 1.440e9 / (   static_cast<double>(clocks_per_click) 
                              * static_cast<double>(microseconds_)      );
                              
    double result2 = 60.0 / seconds_per_clock() / static_cast<double>(clocks_per_click);
    
    if (abs(result - result2) > 0.00001) {
        throw logic_error("your math failed again!");
    }
    
    return result;
}

double Tempo::tick_time(UInt16 ppqn) const
{
    return static_cast<double>(microseconds()) / static_cast<double>(ppqn) / 1.0e6;
}


void BarsBeats::WriteString(ostream &os) const
{
    os  << "BarsBeats("
        << "abs_time=" << abs_time() << ", "
        << "bar_number=" << bar_number() << ", "
        << "beat_number=" << beat_number() << ", "
        << ")";
}

void BarsBeats::bar_number(const std::string& val)
{
    if (val.size() > 7)
    {
        throw BarNameLengthError("bar number " + val + " is longer than 7 characters.");
    } 
    
    ex::strcpy_safe(bar_number_, val.c_str());
}


void BarsBeats::bar_number(const SInt32& val)
{
    stringstream ss;
    ss << val;
    if (ss.str().size() > 7)
    {
        throw BarNameLengthError(
            "bar number = " + ss.str() + " is longer than 7 characters.");
    }
    
    ex::strcpy_safe(bar_number_, ss.str().c_str());
}


void Key::WriteString(ostream &os) const
{
    os  << "Key("
        << "abs_time=" << abs_time() << ", "
        << "sharps=" << sharps() << ", "
        << "mode=" << mode() << ", "
        << "name=" << name()
        << ")";
}


std::string Key::name(SInt16 transposition) const
{
    return Keys::getTransposedName(
        this->mode(),
        this->sharps(),
        transposition);
}


void PageNum::WriteString(ostream &os) const
{
    os  << "PageNum("
        << "abs_time=" << abs_time() << ", "
        << "page_number=" << page_number() << ", "
        << ")";
}

void Meter::WriteString(ostream &os) const
{
    os  << "Meter("
        << "abs_time=" << abs_time() << ", "
        << "numerator=" << UInt16(numerator()) << ", "
        << "denominator=" << UInt16(denominator()) << ", "
        << "clocks_per_click=" << UInt16(clocks_per_click()) << ", "
        << "notated_32_per_quarter=" << UInt16(notated_32_per_quarter())
        << ")";
}


double Meter::beats_per_measure() const
{
    /**
    24.0 clocks   notated_32    click       quarter        clicks (beats)
    ----------- x ---------- x --------- x ------------ = ----------------
      quarter      measure      clocks      notated_32          measure
    
    **/
    int notated_32_per_numerator = 32 / denominator_;
    return  24.0
            * numerator_
            * notated_32_per_numerator
            / clocks_per_click_
            / notated_32_per_quarter_;
}

std::vector<int> create_click_values()
{
    std::vector<int> click_values = {1, 2, 3, 4, 6, 7, 8, 12, 14, 15, 16, 24, 28, 30, 31, 32};
    return click_values;
}

ROCS_CORE_API std::vector<int> click_values = create_click_values();

std::vector<int> Meter::get_click_values() const
{
    int n32pden = 32 / denominator_;
    int n32pM = n32pden * numerator_;
    std::list<int> values;
    for (int i = notated_32_per_click(); i > 0; i--)
    {
        if (n32pM % i == 0) values.push_front(i);
    }
    std::vector<int> result;
    set_intersection(values.begin(), values.end(), click_values.begin(), click_values.end(),
                    back_inserter(result)    );
    return result;
}

UInt32 Meter::ticks_per_beat(UInt16 ppqn) const
{
    return static_cast<UInt32>(round(static_cast<double>(clocks_per_click_) * ppqn / 24.));
}

UInt8 Meter::notated_32_per_click() const
{
    double clocksPerQuarterNote = 24.0;
    double clocksPerClick = this->clocks_per_click();
    double quarterNotesPerClick = clocksPerClick / clocksPerQuarterNote;
    double notated32PerClick = quarterNotesPerClick * this->notated_32_per_quarter();
    return static_cast<UInt32>(round(notated32PerClick));
}


ROCS_CORE_API bool operator==(const Tempo& lhs, const Tempo& rhs)
{
    if (lhs.abs_time() != rhs.abs_time()) return false;
    return lhs.microseconds() == rhs.microseconds();
}

ROCS_CORE_API bool operator==(const BarsBeats& lhs, const BarsBeats& rhs)
{
    if (lhs.abs_time() != rhs.abs_time()) return false;
    if (lhs.bar_number() != rhs.bar_number()) return false;
    return lhs.beat_number() == rhs.beat_number();
}

ROCS_CORE_API bool operator==(const Key& lhs, const Key& rhs)
{
    if (lhs.abs_time() != rhs.abs_time()) return false;
    if (lhs.sharps() != rhs.sharps()) return false;
    return lhs.mode() == rhs.mode();
}

ROCS_CORE_API bool operator==(const PageNum& lhs, const PageNum& rhs)
{
    if (lhs.abs_time() != rhs.abs_time()) return false;
    return lhs.page_number() == rhs.page_number();
}

ROCS_CORE_API bool operator==(const Meter& lhs, const Meter& rhs)
{
    if (lhs.abs_time() != rhs.abs_time()) return false;
    if (lhs.numerator() != rhs.numerator()) return false;
    if (lhs.denominator() != rhs.denominator()) return false;
    if (lhs.clocks_per_click() != rhs.clocks_per_click()) return false;
    return lhs.notated_32_per_quarter() == rhs.notated_32_per_quarter();
}

ROCS_CORE_API bool operator!=(const Tempo& lhs, const Tempo& rhs)
{
    return !(lhs == rhs);
}

ROCS_CORE_API bool operator!=(const BarsBeats& lhs, const BarsBeats& rhs)
{
    return !(lhs == rhs);
}

ROCS_CORE_API bool operator!=(const Key& lhs, const Key& rhs)
{
    return !(lhs == rhs);
}

ROCS_CORE_API bool operator!=(const PageNum& lhs, const PageNum& rhs)
{
    return !(lhs == rhs);
}

ROCS_CORE_API bool operator!=(const Meter& lhs, const Meter& rhs)
{
    return !(lhs == rhs);
}


}
