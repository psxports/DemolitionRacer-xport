#include "draft_signatures.h"

extern uint32 sub_8001EA08(uint32 sound, sint32 voice, sint32 volume, sint32 frequency, const VECTOR *position);

uint32 sub_8001CF18(uint32 owner, sint32 strength, const VECTOR *position)
{
    sint32 previous = (sint32)r_u32(0x800740F0u);
    uint32 raised;
    /* Original owner argument is unused */
    if (previous <= 0)
        return sub_8001EA08(14u, 10, strength, 4096, position);
    raised = strength < previous ? 1u : 0u;
    if (raised != 0u)
        strength = previous;
    w_u32(0x800740F0u, (uint32)strength);
    return raised;
}
