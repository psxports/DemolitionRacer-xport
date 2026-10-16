#include "draft_signatures.h"
#include <limits.h>

extern uint32 sub_8002FDA4(uint32 mask);
extern sint32 sub_80030110(uint32 owner, uint32 zone, sint32 force);
extern void sub_80035458(const sint16 *first, const sint16 *second, sint16 *output);

static sint32 pickup_sub(sint32 first, sint32 second)
{
    return (sint32)((uint32)first - (uint32)second);
}

static sint32 pickup_mul(sint32 first, sint32 second)
{
    return (sint32)((uint32)first * (uint32)second);
}

static sint32 pickup_div(sint32 value, sint32 divisor)
{
    if (!divisor)
        xport_mips_break(7);
    if (value == INT_MIN && divisor == -1)
        xport_mips_break(6);
    return value / divisor;
}

static sint32 pickup_abs(sint32 value)
{
    return value < 0 ? pickup_sub(0, value) : value;
}

static void pickup_quaternion(uint32 object, sint16 quaternion[4], sint16 inverse[4])
{
    uint32 index;
    for (index = 0; index < 4; ++index)
        quaternion[index] = r_s16(object + 252 + index * 2);
    for (index = 0; index < 3; ++index)
        inverse[index] = (sint16)(0 - (uint16)quaternion[index]);
    inverse[3] = quaternion[3];
}

uint32 sub_80015968(uint32 position)
{
    uint32 model = r_u32(0x80066D14), owner = 0, axis, object, zone;
    sint32 width = r_s16(model + 40) * 2, length = r_s16(model + 44) * 2;
    sint32 strength, distance, result = r_s16(model + 44);
    VECTOR delta;
    sint16 quaternion[4], inverse[4], vector[4];
    w_u32(0x800651F4, 0);
    if (r_s32(0x80065254) <= 0)
        return (uint32)result;
    do
    {
        object = 0x80066D14 + owner * 952;
        delta.vx = pickup_sub(r_s32(position), r_s32(object + 100)) >> 8;
        delta.vy = pickup_sub(r_s32(position + 4), r_s32(object + 104)) >> 8;
        delta.vz = pickup_sub(r_s32(position + 8), r_s32(object + 108)) >> 8;
        if ((sint32)dr_pair_squared_distance(&delta) <= 3999999)
        {
            vector[0] = (sint16)delta.vx;
            vector[1] = (sint16)delta.vy;
            vector[2] = (sint16)delta.vz;
            vector[3] = 0;
            pickup_quaternion(object, quaternion, inverse);
            sub_80035458(inverse, vector, vector);
            sub_80035458(vector, quaternion, vector);
            distance = pickup_abs(vector[0]);
            strength = width < distance ? pickup_div((sint32)((uint32)width << 12), distance) : 4096;
            distance = pickup_abs(vector[2]);
            if (length < distance)
                strength = pickup_div(pickup_mul(strength, length), distance);
            distance = pickup_abs(vector[1]);
            if (width < distance)
                strength = pickup_div(pickup_mul(strength, width), distance);
            zone = delta.vz > 0 ? (delta.vx > 0 ? 2 : 0) : (delta.vx > 0 ? 6 : 8);
            sub_80030110(owner, zone, strength >> 1);
            pickup_quaternion(object, quaternion, inverse);
            vector[0] = 0;
            vector[1] = (sint16)strength;
            vector[2] = 0;
            vector[3] = 0;
            sub_80035458(quaternion, vector, vector);
            sub_80035458(vector, inverse, vector);
            for (axis = 0; axis < 2; ++axis)
                w_u32(object + 112 + axis * 4, r_u32(object + 112 + axis * 4) + ((uint32)(sint32)vector[axis] << 4));
            w_u32(0x800651F4, r_u32(0x800651F4) + 1);
            w_u32(object + 120, r_u32(object + 120) + ((uint32)(sint32)vector[2] << 4));
        }
        ++owner;
        result = (sint32)owner < r_s32(0x80065254);
    } while (result);
    return (uint32)result;
}

uint32 sub_8002FEF4(uint32 player)
{
    uint32 object = 0x80066D14 + player * 952, index, classification, vertex, address;
    sint32 previous = r_s16(object + 320), value, scale, scaled;
    value = pickup_mul(r_s16(object + 322), 3);
    value = pickup_sub(previous, value / 10);
    if (value <= 0)
        sub_8002FDA4(1u << (player & 31));
    else
    {
        w_u16(object + 320, (uint16)value);
        for (index = 0; index < 13; ++index)
            w_u8(object + 376 - index, 0);
        scale = pickup_div((sint32)((uint32)value << 10), previous);
        for (index = 0; index < 13; ++index)
        {
            scaled = pickup_mul(r_s16(object + 338 + index * 2), scale) / 1024;
            w_u16(object + 338 + index * 2, (uint16)scaled);
            for (classification = 0; classification < 4; ++classification)
                if (scaled >= r_s16(0x8005AAA8 + classification * 2) && scaled < r_s16(0x8005AAAA + classification * 2))
                    break;
            for (vertex = 0; vertex < 3; ++vertex)
            {
                address = object + (uint32)(sint32)r_s16(0x8005AA48 + index * 6 + vertex * 2) + 364;
                if (r_u8(address) < classification)
                    w_u8(address, (uint8)classification);
            }
        }
        if (r_u8(object + 376) < 3)
            w_u8(object + 378, r_u8(object + 378) & 0xF3);
        if (r_u8(object + 375) < 3)
            w_u8(object + 378, r_u8(object + 378) & 0xCF);
    }
    return (uint32)sub_8003132C(object, 31);
}

uint32 sub_80015780(uint32 player, uint32 type, uint32 position)
{
    uint32 object = 0x80066D14 + player * 952, flags = r_u8(object + 378), points;
    sint32 total, pending;
    if (flags & 1)
        return flags & 1;
    switch (type)
    {
        case 6:
            w_u8(object + 378, flags | 0x40);
            w_u32(object + 316, r_u32(0x80064A30) + 500);
            break;
        case 7:
            sub_8002FEF4(player);
            w_u8(object + 378, r_u8(object + 378) | (flags & 0x40));
            break;
        case 8:
            points = (sub_8001B884(player) % 11) * 10 + 50;
            if (!r_u8(object + 945))
            {
                total = r_s16(object + 388);
                pending = r_s16(object + 390);
                if (r_s32(0x80064C14) < (sint32)player || pending < 0)
                    total += points;
                else
                {
                    total += pending;
                    w_u16(object + 390, (uint16)points);
                    w_u32(object + 384, r_u32(0x80064A30));
                }
                w_u16(object + 388, (uint16)total);
                w_u8(object + 392, 13);
            }
            break;
        case 9:
            sub_80015968(position);
            break;
    }
    if ((sint32)type < 6)
        return 8;
    if (type != 8)
    {
        uint16 total_message = r_u16(object + 388), pending_message = r_u16(object + 390);
        w_u16(object + 390, (uint16)(0 - type));
        w_u16(object + 388, total_message + pending_message);
        w_u32(object + 384, r_u32(0x80064A30));
    }
    return 0 - type;
}
