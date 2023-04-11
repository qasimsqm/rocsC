#include "exlib/uuid.h"

using namespace std;

namespace ex {

EXLIB_API UUID::UUID()
{
    memset(this, 0, 16);
}

EXLIB_API UUID::UUID(istream& is, bool little_endian)
{
    is.read((char *)this, 16);
    if (little_endian) {
        change_endianness();
    }
}

EXLIB_API UUID::UUID(UInt8 *bytes_)
{
    bytes(bytes_);
}


UUID::UUID(const std::string& val)
{
    uuid_string(val);
}

void UUID::bytes(UInt8 *bytes_)
{
    memcpy(this, bytes_, 16);
}

void UUID::bytes_le(UInt8 *bytes_)
{
    memcpy(this, bytes_, 16);
    change_endianness();
}


void UUID::WriteBinary(ostream& os, bool little_endian) const
{
    if (little_endian) {
        UUID _le_uuid;
        _le_uuid.bytes_le((UInt8 *)this);
        os.write((char *)&_le_uuid, 16);
    } else {
        os.write((char *)this, 16);
    }
}

std::string UUID::uuid_string() const
{
    /**
    ostream operator<< automatically converts from system byte order to big-endian.
    The bytes stored in time_low, time_mid, and time_hi_version are already big-endian.
    It seems counter-intuitive, but before writing these three values, the byte-order
    must be reversed.
    **/
    UInt32 _time_low = time_low;
    UInt16 _time_mid = time_mid;
    UInt16 _time_hi_version = time_hi_version;
    reverse_byte_order(_time_low);
    reverse_byte_order(_time_mid);
    reverse_byte_order(_time_hi_version);
    stringstream ss;
    ss << hex << setfill('0') << setw(8) << _time_low << "-";
    ss << setw(4) << _time_mid << "-";
    ss << setw(4) << _time_hi_version << "-";
    ss << setw(2) << UInt16(clock_seq_hi_variant);
    ss << setw(2) << UInt16(clock_seq_low) << "-";
    ss << setw(2) << UInt16(node.node0);
    ss << setw(2) << UInt16(node.node1);
    ss << setw(2) << UInt16(node.node2);
    ss << setw(2) << UInt16(node.node3);
    ss << setw(2) << UInt16(node.node4);
    ss << setw(2) << UInt16(node.node5);
    return ss.str();
}

void UUID::uuid_string(const std::string& val)
{
    vector<string> parts = split_string(val, "-");
    time_low = hex_str_to_num<UInt32>(parts.at(0));
    time_mid = hex_str_to_num<UInt16>(parts.at(1));
    time_hi_version = hex_str_to_num<UInt16>(parts.at(2));
    /** Reverse byte order.  See comment for the getter above. **/
    change_endianness();

    UInt16 clock_seq = hex_str_to_num<UInt16>(parts.at(3));

    clock_seq_hi_variant = (clock_seq & 0xFF00) >> 8;
    clock_seq_low = clock_seq & 0x00FF;

    UInt64 node_parts = hex_str_to_num<UInt64>(parts.at(4));
    /** the six node bytes are also in reverse order **/
    memcpy(&node, &node_parts, 6);
    reverse_byte_order(node);
}

bool UUID::operator==(const UUID& rhs) const
{
    return uuid_string() == rhs.uuid_string();
}

void UUID::change_endianness()
{
    reverse_byte_order(time_low);
    reverse_byte_order(time_mid);
    reverse_byte_order(time_hi_version);
}

} // end namespace ex
