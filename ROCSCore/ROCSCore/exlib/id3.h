#pragma once

#include "exlib/win32/declspec.h"

#include <string>
#include <istream>
#include <iostream>
#include <stdexcept>
#include "exlib/xplatform_types.h"
#include "exlib/reverse_byte_order.h"

namespace ex {

struct EXLIB_API ID3Header {
    char id[3];
    Byte version[2];
    Byte flags;
    Byte size[4];

    std::string id_str() const;
    UInt32 get_size() const;
    bool unsynchronisation() const;
    bool extended_header() const;
    bool experimental() const;
};

class EXLIB_API ExtendedHeader {
private:
    UInt32 size_;
    Byte flags[2];
    UInt32 padding_size_;
public:
    ExtendedHeader() {}
    ExtendedHeader(std::istream& is);
    UInt32 size() const;
};

class EXLIB_API FrameHeader {
public:
    char id[4];
private:
    UInt32 size_;
public:
    Byte flags[2];
    FrameHeader() {}
    FrameHeader(std::istream& is);
    std::string id_str() const;
    UInt32 size() const;
};

// http://msdn.microsoft.com/en-us/library/3tdb471s.aspx
MSC_DISABLE_WARNING(4275);

class EXLIB_API NoID3Found : public std::runtime_error
{
public:
    NoID3Found(const std::string& what): std::runtime_error(what) {}
};


class EXLIB_API NotACOMM : public std::runtime_error
{
public:
    NotACOMM(const std::string& what): std::runtime_error(what) {}
};

MSC_RESTORE_WARNING(4275);

MSC_DISABLE_WARNING(4251);
class EXLIB_API COMMFrame {
public:
    FrameHeader hdr;
    Byte encoding;
    char language[3];
    std::string short_description;
    std::string comment;
    COMMFrame() {}
    COMMFrame(std::istream& is);
    std::string id_str() const;
};
MSC_RESTORE_WARNING(4251);

/** is should be an istream containing a complete ID3 tag. **/
EXLIB_API std::string get_comm(std::istream& is);

}
