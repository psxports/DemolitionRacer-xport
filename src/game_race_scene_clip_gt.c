#include "draft_signatures.h"
#include "game_race_scene_packets.h"

static void gt_vertex_load(uint32 index, const SceneSubdivisionVertex *vertex)
{
    xport_gte_write_data(index * 2u, (uint16)vertex->x | ((uint32)(uint16)vertex->y << 16));
    xport_gte_write_data(index * 2u + 1u, (uint32)(sint32)vertex->z);
}

static uint32 gt_link(uint32 packet, uint32 ordering, uint32 depth, uint32 words, uint32 size)
{
    uint32 slot = ordering + depth * 4u;
    uint32 tag = r_u32(slot);
    uint32 result = (tag & 0xFF000000u) | (packet & 0xFFFFFFu);
    w_u32(packet, (tag & 0xFFFFFFu) | (words << 24));
    w_u32(slot, result);
    w_u32(0x80065570u, packet + size);
    return result;
}

uint32 scene_80051D7C(const SceneSubdivisionHeader *header, SceneSubdivisionVertex *const *vertices)
{
    uint32 packet, depth;
    gt_vertex_load(1u, vertices[1]);
    gt_vertex_load(2u, vertices[2]);
    gt_vertex_load(0u, vertices[0]);
    packet = r_u32(0x80065570u);
    xport_gte_execute(0x280030u);
    w_u16(packet + 24u, (uint16)vertices[1]->uv);
    w_u16(packet + 14u, header->clut);
    w_u16(packet + 12u, (uint16)vertices[0]->uv);
    w_u16(packet + 26u, header->tpage);
    w_u32(packet + 4u, vertices[0]->color | 0x34000000u);
    w_u32(packet + 16u, vertices[1]->color);
    w_u32(packet + 8u, xport_gte_read_data(12u));
    w_u32(packet + 20u, xport_gte_read_data(13u));
    w_u32(packet + 32u, xport_gte_read_data(14u));
    w_u32(packet + 28u, vertices[2]->color);
    xport_gte_execute(0x158002Du);
    w_u16(packet + 36u, (uint16)vertices[2]->uv);
    depth = xport_gte_read_data(7u);
    if ((sint32)depth <= 0)
        return header->ordering;
    return gt_link(packet, header->ordering, depth, 9u, 40u);
}

uint32 scene_80051E5C(const SceneSubdivisionHeader *header, SceneSubdivisionVertex *const *vertices)
{
    uint32 packet, depth;
    gt_vertex_load(1u, vertices[1]);
    gt_vertex_load(2u, vertices[2]);
    gt_vertex_load(0u, vertices[0]);
    packet = r_u32(0x80065570u);
    xport_gte_execute(0x280030u);
    w_u16(packet + 24u, (uint16)vertices[1]->uv);
    w_u16(packet + 14u, header->clut);
    w_u32(packet + 4u, vertices[0]->color | 0x3C000000u);
    w_u32(packet + 16u, vertices[1]->color);
    w_u32(packet + 28u, vertices[2]->color);
    w_u32(packet + 40u, vertices[3]->color);
    w_u32(packet + 8u, xport_gte_read_data(12u));
    w_u32(packet + 20u, xport_gte_read_data(13u));
    w_u32(packet + 32u, xport_gte_read_data(14u));
    gt_vertex_load(0u, vertices[3]);
    xport_gte_execute(0x180001u);
    w_u16(packet + 12u, (uint16)vertices[0]->uv);
    w_u16(packet + 26u, header->tpage);
    w_u16(packet + 36u, (uint16)vertices[2]->uv);
    w_u16(packet + 48u, (uint16)vertices[3]->uv);
    xport_gte_execute(0x168002Eu);
    depth = xport_gte_read_data(7u);
    if ((sint32)depth <= 0)
        return header->ordering;
    w_u32(packet + 44u, xport_gte_read_data(14u));
    return gt_link(packet, header->ordering, depth, 12u, 52u);
}

uint32 scene_8005203C(SceneSubdivisionVertex *output, const SceneSubdivisionVertex *left, const SceneSubdivisionVertex *right)
{
    uint8 *destination = (uint8 *)&output->color;
    const uint8 *a = (const uint8 *)&left->color;
    const uint8 *b = (const uint8 *)&right->color;
    uint32 red = ((uint32)a[0] + b[0]) >> 1;
    uint32 green = ((uint32)a[1] + b[1]) >> 1;
    destination[0] = (uint8)red;
    destination[1] = (uint8)green;
    destination[2] = (uint8)(((uint32)a[2] + b[2]) >> 1);
    destination[3] = b[3];
    /* Remaining midpoint instructions are identical to the FT leaf */
    return scene_80051F60(output, left, right);
}
