#pragma once

#include "core/win32/declspec.h"
#include "core/common/file_version.h"

namespace rocs_midi
{

const UInt8 major_file_version = 2;

const UInt8 minor_file_version = 0;

const UInt8 minimum_major_file_version = 1;

const UInt8 minimum_minor_file_version = 0;

ROCS_CORE_API
extern cmn::FileVersion LatestFileVersion;

ROCS_CORE_API
extern cmn::FileVersion MinimumFileVersion;

}
