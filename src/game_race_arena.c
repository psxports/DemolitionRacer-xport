#include "draft_signatures.h"
#include <limits.h>

extern uint32 sub_80043450(uint32 x, uint32 z);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern void game_8009AC50(uint32 car);
extern void game_8009ABC4(uint32 car);
extern void game_800946D4(uint32 first_car, uint32 second_car);
extern uint32 sub_8003B898(void);
extern uint32 sub_800418B4(uint32 object, sint32 maximum);

static uint32 arena_umod(uint32 value, uint32 divisor)
{
    if (!divisor)
        xport_mips_break(7);
    return value % divisor;
}

static uint32 arena_div(uint32 value, uint32 divisor)
{
    if (!divisor)
        xport_mips_break(7);
    if (value == 0x80000000 && divisor == 0xFFFFFFFF)
        xport_mips_break(6);
    return (uint32)((sint32)value / (sint32)divisor);
}

static uint32 arena_trunc8(uint32 value)
{
    return (uint32)((sint32)value / 256);
}

static uint32 arena_turn(uint32 object, uint32 x, uint32 z)
{
    uint32 angle = sub_80043450(x, z);
    uint32 relative = ((angle - arena_trunc8(r_u32(object + 128)) + 2048) & 4095) - 2048;
    sint32 steering = (sint32)(relative << 8);
    if (steering > 131072)
        steering = 131072;
    else if (steering < -131072)
        steering = -131072;
    w_u32(object + 8, (uint32)steering);
    return relative;
}

static void arena_stuck(uint32 cache, uint32 object)
{
    if (r_s16(cache + 70) > 0)
        w_u16(cache + 70, r_u16(cache + 70) - 1);
    if (r_s32(object + 888) < 25600)
    {
        sint32 limit;
        if (r_u8(object + 941))
            limit = 9;
        else if (r_s8(object + 946) >= 0)
            limit = 61;
        else
            return;
        w_u16(cache + 70, r_u16(cache + 70) + 8);
        if (r_s16(cache + 70) >= limit)
        {
            w_u8(cache + 95, 3);
            w_u16(cache + 60, 30);
            w_u16(cache + 70, 0);
        }
    }
}

static void arena_choose(uint32 cache, uint32 object)
{
    uint32 owner = r_u8(object + 937), value, radius, angle, attempt, selected;
    value = sub_8001B884(owner);
    w_u16(cache + 60, value % 500 + 100);
    value = sub_8001B884(owner);
    if (!arena_umod(value, r_u8(cache + 91)))
    {
        if (!r_u32(0x800593A8))
            w_u8(cache + 94, 0);
        else
            w_u8(cache + 94, sub_8001B884(owner) & 1);
        w_u8(cache + 95, 1);
        return;
    }
    value = sub_8001B884(owner);
    if (!arena_umod(value, (uint32)(sint32)r_s16(cache + 78)))
    {
        radius = sub_8001B884(owner) % 10000;
        angle = sub_8001B884(owner) & 4095;
        value = (uint32)draft_agent3_missing_8004388C(angle) * radius;
        w_u32(cache + 4, (uint32)((sint32)value / 4096));
        value = (uint32)draft_agent3_missing_800437EC(angle) * radius;
        w_u32(cache + 12, (uint32)((sint32)value / 4096));
        w_u8(cache + 95, 4);
        w_u16(cache + 60, 100);
        return;
    }
    for (attempt = 0;; ++attempt)
    {
        selected = arena_umod(sub_8001B884(owner), r_u32(0x80065254));
        if (attempt >= 4)
            break;
        if (selected == owner)
            continue;
        if (!(r_u8(0x80066D14 + selected * 952 + 378) & 1))
            break;
    }
    w_u8(cache + 94, selected);
    w_u8(cache + 95, 1);
}

static void arena_pursue(uint32 cache, uint32 object)
{
    uint32 target = r_u8(cache + 94), other = 0x80066D14 + target * 952;
    uint32 x = arena_trunc8(r_u32(other + 100) - r_u32(object + 100));
    uint32 z = arena_trunc8(r_u32(other + 108) - r_u32(object + 108));
    uint32 vx = (uint32)((sint32)(r_u32(object + 112) - r_u32(other + 112)) >> 8);
    uint32 vz = (uint32)((sint32)(r_u32(object + 120) - r_u32(other + 120)) >> 8);
    uint32 closing = x * vx + z * vz, squared = x * x + z * z, time = 60, relative;
    if ((sint32)closing > 0)
    {
        time = arena_div(squared, closing);
        if ((sint32)time >= 61)
            time = 60;
    }
    x += time * (uint32)(r_s32(other + 112) >> 8);
    z += time * (uint32)(r_s32(other + 120) >> 8);
    relative = arena_turn(object, x, z);
    if (relative + 1023 < 2047)
    {
        w_u16(object + 16, r_s32(object + 900) < 8000 ? r_u16(cache + 20) : 0);
        w_u8(object + 19, 0);
    }
    else if (r_s32(object + 888) < 25600)
    {
        w_u8(object + 19, 0);
        w_u16(object + 16, r_u16(cache + 20));
    }
    else
    {
        uint32 throttle = r_u16(cache + 20);
        w_u8(object + 19, 1);
        w_u16(object + 16, 0u - throttle);
    }
    arena_stuck(cache, object);
    w_u16(cache + 60, r_u16(cache + 60) - 1);
    if (r_s16(cache + 60) <= 0)
        w_u8(cache + 95, 2);
    if (r_s8(object + 946) == (sint32)target)
    {
        w_u8(cache + 95, 3);
        w_u16(cache + 60, 30);
        w_u16(cache + 70, 0);
    }
}

static void arena_wander(uint32 cache, uint32 object)
{
    uint32 x = r_u32(cache + 4) - arena_trunc8(r_u32(object + 100));
    uint32 z = r_u32(cache + 12) - arena_trunc8(r_u32(object + 108));
    arena_turn(object, x, z);
    w_u16(object + 16, r_s32(object + 900) < 8000 ? r_u16(cache + 20) : 0);
    w_u8(object + 19, 0);
    w_u16(cache + 60, r_u16(cache + 60) - 1);
    if (r_s16(cache + 60) <= 0)
        w_u8(cache + 95, 2);
    arena_stuck(cache, object);
}

void game_800B8704(void)
{
    uint32 index;
    if (r_u32(0x800BEEA8))
    {
        for (index = 0; (sint32)index < (sint32)r_u32(0x80065254); ++index)
        {
            uint32 object = 0x80066D14 + index * 952, cache = 0x800C7698 + index * 104;
            if (!r_u8(0x80059A28 + index))
                continue;
            switch (r_u8(cache + 95))
            {
                case 1:
                    arena_pursue(cache, object);
                    break;
                case 2:
                    arena_choose(cache, object);
                    break;
                case 3:
                    w_u32(object + 8, 0);
                    w_u8(object + 19, 0);
                    w_u8(object + 18, 0);
                    w_u16(object + 16, 0u - r_u16(cache + 20));
                    w_u16(cache + 60, r_u16(cache + 60) - 1);
                    if (r_s16(cache + 60) <= 0)
                    {
                        w_u8(cache + 95, 2);
                        w_u8(object + 18, 2);
                    }
                    break;
                case 4:
                    arena_wander(cache, object);
                    break;
            }
        }
        for (index = 0; (sint32)index < (sint32)r_u32(0x80065254); ++index)
            game_8009AC50(index);
    }
    else
        for (index = 0; (sint32)index < (sint32)r_u32(0x80065254); ++index)
            game_8009ABC4(index);
}

void game_8009545C(void)
{
    uint32 index, other;
    for (index = 0; (sint32)index < (sint32)r_u32(0x80065254); ++index)
    {
        uint32 object = 0x80066D14 + index * 952, bounds = 0x800C03D0 + index * 20;
        uint32 radius = (uint32)(sint32)r_s16(r_u32(object) + 128);
        w_u32(bounds, (uint32)(r_s32(object + 100) >> 8) - radius);
        w_u32(bounds + 4, (uint32)(r_s32(object + 108) >> 8) - radius);
        w_u32(bounds + 8, (uint32)(r_s32(object + 100) >> 8) + radius);
        w_u32(bounds + 12, (uint32)(r_s32(object + 108) >> 8) + radius);
        w_u8(object + 946, 255);
        if (r_u8(object + 306))
            game_80093F94(object);
    }
    for (index = 0; (sint32)index < (sint32)(r_u32(0x80065254) - 1); ++index)
        for (other = index + 1; (sint32)other < (sint32)r_u32(0x80065254); ++other)
        {
            uint32 a = 0x80066D14 + index * 952, b = 0x80066D14 + other * 952;
            if (!r_u8(a + 940) && r_u8(a + 942) < 15 && !r_u8(b + 940) && r_u8(b + 942) < 15)
                game_800946D4(index, other);
            else
                game_80095054(index, other);
        }
}

void game_800B9518(void)
{
    uint32 list = r_u32(0x800C7D58), count = r_u32(list), index, axis;
    sint32 translation[3];
    for (axis = 0; axis < 3; ++axis)
        translation[axis] = r_s32(0x80081BB8 + axis * 4);
    for (axis = 0; axis < 3; ++axis)
        w_u32(0x80081BB8 + axis * 4, (uint32)(translation[axis] >> 2));
    w_u32(0x80065170, 0);
    w_u32(0x80065134, 0);
    sub_8003B898();
    for (axis = 0; axis < 5; ++axis)
        xport_gte_write_control(axis, r_u32(0x80081BA4 + axis * 4));
    for (index = 0; (sint32)index < (sint32)count; ++index)
        sub_800418B4(r_u32(list + 4 + index * 4), 16384);
    for (axis = 0; axis < 3; ++axis)
        w_u32(0x80081BB8 + axis * 4, (uint32)translation[axis]);
    w_u32(0x80065170, 2);
    w_u32(0x80065134, 2);
    sub_8003B898();
}
