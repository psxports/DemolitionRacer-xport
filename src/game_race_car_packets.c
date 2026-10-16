#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
extern uint32 scene_packet_far_core(uint32 packet, sint32 dx, sint32 dy, sint32 dz);

static sint32 car_sub(uint32 a, uint32 b)
{
    uint32 result = a - b;
    if (((a ^ b) & (a ^ result) & 0x80000000u) != 0u)
    {
        draft_agent1_missing_diagnostic("Car packet original signed SUB overflow\n");
        abort();
    }
    return (sint32)result;
}

static uint32 car_add(uint32 a, uint32 b)
{
    uint32 result = a + b;
    if ((~(a ^ b) & (a ^ result) & 0x80000000u) != 0u)
    {
        draft_agent1_missing_diagnostic("Car packet original signed ADD overflow\n");
        abort();
    }
    return result;
}

static SVECTOR car_vertex(uint32 address, sint32 dx, sint32 dy, sint32 dz)
{
    SVECTOR v;
    uint32 packed = R32(address);
    v.vx = (sint16)(car_add(packed, (uint32)dx) & 0xFFFFu);
    v.vy = (sint16)((car_add(packed, ((uint32)dy << 16)) & 0xFFFF0000u) >> 16);
    v.vz = (sint16)car_add(R32(address + 4u), (uint32)dz);
    v.pad = 0;
    return v;
}

void scene_8004FCBC(uint32 packet)
{
    PsxGteSnapshot saved;
    uint32 i;
    SVECTOR position;
    position.vx = (sint16)car_sub(R32(packet + 16u), R32(0x80081BB8u));
    position.vy = (sint16)car_sub(R32(packet + 20u), R32(0x80081BBCu));
    position.vz = (sint16)car_sub(R32(packet + 24u), R32(0x80081BC0u));
    position.pad = 0;
    xport_gte_write_data(0u, (uint16)position.vx | ((uint32)(uint16)position.vy << 16));
    xport_gte_write_data(1u, (uint16)position.vz);
    xport_gte_mvmva(0x486012u);
    for (i = 0u; i < 3u; ++i)
        xport_gte_write_control(5u + i, xport_gte_read_data(25u + i));
    psx_gte_snapshot(&saved);
    for (i = 0u; i < 5u; ++i)
        xport_gte_write_control(i, R32(0x80081BE4u + i * 4u));
    scene_packet_far_core(packet, 0, 0, 0);
    SetRotMatrix(&saved.rotation);
}

void scene_80050700(uint32 packet)
{
    sint32 dx, dy, dz, shift, limit;
    uint32 group, vertices, primitive, ordering, type;
    uint32 i;
    for (i = 0u; i < 3u; ++i)
        xport_gte_write_control(5u + i, 0u);
    dx = car_sub((uint32)((sint32)R32(packet + 16u) >> 8), R32(0x80081BB8u));
    dy = car_sub((uint32)((sint32)R32(packet + 20u) >> 8), R32(0x80081BBCu));
    dz = car_sub((uint32)((sint32)R32(packet + 24u) >> 8), R32(0x80081BC0u));
    group = packet + R32(packet + 48u);
    vertices = packet + R32(packet + 44u);
    primitive = packet + R32(packet + 52u + R32(0x80065128u) * 4u);
    shift = car_sub(R32(0x80065134u), R32(0x80065170u));
    ordering = R32(R32(0x80065118u) + 112u) + ((uint32)((sint32)(sint16)R16(packet + 12u) >> (shift & 31)) * 4u);
    limit = (sint32)R32(0x8006554Cu);
    for (;;)
    {
        sint32 count = (sint16)R16(group);
        uint32 first = 1u;
        type = R8(group + 2u);
        if (!count)
            return;
        if (type >= 4u)
        {
            draft_agent1_missing_diagnostic("TODO Car50700 primitive table index %u\n", type);
            abort();
        }
        group += 4u;
        do
        {
            SVECTOR input[3], fourth;
            sint32 screen[3], depths[3], screen4, depth4, area, depth;
            uint32 indices[3], stride = (type == 0u || type == 1u) ? 20u : (type == 2u ? 44u : 56u);
            uint32 packet_size = type == 0u ? 32u : (type == 3u ? 52u : 40u);
            uint32 xy_stride = type < 2u ? 8u : 12u;
            int quad = (type == 1u || type == 3u);
            for (i = 0u; i < 3u; ++i)
            {
                sint32 index;
                if (quad && !first)
                    index = (sint16)R16(group + i * 2u);
                else if (type == 1u)
                    index = R8(group + i * 2u);
                else
                    index = (sint8)R8(group + i * 2u);
                indices[i] = vertices + (uint32)index * 8u;
                input[i] = car_vertex(indices[i], dx, dy, dz);
            }
            gte_project3_full_depth(input, screen, depths, NULL);
            area = NormalClip(screen[0], screen[1], screen[2]);
            if (!quad)
            {
                depth = AverageZ3(depths[0], depths[1], depths[2]);
                if (area <= 0 && depth > 0)
                {
                    R32(primitive + 8u) = (uint32)screen[0];
                    if (depth < limit)
                    {
                        uint32 slot = ordering + ((uint32)(depth >> (shift & 31)) * 4u), head;
                        R32(primitive + 8u + xy_stride) = (uint32)screen[1];
                        R32(primitive + 8u + xy_stride * 2u) = (uint32)screen[2];
                        head = R32(slot);
                        R32(slot) = primitive & 0xFFFFFFu;
                        R32(primitive) = (type == 0u ? 0x07000000u : 0x09000000u) | (head & 0xFFFFFFu);
                    }
                }
            }
            else
            {
                sint32 fourth_index = type == 1u ? (sint32)R8(group + 6u) : (sint32)(sint8)R8(group + 6u);
                R32(primitive + 8u) = (uint32)screen[0];
                R32(primitive + 8u + xy_stride) = (uint32)screen[1];
                R32(primitive + 8u + xy_stride * 2u) = (uint32)screen[2];
                fourth = car_vertex(vertices + (uint32)fourth_index * 8u, dx, dy, dz);
                depth4 = gte_project_full_depth(&fourth, &screen4, NULL);
                if (area <= 0 || NormalClip(screen[1], screen[2], screen4) >= 0)
                {
                    depth = AverageZ4(depths[0], depths[1], depths[2], depth4);
                    R32(primitive + 8u + xy_stride * 3u) = (uint32)screen4;
                    if (depth > 0 && depth < limit)
                    {
                        uint32 slot = ordering + ((uint32)(depth >> (shift & 31)) * 4u), head = R32(slot);
                        R32(slot) = primitive & 0xFFFFFFu;
                        R32(primitive) = (type == 1u ? 0x09000000u : 0x0C000000u) | (head & 0xFFFFFFu);
                    }
                }
            }
            count = (sint32)((uint32)count - 1u);
            group += stride;
            primitive += packet_size;
            first = 0u;
        } while (count);
    }
}
