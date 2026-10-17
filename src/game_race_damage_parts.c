#include "draft_signatures.h"

static sint32 parts_add(sint32 a, sint32 b)
{
    return (sint32)((uint32)a + (uint32)b);
}

static sint32 parts_angle(sint32 damage, sint32 start)
{
    sint32 product = (sint32)(((uint32)damage - 768u) * (682u - (uint32)start));
    sint32 high = (sint32)(((sint64)product * (sint32)0x80808081u) >> 32);
    sint32 scaled = parts_add(high, product) >> 7;
    return parts_add(parts_add(scaled, -(product >> 31)), start);
}

static sint32 parts_damage(uint32 object, sint32 damage, uint32 enabled_address, uint32 start_address, uint32 started, uint32 completed, uint32 reset_offset, uint32 start_offset, uint32 angle_offset)
{
    sint32 result = (sint16)r_u16(enabled_address), start, angle;
    uint32 flags;
    if (!result)
        return result;
    flags = r_u8(object + 378u);
    result = (sint32)(flags & started);
    if (flags & completed)
        return result;
    if (!result)
    {
        w_u8(object + 378u, (uint8)(flags | started));
        start = r_u16(start_address);
        w_u16(object + reset_offset, 0u);
        w_u16(object + start_offset, (uint16)start);
    }
    if (damage >= 1024)
    {
        flags = r_u8(object + 378u);
        w_u16(object + angle_offset, 682u);
        result = (sint32)(flags | completed);
        w_u8(object + 378u, (uint8)result);
        return result;
    }
    start = (sint16)r_u16(start_address);
    /* Preserve the wrapped signed multiply-high division sequence */
    angle = (sint16)r_u16(object + angle_offset);
    result = parts_angle(damage, start);
    if (angle < result)
        w_u16(object + angle_offset, (uint16)result);
    return result;
}

sint32 sub_80033790(uint32 object, sint32 damage)
{
    return parts_damage(object, damage, 0x80066C50u, 0x80066C54u, 4u, 8u, 336u, 328u, 332u);
}

sint32 sub_80033854(uint32 object, sint32 damage)
{
    return parts_damage(object, damage, 0x80066C42u, 0x80066C46u, 16u, 32u, 334u, 326u, 330u);
}
