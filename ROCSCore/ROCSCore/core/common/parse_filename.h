#pragma once

#include "core/win32/declspec.h"

#include <string>
#include <vector>
#include "exlib/xplatform_types.h"
#include "exlib/string_lib.h"
#include "exlib/path.h"
#include "core/common/rocs_exception.h"


namespace cmn {

class ROCS_CORE_API FilenameError : public ROCSException
{
public:
    inline FilenameError(const std::string& what): ROCSException(what) {}
};

ROCS_CORE_API void parse_filename(const std::string& filename,
                    std::string& song_number, std::string& song_name, UInt8& attacca_state);



} // namespace cmn
