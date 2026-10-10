#include "draft_signatures.h"
uint32 sub_80043450(uint32 x, uint32 z);

uint32 dr_race_motion_sqrt(uint32 value)
{
    uint32 normalized = value, leading = 0u, even, index, table, shift;
    if (normalized & 0x80000000u)
        normalized = ~normalized;
    if (!normalized)
        return 0u;
    while (!(normalized & 0x80000000u))
    {
        normalized <<= 1;
        ++leading;
    }
    even = leading & ~1u;
    shift = (uint32)((31 - (sint32)even) >> 1);
    index = even >= 24u ? value << ((even - 24u) & 31u) : (uint32)((sint32)value >> ((24u - even) & 31u));
    table = (uint32)(sint32)(sint16)r_u16(0x800602C0u + (index - 64u) * 2u);
    return (table << (shift & 31u)) >> 12;
}

uint32 sub_8002F5F8(uint32 angles, uint32 positions, uint32 heights)
{
    uint32 x, z, y, horizontal, result;
    x = (uint32)((sint32)(r_u32(positions) + r_u32(positions + 24u) - r_u32(positions + 12u) - r_u32(positions + 36u)) >> 8);
    z = (uint32)((sint32)(r_u32(positions + 8u) + r_u32(positions + 32u) - r_u32(positions + 20u) - r_u32(positions + 44u)) >> 8);
    y = (uint32)((sint32)(r_u32(positions + 4u) + r_u32(heights) + r_u32(positions + 28u) + r_u32(heights + 8u) - r_u32(positions + 16u) - r_u32(heights + 4u) - r_u32(positions + 40u) - r_u32(heights + 12u)) >> 8);
    horizontal = dr_race_motion_sqrt(x * x + z * z);
    result = sub_80043450(0u - y, horizontal);
    w_u16(angles + 4u, (uint16)result);
    x = (uint32)((sint32)(r_u32(positions) + r_u32(positions + 12u) - r_u32(positions + 24u) - r_u32(positions + 36u)) >> 8);
    z = (uint32)((sint32)(r_u32(positions + 8u) + r_u32(positions + 20u) - r_u32(positions + 32u) - r_u32(positions + 44u)) >> 8);
    y = (uint32)((sint32)(r_u32(positions + 4u) + r_u32(heights) + r_u32(positions + 16u) + r_u32(heights + 4u) - r_u32(positions + 28u) - r_u32(heights + 8u) - r_u32(positions + 40u) - r_u32(heights + 12u)) >> 8);
    horizontal = dr_race_motion_sqrt(x * x + z * z);
    result = sub_80043450(0u - y, horizontal);
    w_u16(angles, (uint16)result);
    return result;
}

uint32 sub_8002F78C(uint32 angles, uint32 positions, uint32 heights)
{
    uint32 x, z, y, result;
    x = (uint32)((sint32)(r_u32(positions) + r_u32(positions + 12u) - r_u32(positions + 24u) - r_u32(positions + 36u)) >> 8);
    z = (uint32)((sint32)(r_u32(positions + 8u) + r_u32(positions + 20u) - r_u32(positions + 32u) - r_u32(positions + 44u)) >> 8);
    y = (uint32)((sint32)(r_u32(positions + 4u) + r_u32(heights) + r_u32(positions + 16u) + r_u32(heights + 4u) - r_u32(positions + 28u) - r_u32(heights + 8u) - r_u32(positions + 40u) - r_u32(heights + 12u)) >> 8);
    result = sub_80043450(0u - y, dr_race_motion_sqrt(x * x + z * z));
    w_u16(angles, (uint16)result);
    return result;
}

uint32 sub_8002F860(uint32 angles, uint32 positions, uint32 heights)
{
    uint32 x, z, y, result;
    x = (uint32)((sint32)(r_u32(positions) + r_u32(positions + 24u) - r_u32(positions + 12u) - r_u32(positions + 36u)) >> 8);
    z = (uint32)((sint32)(r_u32(positions + 8u) + r_u32(positions + 32u) - r_u32(positions + 20u) - r_u32(positions + 44u)) >> 8);
    y = (uint32)((sint32)(r_u32(positions + 4u) + r_u32(heights) + r_u32(positions + 28u) + r_u32(heights + 8u) - r_u32(positions + 16u) - r_u32(heights + 4u) - r_u32(positions + 40u) - r_u32(heights + 12u)) >> 8);
    result = sub_80043450(0u - y, dr_race_motion_sqrt(x * x + z * z));
    w_u16(angles + 4u, (uint16)result);
    return result;
}
