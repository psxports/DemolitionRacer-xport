#include "draft_signatures.h"
#include <limits.h>
#include <stdlib.h>

static uint32 pair_leading(sint32 value)
{
    uint32 bits = (uint32)value, leading = 0u;
    if (value < 0) bits = ~bits;
    if (!bits) return 32u;
    while (!(bits & 0x80000000u)) { ++leading; bits <<= 1; }
    return leading;
}

static sint32 pair_checked_add(sint32 first, sint32 second)
{
    sint64 sum = (sint64)first + second;
    if (sum > INT_MAX || sum < INT_MIN) abort();
    return (sint32)sum;
}

/* Original 366D4 excludes the X square on its negative-scale branch */
uint32 dr_pair_squared_distance(const VECTOR *vector)
{
    sint32 values[3] = {vector->vx, vector->vy, vector->vz};
    uint32 leading = pair_leading(values[0]), index;
    sint32 scale, squares[3];
    for (index = 1u; index < 3u; ++index) {
        uint32 candidate = pair_leading(values[index]);
        if (candidate < leading) leading = candidate;
    }
    scale = (sint32)leading - 17;
    for (index = 0u; index < 3u; ++index) {
        sint32 value = scale < 0 ? values[index] >> ((uint32)-scale & 31u) : values[index];
        value = (sint16)value;
        squares[index] = (value * value) >> 12;
    }
    if (scale < 0) {
        (void)pair_checked_add(squares[0], squares[1]);
        return (uint32)pair_checked_add(squares[1], squares[2]);
    }
    return (uint32)pair_checked_add(pair_checked_add(squares[0], squares[1]), squares[2]);
}

/* Original 4ACF4 uses a different output scale from 4E4D4 */
uint32 dr_pair_sqrt_q12(sint32 value)
{
    uint32 leading = pair_leading(value), even, normalized, table;
    sint32 shift;
    if (leading == 32u) return 0u;
    even = leading & ~1u;
    shift = (19 - (sint32)even) >> 1;
    normalized = even >= 24u ? (uint32)value << ((even - 24u) & 31u) :
        (uint32)(value >> ((24u - even) & 31u));
    table = (uint32)(sint32)(sint16)r_u16(0x800602C0u + (normalized - 64u) * 2u);
    return shift < 0 ? table >> ((uint32)-shift & 31u) : table << ((uint32)shift & 31u);
}
