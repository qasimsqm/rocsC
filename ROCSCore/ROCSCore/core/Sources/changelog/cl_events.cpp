#include "core/changelog/cl_events.h"
#include "core/changelog/change_log_version.h"

using namespace std;
using namespace cmn;

namespace CL
{
const UInt8 InPoint::evtCode = codes::inPoint;
const UInt8 OutPoint::evtCode = codes::outPoint;
const UInt8 CustomBar::evt_code = codes::bar;
const UInt8 Marker::evt_code = codes::marker;
const UInt8 Caesura::evt_code = codes::caesura;
const UInt8 Cut::evt_code = codes::cut;
const UInt8 Vamp::evt_code = codes::vamp;
const UInt8 Repeat::evt_code = codes::repeat;
const UInt8 Transpose::evt_code = codes::transpose;
const UInt8 TempoScale::evt_code = codes::tempo_scale;
const UInt8 Click::evt_code = codes::click;
const UInt8 Fermata::evt_code = codes::fermata;

UInt32 caesuraVersionTarget_ = CAESURA_TARGETS_ALL;
UInt32 caesuraIgnoredVersionTarget_ = 3;
// Because 3 is not predefined, no caesuras will be ignored.

UInt32 GetCaesuraVersionTarget()
{
    return caesuraVersionTarget_;
}

UInt32 GetCaesuraIgnoredVersionTarget()
{
    return caesuraIgnoredVersionTarget_;
}

void SetCaesuraVersionTarget(UInt32 caesuraVersionTarget)
{
    #ifdef DEBUG
    if (!((caesuraVersionTarget == CAESURA_TARGETS_ALL)
        || (caesuraVersionTarget == CAESURA_TARGETS_SHOWREADY)
        || (caesuraVersionTarget == CAESURA_TARGETS_STAGETRACKS_FILLHARMONIC)))
    {
        throw std::logic_error(
            "SetCaesuraVersionTarget does not use a predefined value");
    }
    #endif 

    caesuraVersionTarget_ = caesuraVersionTarget;
    caesuraIgnoredVersionTarget_ = caesuraVersionTarget;
//    if (caesuraVersionTarget == CAESURA_TARGETS_SHOWREADY)
//    {
//        caesuraIgnoredVersionTarget_ = CAESURA_TARGETS_SHOWREADY;
//    } else if (caesuraVersionTarget == CAESURA_TARGETS_STAGETRACKS_FILLHARMONIC)
//    {
//        caesuraIgnoredVersionTarget_ = CAESURA_TARGETS_STAGETRACKS_FILLHARMONIC;
//    } else
//    {
//        caesuraIgnoredVersionTarget_ = 3;
//    }
}

void CustomBar::WriteString(ostream& os) const
{
    os  << codes::code_names[code()]
        << "("
        << "abs_time="  << abs_time() << ", "
        << "value="     << value()
        << ")";
}

bool CustomBar::operator==(const ROCSEvent &other) const
{
    return ((this->code() == other.code())
        && (this->abs_time() == other.abs_time()) 
        && (this->value() == other.value()));
}

Marker::Marker(istream& is, const cmn::FileVersion &)
{
    is.read((char *)&this->abs_time_, sizeof(this->abs_time_));
    this->value_ = ex::ReadString(is);
}


void Marker::WriteBinary(ostream& os, const cmn::FileVersion &) const
{
    os.write((char *)&abs_time_, sizeof(abs_time_));
    ex::WriteString(os, value_);
}


void Marker::value(const string& val)
{
    this->value_ = val;
    if (this->value_.size() > 256)
    {
        this->value_.resize(256);
    }
}

bool Marker::operator<(const ROCSEvent &other) const
{
    if (abs_time() == other.abs_time()) {
        return (this->value().compare(other.value()) < 0);
    } else {
        return abs_time() < other.abs_time();
    }
}

bool Marker::operator==(const ROCSEvent &other) const
{
    return ((this->code() == other.code())
        && (this->abs_time() == other.abs_time()) 
        && (this->value() == other.value()));
}

void Marker::WriteString(ostream& os) const
{
    os  << "Marker("
        << "abs_time="  << abs_time() << ", "
        << "value="     << value()
        << ")";
}

Caesura::Caesura(istream &is, const cmn::FileVersion &fileVersion)
{
    this->abs_time_ = ex::read<UInt32>(is);

    // Version 2.x
    if (fileVersion >= cmn::FileVersion(2, 0))
    {
        this->versionTarget_ = ex::read<UInt32>(is);
    } else
    {
        this->versionTarget_ = CAESURA_TARGETS_ALL;
    }
}

void Caesura::WriteBinary(ostream &os, const cmn::FileVersion &fileVersion ) const
{
    ex::write(os, this->abs_time_);

    // Version 2.x
    if (fileVersion >= cmn::FileVersion(2, 0))
    {
        ex::write(os, this->versionTarget_);
    }
}

void Caesura::WriteString(ostream& os) const
{
    os  << "Caesura("
        << "abs_time=" << this->abs_time_ 
        << ", versionTarget=" << this->versionTarget_
        << ")";
}

PairedEvent::PairedEvent(istream &is, const cmn::FileVersion &)
{
    start_ = ex::read<UInt32>(is);
    end_ = ex::read<UInt32>(is);
    int_value_ = ex::read<SInt32>(is);
}

void PairedEvent::WriteBinary(ostream& os, const cmn::FileVersion &) const
{
    ex::write(os, start_);
    ex::write(os, end_);
    ex::write(os, int_value_);
}

void PairedEvent::WriteString(ostream& os) const
{
    os  << codes::code_names[code()]
        << "("
        << "start="     << start()  << ", "
        << "end="       << end()    << ", "
        << "value="     << int_value()
        << ")";
}


bool PairedEvent::operator<(const ROCSEvent &other) const
{
    if (start() == other.start()) {
        if (end() == other.end()) {
            return int_value() < other.int_value();
        } else {
            return (end() < other.end());
        }
    } else {
        return start() < other.start();
    }
}

bool PairedEvent::operator==(const ROCSEvent &other) const
{
    return ((this->code() == other.code())
        && (this->start() == other.start())
        && (this->end() == other.end())
        && (this->int_value() == other.int_value()));
}

std::string PairedEvent::value() const
{
    stringstream ss;
    ss << int_value();
    return ss.str();
}

std::string Transpose::value() const
{
    stringstream ss;
    ss << showpos << int_value();
    return ss.str();
}

void TempoScale::WriteString(ostream& os) const
{
    os  << "TempoScale("
        << "start="     << start()          << ", "
        << "end="       << end()            << ", "
        << "value="     << float_value()
        << ")";
}

bool TempoScale::operator<(const ROCSEvent &other) const
{
    if (start() == other.start()) {
        if (end() == other.end()) {
            return float_value() < other.float_value();
        } else {
            return (end() < other.end());
        }
    } else {
        return start() < other.start();
    }
}

std::string TempoScale::value() const
{
    stringstream ss;
    ss.precision(2);
    ss << fixed << float_value() * 100.0 << "%";
    return ss.str();
}
/*
ROCS_CORE_API bool operator==(const Marker& lhs, const Marker& rhs)
{
    if (lhs.abs_time() != rhs.abs_time()) return false;
    return lhs.value() == rhs.value();
}

ROCS_CORE_API bool operator==(const Caesura& lhs, const Caesura& rhs)
{
    return lhs.abs_time() == rhs.abs_time();
}

ROCS_CORE_API bool operator==(const CustomBar& lhs, const CustomBar& rhs)
{
    if (lhs.abs_time() != rhs.abs_time()) return false;
    return lhs.value() == rhs.value();
}

ROCS_CORE_API bool operator==(const PairedEvent& lhs, const PairedEvent& rhs)
{
    if (lhs.code() != rhs.code()) return false;
    if (lhs.start() != rhs.start()) return false;
    if (lhs.end() != rhs.end()) return false;
    return lhs.int_value() == rhs.int_value();
}

bool operator!=(const CustomBar& lhs, const CustomBar& rhs) { return !(lhs == rhs); }
bool operator!=(const Marker& lhs, const Marker& rhs) { return !(lhs == rhs); }
bool operator!=(const Caesura& lhs, const Caesura& rhs) { return !(lhs == rhs); }
bool operator!=(const PairedEvent& lhs, const PairedEvent& rhs) { return !(lhs == rhs); }
*/
} // End namespace CL
