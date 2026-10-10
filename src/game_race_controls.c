#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))

static uint32 controls_div(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        xport_mips_break(7u);
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        xport_mips_break(6u);
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 controls_half(uint32 address)
{
    return (uint32)(sint32)(sint16)R16(address);
}

uint32 sub_80042754(void)
{
    uint32 index = R32(0x80065128u), size = R32(0x80065584u);
    uint32 result = R32(0x8006557Cu + index * 4u);
    R32(0x80065578u) = 0u;
    R32(0x80065574u) = size;
    R32(0x80065570u) = result;
    return result;
}

void game_8008FCA4(uint32 port)
{
    uint32 kind = R32(0x80065538u + port * 4u), half = port * 2u;
    uint32 output = 0x800BFFB0u + half, flags, delta, dead, bound, value, low, high;
    if (kind == 0x2000u || kind == 0x5000u || kind == 0x7000u)
    {
        delta = controls_half(0x800BE828u + half) - R32(0x80065508u + port * 4u);
        dead = controls_half(0x800BE830u + half);
        R16(output) = 0x8000u;
        if ((sint32)delta < (sint32)(0u - dead))
        {
            delta += dead;
            bound = 0u - controls_half(0x800BE82Cu + half);
            if ((sint32)delta < (sint32)bound)
                delta = bound;
        }
        else if ((sint32)dead < (sint32)delta)
        {
            delta -= dead;
            bound = controls_half(0x800BE82Cu + half);
            if ((sint32)bound < (sint32)delta)
                delta = bound;
        }
        else
            delta = 0u;
        value = controls_div(delta * 15u, controls_half(0x800BE82Cu + half));
        flags = R16(output) | ((value + 16u) << 3);
        R16(output) = (uint16)flags;
        value = controls_half(0x800BE838u + half) - controls_half(0x800BE834u + half);
        value = (uint32)((sint32)(value + (value >> 31)) >> 1);
        if ((sint32)value < (sint32)R32(0x80065518u + port * 4u))
            R16(output) = (uint16)(flags | 0x100u);
        else
        {
            value = R32(0x80065510u + port * 4u);
            low = controls_half(0x800BE83Cu + half);
            if ((sint32)value < (sint32)low)
                value = low;
            high = controls_half(0x800BE840u + half);
            if ((sint32)high < (sint32)value)
                value = high;
            value = controls_div((value - low) * 7u, high - low);
            R16(output) = (uint16)(flags | (value << 9));
        }
    }
    else if (kind == 0x4000u)
    {
        R16(output) = 0u;
        if (R16(0x80065520u + half) & R16(0x800BE814u + half))
            R16(output) = 8u;
        if (R16(0x80065520u + half) & R16(0x800BE818u + half))
            R16(output) |= 16u;
        if (R16(0x80065520u + half) & R16(0x800BE808u + half))
            R16(output) |= 32u;
        if (R16(0x80065520u + half) & R16(0x800BE80Cu + half))
            R16(output) |= 64u;
    }
    if (R16(0x80065520u + half) & R16(0x800BE810u + half))
        R16(output) |= 0x1000u;
    if (R16(0x80065520u + half) & R16(0x800BE81Cu + half))
        R16(output) |= 0x6000u;
}

void sub_80039270(uint32 flags, uint32 player)
{
    uint32 init = 0x8007AA44u + player * 2u;
    uint32 offset = player * 4u, signed_offset = (uint32)(sint32)(sint16)player * 4u;
    uint32 index_slot, count_slot, index, record, count, object;
    if (!R16(init))
    {
        R32(0x8007AA58u + signed_offset) = 1u;
        index_slot = 0x8007AA48u + signed_offset;
        index = R32(index_slot);
        R32(0x8007AA78u + index * 4u) = flags | 0x10000u;
        R32(0x8007AA78u + (R32(index_slot) + 1u) * 4u) = 0xFFFFu;
        R16(init) = 1u;
        return;
    }
    index_slot = 0x8007AA48u + offset;
    index = R32(index_slot);
    if (index == controls_half(0x8007AA3Cu) - 1u)
        return;
    if ((uint32)R16(0x8007AA78u + index * 4u) == flags)
    {
        if (R32(0x8007AA58u) != 15u)
        {
            count_slot = 0x8007AA58u + offset;
            count = R32(count_slot) + 1u;
            R32(count_slot) = count;
            index = R32(0x8007AA48u + signed_offset);
            record = 0x8007AA78u + index * 4u;
            R32(record) = (uint32)R16(record) | (count << 16);
            goto save_position;
        }
        R32(index_slot) = index + 1u;
        R32(0x8007AA58u + signed_offset) = 1u;
    }
    else
    {
        R32(index_slot) = index + 1u;
        R32(0x8007AA58u + signed_offset) = 1u;
    }
    index_slot = 0x8007AA48u + signed_offset;
    index = R32(index_slot);
    R32(0x8007AA78u + index * 4u) = flags | 0x10000u;
    R32(0x8007AA78u + (R32(index_slot) + 1u) * 4u) = 0xFFFFu;
save_position:
    object = 0x80066D14u + player * 952u;
    R32(0x8007AA60u + offset) = R32(object + 100u);
    R32(0x8007AA68u + offset) = R32(object + 104u);
    R32(0x8007AA70u + offset) = R32(object + 108u);
}

uint32 sub_80039484(uint32 player)
{
    uint32 init = 0x8007AA44u + player * 2u, offset = player * 4u;
    uint32 index_slot = 0x8007AA48u + offset, count_slot = 0x8007AA50u + offset;
    uint32 record, flags, object;
    if (!R16(init))
    {
        R32(count_slot) = R16(0x8007AA7Au + R32(index_slot) * 4u);
        R16(init) += 1u;
    }
    if (!R32(count_slot))
    {
        R32(index_slot) += 1u;
        R32(count_slot) = R16(0x8007AA7Au + R32(index_slot) * 4u);
    }
    record = R32(0x8007AA78u + R32(index_slot) * 4u);
    flags = record & 0xFFFFu;
    if (record == 0xFFFFu)
        R32(0x80064A44u) = 1u;
    else
        R32(count_slot) -= 1u;
    if (!R32(count_slot) && R32(0x8007AA78u + (R32(index_slot) + 1u) * 4u) == 0xFFFFu)
    {
        object = 0x80066D14u + player * 952u;
        if (R32(0x8007AA60u + offset) == R32(object + 100u) && R32(0x8007AA68u + offset) == R32(object + 104u) && R32(0x8007AA70u + offset) == R32(object + 108u))
        {
            R32(0x80064A44u) = 1u;
            fputs((const char *)psx_addr(0x8001201Cu, 1u), stdout);
        }
        else
            R32(0x80064A44u) = 1u;
    }
    return (uint32)(sint32)(sint16)flags;
}

uint32 sub_8001ED2C(uint32 sound, uint32 voice, uint32 mode, sint32 frequency, uint32 position, uint32 velocity);
uint32 sub_80043450(uint32 x, uint32 z);

uint32 sub_8001CF78(uint32 player, uint32 position, uint32 velocity, uint32 flags)
{
    return sub_8001ED2C(41u, player + 6u, (flags != 0u) << 12, (sint32)(player * 400u + 3896u), position, velocity);
}

static uint32 controls_shift(uint32 value, uint32 bits)
{
    if ((sint32)value < 0)
        value += (1u << bits) - 1u;
    return (uint32)((sint32)value >> bits);
}

static uint32 controls_yaw_damping(uint32 object)
{
    uint32 coefficient = controls_half(R32(object + 4u) + 120u);
    uint32 first = controls_shift(R32(object + 900u) * coefficient, 11u);
    uint32 second = controls_shift(coefficient * 1500u, 8u);
    if ((sint32)first < (sint32)second)
        second = first;
    return controls_shift(second * R32(0x800651C4u), 8u);
}

void game_80090040(uint32 player, uint32 port)
{
    uint32 flags = R16(0x800BFFB0u + port * 2u), object, inputs, data;
    uint32 steering_constant, turn_constant, speed, longitudinal, sensitivity, turn_limit;
    uint32 angle, product, positive, negative, acceleration, damping, steering, old_flags;
    if (R8(0x80059A28u + player))
        return;
    object = 0x80066D14u + player * 952u;
    inputs = object + 8u;
    if (!R32(0x80065110u))
        sub_80039270(flags, player);
    else
        flags = sub_80039484(player);
    data = R32(object + 4u);
    steering_constant = controls_half(data + 126u);
    turn_constant = controls_half(data + 128u);
    speed = (uint32)((sint32)R32(object + 888u) >> 8);
    if ((sint32)speed < 0)
        speed = 0u - speed;
    longitudinal = (uint32)((sint32)R32(object + 896u) >> 8);
    sensitivity = R8(object + 938u) ? 0u : controls_div(speed << 6, longitudinal * longitudinal + 64u);
    sensitivity = controls_div((steering_constant * 3u) << 12, steering_constant + sensitivity);
    turn_limit = controls_div((turn_constant * 3u) << 15, turn_constant + (R8(object + 938u) ? 0u : speed));
    angle = sub_80043450((uint32)((sint32)R32(object + 112u) >> 8), (uint32)((sint32)R32(object + 120u) >> 8));
    angle = ((angle - (uint32)((sint32)R32(object + 128u) >> 8) + 2048u) & 4095u) - 2048u;
    product = R32(object + 900u);
    if ((sint32)product < 2048 && !R8(inputs + 11u) && !R8(inputs + 12u))
        angle = controls_shift(angle * product, 11u);
    product = angle << 8;
    positive = (sint32)product < (sint32)turn_limit ? turn_limit : product;
    if ((sint32)positive > 98304)
        positive = 98304u;
    negative = (sint32)(0u - turn_limit) < (sint32)product ? 0u - turn_limit : product;
    if ((sint32)negative < -98304)
        negative = (uint32)-98304;
    acceleration = controls_shift(sensitivity * R32(0x800651C4u), 8u);
    if (flags & 0x8000u)
    {
        steering = ((flags & 248u) >> 3) - 16u;
        product = (sint32)steering > 0 ? steering * positive : (0u - steering) * negative;
        R32(inputs) = (uint32)((sint32)product / 15);
        if ((sint32)R32(object + 896u) > 0)
        {
            damping = controls_yaw_damping(object);
            if ((sint32)steering < -2 && (sint32)R32(object + 140u) > 0)
                R32(object + 140u) -= damping;
            if ((sint32)steering >= 3 && (sint32)R32(object + 140u) < 0)
                R32(object + 140u) += damping;
        }
        if (flags & 0x1000u)
            goto reverse;
        R8(inputs + 12u) = 0u;
        if (flags & 0x100u)
            goto brake;
        R8(inputs + 13u) = 1u;
        R8(inputs + 11u) = 0u;
        product = ((flags & 0xE00u) >> 9) * controls_half(0x800BE844u + port * 2u);
        R16(inputs + 8u) = (uint16)((sint32)product / 7);
        goto sound;
    }
    if (flags & 16u)
    {
        product = R32(inputs);
        R32(inputs) = product - ((sint32)product > 0 ? acceleration << 2 : acceleration);
        if ((sint32)R32(inputs) < (sint32)negative)
            R32(inputs) = negative;
    }
    if (flags & 8u)
    {
        product = R32(inputs);
        R32(inputs) = product + ((sint32)product < 0 ? acceleration << 2 : acceleration);
        if ((sint32)R32(inputs) > (sint32)positive)
            R32(inputs) = positive;
    }
    if (!(flags & 24u))
    {
        if ((sint32)(sint16)R16(inputs + 6u) > 0)
            R16(inputs + 6u) -= 64u;
        product = R32(inputs);
        if ((sint32)product <= (sint32)(0u - (acceleration << 3)))
            R32(inputs) = product + (acceleration << 3);
        else if ((sint32)product >= (sint32)(acceleration << 3))
            R32(inputs) = product - (acceleration << 3);
        else
            R32(inputs) = 0u;
    }
    if ((sint32)R32(object + 900u) > 0)
    {
        damping = controls_yaw_damping(object);
        if ((flags & 16u) && (sint32)R32(object + 140u) > 0)
            R32(object + 140u) -= damping;
        if ((flags & 8u) && (sint32)R32(object + 140u) < 0)
            R32(object + 140u) += damping;
    }
    if (flags & 0x1000u)
    {
    reverse:
        R16(inputs + 8u) = (uint16)-256;
        R8(inputs + 12u) = 1u;
        R8(inputs + 11u) = 1u;
        goto sound;
    }
    R8(inputs + 12u) = 0u;
    if (flags & 64u)
    {
    brake:
        if (R8(inputs + 13u) == 1u && (sint32)speed < 10 && !R8(object + 938u))
            R8(inputs + 13u) = 0u;
        R16(inputs + 8u) = (uint16)-256;
        R8(inputs + 11u) = R8(inputs + 13u);
    }
    else
    {
        R8(inputs + 13u) = 1u;
        R8(inputs + 11u) = 0u;
        R16(inputs + 8u) = flags & 32u ? R16(0x800BE844u + port * 2u) : 0u;
    }
sound:
    old_flags = R32(0x800BE854u + player * 4u);
    flags &= 0x6000u;
    if (flags != old_flags)
    {
        sub_8001CF78(player, object + 100u, object + 112u, flags);
        R32(0x800BE854u + player * 4u) = flags;
    }
}
