#include "psx.h"
#include <limits.h>
#include <stdlib.h>

typedef struct RaceTrackCell
{
    uint32 address, type, count;
    sint32 first, second, x, z;
} RaceTrackCell;

static sint32 race_sub(sint32 a, sint32 b)
{
    return (sint32)((uint32)a - (uint32)b);
}

static sint32 race_add(sint32 a, sint32 b)
{
    return (sint32)((uint32)a + (uint32)b);
}

static sint32 race_mul(sint32 a, sint32 b)
{
    return (sint32)((uint32)a * (uint32)b);
}

static sint32 race_div(sint32 a, sint32 b)
{
    if (!b || (a == INT_MIN && b == -1))
        abort();
    return a / b;
}

static uint32 race_cell_address(sint32 index)
{
    return r_u32(0x800653D4u) + (uint32)index * 24u;
}

static RaceTrackCell race_cell_native(sint32 index, const sint32 *position)
{
    RaceTrackCell cell;
    cell.address = race_cell_address(index);
    cell.type = r_u8(cell.address);
    cell.count = r_u8(cell.address + 3u) & 15u;
    cell.first = race_add(r_u16(cell.address + 4u), (sint8)r_u8(0x8005AA30u + cell.type * 2u));
    cell.second = race_add(r_u16(cell.address + 6u), (sint8)r_u8(0x8005AA31u + cell.type * 2u));
    cell.x = race_sub(position[0] >> 8, (sint32)r_u32(cell.address + 12u));
    cell.z = race_sub(position[2] >> 8, (sint32)r_u32(cell.address + 20u));
    return cell;
}

static uint32 race_vertex(sint32 index)
{
    return r_u32(0x800653C4u) + (uint32)index * 6u;
}

static uint32 race_outside(const RaceTrackCell *cell, sint32 left, sint32 right)
{
    uint32 a = race_vertex(left), b = race_vertex(right);
    sint32 x = (sint16)r_u16(a), z = (sint16)r_u16(a + 4u);
    sint32 dx = race_sub((sint16)r_u16(b), x);
    sint32 dz = race_sub((sint16)r_u16(b + 4u), z);
    return race_sub(race_mul(race_sub(cell->z, z), dx), race_mul(race_sub(cell->x, x), dz)) > 0;
}

static uint32 race_edge(const RaceTrackCell *cell, sint32 lane, uint32 bit)
{
    sint32 a = race_add(cell->first, lane), b = race_add(cell->second, lane);
    switch (bit)
    {
        case 1u:
            return race_outside(cell, b, a);
        case 2u:
            return race_outside(cell, race_add(b, 1), b);
        case 4u:
            return race_outside(cell, race_add(a, 1), race_add(b, 1));
        default:
            return race_outside(cell, a, race_add(a, 1));
    }
}

static sint32 race_neighbor(const RaceTrackCell *cell, sint32 index, sint32 *lane, sint32 direction)
{
    sint32 next = race_add(index, direction);
    uint32 next_address;
    if (cell->type == (direction > 0 ? 10u : 9u))
    {
        next = (sint16)r_u16(cell->address + 8u);
        *lane = race_add(*lane, race_sub(r_u8(race_cell_address(next) + 3u) & 15u, cell->count));
    }
    else if (cell->type == (direction > 0 ? 8u : 11u))
    {
        if (*lane >= (sint32)(r_u8(race_cell_address(next) + 3u) & 15u))
        {
            next = (sint16)r_u16(cell->address + 8u);
            *lane = race_add(*lane, race_sub(r_u8(race_cell_address(next) + 3u) & 15u, cell->count));
        }
    }
    else
    {
        next_address = race_cell_address(next);
        *lane = race_sub(*lane, race_sub(r_u32(next_address) >> 28, r_u32(cell->address) >> 28));
    }
    return next;
}

static void race_progress(uint8 *state, sint32 direction)
{
    sint16 count = (sint16)(((uint16)state[4] | ((uint16)state[5] << 8)) + direction);
    state[4] = (uint8)count;
    state[5] = (uint8)((uint16)count >> 8);
    if (direction > 0 && (sint16)((uint16)state[6] | ((uint16)state[7] << 8)) < count)
    {
        state[6] = (uint8)count;
        state[7] = (uint8)((uint16)count >> 8);
    }
}

static sint32 race_angle(sint32 y, sint32 x)
{
    sint32 ratio, result;
    if (!(x | y))
        return 0;
    if (y < 0)
    {
        if (x <= 0)
        {
            if (x >= y)
            {
                ratio = race_div(race_add((sint32)((uint32)x << 10), y / 2), y);
                result = 3072;
            }
            else
                return (sint16)r_u16(0x8005CDB0u + (uint32)race_div(race_add((sint32)((uint32)y << 10), x / 2), x) * 2u) + 2048;
        }
        else if (race_sub(0, y) < x)
        {
            ratio = race_div(race_sub(x / 2, (sint32)((uint32)y << 10)), x);
            result = 4096;
        }
        else
            return (sint16)r_u16(0x8005CDB0u + (uint32)race_div(race_sub(y / 2, (sint32)((uint32)x << 10)), y) * 2u) + 3072;
    }
    else if (x > 0)
    {
        if (y < x)
            return (sint16)r_u16(0x8005CDB0u + (uint32)race_div(race_add((sint32)((uint32)y << 10), x / 2), x) * 2u);
        ratio = race_div(race_add((sint32)((uint32)x << 10), y / 2), y);
        result = 1024;
    }
    else if (x < race_sub(0, y))
    {
        ratio = race_div(race_sub(x / 2, (sint32)((uint32)y << 10)), x);
        result = 2048;
    }
    else
        return (sint16)r_u16(0x8005CDB0u + (uint32)race_div(race_sub(y / 2, (sint32)((uint32)x << 10)), y) * 2u) + 1024;
    return result - (sint16)r_u16(0x8005CDB0u + (uint32)ratio * 2u);
}

uint32 dr_track_find(uint8 *state, const sint32 *position)
{
    sint32 lane = state[8], index = (sint16)((uint16)state[0] | ((uint16)state[1] << 8));
    uint32 result;
    if (r_u32(0x800593A0u))
    {
        sint32 lower = 0, upper = (sint32)r_u32(0x800653D0u);
        sint32 angle = race_angle(position[2] >> 8, position[0] >> 8);
        while (lower != upper)
        {
            sint32 middle = race_add(race_add(lower, upper), 1) >> 1;
            if (angle >= (sint32)r_u32(r_u32(0x800653ACu) + (uint32)middle * 16u + 12u))
                lower = middle;
            else
                upper = race_sub(middle, 1);
        }
        state[0] = (uint8)lower;
        state[1] = (uint8)((uint16)lower >> 8);
    }
    else
    {
        RaceTrackCell cell = race_cell_native(index, position), next;
        uint32 allowed = lane ? 15u : r_u8(0x8005AA18u + cell.type * 2u) & 15u;
        uint32 mask = 0, bit;
        sint32 neighbor, shift;
        if (lane == (sint32)cell.count - 1)
            allowed &= r_u8(0x8005AA19u + cell.type * 2u);
        for (bit = 1u; bit <= 8u; bit <<= 1)
            if ((allowed & bit) && race_edge(&cell, lane, bit))
                mask |= bit;
        if (mask == 1u || mask == 4u)
        {
            shift = mask == 1u ? -1 : 1;
            allowed = 15u;
            if (shift < 0 && lane - 1 <= 0)
                allowed = r_u8(0x8005AA18u + cell.type * 2u);
            if (shift > 0 && lane + 1 >= (sint32)cell.count - 1)
                allowed = r_u8(0x8005AA19u + cell.type * 2u);
            if ((allowed & 2u) && race_edge(&cell, lane + shift, 2u))
            {
                neighbor = race_neighbor(&cell, index, &lane, 1);
                race_progress(state, 1);
                state[0] = (uint8)neighbor;
                state[1] = (uint8)((uint16)neighbor >> 8);
            }
            else if ((allowed & 8u) && race_edge(&cell, lane + shift, 8u))
            {
                neighbor = race_neighbor(&cell, index, &lane, -1);
                state[0] = (uint8)neighbor;
                state[1] = (uint8)((uint16)neighbor >> 8);
                race_progress(state, -1);
            }
            state[8] = (uint8)(lane + shift);
        }
        else if (mask == 2u || mask == 3u || mask == 6u || mask == 8u || mask == 9u || mask == 12u)
        {
            sint32 direction = mask < 8u ? 1 : -1;
            neighbor = race_neighbor(&cell, index, &lane, direction);
            race_progress(state, direction);
            next = race_cell_native(neighbor, position);
            allowed = lane <= 0 ? r_u8(0x8005AA18u + next.type * 2u) : 15u;
            if (mask == 2u || mask == 8u)
            {
                if (lane >= (sint32)next.count - 1)
                    allowed &= r_u8(0x8005AA19u + next.type * 2u);
                if ((allowed & 1u) && race_edge(&next, lane, 1u))
                    --lane;
                else if ((allowed & 4u) && race_edge(&next, lane, 4u))
                    ++lane;
            }
            else if (mask == 3u || mask == 9u)
            {
                if ((allowed & 1u) && race_edge(&next, lane, 1u))
                {
                    allowed = lane - 1 <= 0 ? r_u8(0x8005AA18u + next.type * 2u) : 15u;
                    bit = mask == 3u ? 8u : 2u;
                    if ((allowed & bit) && race_edge(&next, lane - 1, bit))
                    {
                        state[8] = (uint8)(state[8] - 1u);
                        goto complete;
                    }
                    --lane;
                }
            }
            else
            {
                allowed = lane >= (sint32)next.count - 1 ? r_u8(0x8005AA19u + next.type * 2u) : 15u;
                if ((allowed & 4u) && race_edge(&next, lane, 4u))
                {
                    allowed = lane + 1 >= (sint32)next.count - 1 ? r_u8(0x8005AA19u + next.type * 2u) : 15u;
                    bit = mask == 6u ? 8u : 2u;
                    if ((allowed & bit) && race_edge(&next, lane + 1, bit))
                    {
                        state[8] = (uint8)(state[8] + 1u);
                        goto complete;
                    }
                    ++lane;
                }
            }
            state[8] = (uint8)lane;
            state[0] = (uint8)neighbor;
            state[1] = (uint8)((uint16)neighbor >> 8);
        }
    }
complete:
    result = state[8] < 129u;
    if (!result)
        state[8] = 0u;
    return result;
}

uint32 sub_8002C188(uint32 state, uint32 position)
{
    return dr_track_find((uint8 *)psx_addr(state, 10u), (const sint32 *)psx_addr(position, 12u));
}

void dr_race_normalize_vector(VECTOR *input, VECTOR *output)
{
    SVECTOR source = {0};
    VECTOR product = {0};
    uint32 sum, bits, leading = 0u, normalized;
    sint32 shift;
    sint16 coefficient;
    source.vx = (sint16)input->vx;
    source.vy = (sint16)input->vy;
    source.vz = (sint16)input->vz;
    sum = (uint32)((sint32)source.vx * source.vx) + (uint32)((sint32)source.vy * source.vy);
    if (sum > INT_MAX)
        abort();
    sum += (uint32)((sint32)source.vz * source.vz);
    if (sum > INT_MAX)
        abort();
    bits = sum;
    if (!bits)
        leading = 32u;
    else
        while (!(bits & 0x80000000u))
        {
            ++leading;
            bits <<= 1;
        }
    leading &= ~1u;
    normalized = leading < 24u ? sum >> (24u - leading) : sum << (leading - 24u);
    coefficient = (sint16)r_u16(0x8005E91Cu + (normalized - 64u) * 2u);
    gte_gpf0(&source, coefficient, &product);
    shift = ((31 - (sint32)leading) >> 1) & 31;
    output->vx = product.vx >> shift;
    output->vy = product.vy >> shift;
    output->vz = product.vz >> shift;
}

void dr_race_normalize(VECTOR *input, SVECTOR *output)
{
    VECTOR normalized = {0};
    dr_race_normalize_vector(input, &normalized);
    output->vx = (sint16)normalized.vx;
    output->vy = (sint16)normalized.vy;
    output->vz = (sint16)normalized.vz;
}

static void race_triangle_normal(sint32 first, sint32 second, sint32 third, sint16 *output)
{
    uint32 a = race_vertex((sint16)first), b = race_vertex((sint16)second), c = race_vertex((sint16)third);
    VECTOR left = {0}, right = {0}, cross = {0};
    SVECTOR normal = {0};
    left.vx = (sint16)r_u16(a) - (sint16)r_u16(b);
    left.vy = (sint16)r_u16(a + 2u) - (sint16)r_u16(b + 2u);
    left.vz = (sint16)r_u16(a + 4u) - (sint16)r_u16(b + 4u);
    right.vx = (sint16)r_u16(a) - (sint16)r_u16(c);
    right.vy = (sint16)r_u16(a + 2u) - (sint16)r_u16(c + 2u);
    right.vz = (sint16)r_u16(a + 4u) - (sint16)r_u16(c + 4u);
    OuterProduct0(&left, &right, &cross);
    cross.vx >>= 12;
    cross.vy >>= 12;
    cross.vz >>= 12;
    if (cross.vx || cross.vz)
    {
        dr_race_normalize(&cross, &normal);
        if (!normal.vy)
            normal.vy = 1;
    }
    else
        normal.vy = -4096;
    output[0] = normal.vx;
    output[1] = normal.vy;
    output[2] = normal.vz;
}

uint32 dr_track_normal(const uint8 *state, const sint32 *position, sint16 *output)
{
    if (!r_u32(0x800593A0u))
    {
        RaceTrackCell cell = race_cell_native((sint16)((uint16)state[0] | ((uint16)state[1] << 8)), position);
        sint32 lane = state[8];
        sint32 a = race_add(cell.first, lane), b = race_add(cell.second, lane);
        uint32 selection = 0u;
        if (!lane)
        {
            if (cell.type < 1u || cell.type > 11u)
                return 0u;
            if (cell.type == 3u || cell.type == 4u)
                selection = 2u;
            else if (cell.type == 6u || cell.type == 7u)
                selection = 1u;
            else
                selection = race_outside(&cell, a, race_add(b, 1)) ? 1u : 0u;
        }
        else if (lane == (sint32)cell.count - 1)
        {
            if (cell.type < 1u || cell.type > 11u)
                return 0u;
            if (cell.type == 2u || cell.type == 4u)
                selection = 0u;
            else if (cell.type == 5u || cell.type == 7u)
                selection = 3u;
            else
                selection = race_outside(&cell, a, race_add(b, 1)) ? 1u : 0u;
        }
        else
            selection = race_outside(&cell, a, race_add(b, 1)) ? 1u : 0u;
        if (selection == 1u)
            race_triangle_normal(a, race_add(b, 1), race_add(a, 1), output);
        else if (selection == 2u)
            race_triangle_normal(race_add(a, 1), b, race_add(b, 1), output);
        else if (selection == 3u)
            race_triangle_normal(a, b, race_add(a, 1), output);
        else
            race_triangle_normal(a, b, race_add(b, 1), output);
    }
    else
    {
        sint32 width = (sint32)r_u32(0x800653BCu), size = (sint32)r_u32(0x800653B4u);
        sint32 offset = race_mul(width / 2, size);
        sint32 x = race_add(position[0] / 256, offset);
        sint32 z = race_add(position[2] / 256, offset);
        sint32 grid_x = race_div(x, size), grid_z = race_div(z, size);
        sint32 fraction_x = race_sub(x, race_mul(grid_x, size));
        sint32 fraction_z = race_sub(z, race_mul(grid_z, size));
        uint32 heights = r_u32(0x800653C0u);
        uint32 row0 = heights + (uint32)race_add(grid_z, race_mul(grid_x, width)) * 2u;
        uint32 row1 = heights + (uint32)race_add(grid_z, race_mul(race_add(grid_x, 1), width)) * 2u;
        sint32 h00 = (sint16)r_u16(row0), h01 = (sint16)r_u16(row0 + 2u);
        sint32 h10 = (sint16)r_u16(row1), h11 = (sint16)r_u16(row1 + 2u);
        VECTOR left = {0}, right = {0}, cross = {0};
        SVECTOR normal = {0};
        w_u32(0x80065368u, (uint32)grid_x);
        w_u32(0x8006536Cu, (uint32)grid_z);
        w_u32(0x80065370u, (uint32)fraction_x);
        w_u32(0x80065374u, (uint32)fraction_z);
        w_u32(0x80065378u, (uint32)h00);
        w_u32(0x80065380u, (uint32)h01);
        w_u32(0x8006537Cu, (uint32)h10);
        w_u32(0x80065384u, (uint32)h11);
        if (fraction_z < fraction_x)
        {
            left.vx = size;
            left.vy = race_sub(h10, h00);
            right.vx = size;
            right.vy = race_sub(h11, h00);
            right.vz = size;
        }
        else
        {
            left.vx = size;
            left.vy = race_sub(h11, h00);
            left.vz = size;
            right.vy = race_sub(h01, h00);
            right.vz = size;
        }
        OuterProduct0(&left, &right, &cross);
        cross.vx >>= 7;
        cross.vy >>= 7;
        cross.vz >>= 7;
        dr_race_normalize(&cross, &normal);
        output[0] = normal.vx;
        output[1] = normal.vy;
        output[2] = normal.vz;
    }
    return 1u;
}

void sub_8002EDE0(uint32 state, uint32 position, uint32 output)
{
    dr_track_normal((const uint8 *)psx_addr(state, 10u), (const sint32 *)psx_addr(position, 12u), (sint16 *)psx_addr(output, 6u));
}
