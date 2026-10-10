#include "xport.h"
#include <stdlib.h>

#define XPORT_LINK_STUB(symbol, address, image) \
    uint32 symbol() \
    { \
        FUNCTION_MARKER(address, image); \
        abort(); \
        return 0; \
    }

XPORT_LINK_STUB(sub_8001F860, 0x8001F860u, "SLUS_009.69")
XPORT_LINK_STUB(sub_8001FBE4, 0x8001FBE4u, "SLUS_009.69")
