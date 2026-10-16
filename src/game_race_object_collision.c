#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern uint32 sub_8001ED2C(uint32, uint32, uint32, sint32, uint32, uint32);
void game_800BC824(uint32 model, uint32 angles, uint32 velocity, uint32 flags);
void game_800BB800(uint32 kind, uint32 origin, uint32 angles, uint32 velocity, uint32 vertex0, uint32 vertex1, uint32 vertex2, uint32 vertex3, uint32 packet_a, uint32 packet_b, uint32 flags);
static void effect_polygon(uint32 kind, uint32 origin, uint32 angles, uint32 velocity, uint32 vertex0, uint32 vertex1, uint32 vertex2, uint32 vertex3, uint32 packet_a, uint32 packet_b, uint32 flags, const VECTOR *native_velocity);

static sint32 collision_add(sint32 a, sint32 b)
{
    return (sint32)((uint32)a + (uint32)b);
}

static sint32 collision_sub(sint32 a, sint32 b)
{
    return (sint32)((uint32)a - (uint32)b);
}

static sint32 collision_mul(sint32 a, sint32 b)
{
    return (sint32)((uint32)a * (uint32)b);
}

static sint32 collision_truncate(sint32 value, uint32 shift)
{
    uint32 bits = (uint32)value;
    if (value < 0)
        bits += (1u << shift) - 1u;
    return (sint32)bits >> shift;
}

static sint32 collision_div(sint32 value, sint32 divisor)
{
    if (!divisor)
        xport_mips_break(7u);
    if (value == INT_MIN && divisor == -1)
        xport_mips_break(6u);
    return value / divisor;
}

static sint32 collision_rotation_divisor(sint32 value)
{
    sint32 high = (sint32)(((int64_t)value * 0x0C907DA5) >> 32);
    return collision_sub(high >> 5, value >> 31);
}

void game_800AE184(uint32 object, sint32 impulse, uint32 flags)
{
    uint32 material = r_u8(object + 0x124u) >> 4;
    sint32 magnitude = impulse < 0 ? (sint32)(0u - (uint32)impulse) : impulse;
    sint32 volume = (sint32)((uint32)magnitude << 7);
    uint32 sound = r_u8(0x800BF038u + material);
    if (material >= 6u || volume > 4096)
        volume = 4096;
    sub_8001ED2C(sound, 0xFFFFFFFFu, (uint32)volume, 4096, object + 12u, 0u);
    if (magnitude >= 49 && material < 6u && (r_u8(object + 0x128u) & 8u))
    {
        game_800BC824(r_u32(object + 0x78u), object + 0x60u, object + 0x18u, flags);
        if (!r_u8(object + 0x12Bu))
            w_u8(object + 0x12Bu, 4u);
    }
}

void game_800AAA94(uint32 object, uint32 position, sint32 angle, sint32 penetration)
{
    sint32 cosine = draft_agent3_missing_800437EC((uint32)angle);
    sint32 sine = draft_agent3_missing_8004388C((uint32)angle);
    sint32 vx, vz, rotation, along_product, along, tangent, lever, incoming, mass, mass_scale;
    sint32 impulse, rotation_delta, new_tangent, new_along, term, reduction;
    penetration = collision_sub(penetration, 4);
    vx = (sint32)r_u32(object + 24u);
    vz = (sint32)r_u32(object + 32u);
    rotation = (sint32)r_u32(object + 108u);
    w_u32(object + 12u, r_u32(object + 12u) + (uint32)collision_truncate(collision_mul((sint32)(0u - (uint32)penetration), sine), 4u));
    w_u32(object + 20u, r_u32(object + 20u) + (uint32)collision_truncate(collision_mul(penetration, cosine), 4u));
    along_product = collision_add(collision_mul(vx, cosine), collision_mul(vz, sine));
    along = collision_truncate(along_product, 12u);
    tangent = collision_truncate(collision_sub(collision_mul(vz, cosine), collision_mul(vx, sine)), 12u);
    /* Position may alias the object and must be reread after the two position writes */
    lever = collision_truncate(collision_add(collision_mul(collision_sub((sint32)r_u32(object + 12u), (sint32)r_u32(position)) >> 8, cosine), collision_mul(collision_sub((sint32)r_u32(object + 20u), (sint32)r_u32(position + 8u)) >> 8, sine)), 12u);
    incoming = collision_add(tangent, collision_mul(lever, collision_rotation_divisor(rotation)));
    if (incoming < 0)
        return;
    mass = (sint32)r_u32(0x800BE968u);
    mass_scale = collision_truncate((sint32)((uint32)collision_mul(collision_truncate(mass, 8u), 17) << 8), 12u);
    impulse = collision_div(collision_mul((sint32)(0u - (uint32)mass_scale), incoming), collision_truncate(collision_add(mass, collision_mul(lever, lever)), 8u));
    rotation_delta = collision_div(collision_mul(impulse, lever), collision_rotation_divisor(mass));
    new_tangent = collision_add(tangent, impulse);
    if (along > 0)
    {
        sint32 adjusted = along_product;
        if (adjusted < 0)
            adjusted = collision_add(adjusted, 4095);
        term = collision_add(collision_add((sint32)((uint32)incoming << 1), adjusted >> 18), 2048);
        reduction = collision_truncate((sint32)((uint32)collision_mul(term, 3) << 7), 11u);
        new_along = collision_sub(along, reduction);
        if (new_along < 0)
            new_along = 0;
    }
    else
    {
        term = collision_sub((sint32)((uint32)incoming << 1), collision_sub(collision_truncate(along, 6u), 2048));
        reduction = collision_truncate((sint32)((uint32)collision_mul(term, 3) << 7), 11u);
        new_along = collision_add(along, reduction);
        if (new_along > 0)
            new_along = 0;
    }
    w_u32(object + 108u, (uint32)collision_add(rotation, rotation_delta));
    w_u32(object + 24u, (uint32)collision_truncate(collision_sub(collision_mul(new_along, cosine), collision_mul(new_tangent, sine)), 12u));
    w_u32(object + 32u, (uint32)collision_truncate(collision_add(collision_mul(new_tangent, cosine), collision_mul(new_along, sine)), 12u));
    game_800AE184(object, impulse >> 10, 0u);
}

static void effect_model(uint32 model, uint32 angles, uint32 velocity, uint32 flags, const VECTOR *native_velocity)
{
    uint32 stream = model + r_u32(model + 48u), vertices = model + r_u32(model + 44u);
    uint32 packet_a = model + r_u32(model + 52u), packet_b = model + r_u32(model + 56u);
    while (r_u8(stream + 3u))
    {
        uint32 kind = r_u8(stream + 2u);
        sint32 count = (sint16)r_u16(stream), index;
        stream += 4u;
        if (kind <= 3u)
            for (index = 0; index < count; ++index)
            {
                uint32 vertex[4] = {0}, i, vertex_count = (kind & 1u) ? 4u : 3u;
                uint32 stride = kind < 2u ? 20u : kind == 2u ? 44u : 56u;
                uint32 packet_bytes = kind == 0u ? 32u : kind == 3u ? 52u : 40u;
                for (i = 0u; i < vertex_count; ++i)
                    vertex[i] = vertices + (uint32)(sint32)(sint16)r_u16(stream + i * 2u) * 8u;
                stream += stride;
                effect_polygon(kind, model + 16u, angles, velocity, vertex[0], vertex[1], vertex[2], vertex[3], packet_a, packet_b, flags, native_velocity);
                packet_a += packet_bytes;
                packet_b += packet_bytes;
            }
    }
}

void game_800BC824(uint32 model, uint32 angles, uint32 velocity, uint32 flags)
{
    effect_model(model, angles, velocity, flags, NULL);
}

void dr_effect_model_native(uint32 model, uint32 angles, const VECTOR *velocity, sint32 flags)
{
    effect_model(model, angles, 0, (uint32)flags, velocity);
}

static void effect_copy_words(uint32 destination, uint32 source, uint32 bytes)
{
    uint32 offset;
    for (offset = 0u; offset < bytes; offset += 16u)
    {
        uint32 count = bytes - offset >= 16u ? 4u : (bytes - offset) / 4u;
        uint32 words[4], i;
        for (i = 0u; i < count; ++i)
            words[i] = r_u32(source + offset + i * 4u);
        for (i = 0u; i < count; ++i)
            w_u32(destination + offset + i * 4u, words[i]);
    }
}

static void effect_rotate_vertex(const MATRIX *matrix, uint32 vertex)
{
    uint32 i;
    const uint8 *bytes = (const uint8 *)matrix;
    for (i = 0u; i < 5u; ++i)
    {
        uint32 value = (uint32)bytes[i * 4u] | ((uint32)bytes[i * 4u + 1u] << 8) | ((uint32)bytes[i * 4u + 2u] << 16) | ((uint32)bytes[i * 4u + 3u] << 24);
        xport_gte_write_control(i, value);
    }
    xport_gte_write_data(0u, r_u32(vertex));
    xport_gte_write_data(1u, r_u32(vertex + 4u));
    xport_gte_execute(0x486012u);
    for (i = 0u; i < 3u; ++i)
        w_u16(vertex + i * 2u, (uint16)xport_gte_read_data(9u + i));
}

static void effect_polygon(uint32 kind, uint32 origin, uint32 angles, uint32 velocity, uint32 vertex0, uint32 vertex1, uint32 vertex2, uint32 vertex3, uint32 packet_a, uint32 packet_b, uint32 flags, const VECTOR *native_velocity)
{
    MATRIX matrix;
    sint32 center[4];
    uint32 vertex[4] = {vertex0, vertex1, vertex2, vertex3};
    uint32 effect, i, j, count = (kind & 255u) == 1u || (kind & 255u) == 3u ? 4u : 3u;
    sint32 random, largest = 0;
    uint8 *matrix_bytes = (uint8 *)&matrix;
    for (i = 0u; i < 32u; ++i)
        matrix_bytes[i] = r_u8(0x8008BF58u + i);
    for (i = 0u; i < 4u; ++i)
        center[i] = (sint32)r_u32(0x8008BF78u + i * 4u);
    effect = r_u32(0x800C89E8u);
    if (effect == 0x800C89E8u)
        return;
    w_u32(0x800C89E8u, r_u32(effect));
    w_u32(r_u32(effect) + 4u, 0x800C89E8u);
    w_u32(effect + 4u, 0x800C8AB4u);
    w_u32(effect, r_u32(0x800C8AB4u));
    w_u32(r_u32(0x800C8AB4u) + 4u, effect);
    w_u32(0x800C8AB4u, effect);
    random = (sint32)randPSX();
    w_u32(effect + 96u, flags);
    w_u32(effect + 92u, 0u);
    w_u16(effect + 24u, 0u);
    w_u16(effect + 26u, 0u);
    w_u16(effect + 28u, 0u);
    w_u8(effect + 8u, (uint8)(random % 30 + 30));
    for (i = 0u; i < 3u; ++i)
    {
        random = (sint32)randPSX();
        w_u16(effect + 32u + i * 2u, (uint16)(32 - random % 64));
    }
    for (i = 0u; i < count; ++i)
    {
        uint32 words[2] = {r_u32(vertex[i]), r_u32(vertex[i] + 4u)};
        w_u32(effect + 60u + i * 8u, words[0]);
        w_u32(effect + 64u + i * 8u, words[1]);
    }
    if (angles)
    {
        sub_8003C960((sint16 *)&matrix, (const sint16 *)psx_addr(angles, 6u));
        for (i = 0u; i < count; ++i)
            effect_rotate_vertex(&matrix, effect + 60u + i * 8u);
    }
    for (i = 0u; i < count; ++i)
        for (j = 0u; j < 3u; ++j)
            center[j] = collision_add(center[j], (sint16)r_u16(effect + 60u + i * 8u + j * 2u));
    for (j = 0u; j < 3u; ++j)
    {
        sint32 magnitude;
        center[j] = collision_div(center[j], (sint32)count);
        w_u16(effect + 52u + j * 2u, (uint16)center[j]);
        magnitude = (sint16)center[j];
        if (magnitude < 0)
            magnitude = -magnitude;
        if (magnitude > largest)
            largest = magnitude;
    }
    for (j = 0u; j < 3u; ++j)
        w_u16(effect + 52u + j * 2u, (uint16)collision_div((sint16)r_u16(effect + 52u + j * 2u) * 32, largest));
    if (velocity || native_velocity)
        for (j = 0; j < 3; ++j)
        {
            sint32 input = native_velocity ? (j == 0 ? native_velocity->vx : j == 1 ? native_velocity->vy : native_velocity->vz) : r_s32(velocity + j * 4);
            w_u16(effect + 52 + j * 2, (uint16)(r_u16(effect + 52 + j * 2) + (input >> 8)));
        }
    for (j = 0u; j < 3u; ++j)
        w_u32(effect + 40u + j * 4u, ((uint32)center[j] << 8) + r_u32(origin + j * 4u));
    for (i = 0u; i < count; ++i)
        for (j = 0u; j < 3u; ++j)
            w_u16(effect + 60u + i * 8u + j * 2u, (uint16)(r_u16(effect + 60u + i * 8u + j * 2u) - (uint32)center[j]));
    w_u32(effect + 12u, 0u);
    w_u32(effect + 16u, 0u);
    w_u32(effect + 20u, 0u);
    if ((kind & 255u) <= 3u)
    {
        uint32 bytes = (kind & 255u) == 0u ? 32u : (kind & 255u) == 3u ? 52u : 40u;
        w_u8(effect + 9u, (uint8)kind);
        effect_copy_words(effect + 100u, packet_a, bytes);
        effect_copy_words(effect + 100u + bytes, packet_b, bytes);
    }
}

void game_800BB800(uint32 kind, uint32 origin, uint32 angles, uint32 velocity, uint32 vertex0, uint32 vertex1, uint32 vertex2, uint32 vertex3, uint32 packet_a, uint32 packet_b, uint32 flags)
{
    effect_polygon(kind, origin, angles, velocity, vertex0, vertex1, vertex2, vertex3, packet_a, packet_b, flags, NULL);
}
