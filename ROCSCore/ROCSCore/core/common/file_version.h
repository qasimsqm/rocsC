#pragma once

#include <ostream>
#include "core/win32/declspec.h"
#include "exlib/xplatform_types.h"

namespace cmn
{

struct ROCS_CORE_API FileVersion
{
    FileVersion(): majorVersion_(0), minorVersion_(0) {}  
    
    FileVersion(UInt8 inMajor, UInt8 inMinor)
        :
        majorVersion_(inMajor),
        minorVersion_(inMinor)
    {}
    
    bool operator<(const FileVersion &other) const
    {
        if (this->majorVersion_ == other.majorVersion_)
        {
            return this->minorVersion_ < other.minorVersion_;
        } else
        {
            return this->majorVersion_ < other.majorVersion_;
        }
    }

    bool operator>(const FileVersion &other) const
    {
        if (this->majorVersion_ == other.majorVersion_)
        {
            return this->minorVersion_ > other.minorVersion_;
        } else 
        {
            return this->majorVersion_ > other.majorVersion_;
        }
    }

    bool operator==(const FileVersion &other) const
    {
        return this->majorVersion_ == 
            other.majorVersion_ && this->minorVersion_ == other.minorVersion_;
    }

    bool operator<=(const FileVersion &other) const
    {
        return !(*this > other);
    }

    bool operator>=(const FileVersion &other) const
    {
        return !(*this < other); 
    }

    bool operator!=(const FileVersion &other) const
    {
        return !(*this == other);
    }
    
    UInt8 GetMajorVersion() const { return this->majorVersion_; }
    
    void SetMajorVersion(UInt8 majorVersion) { this->majorVersion_ = majorVersion; }

    UInt8 GetMinorVersion() const { return this->minorVersion_; }

    void SetMinorVersion(UInt8 minorVersion) { this->minorVersion_ = minorVersion; }

private:
    UInt8 majorVersion_;
    UInt8 minorVersion_;
};

#ifndef SWIG
ROCS_CORE_API
#endif
FileVersion get_showdata_version(std::istream& is);

#ifndef SWIG
ROCS_CORE_API
#endif
FileVersion get_logdata_version(std::istream& is);

inline
std::ostream &operator<<(std::ostream &os, const FileVersion &fileVersion)
{
    os << static_cast<UInt16>(fileVersion.GetMajorVersion()) << "."
        << static_cast<UInt16>(fileVersion.GetMinorVersion());
    return os;
}

}
