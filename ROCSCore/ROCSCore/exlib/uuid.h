#pragma once

#include "exlib/win32/declspec.h"

#include <istream>
#include <ostream>
#include <sstream>
#include <string>
#include <stdexcept>
#include "exlib/reverse_byte_order.h"
#include "exlib/xplatform_types.h"
#include "exlib/string_lib.h"

#include <cstring>

namespace ex {

// http://msdn.microsoft.com/en-us/library/3tdb471s.aspx
MSC_DISABLE_WARNING(4275);

class EXLIB_API UUIDError : public std::runtime_error
{
public:
    UUIDError(const std::string& what): std::runtime_error(what) {}
};

struct EXLIB_API NodeT {
    UInt8 node0;
    UInt8 node1;
    UInt8 node2;
    UInt8 node3;
    UInt8 node4;
    UInt8 node5;
};

class EXLIB_API UUID {
    UInt32  time_low;
    UInt16  time_mid;
    UInt16  time_hi_version;
    UInt8   clock_seq_hi_variant;
    UInt8   clock_seq_low;
    NodeT   node;

public:
    UUID();
    UUID(std::istream&, bool little_endian=false);
    UUID(UInt8 *bytes);
    UUID(const std::string& val);
    
    void bytes(UInt8 *bytes);
    void bytes_le(UInt8 *bytes);
    void WriteBinary(std::ostream&, bool little_endian=false) const;
    
    std::string uuid_string() const;
    void uuid_string(const std::string& val);
    
    bool operator==(const UUID& rhs) const;
    bool operator!=(const UUID& rhs) const { return !(operator==(rhs)); }

    void change_endianness();
};

MSC_RESTORE_WARNING(4275);

} // namespace ex
