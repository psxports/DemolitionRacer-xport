#include "draft_signatures.h"
#include <stdio.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
#define S16(a) ((sint32)(sint16)R16(a))

extern void game_8008F924(uint32 object);
extern uint32 dr_track_find(uint8 *state, const sint32 *position);
extern uint32 dr_track_surface(const uint8 *state, const sint32 *position, sint16 *normal, uint32 mode);
extern void dr_race_normalize(VECTOR *input, SVECTOR *output);
extern void sub_80035458(const sint16 *first, const sint16 *second, sint16 *output);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);

static uint32 detach_half(uint32 value)
{
    return (uint32)((sint32)(value + (value >> 31)) >> 1);
}

static uint32 detach_quotient(uint32 value)
{
    if (!value)
        xport_mips_break(7u);
    return (uint32)(0x1000000 / (sint32)value);
}

uint32 game_8008F894(void)
{
    uint32 index, object;
    for (index = 0; index < 10; ++index)
    {
        if (R32(0x800BFF50u + index * 4u))
            continue;
        object = sub_80042544(164u);
        if (object)
        {
            R32(0x800BFF50u + index * 4u) = object;
            return object;
        }
        draft_agent1_missing_diagnostic("%s\n", (const char *)psx_addr(0x8008B464u, 1u));
        return 0;
    }
    return 0;
}

/* Original 3621C retains its GTE leading-zero scaling and dot-product state */
static uint32 detach_dot(uint32 normal, const VECTOR *position)
{
    uint32 component[3], leading[3], minimum, result, index;
    sint32 shift;
    component[0] = (uint32)position->vx;
    component[1] = (uint32)position->vy;
    component[2] = (uint32)position->vz;
    for (index = 0; index < 3; ++index)
    {
        xport_gte_write_data(30u, component[index]);
        leading[index] = xport_gte_read_data(31u);
    }
    minimum = leading[0] < leading[1] ? leading[0] : leading[1];
    if (leading[2] < minimum)
        minimum = leading[2];
    shift = (sint32)minimum - 17;
    if (shift < 0)
        for (index = 0; index < 3; ++index)
            component[index] = (uint32)((sint32)component[index] >> ((0u - (uint32)shift) & 31u));
    xport_gte_write_control(0u, R16(normal) + ((uint32)R16(normal + 2u) << 16));
    xport_gte_write_control(1u, R16(normal + 4u));
    for (index = 0; index < 3; ++index)
        xport_gte_write_data(9u + index, component[index]);
    xport_gte_execute(0x49E012u);
    result = xport_gte_read_data(25u);
    if (shift < 0)
        result <<= (0u - (uint32)shift) & 31u;
    return result;
}

static uint32 detach_surface_type(const uint8 *state)
{
    uint32 record, value;
    sint16 section;
    if (R32(0x800593A0u) == 1u)
        return 1;
    memcpy(&section, state, 2u);
    record = R32(0x800653D4u) + (uint32)(sint32)section * 24u;
    value = R8(R32(0x800653A0u) + (uint32)R8(record + 1u) * 8u + state[8]);
    return value < 32u ? value : 1u;
}

void dr_detached_ground(uint32 object, const sint32 *position, uint32 force)
{
    uint8 state[10] = {0};
    uint16 previous = R16(object + 152u), section_word;
    uint32 lane = R8(object + 154u), section, first, second, vertices, type, count, value;
    VECTOR edge = {0}, plane = {0};
    SVECTOR normal = {0};
    if (R32(0x800593A0u))
    {
        R32(object + 124u) = dr_track_surface(NULL, position, (sint16 *)psx_addr(object + 116u, 6u), 1u);
        R16(object + 116u) = (uint16)(0u - R16(object + 116u));
        R16(object + 118u) = (uint16)(0u - R16(object + 118u));
        R16(object + 120u) = (uint16)(0u - R16(object + 120u));
        R16(object + 130u) = 1;
        R8(object + 155u) = 0;
        R16(object + 128u) = (uint16)((uint32)R8(0x800BE7E8u) << 4);
        return;
    }
    memcpy(state, &previous, 2u);
    state[8] = (uint8)lane;
    dr_track_find(state, position);
    memcpy(&section_word, state, 2u);
    if ((section_word == previous || state[8] == lane) && !force)
        return;
    /* Original stores the previous section while computing from the local update */
    R16(object + 152u) = previous;
    R8(object + 154u) = (uint8)lane;
    R32(object + 124u) = dr_track_surface(state, position, (sint16 *)psx_addr(object + 116u, 6u), 1u);
    R16(object + 116u) = (uint16)(0u - R16(object + 116u));
    R16(object + 118u) = (uint16)(0u - R16(object + 118u));
    R16(object + 120u) = (uint16)(0u - R16(object + 120u));
    value = detach_surface_type(state);
    R16(object + 130u) = 1;
    R16(object + 128u) = (uint16)((uint32)R8(0x800BE7E8u + value) << 4);
    section = R32(0x800653D4u) + (uint32)(sint32)(sint16)section_word * 24u;
    vertices = R32(0x800653C4u);
    if (!state[8])
    {
        R8(object + 155u) = 1;
        first = vertices + (uint32)R16(section + 6u) * 6u;
        second = vertices + (uint32)R16(section + 4u) * 6u;
    }
    else
    {
        count = R8(section + 3u) & 15u;
        if ((sint32)state[8] < (sint32)(count - 1u))
        {
            R8(object + 155u) = 0;
            return;
        }
        R8(object + 155u) = 1;
        type = R8(section);
        first = vertices + ((uint32)R16(section + 4u) + count + (uint32)(sint32)(sint8)R8(0x800BE970u + type * 2u)) * 6u;
        second = vertices + ((uint32)R16(section + 6u) + count + (uint32)(sint32)(sint8)R8(0x800BE971u + type * 2u)) * 6u;
    }
    edge.vx = (sint32)((uint32)S16(second + 4u) - (uint32)S16(first + 4u));
    edge.vz = (sint32)((uint32)S16(first) - (uint32)S16(second));
    dr_race_normalize(&edge, &normal);
    memcpy(psx_addr(object + 132u, 6u), &normal, 6u);
    plane.vx = (sint32)((uint32)S16(first) + R32(section + 12u));
    plane.vy = (sint32)((uint32)S16(first + 2u) + R32(section + 16u));
    plane.vz = (sint32)((uint32)S16(first + 4u) + R32(section + 20u));
    R32(object + 140u) = detach_dot(object + 132u, &plane) << 8;
}

void game_8008ED9C(uint32 object, uint32 position, uint32 force)
{
    dr_detached_ground(object, (const sint32 *)psx_addr(position, 12u), force);
}

static void detach_cross(const SVECTOR *first, const SVECTOR *second, SVECTOR *output)
{
    uint32 left[3] = {(uint16)first->vx, (uint16)first->vy, (uint16)first->vz};
    uint32 right[3] = {(uint32)(sint32)second->vx, (uint32)(sint32)second->vy, (uint32)(sint32)second->vz};
    uint32 index;
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(index * 2u, left[index]);
    for (index = 0; index < 3; ++index)
        xport_gte_write_data(index + 9u, right[index]);
    xport_gte_execute(0x178000Cu);
    output->vx = (sint16)xport_gte_read_data(25u);
    output->vy = (sint16)xport_gte_read_data(26u);
    output->vz = (sint16)xport_gte_read_data(27u);
}

static void detach_square(uint32 vector)
{
    uint32 input[3] = {(uint32)S16(vector), (uint32)S16(vector + 2u), (uint32)S16(vector + 4u)};
    uint32 index;
    for (index = 0; index < 3; ++index)
        xport_gte_write_data(index + 9u, input[index]);
    xport_gte_execute(0xA80428u);
    for (index = 0; index < 3; ++index)
        R16(vector + index * 2u) = (uint16)xport_gte_read_data(25u + index);
}

void sub_80034C74(uint32 player, uint32 object, uint32 side)
{
    uint32 detached = game_8008F894(), bytes, buffer, previous, part, specification, index;
    uint32 angle, radius, sine, cosine, component, half_z, half_x, sum, reciprocal_z, reciprocal_sum, reciprocal_x;
    SVECTOR arm = {0}, offset = {0}, product = {0};
    sint16 quaternion[4];
    VECTOR transformed = {0};
    if (!detached)
        return;
    bytes = R32(side ? 0x80065268u : 0x80065264u);
    buffer = sub_80042544(bytes);
    if (!buffer)
    {
        R32(detached + 92u) = 0;
        game_8008F924(detached);
        return;
    }
    memcpy(psx_addr(buffer, bytes), psx_addr(R32(side ? 0x80066D34u : 0x80066D38u), bytes), bytes);
    R32(detached + 92u) = buffer;
    part = side ? 32u : 36u;
    previous = R32(object + part);
    specification = side ? 0x80066C48u : 0x80066C3Au;
    R32(object + part) = buffer;
    sub_8003132C(object, side ? 2u : 4u);
    R32(object + part) = previous;
    angle = (uint32)S16(specification + 10u) - (uint32)S16(object + (side ? 328u : 326u));
    radius = (uint32)S16(specification + 8u);
    if (side)
        radius = 0u - radius;
    radius = detach_half(radius);
    sine = (uint32)draft_agent3_missing_8004388C(angle);
    cosine = (uint32)draft_agent3_missing_800437EC(angle);
    arm.vy = (sint16)((sint32)(sine * radius) >> 12);
    arm.vz = (sint16)((sint32)(cosine * radius) >> 12);
    part = side ? 0x800653D8u : 0x800653E0u;
    offset.vx = (sint16)R16(part);
    offset.vy = (sint16)(R16(part + 2u) + ((sint32)(sine * radius) >> 12));
    offset.vz = (sint16)(R16(part + 4u) + ((sint32)(cosine * radius) >> 12));
    ApplyMatrix((MATRIX *)psx_addr(object + 148u, 32u), &offset, &transformed);
    R32(detached + 40u) = R32(object + 100u) + ((uint32)transformed.vx << 8);
    R32(detached + 44u) = R32(object + 104u) + ((uint32)transformed.vy << 8);
    R32(detached + 48u) = R32(object + 108u) + ((uint32)transformed.vz << 8);
    component = (uint32)S16(object + (side ? 336u : 334u));
    if (side)
        component = 0u - component;
    R16(detached + 78u) = 0;
    R16(detached + 72u) = (uint16)(component << 6);
    R16(detached + 74u) = (uint16)((uint32)randPSX() & 511u);
    R16(detached + 76u) = (uint16)((uint32)randPSX() & 511u);
    angle = (uint32)((sint32)angle >> 1);
    quaternion[3] = (sint16)draft_agent3_missing_800437EC(angle);
    quaternion[0] = (sint16)draft_agent3_missing_8004388C(angle);
    quaternion[2] = quaternion[1] = 0;
    sub_80035458(quaternion, (const sint16 *)psx_addr(object + 252u, 8u), (sint16 *)psx_addr(detached + 64u, 8u));
    detach_cross(&arm, (const SVECTOR *)psx_addr(detached + 72u, 8u), &product);
    for (index = 0; index < 3; ++index)
    {
        component = (uint32)(sint32)(index == 0 ? product.vx : index == 1 ? product.vy : product.vz) << 8;
        if ((sint32)component < 0)
            component += 3u;
        R32(detached + 52u + index * 4u) = R32(object + 112u + index * 4u) + (uint32)((sint32)component >> 2);
    }
    component = R32(detached + 56u);
    if ((sint32)component < 0)
        R32(detached + 56u) = 0u - component;
    R32(detached + 80u) = (uint32)S16(specification);
    R32(detached + 84u) = detach_half((uint32)S16(specification + 6u));
    R32(detached + 88u) = detach_half((uint32)S16(specification + 8u));
    R16(detached + 98u) = (uint16)(R32(detached + 80u) << 5);
    R16(detached + 100u) = (uint16)(R32(detached + 84u) << 5);
    R16(detached + 102u) = (uint16)(R32(detached + 88u) << 5);
    detach_square(detached + 98u);
    half_z = detach_half((uint32)S16(detached + 102u));
    reciprocal_z = detach_quotient(half_z);
    R16(detached + 98u) = (uint16)detach_half((uint32)S16(detached + 98u));
    half_x = (uint32)S16(detached + 98u);
    sum = half_z + half_x;
    reciprocal_sum = detach_quotient(sum);
    reciprocal_x = detach_quotient(half_x);
    R16(detached + 102u) = (uint16)half_z;
    R16(detached + 98u) = (uint16)half_z;
    R16(detached + 102u) = (uint16)half_x;
    R16(detached + 100u) = (uint16)detach_half((uint32)S16(detached + 100u));
    R16(detached + 100u) = (uint16)sum;
    R16(detached + 106u) = (uint16)reciprocal_z;
    R16(detached + 108u) = (uint16)reciprocal_sum;
    R16(detached + 110u) = (uint16)reciprocal_x;
    R16(detached + 96u) = R16(R32(detached + 92u) + 8u);
    R16(detached + 152u) = R16(object + 88u);
    R8(detached + 154u) = R8(object + 96u);
    game_8008ED9C(detached, detached + 40u, 1u);
    R8(detached + 156u) = 1;
    R8(detached + 157u) = 0;
    component = R32(0x80065400u);
    R32(detached + 148u) = 0;
    R16(detached + 130u) = 0;
    R16(detached + 128u) = 0;
    ++component;
    R32(0x80065400u) = component;
    R8(detached + 158u) = (uint8)component;
    R8(detached + 159u) = (uint8)player;
    R8(detached + 160u) = (uint8)side;
}
