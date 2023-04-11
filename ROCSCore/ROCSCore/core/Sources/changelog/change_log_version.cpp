#include "core/changelog/change_log_version.h"

namespace CL
{

ROCS_CORE_API
cmn::FileVersion LatestFileVersion(major_file_version, minor_file_version);

ROCS_CORE_API
cmn::FileVersion MinimumFileVersion(
    minimum_major_file_version,
    minimum_minor_file_version);

}
