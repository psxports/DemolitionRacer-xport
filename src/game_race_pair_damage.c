#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>

static sint32 damage_add(sint32 a, sint32 b)
{
    return (sint32)((uint32)a + (uint32)b);
}

static sint32 damage_accumulate(uint32 player, sint32 amount, uint32 event)
{
    uint32 object = 0x80066E94u + player * 952u;
    sint32 result = (sint16)r_u16(object + 4u);
    if ((sint32)r_u32(0x80064C14u) >= (sint32)player)
    {
        sint32 pending = (sint16)r_u16(object + 6u);
        if (pending >= 0)
        {
            result = damage_add(result, pending);
            w_u16(object + 6u, (uint16)amount);
            w_u32(object, r_u32(0x80064A30u));
        }
        else
            result = damage_add(result, amount);
    }
    else
        result = damage_add(result, amount);
    w_u16(object + 4u, (uint16)result);
    w_u8(object + 8u, (uint8)event);
    return result;
}

sint32 sub_8001532C(uint32 player, uint32 event)
{
    sint32 amount = (sint16)r_u16(0x800594B0u + event * 2u);
    if (r_u8(0x800670C5u + player * 952u))
        return 0;
    return damage_accumulate(player, amount, event);
}

sint32 sub_800153DC(uint32 player, sint32 strength)
{
    uint32 i;
    if (strength < (sint32)r_u16(r_u32(0x800651E4u) + 0xB1Cu))
        return 0;
    for (i = 0; i < 3u; ++i)
    {
        if (strength < (sint32)r_u16(r_u32(0x800651E4u) + 0xB1Eu + i * 2u))
        {
            sint32 amount = (sint16)r_u16(0x800594BAu + i * 2u);
            if (r_u8(0x800670C5u + player * 952u))
                return 0;
            return damage_accumulate(player, amount, i + 5u);
        }
    }
    /* Read the amount before the disabled flag branch */
    i = r_u8(0x800670C5u + player * 952u);
    {
        sint32 amount = (sint16)r_u16(0x800594C0u);
        if (i)
            return 0;
        return damage_accumulate(player, amount, 8u);
    }
}

sint32 sub_80030110(uint32 player, uint32 region, sint32 strength)
{
    uint32 object = 0x80066D14u + player * 952u, address;
    sint32 old, value, increment, limit;
    if (r_u8(0x800670C5u + player * 952u) || r_u32(0x80064A44u))
        return 0;
    if ((r_u8(object + 378u) & 0x41u) || !strength)
        return 0;
    address = object + 338u + region * 2u;
    old = (sint16)r_u16(address);
    value = damage_add(old, strength);
    if (value > 1024)
        value = 1024;
    else if (value >= 512)
    {
        increment = damage_add(value, -512);
        if (increment >= strength)
            increment = strength;
        value = damage_add(value, -(increment / 2));
    }
    if (old != value)
    {
        w_u16(address, (uint16)value);
        sub_80030470(player, object, region, value);
    }
    old = (sint16)r_u16(object + 362u);
    increment = strength >> 3;
    value = damage_add(old, increment);
    if (value > 1024)
        value = 1024;
    else if (value >= 512)
    {
        sint32 excess = damage_add(value, -512);
        if (excess < increment)
            increment = excess;
        value = damage_add(value, -(increment / 2));
    }
    if (old != value)
    {
        w_u16(object + 362u, (uint16)value);
        sub_80030470(player, object, 12u, value);
    }
    old = (sint16)r_u16(object + 320u);
    limit = (sint16)r_u16(object + 322u);
    value = damage_add(old, strength);
    if (limit < value)
    {
        sub_800302F8(player, object);
        return -1;
    }
    {
        uint32 flags = r_u8(object + 378u);
        w_u16(object + 320u, (uint16)value);
        if (!(flags & 2u))
        {
            limit = (sint16)r_u16(object + 322u);
            if (damage_add(limit, -(limit / 4)) < value)
            {
                w_u8(object + 378u, (uint8)(flags | 2u));
                return -2;
            }
        }
    }
    return (sint32)((uint32)value - (uint32)old);
}
