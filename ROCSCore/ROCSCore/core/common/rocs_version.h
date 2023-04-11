#pragma once

#include "core/win32/declspec.h"

#include <istream>
#include <ostream>
#include <string>
#include <stdexcept>
#include "exlib/format.h"
#include "exlib/string_lib.h"
#include "exlib/xplatform_types.h"
#include "core/common/file_version.h"

namespace cmn
{

class ROCS_CORE_API ROCSVersion
{
public:
    ROCSVersion()
        :
        major_(0),
        minor_(0)
    {
    
    }
    
    ROCSVersion(UInt16 major, UInt16 minor)
        :
        major_(major),
        minor_(minor)
    {
    
    }


    ROCSVersion(std::istream &is)
    {
        is.read((char *)&this->major_, sizeof(this->major_) + sizeof(this->minor_));
    }

    ROCSVersion(std::istream &is, const cmn::FileVersion &)
    {
        is.read((char *)&this->major_, sizeof(this->major_) + sizeof(this->minor_));
    }

    ROCSVersion(const std::string& versionString)
    {
        this->SetWithString(versionString);
    }
    
    ~ROCSVersion() {}

    UInt16 GetMajorVersion() const { return this->major_; }
 
    void SetMajorVersion(const UInt16& major) { this->major_ = major; }
 
    UInt16 GetMinorVersion() const { return this->minor_; }
 
    void SetMinorVersion(const UInt16& minor) { this->minor_ = minor; }
 
    void IncrementMajorVersion() { this->major_++; }
 
    void IncrementMinorVersion() { this->minor_++; }
    
    std::string GetAsString() const
    {
        return ex::format("%u.%u", this->major_, this->minor_);
    }
    
    void SetWithString(const std::string& val)
    {
        auto parts = ex::split_string(val, ".");
        major_ = ex::str_to_num<UInt16>(parts.at(0));
        minor_ = ex::str_to_num<UInt16>(parts.at(1));
    }
    
    void WriteString(std::ostream& os) const
    {
        os << "ROCSVersion(" << this->major_ << ", " << this-minor_ << ")";
    }

    void WriteBinary(std::ostream &os) const
    {
        os.write((char *)&this->major_, sizeof(this->major_) + sizeof(this->minor_));
    }

    void WriteBinary(std::ostream &os, const cmn::FileVersion &) const
    {
        os.write((char *)&this->major_, sizeof(this->major_) + sizeof(this->minor_));
    }

    bool operator<(const ROCSVersion &other) const
    {
        if (this->major_ < other.major_)
        {
            return true;
        }
        
        if (this->major_ == other.major_)
        {
            if (this->minor_ < other.minor_)
            {
                return true;
            }
        } 

        return false;
    }

    bool operator>(const ROCSVersion &other) const
    {
        if (this->major_ > other.major_)
        {
            return true;
        }
        
        if (this->major_ == other.major_)
        {
            if (this->minor_ > other.minor_)
            {
                return true;
            }
        }

        return false;
    }

    bool operator==(const ROCSVersion &other) const
    {
        return (this->major_ == other.major_) && (this->minor_ == other.minor_);
    }

    bool operator<=(const ROCSVersion &other) const { return !(*this > other); }
    bool operator>=(const ROCSVersion &other) const { return !(*this < other); }
    bool operator!=(const ROCSVersion &other) const { return !(*this == other); }

private:
    UInt16 major_;
    UInt16 minor_;
};

#ifndef SWIG
ROCS_CORE_API
std::ostream &operator<<(std::ostream& os, const ROCSVersion& v);
#endif
} // end namespace cmn
