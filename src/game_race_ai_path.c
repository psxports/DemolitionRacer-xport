#include "draft_signatures.h"
#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
extern void dr_race_normalize(VECTOR *input, SVECTOR *output);

static uint32 path_shift(uint32 value, uint32 shift)
{
    if ((sint32)value < 0)
        value += (1u << shift) - 1u;
    return (uint32)((sint32)value >> shift);
}

void game_800B5F90(sint32 segment, uint32 lane, VECTOR *output, sint32 offset)
{
    uint32 descriptor = R32(0x800653D4u) + (uint32)segment * 24u, table = R32(0x800653C4u);
    uint32 first_index = R16(descriptor + 4u), shape = R8(descriptor), count = R8(descriptor + 3u) & 15u;
    uint32 translation_x = R32(descriptor + 12u), adjust = R32(0x800BF998u + shape * 8u);
    uint32 first = table + first_index * 6u, second = table + (first_index + count + adjust) * 6u;
    uint32 x0 = translation_x + (uint32)(sint32)(sint16)R16(first), x1 = translation_x + (uint32)(sint32)(sint16)R16(second);
    uint32 dx = x1 - x0, translation_z = R32(descriptor + 20u);
    uint32 z0 = translation_z + (uint32)(sint32)(sint16)R16(first + 4u), z1 = translation_z + (uint32)(sint32)(sint16)R16(second + 4u), dz = z1 - z0;
    VECTOR delta;
    SVECTOR normalized;
    output->vx = (sint32)((x0 << 8) + lane * dx);
    output->vz = (sint32)((z0 << 8) + lane * dz);
    delta.vx = (sint16)dx;
    delta.vy = 0;
    delta.vz = (sint16)dz;
    delta.pad = 0;
    dr_race_normalize(&delta, &normalized);
    output->vx = (sint32)((uint32)output->vx + (path_shift((uint32)(sint32)normalized.vx * (uint32)offset, 12u) << 8));
    output->vz = (sint32)((uint32)output->vz + (path_shift((uint32)(sint32)normalized.vz * (uint32)offset, 12u) << 8));
}

void game_800B60F8(uint32 car, sint32 mode)
{
    uint32 ai = 0x800C7698u + car * 104u, object = 0x80066D14u + car * 952u;
    sint32 index = (sint16)R16(object + 90u);
    uint32 route = R32(ai);
    uint32 factor = R8(route + (uint32)index * 3u + 2u), scale = (uint32)(sint32)(sint16)R16(ai + 58u);
    uint32 throttle = (uint32)(sint32)(sint16)R16(ai + 20u), amount = path_shift(factor * scale, 8u);
    if ((sint16)R16(ai + 54u) < (sint32)amount)
        R16(object + 16u) = (uint16)(throttle + (uint32)mode);
    else
        R16(object + 16u) = 0u;
    R8(object + 19u) = 0u;
    R8(object + 21u) = 0u;
}
