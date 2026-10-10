#include "draft_signatures.h"
#include <stdlib.h>
#include <limits.h>
extern void dr_race_normalize(VECTOR *, SVECTOR *);

static sint32 slow_sub(sint32 a, sint32 b)
{
    return (sint32)((uint32)a - (uint32)b);
}

static sint32 slow_mul(sint32 a, sint32 b)
{
    return (sint32)((uint32)a * (uint32)b);
}

static sint32 slow_div(sint32 a, sint32 b)
{
    if (!b || (a == INT_MIN && b == -1))
        abort();
    return a / b;
}

static sint16 slow_up(uint32 object, SVECTOR *orientation)
{
    MATRIX matrix = {0};
    SVECTOR result = {0};
    uint32 row, column;
    for (row = 0; row < 3u; ++row)
        for (column = 0; column < 3u; ++column)
            matrix.m[row][column] = (sint16)r_u16(object + 148u + 2u * (row * 3u + column));
    ApplyMatrixSV(&matrix, orientation, &result);
    return result.vy;
}

void game_80095054(uint32 first_car, uint32 second_car)
{
    uint32 lower = 0x80066D14u + first_car * 952u;
    uint32 upper = 0x80066D14u + second_car * 952u;
    uint32 model = r_u32(lower), temporary, i;
    VECTOR delta = {0};
    SVECTOR orientation, normal = {0};
    sint32 upright, radius, limit, ratio, scale, approach;
    sint32 components[3], impulses[3];
    orientation.vx = (sint16)r_u16(0x8008B480u);
    orientation.vy = (sint16)r_u16(0x8008B482u);
    orientation.vz = (sint16)r_u16(0x8008B484u);
    orientation.pad = (sint16)r_u16(0x8008B486u);
    upright = slow_sub(r_s32(upper + 104u), r_s32(lower + 104u)) >> 8;
    delta.vy = upright;
    if (upright < 0)
    {
        temporary = lower;
        lower = upper;
        upper = temporary;
        delta.vy = (sint32)(0u - (uint32)upright);
    }
    delta.vx = slow_sub(r_s32(upper + 100u), r_s32(lower + 100u)) >> 8;
    delta.vz = slow_sub(r_s32(upper + 108u), r_s32(lower + 108u)) >> 8;
    radius = (sint32)((uint32)(sint32)(sint16)r_u16(model + 128u) << 1u);
    limit = (sint32)((uint32)slow_mul(radius, radius) * 9u) >> 4;
    if ((sint32)(dr_pair_squared_distance(&delta) << 12u) >= limit)
        return;
    upright = slow_up(upper, &orientation);
    if (upright > 0)
    {
        sint32 other_up = slow_up(lower, &orientation);
        if (other_up < upright)
            upright = other_up;
        if (upright > 0)
        {
            ratio = slow_div((sint32)((uint32)(sint32)(sint16)r_u16(model + 42u) << 12u), (sint16)r_u16(model + 44u));
            if (ratio < 0)
                ratio = (sint32)(0u - (uint32)ratio);
            if (ratio < 4096)
            {
                uint32 root = dr_pair_sqrt_q12(slow_sub(4096, slow_mul(upright, upright) >> 12));
                scale = (sint32)((uint32)ratio + (uint32)(slow_mul((sint32)root, slow_sub(4096, ratio)) >> 12));
                if (scale < 1024)
                    scale = 1024;
                delta.vy = slow_div((sint32)((uint32)delta.vy << 12u), scale);
                if ((sint32)(dr_pair_squared_distance(&delta) << 12u) >= limit)
                    return;
            }
        }
    }
    dr_race_normalize(&delta, &normal);
    components[0] = normal.vx;
    components[1] = normal.vy;
    components[2] = normal.vz;
    approach = 0;
    for (i = 0; i < 3u; ++i)
    {
        sint32 velocity = (sint16)(slow_sub(r_s32(upper + 112u + i * 4u), r_s32(lower + 112u + i * 4u)) >> 8);
        approach = (sint32)((uint32)approach + (uint32)slow_mul(components[i], velocity));
    }
    approach >>= 12;
    if (approach >= 0)
        return;
    if (upright >= 4001 && normal.vy >= 2897)
        (void)game_80094DF0(upper, lower, &delta);
    for (i = 0; i < 3u; ++i)
    {
        sint32 product = slow_mul(components[i], approach);
        // Preserve the original one-bit negative correction before shifting
        impulses[i] = (sint32)((uint32)product + ((uint32)product >> 31u)) >> 5;
    }
    for (i = 0; i < 3u; ++i)
        w_u32(lower + 112u + i * 4u, r_u32(lower + 112u + i * 4u) + (uint32)impulses[i]);
    for (i = 0; i < 3u; ++i)
        w_u32(upper + 112u + i * 4u, r_u32(upper + 112u + i * 4u) - (uint32)impulses[i]);
}
