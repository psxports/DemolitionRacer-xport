#include "game_race_scene_packets.h"

uint32 scene_80044B68(SceneSubdivisionHeader *, SceneSubdivisionFace *, uint32);
uint32 scene_80044F70(SceneSubdivisionHeader *, SceneSubdivisionFace *, uint32);
uint32 scene_80051D7C(const SceneSubdivisionHeader *, SceneSubdivisionVertex *const *);
uint32 scene_80051E5C(const SceneSubdivisionHeader *, SceneSubdivisionVertex *const *);
uint32 scene_8005203C(SceneSubdivisionVertex *, const SceneSubdivisionVertex *, const SceneSubdivisionVertex *);

static void gt_child(SceneSubdivisionHeader *h, uint32 level, uint32 quad, SceneSubdivisionVertex *a, SceneSubdivisionVertex *b, SceneSubdivisionVertex *c, SceneSubdivisionVertex *d, sint16 e0, sint16 e1, sint16 e2, sint16 e3)
{
    SceneSubdivisionFace child = {{a, b, c, d}, {e0, e1, e2, e3}};
    if (quad)
        scene_80044F70(h, &child, level);
    else
        scene_80044B68(h, &child, level);
}

uint32 scene_80044B68(SceneSubdivisionHeader *h, SceneSubdivisionFace *f, uint32 level)
{
    uint32 mask = 0u, rotation, mode, i;
    SceneSubdivisionVertex midpoint[3], *a, *b, *c;
    sint16 e0, e1, e2, d0, d1, d2;
    if (f->vertices[0]->clip & f->vertices[1]->clip & f->vertices[2]->clip)
        return 0u;
    ++level;
    if ((sint32)level < 3)
        for (i = 0u; i < 3u; ++i)
        {
            sint32 product = (sint32)((uint32)((sint32)f->vertices[i]->depth + f->vertices[(i + 1u) % 3u]->depth) * (uint32)(sint32)f->edges[i]);
            if (product < (sint32)r_u32(0x80065190u))
                mask |= 1u << i;
        }
    rotation = r_u32(0x8005D638u + mask * 8u);
    if (rotation == 1u || rotation == 2u)
    {
        SceneSubdivisionFace original = *f;
        for (i = 0u; i < 3u; ++i)
        {
            f->vertices[i] = original.vertices[(i + rotation) % 3u];
            f->edges[i] = original.edges[(i + rotation) % 3u];
        }
    }
    mode = r_u32(0x8005D634u + mask * 8u);
    if (!mode)
    {
        if ((sint32)level < 2)
            return 1u;
        scene_80051D7C(h, f->vertices);
        return 0u;
    }
    if (mode > 3u)
        return 0u;
    a = f->vertices[0];
    b = f->vertices[1];
    c = f->vertices[2];
    scene_8005203C(&midpoint[0], a, b);
    e0 = f->edges[0];
    e1 = f->edges[1];
    e2 = f->edges[2];
    d0 = (sint16)(e0 * 2);
    d1 = (sint16)(e1 * 2);
    d2 = (sint16)(e2 * 2);
    if (mode == 1u)
    {
        gt_child(h, level, 0u, a, &midpoint[0], c, 0, d0, e2, e2, 0);
        gt_child(h, level, 0u, c, &midpoint[0], b, 0, e2, d0, f->edges[1], 0);
    }
    else
    {
        scene_8005203C(&midpoint[1], b, c);
        if (mode == 2u)
        {
            gt_child(h, level, 1u, a, &midpoint[0], c, &midpoint[1], d0, d2, d1, e2);
            gt_child(h, level, 0u, &midpoint[0], b, &midpoint[1], 0, d0, d1, d2, 0);
        }
        else
        {
            scene_8005203C(&midpoint[2], c, a);
            gt_child(h, level, 1u, c, &midpoint[2], &midpoint[1], &midpoint[0], d2, d0, d2, d1);
            gt_child(h, level, 0u, a, &midpoint[0], &midpoint[2], 0, d0, d0, d2, 0);
            gt_child(h, level, 0u, b, &midpoint[1], &midpoint[0], 0, d1, d2, d0, 0);
        }
    }
    return 0u;
}

uint32 scene_80044F70(SceneSubdivisionHeader *h, SceneSubdivisionFace *f, uint32 level)
{
    uint32 mask = 0u, rotation, mode, i;
    const uint32 cycle[4] = {0u, 1u, 3u, 2u};
    SceneSubdivisionVertex m[5], *a, *b, *c, *d;
    sint16 e0, e1, e2, e3, d0, d1, d2, d3;
    if (f->vertices[0]->clip & f->vertices[1]->clip & f->vertices[2]->clip & f->vertices[3]->clip)
        return 0u;
    ++level;
    if ((sint32)level < 3)
        for (i = 0u; i < 4u; ++i)
        {
            sint32 product = (sint32)((uint32)((sint32)f->vertices[cycle[i]]->depth + f->vertices[cycle[(i + 1u) & 3u]]->depth) * (uint32)(sint32)f->edges[i]);
            if (product < (sint32)r_u32(0x80065190u))
                mask |= 1u << i;
        }
    rotation = r_u32(0x8005D5B8u + mask * 8u);
    if (rotation > 0u && rotation < 4u)
    {
        SceneSubdivisionFace original = *f;
        for (i = 0u; i < 4u; ++i)
        {
            f->vertices[cycle[i]] = original.vertices[cycle[(i + rotation) & 3u]];
            f->edges[i] = original.edges[(i + rotation) & 3u];
        }
    }
    mode = r_u32(0x8005D5B4u + mask * 8u);
    if (!mode)
    {
        if ((sint32)level < 2)
            return 1u;
        scene_80051E5C(h, f->vertices);
        return 0u;
    }
    if (mode > 5u)
        return 0u;
    a = f->vertices[0];
    b = f->vertices[1];
    c = f->vertices[2];
    d = f->vertices[3];
    scene_8005203C(&m[0], a, b);
    e0 = f->edges[0];
    e1 = f->edges[1];
    e2 = f->edges[2];
    e3 = f->edges[3];
    d0 = (sint16)(e0 * 2);
    d1 = (sint16)(e1 * 2);
    d2 = (sint16)(e2 * 2);
    d3 = (sint16)(e3 * 2);
    switch (mode)
    {
        case 1u:
            gt_child(h, level, 1u, a, &m[0], c, d, d0, e1, e2, e3);
            gt_child(h, level, 0u, &m[0], b, d, 0, d0, f->edges[1], e1, 0);
            break;
        case 2u:
            scene_8005203C(&m[1], b, d);
            gt_child(h, level, 1u, a, &m[0], c, &m[1], d0, d0, e3, e3);
            gt_child(h, level, 0u, &m[0], b, &m[1], 0, d0, d1, d0, 0);
            gt_child(h, level, 0u, &m[1], d, c, 0, d1, f->edges[2], e3, 0);
            break;
        case 3u:
            scene_8005203C(&m[1], d, c);
            gt_child(h, level, 1u, a, &m[0], c, &m[1], d0, e3, d2, e3);
            gt_child(h, level, 1u, d, &m[1], b, &m[0], d2, e3, d0, f->edges[1]);
            break;
        case 4u:
            scene_8005203C(&m[1], b, d);
            scene_8005203C(&m[2], d, c);
            gt_child(h, level, 1u, a, &m[0], c, &m[2], d0, e3, d2, e3);
            gt_child(h, level, 1u, b, &m[1], &m[0], &m[2], d1, d2, e3, d0);
            gt_child(h, level, 0u, &m[1], d, &m[2], 0, d1, d2, d2, 0);
            break;
        case 5u:
            scene_8005203C(&m[1], b, d);
            scene_8005203C(&m[2], d, c);
            scene_8005203C(&m[3], c, a);
            scene_8005203C(&m[4], &m[0], &m[2]);
            gt_child(h, level, 1u, a, &m[0], &m[3], &m[4], d0, d1, d0, d3);
            gt_child(h, level, 1u, b, &m[1], &m[0], &m[4], d1, d2, d1, d0);
            gt_child(h, level, 1u, c, &m[3], &m[2], &m[4], d3, d0, d3, d2);
            gt_child(h, level, 1u, d, &m[2], &m[1], &m[4], d2, d3, d2, d1);
            break;
    }
    return 0u;
}
