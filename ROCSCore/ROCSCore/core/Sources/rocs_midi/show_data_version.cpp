#include "core/rocs_midi/show_data_version.h"

namespace rocs_midi
{

ROCS_CORE_API
cmn::FileVersion LatestFileVersion(major_file_version, minor_file_version);

ROCS_CORE_API
cmn::FileVersion MinimumFileVersion(
    minimum_major_file_version,
    minimum_minor_file_version);

}
