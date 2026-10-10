#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
extern sint32 draft_agent3_missing_800437EC(uint32);
extern sint32 draft_agent3_missing_8004388C(uint32);
extern uint32 sub_8001ED2C(uint32, uint32, uint32, sint32, uint32, uint32);
extern sint32 sub_80030110(uint32, uint32, sint32);
extern sint32 sub_8001532C(uint32, uint32);
extern sint32 sub_800153DC(uint32, sint32);
extern void game_800AF468(uint32, uint32);
extern uint32 sub_8001B884(uint32);

static sint32 pair_add(sint32 a, sint32 b)
{
    return (sint32)((uint32)a + (uint32)b);
}

static sint32 pair_sub(sint32 a, sint32 b)
{
    return (sint32)((uint32)a - (uint32)b);
}

static sint32 pair_mul(sint32 a, sint32 b)
{
    return (sint32)((uint32)a * (uint32)b);
}

static sint32 pair_neg(sint32 a)
{
    return (sint32)(0u - (uint32)a);
}

static sint32 pair_abs(sint32 a)
{
    return a < 0 ? pair_neg(a) : a;
}

static sint32 pair_shift(sint32 a, uint32 n)
{
    return pair_add(a, a < 0 ? (sint32)((1u << n) - 1u) : 0) >> n;
}

static sint32 pair_product(sint32 a, sint32 b, uint32 n)
{
    return pair_shift(pair_mul(a, b), n);
}

static sint32 pair_high(sint32 a, sint32 b)
{
    return (sint32)(((int64_t)a * b) >> 32);
}

static sint32 pair_magic(sint32 a)
{
    return pair_sub(pair_high(a, 0x0C907DA5) >> 5, a >> 31);
}

static sint32 pair_div(sint32 a, sint32 b)
{
    if (!b)
        xport_mips_break(7u);
    if (a == INT_MIN && b == -1)
        xport_mips_break(6u);
    return a / b;
}

static uint32 pair_mod(uint32 a, uint32 b)
{
    if (!b)
        xport_mips_break(7u);
    return a % b;
}

static sint32 pair_damage(uint32 owner, uint32 zone, sint32 force)
{
    return sub_80030110(owner, zone, force);
}

static void pair_effect(uint32 object)
{
    game_800AF468(object, 0u);
}

static uint32 pair_random(uint32 owner)
{
    return sub_8001B884(owner);
}

static sint32 pair_random_remainder(sint32 value, sint32 magic, uint32 shift, sint32 divisor)
{
    sint32 quotient = pair_sub(pair_high(value, magic) >> shift, value >> 31);
    return pair_sub(value, pair_mul(quotient, divisor));
}

static void pair_sound(uint32 object, uint32 sound, sint32 volume)
{
    sint32 random = (sint32)randPSX();
    sint32 frequency = pair_add(pair_random_remainder(random, 0x51EB851F, 7u, 400), 3800);
    sub_8001ED2C(sound, ~0u, (uint32)volume, frequency, object + 100u, 0u);
}

static void pair_abort_sound(uint32 second)
{
    uint32 owner = r_u8(second + 937u);
    if ((sint32)owner < r_s32(0x80064C14u))
        game_8009082C(owner, 1024u);
    pair_sound(second, (uint32)pair_add(pair_random_remainder((sint32)randPSX(), 0x66666667, 1u, 5), 4), 4096);
}

static uint32 pair_zone(sint32 x, sint32 z, sint32 width, sint32 rear, uint32 axis)
{
    sint32 value;
    if (axis)
    {
        value = pair_mul(x, 6);
        if (width < value)
            return z > 0 ? 2u : 6u;
        if (pair_neg(width) < value)
            return z > 0 ? 1u : 7u;
        return z < 1 ? 8u : 0u;
    }
    value = pair_mul(z, 10);
    if (rear < value)
    {
        if (pair_mul(rear, 3) < value)
            return x > 0 ? 2u : 0u;
        return x > 0 ? 3u : 11u;
    }
    if (value < pair_neg(rear))
    {
        if (value < pair_mul(rear, -3))
            return x > 0 ? 6u : 8u;
        return x > 0 ? 5u : 9u;
    }
    return x > 0 ? 10u : 4u;
}

static void pair_award(uint32 object, sint32 value)
{
    if (value < 0)
        (void)sub_8001532C(r_u8(object + 937u), value == -1 ? 11u : 10u);
    else
        (void)sub_800153DC(r_u8(object + 937u), value >> 2);
}

static void pair_spin(uint32 object, sint32 magnitude, sint32 mass, sint32 total_mass)
{
    sint32 force = pair_div((sint32)((uint32)pair_mul(magnitude, mass) << 1), total_mass), limit, offset;
    uint32 owner;
    if (force < 30001)
        return;
    limit = pair_sub(pair_high(force, 0x55555556), force >> 31);
    if (limit > 32767)
        limit = 32767;
    owner = r_u8(object + 937u);
    offset = pair_sub((sint32)pair_mod(pair_random(owner), (uint32)limit), pair_shift(limit, 1u));
    w_u32(object + 136u, (uint32)pair_add(r_s32(object + 136u), offset));
    offset = pair_sub((sint32)pair_mod(pair_random(owner), (uint32)limit), pair_shift(limit, 1u));
    w_u32(object + 140u, (uint32)pair_add(r_s32(object + 140u), offset));
    offset = pair_sub((sint32)pair_mod(pair_random(owner), (uint32)limit << 1), limit);
    w_u32(object + 116u, (uint32)pair_sub(pair_high(force, 0x66666667) >> 1, force >> 31));
    w_u32(object + 144u, (uint32)pair_add(r_s32(object + 144u), offset));
}

/* Full original GAME 92450 with native packed contact input */
void game_80092450(uint32 first, uint32 second, const sint16 contact[4], sint32 angle, sint32 fraction, uint32 corner)
{
    uint32 objects[2], axis, normal, index, component, model, classification, winner, owner_count;
    sint32 cosine, sine, velocity[2][2], angular[2], point[2][2], mass[2], extent_width, extent_rear;
    sint32 inertia, coefficient, impulse, magnitude, denominator, value, other, move_x, move_z;
    sint32 damage[2], squares[2], vx[2], vz[2], dot, category, low, high, rotation, scale, depth;
    objects[0] = first;
    objects[1] = second;
    angular[0] = r_s32(first + 140u);
    angular[1] = r_s32(second + 140u);
    cosine = draft_agent3_missing_800437EC((uint32)angle);
    sine = draft_agent3_missing_8004388C((uint32)angle);
    for (index = 0u; index < 2u; ++index)
    {
        value = r_s32(objects[index] + 112u);
        other = r_s32(objects[index] + 120u);
        velocity[index][0] = pair_shift(pair_sub(pair_mul(value, cosine), pair_mul(other, sine)), 12u);
        velocity[index][1] = pair_shift(pair_add(pair_mul(value, sine), pair_mul(other, cosine)), 12u);
    }
    point[0][0] = contact[0];
    point[0][1] = contact[1];
    point[1][0] = contact[2];
    point[1][1] = contact[3];
    model = r_u32(first);
    extent_width = r_s16(model + 8u);
    extent_rear = pair_neg(r_s16(model + 20u));
    mass[0] = r_u8(first + 949u);
    mass[1] = r_u8(second + 949u);
    value = pair_abs(pair_sub(extent_width, pair_abs(point[0][0])));
    other = point[0][1] > 0 ? pair_abs(pair_sub(r_s16(model + 4u), point[0][1])) : pair_abs(pair_add(extent_rear, point[0][1]));
    axis = (uint32)(value >= other);
    normal = axis ? 1u : 0u;
    value = pair_sub(point[0][normal], point[1][normal]);
    if (!axis)
    {
        move_x = pair_shift(value > 0 ? cosine : pair_neg(cosine), 1u);
        move_z = pair_shift(value > 0 ? pair_neg(sine) : sine, 1u);
    }
    else
    {
        move_x = pair_shift(value > 0 ? sine : pair_neg(sine), 1u);
        move_z = pair_shift(value > 0 ? cosine : pair_neg(cosine), 1u);
    }
    w_u32(first + 100u, (uint32)pair_sub(r_s32(first + 100u), move_x));
    w_u32(first + 108u, (uint32)pair_sub(r_s32(first + 108u), move_z));
    w_u32(second + 100u, (uint32)pair_add(r_s32(second + 100u), move_x));
    w_u32(second + 108u, (uint32)pair_add(r_s32(second + 108u), move_z));
    inertia = r_s32(0x800BE96Cu);
    coefficient = pair_add(pair_mul(pair_add(inertia, pair_mul(point[1][1u - normal], point[1][1u - normal])), mass[0]), pair_mul(pair_add(inertia, pair_mul(point[0][1u - normal], point[0][1u - normal])), mass[1]));
    coefficient = pair_div((sint32)((uint32)pair_mul(pair_shift(inertia, 8u), 17) << 8), pair_shift(coefficient, 8u));
    value = pair_sub(pair_add(velocity[1][normal], pair_magic(pair_mul(angular[1], point[1][1u - normal]))), velocity[0][normal]);
    value = pair_sub(value, pair_magic(pair_mul(angular[0], point[0][1u - normal])));
    impulse = pair_product(coefficient, value, 12u);
    if ((impulse ^ pair_sub(point[1][normal], point[0][normal])) < 0)
    {
        pair_abort_sound(second);
        return;
    }
    value = pair_div(pair_mul(pair_mul(impulse, point[0][1u - normal]), mass[0]), pair_magic(inertia));
    other = pair_div(pair_mul(pair_mul(impulse, point[1][1u - normal]), mass[1]), pair_magic(inertia));
    angular[0] = axis ? pair_sub(angular[0], value) : pair_add(angular[0], value);
    angular[1] = axis ? pair_add(angular[1], other) : pair_sub(angular[1], other);
    velocity[0][normal] = pair_add(velocity[0][normal], pair_mul(impulse, mass[0]));
    velocity[1][normal] = pair_sub(velocity[1][normal], pair_mul(impulse, mass[1]));
    magnitude = pair_abs(pair_mul(impulse, pair_add(mass[0], mass[1])));
    if (magnitude > 0 && (sint32)corner < 256)
    {
        scale = magnitude >> 8;
        damage[0] = pair_damage(r_u8(first + 937u), pair_zone(point[0][0], point[0][1], extent_width, extent_rear, axis), scale);
        damage[1] = pair_damage(r_u8(second + 937u), r_u8(0x800BE988u + corner), scale);
        if (!r_u8(first + 306u) && !r_u8(second + 306u))
        {
            for (index = 0u; index < 2u; ++index)
            {
                vx[index] = r_s32(objects[index] + 112u) >> 8;
                vz[index] = r_s32(objects[index] + 120u) >> 8;
                squares[index] = pair_add(pair_mul(vx[index], vx[index]), pair_mul(vz[index], vz[index]));
            }
            if (squares[0] >= 1025 || squares[1] >= 1025)
            {
                winner = (uint32)(squares[0] < squares[1]);
                if (squares[0] < 1025 || squares[1] < 1025)
                {
                    index = winner ? 0u : 1u;
                    squares[index] = 900;
                    rotation = r_s32(objects[index] + 128u) >> 8;
                    vx[index] = pair_mul(draft_agent3_missing_8004388C((uint32)rotation), 30) >> 12;
                    vz[index] = pair_mul(draft_agent3_missing_800437EC((uint32)rotation), 30) >> 12;
                }
                dot = pair_add(pair_mul(vx[0], vx[1]), pair_mul(vz[0], vz[1])) >> 4;
                value = pair_mul(squares[1] >> 4, squares[0] >> 4);
                other = pair_mul(dot, (sint32)((uint32)dot << 1));
                if ((uint32)value < (uint32)other)
                {
                    if (axis)
                        classification = dot > 0 ? 3u : 4u;
                    else
                    {
                        classification = 2u;
                        low = pair_sub(r_s32(second + 100u), r_s32(first + 100u)) >> 12;
                        high = pair_sub(r_s32(second + 108u), r_s32(first + 108u)) >> 12;
                        value = pair_add(pair_mul(vx[0], low), pair_mul(vz[0], high));
                        other = pair_neg(pair_add(pair_mul(vx[1], low), pair_mul(vz[1], high)));
                        winner = (uint32)(value < other);
                    }
                }
                else
                {
                    winner = 1u - axis;
                    index = winner ? 0u : 1u;
                    rotation = r_s32(objects[index] + 128u) >> 8;
                    low = draft_agent3_missing_8004388C((uint32)rotation);
                    high = draft_agent3_missing_800437EC((uint32)rotation);
                    value = pair_add(pair_mul(point[index][1], low), pair_mul(point[index][0], high)) >> 12;
                    other = pair_sub(pair_mul(point[index][1], high), pair_mul(point[index][0], low)) >> 12;
                    depth = (sint16)r_u16(r_u32(objects[index]) + 128u) >> 4;
                    denominator = pair_mul(pair_mul(squares[1u - index] >> 2, depth), depth);
                    value = pair_sub(pair_mul(value, vz[1u - index]), pair_mul(other, vx[1u - index])) >> 4;
                    classification = (uint32)((uint32)denominator < (uint32)pair_mul(value, value));
                }
                for (index = 0u; index < 2u; ++index)
                {
                    uint32 object = objects[index], opponent = objects[1u - index];
                    w_u32(object + 268u, classification);
                    w_u32(object + 272u, 0u);
                    w_u16(object + 300u, 1u);
                    w_u8(object + 308u, (uint8)(index == winner));
                    w_u8(object + 309u, 0u);
                    for (component = 0u; component < 3u; ++component)
                        w_u32(object + 276u + component * 4u, r_u32(object + 100u + component * 4u));
                    w_u32(object + 288u, r_u32(0x80064A30u));
                    w_u32(object + 292u, r_u32(object + 128u));
                    w_u8(object + 307u, r_u8(opponent + 937u));
                    if (index == winner && damage[1u - index])
                    {
                        w_u8(opponent + 306u, 1u);
                        pair_award(object, damage[1u - index]);
                    }
                    else
                        w_u8(opponent + 306u, 0u);
                    w_u16(object + 302u, (uint16)(r_u16(object + 302u) + r_u8(object + 308u)));
                    w_u16(object + 304u, (uint16)(r_u16(object + 304u) + r_u8(object + 306u)));
                }
            }
        }
    }
    scale = pair_sub(256, fraction);
    for (index = 0u; index < 2u; ++index)
        for (component = 0u; component < 2u; ++component)
        {
            uint32 offset = component ? 108u : 100u, velocity_offset = component ? 120u : 112u;
            value = pair_product(pair_product(scale, r_s32(objects[index] + velocity_offset), 8u), r_s32(0x800651C4u), 8u);
            w_u32(objects[index] + offset, (uint32)pair_sub(r_s32(objects[index] + offset), value));
        }
    for (index = 0u; index < 2u; ++index)
    {
        value = pair_product(pair_product(scale, r_s32(objects[index] + 140u), 8u), r_s32(0x800651C4u), 8u);
        w_u32(objects[index] + 128u, (uint32)pair_sub(r_s32(objects[index] + 128u), value));
    }
    w_u32(first + 140u, (uint32)angular[0]);
    w_u32(second + 140u, (uint32)angular[1]);
    for (index = 0u; index < 2u; ++index)
    {
        value = pair_shift(pair_add(pair_mul(velocity[index][0], cosine), pair_mul(velocity[index][1], sine)), 12u);
        other = pair_shift(pair_add(pair_mul(pair_neg(velocity[index][0]), sine), pair_mul(velocity[index][1], cosine)), 12u);
        w_u32(objects[index] + 112u, (uint32)value);
        w_u32(objects[index] + 120u, (uint32)other);
    }
    for (index = 0u; index < 2u; ++index)
        for (component = 0u; component < 2u; ++component)
        {
            uint32 offset = component ? 108u : 100u, velocity_offset = component ? 120u : 112u;
            value = pair_product(pair_product(scale, r_s32(objects[index] + velocity_offset), 8u), r_s32(0x800651C4u), 8u);
            w_u32(objects[index] + offset, (uint32)pair_add(r_s32(objects[index] + offset), value));
        }
    for (index = 0u; index < 2u; ++index)
    {
        value = pair_product(pair_product(scale, r_s32(objects[index] + 140u), 8u), r_s32(0x800651C4u), 8u);
        w_u32(objects[index] + 128u, (uint32)pair_add(r_s32(objects[index] + 128u), value));
    }
    w_u8(first + 946u, r_u8(second + 937u));
    w_u8(second + 946u, r_u8(first + 937u));
    value = pair_abs(pair_shift(magnitude, 10u));
    if (value >= 256)
        value = 255;
    w_u8(second + 947u, (uint8)value);
    w_u8(second + 947u, r_u8(second + 947u));
    w_u8(first + 947u, r_u8(second + 947u));
    game_8009A8E8(first);
    game_8009A8E8(second);
    owner_count = r_u32(0x80064C14u);
    if (owner_count && r_u8(first + 937u) < 2u && r_u8(second + 937u) < 2u)
    {
        if (magnitude > 12800)
        {
            if (magnitude > 25600)
            {
                value = (sint32)((uint32)magnitude << 12);
                scale = pair_sub(pair_high(value, 0x51EB851F) >> 14, value >> 31);
                scale = pair_add(scale, scale >> 1);
                if (scale > 4096)
                    scale = 4096;
                game_8009082C(r_u8(first + 937u), 1024u);
                game_8009082C(r_u8(second + 937u), 1024u);
                category = pair_add(pair_random_remainder((sint32)randPSX(), 0x66666667, 1u, 5), 4);
            }
            else
            {
                scale = 4096;
                game_8009082C(r_u8(first + 937u), 2048u);
                game_8009082C(r_u8(second + 937u), 2048u);
                category = (sint32)(randPSX() & 1u) + 2;
            }
            pair_sound(first, (uint32)category, scale);
            pair_effect(first);
            pair_effect(second);
        }
    }
    else
    {
        index = owner_count < r_u8(first + 937u) ? 1u : 0u;
        if (owner_count >= r_u8(objects[index] + 937u) && magnitude > 12800)
        {
            if (magnitude > 25600)
            {
                value = (sint32)((uint32)magnitude << 12);
                scale = pair_sub(pair_high(value, 0x51EB851F) >> 14, value >> 31);
                if (scale > 4096)
                    scale = 4096;
                game_8009082C(r_u8(objects[index] + 937u), 1024u);
                category = pair_add(pair_random_remainder((sint32)randPSX(), 0x66666667, 1u, 5), 4);
            }
            else
            {
                scale = 4096;
                game_8009082C(r_u8(objects[index] + 937u), 2048u);
                category = (sint32)(randPSX() & 1u) + 2;
            }
            pair_sound(objects[index], (uint32)category, scale);
            pair_effect(objects[index]);
        }
    }
    pair_spin(first, magnitude, mass[0], pair_add(mass[0], mass[1]));
    pair_spin(second, magnitude, mass[1], pair_add(mass[0], mass[1]));
}
