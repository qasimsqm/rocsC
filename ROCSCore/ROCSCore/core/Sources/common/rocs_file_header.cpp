#include "core/common/rocs_file_header.h"

namespace cmn {

ROCS_CORE_API bool check_file_header(
    std::istream &is,
    const std::string &vendorId,
    const std::string &fileType,
    const FileVersion &minimumFileVersion,
    FileVersion &streamFileVersion)
{
    ROCSFileHeader hdr;
    is.read((char *)&hdr, sizeof(hdr));
    if (!(hdr.GetVendorId() == vendorId))
    {
        throw FileHeaderError("Not a valid " + vendorId + " file.");
    }

    if (!(hdr.GetFileType() == fileType))
    {
        throw FileHeaderError("Not a valid " + fileType + " file.");
    }
    
    streamFileVersion = hdr.GetFileVersion();

    return !(hdr.GetFileVersion() < minimumFileVersion);
}

}
