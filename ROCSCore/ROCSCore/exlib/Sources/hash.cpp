#include "exlib/hash.h"

namespace ex {

EXLIB_API
unsigned long
hash(void *data, unsigned long size)
{
    unsigned char *str = static_cast<unsigned char *>(data);
    unsigned long hash = 5381;
    int c;
    unsigned char *end = str + size;
    
    while (str < end)
    {
        c = *str++;
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }

    return hash;
}

}
