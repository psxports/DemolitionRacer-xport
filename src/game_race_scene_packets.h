#ifndef DR_SCENE_PACKETS_H
#define DR_SCENE_PACKETS_H
#include "draft_signatures.h"

typedef struct SceneSubdivisionVertex
{
    sint16 x, y, z, clip;
    uint32 color;
    sint16 depth, uv;
} SceneSubdivisionVertex;

typedef struct SceneSubdivisionHeader
{
    uint32 ordering, color;
    uint16 tpage, clut;
} SceneSubdivisionHeader;

typedef struct SceneSubdivisionFace
{
    SceneSubdivisionVertex *vertices[4];
    sint16 edges[4];
} SceneSubdivisionFace;

uint32 scene_80044024(SceneSubdivisionHeader *, SceneSubdivisionFace *, uint32);
uint32 scene_8004442C(SceneSubdivisionHeader *, SceneSubdivisionFace *, uint32);
uint32 scene_80051BC0(const SceneSubdivisionHeader *, SceneSubdivisionVertex *const *);
uint32 scene_80051C90(const SceneSubdivisionHeader *, SceneSubdivisionVertex *const *);
uint32 scene_80051F60(SceneSubdivisionVertex *, const SceneSubdivisionVertex *, const SceneSubdivisionVertex *);
uint32 scene_80051D7C(const SceneSubdivisionHeader *, SceneSubdivisionVertex *const *);
uint32 scene_80051E5C(const SceneSubdivisionHeader *, SceneSubdivisionVertex *const *);
uint32 scene_8005203C(SceneSubdivisionVertex *, const SceneSubdivisionVertex *, const SceneSubdivisionVertex *);
#endif
