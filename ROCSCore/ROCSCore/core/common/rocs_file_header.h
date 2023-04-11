#pragma once

#include "core/win32/declspec.h"

#include <string>
#include <ostream>
#include <istream>
#include <cstring>

#include "exlib/xplatform_types.h"
#include "core/common/rocs_exception.h"
#include "core/common/file_version.h"

namespace cmn
{

class ROCS_CORE_API FileHeaderError : public ROCSException
{
public:
    FileHeaderError(const std::string& what): ROCSException(what) {}
};

struct ROCS_CORE_API ROCSFileHeader
{
    ROCSFileHeader() {}
    
    ROCSFileHeader( 
        const std::string& vendorID,
        const std::string& fileType,
        UInt8 majorFileVersion,
        UInt8 minorFileVersion)
    {
        this->SetVendorId(vendorID);
        this->SetFileType(fileType);
        this->SetMajorFileVersion(majorFileVersion);
        this->SetMinorFileVersion(minorFileVersion);
    }

    std::string GetVendorId() const { return std::string(this->vendorID_, 4); }

    void SetVendorId(const std::string& val) { memcpy(this->vendorID_, val.c_str(), 4); }

    std::string GetFileType() const { return std::string(this->fileType_, 4); }

    void SetFileType(const std::string& val) { memcpy(this->fileType_, val.c_str(), 4); }

    UInt8 GetMajorFileVersion() const { return this->majorFileVersion; }

    void SetMajorFileVersion(UInt8 val) { this->majorFileVersion = val; }

    UInt8 GetMinorFileVersion() const { return this->minorFileVersion; }

    void SetMinorFileVersion(UInt8 val) { this->minorFileVersion = val; }

    FileVersion GetFileVersion() const
    {
        return FileVersion(this->majorFileVersion, this->minorFileVersion);
    }

private:
    char vendorID_[4];
    char fileType_[4];
    UInt8 majorFileVersion;
    UInt8 minorFileVersion;
};

ROCS_CORE_API bool check_file_header(
    std::istream &is,
    const std::string &vendorID,
    const std::string &fileType,
    const FileVersion &minimumFileVersion,
    FileVersion &inputFileVersion);

} // end namespace cmn
