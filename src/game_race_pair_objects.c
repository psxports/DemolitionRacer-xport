#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
extern void game_800AE184(uint32 object, sint32 impulse, uint32 flags);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);

typedef struct PairObjectContact
{
    sint16 x, z, offset_x, offset_z;
} PairObjectContact;

static sint32 po_add(sint32 a, sint32 b)
{
    return (sint32)((uint32)a + (uint32)b);
}

static sint32 po_sub(sint32 a, sint32 b)
{
    return (sint32)((uint32)a - (uint32)b);
}

static sint32 po_step(sint32 value, sint32 scale)
{
    sint32 product = (sint32)((uint32)(value / 2) * (uint32)scale);
    return product < 0 ? po_add(product, 255) >> 8 : product >> 8;
}

static sint32 po_product(sint32 first, sint32 second)
{
    return (sint32)((uint32)first * (uint32)second);
}

static uint32 po_contact(PairObjectContact *contact, sint32 x, sint32 z, sint32 center_x, sint32 center_z, sint32 width, sint32 front, sint32 back, uint32 bit)
{
    if (x > width || x < po_sub(0, width) || z > front || z < back)
        return 0;
    contact->offset_x = (sint16)po_sub(x, center_x);
    contact->x = (sint16)x;
    contact->z = (sint16)z;
    contact->offset_z = (sint16)po_sub(z, center_z);
    return bit;
}

static uint32 po_geometry(uint32 player, uint32 object, const VECTOR positions[2], const sint32 angles[2], PairObjectContact contacts[8])
{
    uint32 model = r_u32(player), mask = 0, index;
    sint32 relative = (sint32)((uint32)po_sub(angles[0], angles[1]) << 8) >> 16;
    sint32 cosine = draft_agent3_missing_800437EC((uint32)relative);
    sint32 sine = draft_agent3_missing_8004388C((uint32)relative);
    sint32 player_cosine = draft_agent3_missing_800437EC((uint32)(angles[0] >> 8));
    sint32 player_sine = draft_agent3_missing_8004388C((uint32)(angles[0] >> 8));
    sint32 object_cosine = draft_agent3_missing_800437EC((uint32)(angles[1] >> 8));
    sint32 object_sine = draft_agent3_missing_8004388C((uint32)(angles[1] >> 8));
    sint32 dx = po_sub(positions[1].vx, positions[0].vx) >> 8;
    sint32 dz = po_sub(positions[1].vz, positions[0].vz) >> 8;
    sint32 player_width = r_s16(model + 8), player_front = r_s16(model + 4), player_back = r_s16(model + 20);
    sint32 object_width = r_s16(object + 228), object_front = r_s16(object + 224), object_back = r_s16(object + 240);
    sint32 width_cosine, width_sine, front_cosine, front_sine, back_cosine, back_sine, center_x, center_z;
    sint32 x[4], z[4];
    width_cosine = po_product(cosine, object_width) >> 12;
    width_sine = po_product(sine, object_width) >> 12;
    front_cosine = po_product(cosine, object_front) >> 12;
    front_sine = po_product(sine, object_front) >> 12;
    back_cosine = po_product(cosine, po_sub(0, object_back)) >> 12;
    back_sine = po_product(sine, po_sub(0, object_back)) >> 12;
    center_x = po_sub(po_product(dx, player_cosine), po_product(dz, player_sine)) >> 12;
    center_z = po_add(po_product(dx, player_sine), po_product(dz, player_cosine)) >> 12;
    x[0] = po_sub(po_sub(0, width_cosine), front_sine);
    x[1] = po_sub(width_cosine, front_sine);
    x[2] = po_sub(back_sine, width_cosine);
    x[3] = po_add(width_cosine, back_sine);
    z[0] = po_sub(front_cosine, width_sine);
    z[1] = po_add(width_sine, front_cosine);
    z[2] = po_sub(po_sub(0, width_sine), back_cosine);
    z[3] = po_sub(width_sine, back_cosine);
    for (index = 0; index < 4; ++index)
        mask |= po_contact(&contacts[index], po_add(x[index], center_x), po_add(z[index], center_z), center_x, center_z, player_width, player_front, player_back, 1 << index);
    if (!mask)
        mask = po_contact(&contacts[3], center_x, center_z, center_x, center_z, player_width, player_front, player_back, 8);
    width_cosine = po_product(cosine, player_width) >> 12;
    width_sine = po_product(po_sub(0, sine), player_width) >> 12;
    front_cosine = po_product(cosine, player_front) >> 12;
    front_sine = po_product(po_sub(0, sine), player_front) >> 12;
    back_cosine = po_product(cosine, po_sub(0, player_back)) >> 12;
    back_sine = po_product(po_sub(0, sine), po_sub(0, player_back)) >> 12;
    center_x = po_add(po_product(po_sub(0, dx), object_cosine), po_product(dz, object_sine)) >> 12;
    center_z = po_sub(po_product(po_sub(0, dx), object_sine), po_product(dz, object_cosine)) >> 12;
    x[0] = po_sub(po_sub(0, width_cosine), front_sine);
    x[1] = po_sub(width_cosine, front_sine);
    x[2] = po_sub(back_sine, width_cosine);
    x[3] = po_add(width_cosine, back_sine);
    z[0] = po_sub(front_cosine, width_sine);
    z[1] = po_add(width_sine, front_cosine);
    z[2] = po_sub(po_sub(0, width_sine), back_cosine);
    z[3] = po_sub(width_sine, back_cosine);
    for (index = 0; index < 4; ++index)
        mask |= po_contact(&contacts[index + 4], po_add(x[index], center_x), po_add(z[index], center_z), center_x, center_z, object_width, object_front, object_back, 16 << index);
    if (!mask)
        mask = po_contact(&contacts[7], center_x, center_z, center_x, center_z, object_width, object_front, object_back, 128);
    return mask;
}

static sint32 po_abs(sint32 value)
{
    return value < 0 ? po_sub(0, value) : value;
}

static sint32 po_div(sint32 value, sint32 divisor)
{
    if (!divisor)
        xport_mips_break(7);
    if (value == INT_MIN && divisor == -1)
        xport_mips_break(6);
    return value / divisor;
}

static sint32 po_magic(sint32 value, sint32 constant, uint32 shift)
{
    sint32 high = (sint32)(((sint64)value * constant) >> 32);
    return po_sub(high >> shift, value >> 31);
}

static sint32 po_random(uint32 divisor)
{
    uint32 value = sub_8001B884(0);
    if (!divisor)
        xport_mips_break(7);
    return (sint32)(value % divisor);
}

static void po_integrate(uint32 position, uint32 velocity, sint32 scale, sint32 tick, sint32 direction)
{
    sint32 value = po_product(po_product(scale, r_s32(velocity)) / 256, tick) / 256;
    w_u32(position, direction < 0 ? (uint32)po_sub(r_s32(position), value) : (uint32)po_add(r_s32(position), value));
}

static void po_response(uint32 target, uint32 player, uint32 object, const PairObjectContact *contact, sint32 angle, sint32 fraction)
{
    uint32 object_first = target == 0x800AB4F8, index, axis, component;
    uint32 position[2], velocity_address[2], angular_address[2], rotation_address[2], model;
    sint32 angular[2], velocity[2][2], mass[2], cosine, sine, value, other, width, front, back;
    sint32 point[2][2], move_x, move_z, object_mass, player_mass, inertia, denominator, coefficient;
    sint32 impulse, magnitude, scale, tick, angular_delta[2], limit, offset;
    position[0] = object_first ? object + 12 : player + 100;
    position[1] = object_first ? player + 100 : object + 12;
    velocity_address[0] = object_first ? object + 24 : player + 112;
    velocity_address[1] = object_first ? player + 112 : object + 24;
    angular_address[0] = object_first ? object + 108 : player + 140;
    angular_address[1] = object_first ? player + 140 : object + 108;
    rotation_address[0] = object_first ? object + 88 : player + 128;
    rotation_address[1] = object_first ? player + 128 : object + 88;
    angular[0] = r_s32(angular_address[0]);
    angular[1] = r_s32(angular_address[1]);
    cosine = draft_agent3_missing_800437EC((uint32)angle);
    sine = draft_agent3_missing_8004388C((uint32)angle);
    for (index = 0; index < 2; ++index)
    {
        value = r_s32(velocity_address[index]);
        other = r_s32(velocity_address[index] + 8);
        velocity[index][0] = po_sub(po_product(value, cosine), po_product(other, sine)) / 4096;
        velocity[index][1] = po_add(po_product(value, sine), po_product(other, cosine)) / 4096;
    }
    point[0][0] = contact->x;
    point[0][1] = contact->z;
    point[1][0] = contact->offset_x;
    point[1][1] = contact->offset_z;
    model = object_first ? object : r_u32(player);
    width = r_s16(model + (object_first ? 228 : 8));
    front = r_s16(model + (object_first ? 224 : 4));
    back = r_s16(model + (object_first ? 240 : 20));
    value = po_abs(po_sub(width, po_abs(point[0][0])));
    other = point[0][1] > 0 ? po_abs(po_sub(front, point[0][1])) : po_abs(po_add(po_sub(0, back), point[0][1]));
    axis = value >= other;
    value = po_sub(point[0][axis], point[1][axis]);
    if (!axis)
    {
        move_x = (value > 0 ? cosine : po_sub(0, cosine)) / 2;
        move_z = (value > 0 ? po_sub(0, sine) : sine) / 2;
    }
    else
    {
        move_x = (value > 0 ? sine : po_sub(0, sine)) / 2;
        move_z = (value > 0 ? cosine : po_sub(0, cosine)) / 2;
    }
    object_mass = r_s16(object + 288);
    player_mass = r_u8(player + 949);
    mass[0] = object_first ? object_mass : player_mass;
    mass[1] = object_first ? player_mass : object_mass;
    if (!object_first || object_mass)
    {
        w_u32(position[0], (uint32)po_sub(r_s32(position[0]), move_x));
        w_u32(position[0] + 8, (uint32)po_sub(r_s32(position[0] + 8), move_z));
    }
    if (object_first || object_mass)
    {
        w_u32(position[1], (uint32)po_add(r_s32(position[1]), move_x));
        w_u32(position[1] + 8, (uint32)po_add(r_s32(position[1] + 8), move_z));
    }
    inertia = r_s32(0x800BE96C);
    component = 1 - axis;
    denominator = po_add(po_product(po_add(inertia, po_product(point[1][component], point[1][component])), mass[0]), po_product(po_add(inertia, po_product(point[0][component], point[0][component])), mass[1]));
    coefficient = po_div((sint32)((uint32)po_product(inertia / 256, 17) << 8), denominator / 256);
    value = po_sub(po_add(velocity[1][axis], po_magic(po_product(angular[1], point[1][component]), 0x0C907DA5, 5)), velocity[0][axis]);
    value = po_sub(value, po_magic(po_product(angular[0], point[0][component]), 0x0C907DA5, 5));
    impulse = po_product(coefficient, value) / 4096;
    if ((impulse ^ po_sub(point[1][axis], point[0][axis])) < 0)
        return;
    denominator = po_magic(inertia, 0x0C907DA5, 5);
    for (index = 0; index < 2; ++index)
        angular_delta[index] = po_div(po_product(po_product(impulse, point[index][component]), mass[index]), denominator);
    angular[0] = axis ? po_sub(angular[0], angular_delta[0]) : po_add(angular[0], angular_delta[0]);
    angular[1] = axis ? po_add(angular[1], angular_delta[1]) : po_sub(angular[1], angular_delta[1]);
    velocity[0][axis] = po_add(velocity[0][axis], po_product(impulse, mass[0]));
    velocity[1][axis] = po_sub(velocity[1][axis], po_product(impulse, mass[1]));
    magnitude = po_abs(po_product(impulse, po_add(mass[0], mass[1])));
    scale = po_sub(256, fraction);
    tick = r_s32(0x800651C4);
    for (index = 0; index < 2; ++index)
    {
        if ((object_first ? index == 0 : index == 1) && !object_mass)
            continue;
        po_integrate(position[index], velocity_address[index], scale, tick, -1);
        po_integrate(position[index] + 8, velocity_address[index] + 8, scale, tick, -1);
        if (object_first || index == 1)
        {
            po_integrate(rotation_address[index], angular_address[index], scale, tick, -1);
            w_u32(angular_address[index], (uint32)angular[index]);
        }
    }
    if (!object_first)
    {
        po_integrate(player + 128, player + 140, scale, tick, -1);
        w_u32(player + 140, (uint32)angular[0]);
    }
    for (index = 0; index < 2; ++index)
    {
        value = po_add(po_product(velocity[index][0], cosine), po_product(velocity[index][1], sine)) / 4096;
        other = po_add(po_product(po_sub(0, velocity[index][0]), sine), po_product(velocity[index][1], cosine)) / 4096;
        w_u32(velocity_address[index], (uint32)value);
        w_u32(velocity_address[index] + 8, (uint32)other);
    }
    for (index = 0; index < 2; ++index)
    {
        if ((object_first ? index == 0 : index == 1) && !object_mass)
            continue;
        po_integrate(position[index], velocity_address[index], scale, tick, 1);
        po_integrate(position[index] + 8, velocity_address[index] + 8, scale, tick, 1);
        if (object_first || index == 1)
            po_integrate(rotation_address[index], angular_address[index], scale, tick, 1);
    }
    if (!object_first)
        po_integrate(player + 128, player + 140, scale, tick, 1);
    if (object_first || r_u8(player + 937) < 16)
        game_8009A8E8(player);
    game_800AB0D8(object);
    if (magnitude < 30001 || !object_mass)
        return;
    /* Original chooses a minimum random range of 32767 */
    limit = magnitude / 4;
    if (limit < 32767)
        limit = 32767;
    offset = po_sub(po_random((uint32)limit), limit / 2);
    w_u32(object + 108, (uint32)po_add(r_s32(object + 108), offset));
    if (r_u8(object + 296) & 1)
        limit /= 16;
    else
        w_u32(object + 28, (uint32)po_magic(magnitude, 0x2AAAAAAB, 0));
    offset = po_sub(po_random((uint32)limit), limit / 2);
    w_u32(object + 104, (uint32)po_add(r_s32(object + 104), offset));
    offset = po_sub(po_random((uint32)limit << 1), limit);
    w_u32(object + 112, (uint32)po_add(r_s32(object + 112), offset));
    game_800AE184(object, magnitude / 2048, (uint32)po_magic(r_s32(player + 824), 0x10624DD3, 13));
}

static void po_collect(uint32 player, uint32 type, uint32 position)
{
    sub_80015780(player, type, position);
}

static void po_effect_native(uint32 model, uint32 angles, const VECTOR *velocity, sint32 flags)
{
    dr_effect_model_native(model, angles, velocity, flags);
}

void game_800AD59C(uint32 player_index, uint32 bucket_object_index)
{
    uint32 first_bounds = 0x800C03D0u + player_index * 20u, second_bounds = 0x800C03D0u + bucket_object_index * 20u;
    uint32 player, object, mask, type, i, iteration;
    VECTOR positions[2];
    sint32 angles[2], delta[2][3], fraction = 128, step = 128, scale;
    PairObjectContact contacts[8];
    sint32 distance, extent, high;
    if ((sint32)r_u32(first_bounds) >= (sint32)r_u32(second_bounds + 8u) || (sint32)r_u32(second_bounds) >= (sint32)r_u32(first_bounds + 8u) || (sint32)r_u32(first_bounds + 4u) >= (sint32)r_u32(second_bounds + 12u) || (sint32)r_u32(second_bounds + 4u) >= (sint32)r_u32(first_bounds + 12u))
        return;
    player = 0x80066D14u + player_index * 952u;
    positions[0].vx = (sint32)r_u32(player + 100u);
    positions[0].vy = (sint32)r_u32(player + 104u);
    positions[0].vz = (sint32)r_u32(player + 108u);
    object = 0x800C4378u + (bucket_object_index - r_u32(0x800C08D0u)) * 300u;
    positions[1].vx = (sint32)r_u32(object + 12u);
    positions[1].vy = (sint32)r_u32(object + 16u);
    positions[1].vz = (sint32)r_u32(object + 20u);
    distance = po_sub(positions[1].vy, positions[0].vy) >> 8;
    distance = (sint32)dr_race_motion_sqrt((uint32)distance * (uint32)distance);
    extent = po_add((sint16)r_u16(r_u32(player) + 128u), (sint16)r_u16(r_u32(object + 120u) + 8u));
    high = (sint32)(((sint64)extent * 0x55555556) >> 32);
    if (po_sub(high, extent >> 31) < distance)
        return;
    angles[0] = (sint32)r_u32(player + 128u);
    angles[1] = (sint32)r_u32(object + 88u);
    mask = po_geometry(player, object, positions, angles, contacts);
    if (!mask)
        return;
    type = r_u8(object + 292u) >> 4;
    w_u8(0x800C42B0u + r_u8(object + 298u), 1u);
    if (type >= 6u)
    {
        VECTOR velocity;
        uint32 model;
        velocity.vx = (sint32)r_u32(player + 112u);
        velocity.vy = po_add((sint32)r_u32(player + 116u), 10000);
        velocity.vz = (sint32)r_u32(player + 120u);
        po_collect(player_index, type, object + 12u);
        model = r_u32(object + 120u);
        w_u32(model + 16u, r_u32(object + 12u));
        w_u32(model + 20u, r_u32(object + 16u));
        w_u32(model + 24u, r_u32(object + 20u));
        po_effect_native(r_u32(object + 120u), object + 96u, &velocity, -500);
        if (!r_u8(object + 299u))
            w_u8(object + 299u, 4u);
        game_800AE184(object, 32, 0u);
        return;
    }
    w_u8(object + 297u, 1u);
    scale = (sint32)r_u32(0x800651C4u);
    delta[0][0] = po_step((sint32)r_u32(player + 112u), scale);
    delta[0][1] = po_step((sint32)r_u32(player + 120u), scale);
    delta[1][0] = po_step((sint32)r_u32(object + 24u), scale);
    delta[1][1] = po_step((sint32)r_u32(object + 32u), scale);
    delta[0][2] = po_step((sint32)r_u32(player + 140u), scale);
    delta[1][2] = po_step((sint32)r_u32(object + 108u), scale);
    for (i = 0; i < 2u; ++i)
    {
        positions[i].vx = po_sub(positions[i].vx, delta[i][0]);
        positions[i].vz = po_sub(positions[i].vz, delta[i][1]);
        angles[i] = po_sub(angles[i], delta[i][2]);
    }
    for (iteration = 0; iteration < 7u; ++iteration)
    {
        uint32 current;
        step /= 2;
        for (i = 0; i < 2u; ++i)
        {
            delta[i][0] /= 2;
            delta[i][1] /= 2;
            delta[i][2] /= 2;
        }
        current = po_geometry(player, object, positions, angles, contacts);
        if (current)
        {
            fraction -= step;
            mask = current;
        }
        else
            fraction += step;
        for (i = 0; i < 2u; ++i)
        {
            sint32 dx = delta[i][0], dz = delta[i][1], da = delta[i][2];
            positions[i].vx = current ? po_sub(positions[i].vx, dx) : po_add(positions[i].vx, dx);
            positions[i].vz = current ? po_sub(positions[i].vz, dz) : po_add(positions[i].vz, dz);
            angles[i] = current ? po_sub(angles[i], da) : po_add(angles[i], da);
        }
    }
    fraction -= 16;
    switch (mask)
    {
        case 1u:
        case 2u:
        case 4u:
        case 8u:
            for (i = 0; !(mask & (1u << i)); ++i)
            {
            }
            po_response(0x800AC1F0u, player, object, &contacts[i], angles[0] >> 8, fraction);
            return;
        case 16u:
        case 32u:
        case 64u:
        case 128u:
            for (i = 4; !(mask & (1u << i)); ++i)
            {
            }
            po_response(0x800AB4F8u, player, object, &contacts[i], angles[1] >> 8, fraction);
            return;
        default:
            break;
    }
    for (i = 0; i < 4u; ++i)
        if (mask & (1u << i))
            po_response(0x800AC1F0u, player, object, &contacts[i], angles[0] >> 8, fraction);
    for (i = 4; i < 8u; ++i)
        if (mask & (1u << i))
            po_response(0x800AB4F8u, player, object, &contacts[i], angles[1] >> 8, fraction);
}
