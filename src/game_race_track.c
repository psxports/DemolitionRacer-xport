#include "draft_signatures.h"

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
#define GP 0x80064A10u

extern void dr_race_normalize(VECTOR *input, SVECTOR *output);

static uint32 track_div(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        xport_mips_break(7u);
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        xport_mips_break(6u);
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 track_rem(uint32 numerator, uint32 denominator)
{
    (void)track_div(numerator, denominator);
    return (uint32)((sint32)numerator % (sint32)denominator);
}

static uint32 track_integer(uint32 value)
{
    if ((sint32)value < 0)
        value += 255u;
    return (uint32)((sint32)value >> 8);
}

static uint32 track_vertex(uint32 index)
{
    return R32(GP + 0x9B4u) + index * 6u;
}

static uint32 track_component(uint32 vertex, uint32 offset)
{
    return (uint32)(sint32)(sint16)R16(vertex + offset);
}

static void track_edges(uint32 first, uint32 second, uint32 third, VECTOR *left, VECTOR *right)
{
    uint32 a = track_vertex(first), b = track_vertex(second), c = track_vertex(third);
    left->vx = (sint32)(track_component(a, 0u) - track_component(b, 0u));
    left->vy = (sint32)(track_component(a, 2u) - track_component(b, 2u));
    left->vz = (sint32)(track_component(a, 4u) - track_component(b, 4u));
    right->vx = (sint32)(track_component(a, 0u) - track_component(c, 0u));
    right->vy = (sint32)(track_component(a, 2u) - track_component(c, 2u));
    right->vz = (sint32)(track_component(a, 4u) - track_component(c, 4u));
}

uint32 sub_8002DA40(sint16 first, sint16 second, sint16 third, const sint32 *point)
{
    VECTOR left, right, cross;
    uint32 vertex, numerator;
    track_edges((uint32)(sint32)first, (uint32)(sint32)second, (uint32)(sint32)third, &left, &right);
    OuterProduct0(&left, &right, &cross);
    cross.vx >>= 4;
    cross.vy >>= 4;
    cross.vz >>= 4;
    vertex = track_vertex((uint32)(sint32)first);
    if (!cross.vx && !cross.vz)
        return track_component(vertex, 2u) << 8;
    if (!cross.vy)
        cross.vy = 1;
    numerator = (track_component(vertex, 0u) - (uint32)point[0]) * (uint32)cross.vx;
    numerator += (track_component(vertex, 4u) - (uint32)point[2]) * (uint32)cross.vz;
    return (track_div(numerator, (uint32)cross.vy) + track_component(vertex, 2u)) << 8;
}

static int track_positive_side(uint32 first, uint32 second, const sint32 *point)
{
    uint32 a = track_vertex(first), b = track_vertex(second + 1u);
    uint32 value = ((uint32)point[2] - track_component(a, 4u)) * (track_component(b, 0u) - track_component(a, 0u));
    value -= ((uint32)point[0] - track_component(a, 0u)) * (track_component(b, 4u) - track_component(a, 4u));
    return (sint32)value > 0;
}

/* The four original jump tables select triangle corners by section type */
static int track_triangle(const uint8 *state, const sint32 *position, int height_only, uint32 *section, uint32 *first, uint32 *second, uint32 *third, sint32 *point)
{
    uint32 type, segment, count, left, right, i;
    *section = R32(GP + 0x9C4u) + (uint32)(sint32)(sint16)(state[0] | ((uint16)state[1] << 8)) * 24u;
    type = R8(*section);
    segment = state[8];
    count = R8(*section + 3u) & 15u;
    left = R16(*section + 4u);
    right = R16(*section + 6u);
    for (i = 0u; i < 3u; ++i)
        point[i] = (sint32)((uint32)(position[i] >> 8) - R32(*section + 12u + i * 4u));
    left += (uint32)(sint32)(sint8)R8(0x8005AA30u + type * 2u);
    right += (uint32)(sint32)(sint8)R8(0x8005AA31u + type * 2u);
    left += segment;
    right += segment;
    if (!segment)
    {
        if (height_only && (type - 1u) >= 11u)
            return 0;
        switch (type)
        {
            case 3u:
            case 4u:
                *first = left + 1u;
                *second = right;
                *third = right + 1u;
                return 1;
            case 1u:
            case 2u:
            case 5u:
            case 8u:
            case 9u:
            case 10u:
            case 11u:
                break;
            case 6u:
            case 7u:
            default:
                *first = left;
                *second = right + 1u;
                *third = left + 1u;
                return 1;
        }
    }
    else if (segment == count - 1u)
    {
        if (height_only && (type - 1u) >= 11u)
            return 0;
        switch (type)
        {
            case 2u:
            case 4u:
                *first = left;
                *second = right;
                *third = right + 1u;
                return 1;
            case 1u:
            case 3u:
            case 6u:
            case 8u:
            case 9u:
            case 10u:
            case 11u:
                break;
            case 5u:
            case 7u:
            default:
                *first = left;
                *second = right;
                *third = left + 1u;
                return 1;
        }
    }
    *first = left;
    if (track_positive_side(left, right, point))
    {
        *second = right + 1u;
        *third = left + 1u;
    }
    else
    {
        *second = right;
        *third = right + 1u;
    }
    return 1;
}

static void track_grid(const sint32 *position, int height_only, VECTOR *left, VECTOR *right)
{
    uint32 width = R32(GP + 0x9ACu), spacing = R32(GP + 0x9A4u);
    uint32 half = (uint32)((sint32)(width + (width >> 31)) >> 1);
    uint32 bias = half * spacing, qx, qz, rx, rz, address;
    uint32 h00, h01, h10, h11;
    uint32 xslot = height_only ? 0x9B8u : 0x938u;
    uint32 zslot = height_only ? 0x9BCu : 0x93Cu;
    uint32 rslot = height_only ? 0x988u : 0x940u;
    uint32 hslot = height_only ? 0x978u : 0x948u;
    qx = track_div(track_integer((uint32)position[0]) + bias, spacing);
    R32(GP + xslot) = qx;
    if (height_only)
    {
        qx = track_div(track_integer((uint32)position[0]) + bias, spacing);
        R32(GP + xslot) = qx;
    }
    qz = track_div(track_integer((uint32)position[2]) + bias, spacing);
    R32(GP + zslot) = qz;
    rx = track_rem(track_integer((uint32)position[0]) + bias, spacing);
    R32(GP + rslot) = rx;
    rz = track_rem(track_integer((uint32)position[2]) + bias, spacing);
    address = R32(GP + 0x9B0u) + ((qx * width + qz) << 1);
    h00 = track_component(address, 0u);
    h01 = track_component(address, 2u);
    address = R32(GP + 0x9B0u) + (((qx + 1u) * width + qz) << 1);
    h10 = track_component(address, 0u);
    h11 = track_component(address, 2u);
    R32(GP + hslot) = h00;
    R32(GP + hslot + 8u) = h01;
    R32(GP + hslot + 4u) = h10;
    R32(GP + hslot + 12u) = h11;
    R32(GP + rslot + 4u) = rz;
    left->vx = (sint32)spacing;
    right->vz = (sint32)spacing;
    if ((sint32)rz < (sint32)rx)
    {
        left->vy = (sint32)(h10 - h00);
        left->vz = 0;
        right->vx = (sint32)spacing;
        right->vy = (sint32)(h11 - h00);
    }
    else
    {
        left->vy = (sint32)(h11 - h00);
        left->vz = (sint32)spacing;
        right->vx = 0;
        right->vy = (sint32)(h01 - h00);
    }
}

uint32 dr_track_height(const uint8 *state, const sint32 *position, sint32 *height_output)
{
    uint32 section, first, second, third, result, numerator;
    sint32 point[3];
    VECTOR *left, *right, *cross;
    if (!R32(0x800593A0u))
    {
        if (!track_triangle(state, position, 1, &section, &first, &second, &third, point))
            return ((uint32)R8(section) - 1u) << 2;
        result = sub_8002DA40((sint16)first, (sint16)second, (sint16)third, point);
        result += R32(section + 16u) << 8;
    }
    else
    {
        left = (VECTOR *)psx_addr(0x80078F04u, 12u);
        right = (VECTOR *)psx_addr(0x80078F14u, 12u);
        cross = (VECTOR *)psx_addr(0x80078EF4u, 12u);
        track_grid(position, 1, left, right);
        OuterProduct0(left, right, cross);
        cross->vx >>= 7;
        cross->vy >>= 7;
        cross->vz >>= 7;
        if (!cross->vy)
            cross->vy = 1;
        numerator = R32(GP + 0x988u) * (uint32)cross->vx + R32(GP + 0x98Cu) * (uint32)cross->vz;
        result = (track_div(0u - numerator, (uint32)cross->vy) + R32(GP + 0x978u)) << 8;
    }
    *height_output = (sint32)result;
    return result;
}

uint32 sub_8002DC38(uint32 state, uint32 position, uint32 height_output)
{
    return dr_track_height((const uint8 *)psx_addr(state, 10u), (const sint32 *)psx_addr(position, 12u), (sint32 *)psx_addr(height_output, 4u));
}

uint32 dr_track_surface(const uint8 *state, const sint32 *position, sint16 *normal_output, uint32 mode)
{
    VECTOR left, right, cross;
    uint32 section, first, second, third, vertex, numerator;
    sint32 point[3];
    if (!R32(0x800593A0u))
    {
        track_triangle(state, position, 0, &section, &first, &second, &third, point);
        track_edges(first, second, third, &left, &right);
        OuterProduct0(&left, &right, &cross);
        cross.vx >>= 12;
        cross.vz >>= 12;
        if (!cross.vx && !cross.vz)
        {
            normal_output[2] = 0;
            normal_output[0] = 0;
            normal_output[1] = -4096;
            return (track_component(track_vertex(first), 2u) + R32(section + 16u)) << 8;
        }
        cross.vy >>= 12;
        dr_race_normalize(&cross, (SVECTOR *)normal_output);
        vertex = track_vertex(first);
        if (mode)
        {
            numerator = (track_component(vertex, 0u) + R32(section + 12u)) * (uint32)(sint32)normal_output[0];
            numerator += (track_component(vertex, 2u) + R32(section + 16u)) * (uint32)(sint32)normal_output[1];
            numerator += (track_component(vertex, 4u) + R32(section + 20u)) * (uint32)(sint32)normal_output[2];
            return (uint32)((sint32)(0u - numerator) >> 4);
        }
        if (!normal_output[1])
            normal_output[1] = 1;
        numerator = (track_component(vertex, 0u) - (uint32)point[0]) * (uint32)(sint32)normal_output[0];
        numerator += (track_component(vertex, 4u) - (uint32)point[2]) * (uint32)(sint32)normal_output[2];
        return (track_div(numerator, (uint32)(sint32)normal_output[1]) + track_component(vertex, 2u) + R32(section + 16u)) << 8;
    }
    track_grid(position, 0, &left, &right);
    OuterProduct0(&left, &right, &cross);
    cross.vx >>= 7;
    cross.vy >>= 7;
    cross.vz >>= 7;
    dr_race_normalize(&cross, (SVECTOR *)normal_output);
    if (mode)
    {
        uint32 width = R32(GP + 0x9ACu), spacing = R32(GP + 0x9A4u);
        uint32 half = (uint32)((sint32)(width + (width >> 31)) >> 1);
        uint32 x = (R32(GP + 0x938u) - half) * spacing;
        uint32 z = (R32(GP + 0x93Cu) - half) * spacing;
        numerator = x * (uint32)(sint32)normal_output[0] + z * (uint32)(sint32)normal_output[2];
        R32(GP + 0x940u) = x;
        R32(GP + 0x944u) = z;
        numerator += R32(GP + 0x948u) * (uint32)(sint32)normal_output[1];
        return (uint32)((sint32)(0u - numerator) >> 4);
    }
    if (!cross.vy)
        cross.vy = 1;
    numerator = R32(GP + 0x940u) * (uint32)cross.vx + R32(GP + 0x944u) * (uint32)cross.vz;
    return (track_div(0u - numerator, (uint32)cross.vy) + R32(GP + 0x948u)) << 8;
}

uint32 sub_8002E408(uint32 state, uint32 position, uint32 normal_output, uint32 mode)
{
    return dr_track_surface((const uint8 *)psx_addr(state, 10u), (const sint32 *)psx_addr(position, 12u), (sint16 *)psx_addr(normal_output, 6u), mode);
}

uint32 sub_8002F4A4(uint32 state, uint32 position)
{
    uint32 section = R32(GP + 0x9C4u) + (uint32)(sint32)(sint16)R16(state) * 24u;
    uint32 type = R8(section), count = R8(section + 3u) & 15u;
    uint32 lane = R8(state + 8u), first, second, vertex_a, vertex_b;
    uint32 ax, az, dx, dz, divisor, numerator, x, z;
    first = R16(section + 4u) + (uint32)(sint32)(sint8)R8(0x8005AA30u + type * 2u);
    second = R16(section + 6u) + (uint32)(sint32)(sint8)R8(0x8005AA31u + type * 2u);
    if ((sint32)lane >= (sint32)count)
        lane = count - 1u;
    vertex_a = track_vertex(first + lane);
    vertex_b = track_vertex(second + lane);
    ax = track_component(vertex_a, 0u);
    dx = track_component(vertex_b, 0u) - ax;
    az = track_component(vertex_a, 4u);
    dz = track_component(vertex_b, 4u) - az;
    x = (uint32)((sint32)R32(position) >> 8) - R32(section + 12u);
    z = (uint32)((sint32)R32(position + 8u) >> 8) - R32(section + 20u) - az;
    divisor = (uint32)((sint32)(dx * dx + dz * dz) >> 8);
    if (!divisor)
        return 128u;
    numerator = (x - ax) * dx + z * dz;
    return track_div(numerator, divisor);
}

void game_800A7B6C(uint32 player, uint32 lap, uint32 lap_limit)
{
    uint32 shown_players, texture, end, index, packet, uoffset;
    if ((sint32)lap < 0)
        return;
    shown_players = R32(0x80064C14u);
    if ((sint32)shown_players < (sint32)player)
        return;
    end = player + 2u;
    texture = R32(0x800C0F44u);
    index = player;
    if ((sint32)player < (sint32)(end - shown_players))
    {
        uoffset = lap * 12u;
        packet = 0x800C293Cu + player * 40u;
        do
        {
            R8(packet + 12u) = (uint8)(R8(texture + 24u) + uoffset);
            R8(packet + 13u) = R8(texture + 25u);
            R8(packet + 20u) = (uint8)(R8(texture + 24u) + uoffset + 12u);
            R8(packet + 21u) = R8(texture + 25u);
            R8(packet + 28u) = (uint8)(R8(texture + 24u) + uoffset);
            R8(packet + 29u) = (uint8)(R8(texture + 25u) + 16u);
            R8(packet + 36u) = (uint8)(R8(texture + 24u) + uoffset + 12u);
            R8(packet + 37u) = (uint8)(R8(texture + 25u) + 16u);
            shown_players = R32(0x80064C14u);
            ++index;
            packet += 40u;
        } while ((sint32)index < (sint32)(end - shown_players));
    }
    R16(0x800C4080u + player * 2u) = lap == lap_limit ? 0xFFFFu : 7u;
    R16(0x800C4088u + player * 2u) = 8u;
}
