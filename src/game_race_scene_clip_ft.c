#include "draft_signatures.h"
#include "game_race_scene_packets.h"

static void ft_vertex_load(uint32 index, const SceneSubdivisionVertex *vertex)
{
    xport_gte_write_data(index * 2u, (uint16)vertex->x | ((uint32)(uint16)vertex->y << 16));
    xport_gte_write_data(index * 2u + 1u, (uint32)(sint32)vertex->z);
}
static uint32 ft_link(uint32 packet, uint32 ordering, uint32 depth, uint32 words, uint32 size)
{
    uint32 slot = ordering + depth * 4u;
    uint32 tag = r_u32(slot);
    uint32 result = (tag & 0xFF000000u) | (packet & 0xFFFFFFu);
    w_u32(packet, (tag & 0xFFFFFFu) | (words << 24));
    w_u32(slot, result);
    w_u32(0x80065570u, packet + size);
    return result;
}
uint32 scene_80051BC0(const SceneSubdivisionHeader *header, SceneSubdivisionVertex *const *vertices)
{
    uint32 packet, depth;
    ft_vertex_load(1u, vertices[1]);
    ft_vertex_load(2u, vertices[2]);
    ft_vertex_load(0u, vertices[0]);
    packet = r_u32(0x80065570u);
    xport_gte_execute(0x280030u);
    w_u16(packet + 20u, (uint16)vertices[1]->uv);
    w_u16(packet + 14u, header->clut);
    w_u32(packet + 4u, header->color | 0x24000000u);
    w_u32(packet + 8u, xport_gte_read_data(12u));
    w_u32(packet + 16u, xport_gte_read_data(13u));
    w_u32(packet + 24u, xport_gte_read_data(14u));
    w_u16(packet + 12u, (uint16)vertices[0]->uv);
    xport_gte_execute(0x158002Du);
    w_u16(packet + 22u, header->tpage);
    w_u16(packet + 28u, (uint16)vertices[2]->uv);
    depth = xport_gte_read_data(7u);
    if ((sint32)depth <= 0) return header->ordering;
    return ft_link(packet, header->ordering, depth, 7u, 32u);
}
uint32 scene_80051C90(const SceneSubdivisionHeader *header, SceneSubdivisionVertex *const *vertices)
{
    uint32 packet, depth;
    ft_vertex_load(1u, vertices[1]);
    ft_vertex_load(2u, vertices[2]);
    ft_vertex_load(0u, vertices[0]);
    packet = r_u32(0x80065570u);
    xport_gte_execute(0x280030u);
    w_u16(packet + 20u, (uint16)vertices[1]->uv);
    w_u16(packet + 14u, header->clut);
    w_u32(packet + 4u, header->color | 0x2C000000u);
    w_u32(packet + 8u, xport_gte_read_data(12u));
    w_u32(packet + 16u, xport_gte_read_data(13u));
    w_u32(packet + 24u, xport_gte_read_data(14u));
    ft_vertex_load(0u, vertices[3]);
    xport_gte_execute(0x180001u);
    w_u16(packet + 12u, (uint16)vertices[0]->uv);
    w_u16(packet + 22u, header->tpage);
    w_u16(packet + 28u, (uint16)vertices[2]->uv);
    w_u16(packet + 36u, (uint16)vertices[3]->uv);
    xport_gte_execute(0x168002Eu);
    depth = xport_gte_read_data(7u);
    if ((sint32)depth <= 0) return header->ordering;
    w_u32(packet + 32u, xport_gte_read_data(14u));
    return ft_link(packet, header->ordering, depth, 9u, 40u);
}
uint32 scene_80051F60(SceneSubdivisionVertex *output, const SceneSubdivisionVertex *left,
    const SceneSubdivisionVertex *right)
{
    sint16 x = (sint16)(((sint32)left->x + right->x) >> 1);
    sint16 y = (sint16)(((sint32)left->y + right->y) >> 1);
    sint16 z = (sint16)(((sint32)left->z + right->z) >> 1);
    uint32 screen, clip, u, v;
    sint32 sx, sy, width, height;
    xport_gte_write_data(0u, (uint16)x | ((uint32)(uint16)y << 16));
    xport_gte_write_data(1u, (uint32)(sint32)z);
    output->x = x; output->y = y; output->z = z;
    xport_gte_execute(0x180001u);
    u = ((uint16)left->uv & 255u) + ((uint16)right->uv & 255u);
    output->depth = (sint16)(((sint32)left->depth + right->depth) >> 1);
    v = ((uint16)left->uv >> 8) + ((uint16)right->uv >> 8);
    output->uv = (sint16)((u >> 1) | ((v >> 1) << 8));
    screen = xport_gte_read_data(14u);
    sx = (sint16)screen; sy = (sint16)(screen >> 16);
    width = (sint32)r_u32(0x8006511Cu); height = (sint32)r_u32(0x80065120u);
    clip = sx < 0 ? 1u : 0u;
    if (sx >= width) clip |= 2u;
    if (sy < 0) clip |= 4u;
    if (sy >= height) clip |= 8u;
    output->clip = (sint16)clip;
    return (uint32)(sy < height);
}
