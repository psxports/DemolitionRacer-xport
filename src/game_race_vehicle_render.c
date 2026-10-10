#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

extern uint32 game_800A8848(uint32 template_address, const VECTOR *position, const SVECTOR *rotation, const SVECTOR *scale, uint32 flags);

static uint32 vehicle_random(uint32 car)
{
    uint32 address = 0x80067050u + car * 952u;
    uint32 value = r_u32(address) * 0x01DF5E0Du + 1u;
    w_u32(address, value);
    return (value << 24) | (value >> 8);
}

void game_800AF8C4(uint32 object, uint32 effect, uint32 mask)
{
    uint32 template_address = r_u32(0x800BF758u + effect * 4u), i;
    SVECTOR velocity = {0}, acceleration = {0};
    sint32 components[3], component;
    mask &= r_u8(object + 942u) ^ 15u;
    for (i = 0; i < 3; ++i)
    {
        component = (sint32)r_u32(object + 112u + i * 4u) >> 4;
        components[i] = (sint32)((uint32)component * 3u) / 4;
    }
    velocity.vx = (sint16)components[0];
    velocity.vy = (sint16)components[1];
    velocity.vz = (sint16)components[2];
    acceleration.vx = (sint16)(-(sint32)velocity.vx / 32);
    acceleration.vy = (sint16)(-(sint32)velocity.vy / 32);
    acceleration.vz = (sint16)(-(sint32)velocity.vz / 32);
    velocity.vy = (sint16)((uint32)components[1] + (uint32)((sint32)(sint8)r_u8(template_address + 28u) * 4));
    if (r_u32(0x80064A30u) & 3u)
        return;
    for (i = 0; i < 4; ++i, mask >>= 1)
    {
        if (mask & 1u)
        {
            uint32 source = object + 576u + i * 12u;
            VECTOR position = {0};
            position.vx = (sint32)r_u32(source);
            position.vy = (sint32)(r_u32(source + 4u) + ((uint32)r_u8(template_address + 26u) << 10));
            position.vz = (sint32)r_u32(source + 8u);
            game_800A8848(template_address, &position, &velocity, &acceleration, 32u);
        }
    }
}

static void vehicle_shake(uint32 object)
{
    sint32 scale = (sint16)r_u16(object + 904u) / 4;
    uint32 i;
    for (i = 0; i < 4; ++i)
    {
        sint32 offset = (sint32)(vehicle_random(r_u8(object + 937u)) & 15u) - 8;
        uint32 address = object + 856u + i * 4u;
        w_u32(address, r_u32(address) + (uint32)scale * (uint32)offset);
    }
}

static void vehicle_axle_effects(uint32 object)
{
    if ((sint32)r_u32(object + 900u) > 30720)
        game_800AF8C4(object, 2u, 3u);
    if ((sint32)r_u32(object + 896u) > 30720)
        game_800AF8C4(object, 2u, 12u);
}

void game_80090C50(uint32 object, uint32 view)
{
    uint32 target;
    if (r_u8(object + 940u))
        return;
    target = r_u32(0x800BE8E0u + view * 4u);
    switch (target)
    {
        /* These original callbacks consist of JR RA and NOP */
        case 0x80090C90u:
        case 0x80090ED8u:
            return;
        case 0x80090C98u:
        case 0x80090F7Cu:
            vehicle_shake(object);
            vehicle_axle_effects(object);
            return;
        case 0x80090E78u:
            vehicle_axle_effects(object);
            return;
        case 0x80090B60u:
            vehicle_shake(object);
            if ((sint16)r_u16(object + 904u) > 200)
                game_800AF8C4(object, 1u, 15u);
            return;
        case 0x80090DA8u:
            if ((sint16)r_u16(object + 904u) > 200)
                game_800AF8C4(object, 0u, 15u);
            return;
        case 0x80090DDCu:
            if ((sint16)r_u16(object + 904u) > 200)
                game_800AF8C4(object, 4u, 15u);
            return;
        case 0x80090E10u:
            if ((sint16)r_u16(object + 904u) > 200)
                game_800AF8C4(object, 5u, 15u);
            return;
        case 0x80090E44u:
            if ((sint16)r_u16(object + 904u) > 200)
                game_800AF8C4(object, 3u, 15u);
            return;
        case 0x80090EE0u:
            if ((sint16)r_u16(object + 904u) > 200)
                game_800AF8C4(object, 8u, 15u);
            return;
        default:
            fprintf(stderr, "Unsupported vehicle callback %08X object=%08X view=%u\n", target, object, view);
            abort();
    }
}

extern uint32 sub_8003AAC8(uint32 player, uint32 first, uint32 second);

static sint32 vehicle_divide(sint32 numerator, sint32 denominator)
{
    if (!denominator || (numerator == INT_MIN && denominator == -1))
        abort();
    return numerator / denominator;
}

static sint32 vehicle_product(sint32 first, sint32 second)
{
    return (sint32)((uint32)first * (uint32)second);
}

void game_80090858(uint32 player, uint32 port)
{
    uint32 object = 0x80066D14u + port * 952u;
    uint32 first = 0u;
    sint32 second = 0;
    uint32 timer_address = 0x800BE85Cu + port * 2u;
    if (r_u8(object + 945u))
    {
        sub_8003AAC8(player, 0u, 0u);
        return;
    }
    if (r_u8(object + 378u) & 1u)
    {
        sint32 timer = (sint16)r_u16(timer_address);
        if (timer >= 0)
        {
            ++timer;
            second = randPSX() & 255u;
            first = randPSX() & 1u;
            if (timer >= 6 && ((~r_u8(object + 942u)) & 15u))
                timer = -1;
        }
        w_u16(timer_address, (uint16)timer);
    }
    else
    {
        uint32 accumulator_address = 0x800BFFA8u + port * 4u;
        sint32 accumulator, combined;
        w_u16(timer_address, 0u);
        combined = (sint32)(r_u32(object + 888u) + r_u32(object + 892u));
        if (r_u8(object + 942u) != 15u && combined > 0 && (sint16)r_u16(object + 16u))
        {
            sint32 ratio = vehicle_divide(vehicle_product((sint16)r_u16(object + 320u), 70), (sint16)r_u16(object + 322u));
            sint32 scale, limit;
            second = vehicle_product(ratio, combined >> 11) / 128;
            if (second >= 71)
                second = 70;
            scale = (sint32)r_u32(0x800BE850u);
            if (scale)
                second = vehicle_product(scale, second) / 256;
            limit = (sint32)r_u32(0x800BE84Cu);
            if (limit && limit < second)
                second = limit;
        }
        accumulator = (sint32)r_u32(accumulator_address);
        if (accumulator)
        {
            sint32 ratio = vehicle_divide((sint32)r_u32(object + 888u), accumulator);
            first = ratio < 0 || ratio >= 256 ? 255u : (uint32)ratio;
            first |= 1u;
        }
        if (!accumulator || accumulator >= 8193)
            w_u32(accumulator_address, 0u);
        else
        {
            sint32 ratio = vehicle_divide((sint32)((uint32)(sint32)(sint16)r_u16(object + 320u) << 9), (sint16)r_u16(object + 322u));
            w_u32(accumulator_address, (uint32)accumulator + 1024u - (uint32)ratio);
        }
    }
    sub_8003AAC8(player, first, (uint32)second);
}
