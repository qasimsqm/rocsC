#include "core/std_midi/utility.h"

using namespace std;

namespace std_midi {

/*  Reads numbers stored as a VARIABLE-LENGTH QUANTITY.
    Each byte has bit 7 set, except for the last byte.  */
ROCS_CORE_API UInt32 read_variable_int(istream& is) // throw(StdMIDIError)
{
    UInt32 ret = 0;
    int count = 0;
    Byte b;
    while (true) {
        b = is.get();
        ret = (ret << 7) + (b & 0x7F);
        if (!(b & 0x80)) break;
        if (++count > 3) {
            throw StdMIDIError(
                ex::format( "VARIABLE-LENGTH QUANTITY has more than 4 bytes at %u",
                            is.tellg() - static_cast<istream::pos_type>(4)         )
            );
        }
    }

    return ret;
}
}
