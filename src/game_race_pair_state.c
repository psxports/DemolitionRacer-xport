#include "draft_signatures.h"

void game_80093F94(uint32 object)
{
    uint32 missing = ~(uint32)r_u8(object + 942u);
    sint32 delta = (sint32)(r_u32(object + 128u) - r_u32(object + 292u)) >> 8;
    uint32 torque_x = r_u32(object + 896u), torque_z = r_u32(object + 900u);
    sint32 category;
    if ((missing & 5u) != 0u && (missing & 10u) != 0u && ((torque_z + 1000u) < 2001u || (torque_x + 1000u) < 2001u))
    {
        uint32 previous = r_u32(object + 272u);
        if (previous != 0u)
        {
            if ((sint32)previous >= 5)
                previous -= 4u;
            (void)sub_8001532C(r_u8(object + 307u), previous - 1u);
        }
        w_u8(object + 306u, 0u);
        return;
    }
    if (delta > 1024)
    {
        if (delta < 3073)
            category = delta < 2049 ? 5 : 6;
        else
            category = delta < 4097 ? 7 : 8;
    }
    else if (delta < -1024)
    {
        if (delta < -3072)
            category = delta < -4096 ? 4 : 3;
        else
            category = delta < -2048 ? 2 : 1;
    }
    else
        category = 0;
    w_u32(object + 272u, (uint32)category);
}
