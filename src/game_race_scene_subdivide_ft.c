#include "game_race_scene_packets.h"
#define R32(a) (*(uint32*)psx_addr((a),4u))
static sint16 twice(sint16 x) { return (sint16)((uint32)(sint32)x*2u); }
static void face_set(SceneSubdivisionFace *f,SceneSubdivisionVertex *a,SceneSubdivisionVertex *b,SceneSubdivisionVertex *c,SceneSubdivisionVertex *d,sint16 e0,sint16 e1,sint16 e2,sint16 e3)
{ f->vertices[0]=a;f->vertices[1]=b;f->vertices[2]=c;f->vertices[3]=d;f->edges[0]=e0;f->edges[1]=e1;f->edges[2]=e2;f->edges[3]=e3; }
static int edge_near(SceneSubdivisionVertex *a,SceneSubdivisionVertex *b,sint16 edge)
{ return (sint32)(((uint32)(sint32)a->depth+(uint32)(sint32)b->depth)*(uint32)(sint32)edge)<(sint32)R32(0x80065190u); }
uint32 scene_80044024(SceneSubdivisionHeader *h,SceneSubdivisionFace *f,uint32 level)
{
    uint32 mask=0u,rotation,type,n=level+1u; SceneSubdivisionFace c;
    SceneSubdivisionVertex a,b,d; sint16 e0,e1,e2; SceneSubdivisionFace old;
    if ((uint16)(f->vertices[0]->clip&f->vertices[1]->clip&f->vertices[2]->clip)) return 0u;
    if ((sint32)n<3) { if(edge_near(f->vertices[0],f->vertices[1],f->edges[0]))mask|=1u;if(edge_near(f->vertices[1],f->vertices[2],f->edges[1]))mask|=2u;if(edge_near(f->vertices[2],f->vertices[0],f->edges[2]))mask|=4u; }
    rotation=R32(0x8005D638u+mask*8u); old=*f;
    if(rotation==1u)face_set(f,old.vertices[1],old.vertices[2],old.vertices[0],0,old.edges[1],old.edges[2],old.edges[0],0);
    else if(rotation==2u)face_set(f,old.vertices[2],old.vertices[0],old.vertices[1],0,old.edges[2],old.edges[0],old.edges[1],0);
    type=R32(0x8005D634u+mask*8u);
    if(type==0u){if((sint32)n<2)return 1u;scene_80051BC0(h,f->vertices);return 0u;}
    if(type>3u)return 0u;
    scene_80051F60(&a,f->vertices[0],f->vertices[1]);
    if(type==1u){ e0=twice(f->edges[0]);e2=f->edges[2];
        face_set(&c,f->vertices[0],&a,f->vertices[2],0,e0,e2,f->edges[2],0);scene_80044024(h,&c,n);
        face_set(&c,f->vertices[2],&a,f->vertices[1],0,e2,e0,f->edges[1],0);scene_80044024(h,&c,n);return 0u; }
    scene_80051F60(&b,f->vertices[1],f->vertices[2]);
    if(type==3u)scene_80051F60(&d,f->vertices[2],f->vertices[0]);
    e0=twice(f->edges[0]);e1=twice(f->edges[1]);e2=twice(f->edges[2]);
    if(type==2u){face_set(&c,f->vertices[0],&a,f->vertices[2],&b,e0,e2,e1,f->edges[2]);scene_8004442C(h,&c,n);face_set(&c,&a,f->vertices[1],&b,0,e0,e1,e2,0);scene_80044024(h,&c,n);}
    else {face_set(&c,f->vertices[2],&d,&b,&a,e2,e0,e2,e1);scene_8004442C(h,&c,n);face_set(&c,f->vertices[0],&a,&d,0,e0,e0,e2,0);scene_80044024(h,&c,n);face_set(&c,f->vertices[1],&b,&a,0,e1,e2,e0,0);scene_80044024(h,&c,n);}
    return 0u;
}
uint32 scene_8004442C(SceneSubdivisionHeader *h,SceneSubdivisionFace *f,uint32 level)
{
    uint32 mask=0u,rotation,type,n=level+1u; SceneSubdivisionFace c,old;
    SceneSubdivisionVertex a,b,d,e,g; sint16 e0,e1,e2,e3;
    if((uint16)(f->vertices[0]->clip&f->vertices[1]->clip&f->vertices[2]->clip&f->vertices[3]->clip))return 0u;
    if((sint32)n<3){if(edge_near(f->vertices[0],f->vertices[1],f->edges[0]))mask|=1u;if(edge_near(f->vertices[1],f->vertices[3],f->edges[1]))mask|=2u;if(edge_near(f->vertices[3],f->vertices[2],f->edges[2]))mask|=4u;if(edge_near(f->vertices[2],f->vertices[0],f->edges[3]))mask|=8u;}
    rotation=R32(0x8005D5B8u+mask*8u);old=*f;
    if(rotation==1u)face_set(f,old.vertices[1],old.vertices[3],old.vertices[0],old.vertices[2],old.edges[1],old.edges[2],old.edges[3],old.edges[0]);
    else if(rotation==2u)face_set(f,old.vertices[3],old.vertices[2],old.vertices[1],old.vertices[0],old.edges[2],old.edges[3],old.edges[0],old.edges[1]);
    else if(rotation==3u)face_set(f,old.vertices[2],old.vertices[0],old.vertices[3],old.vertices[1],old.edges[3],old.edges[0],old.edges[1],old.edges[2]);
    type=R32(0x8005D5B4u+mask*8u);
    if(type==0u){if((sint32)n<2)return 1u;scene_80051C90(h,f->vertices);return 0u;}if(type>5u)return 0u;
    scene_80051F60(&a,f->vertices[0],f->vertices[1]);
    if(type==1u){e1=f->edges[1];e0=twice(f->edges[0]);face_set(&c,f->vertices[0],&a,f->vertices[2],f->vertices[3],e0,e1,f->edges[2],f->edges[3]);scene_8004442C(h,&c,n);face_set(&c,&a,f->vertices[1],f->vertices[3],0,e0,f->edges[1],e1,0);scene_80044024(h,&c,n);return 0u;}
    if(type==3u)scene_80051F60(&b,f->vertices[3],f->vertices[2]);else scene_80051F60(&b,f->vertices[1],f->vertices[3]);
    if(type>=4u)scene_80051F60(&d,f->vertices[3],f->vertices[2]);
    if(type==5u){scene_80051F60(&e,f->vertices[2],f->vertices[0]);scene_80051F60(&g,&a,&d);}
    e0=twice(f->edges[0]);e1=twice(f->edges[1]);e2=twice(f->edges[2]);e3=f->edges[3];
    if(type==2u){face_set(&c,f->vertices[0],&a,f->vertices[2],&b,e0,e0,e3,f->edges[3]);scene_8004442C(h,&c,n);face_set(&c,&a,f->vertices[1],&b,0,e0,e1,e0,0);scene_80044024(h,&c,n);face_set(&c,&b,f->vertices[3],f->vertices[2],0,e1,f->edges[2],e3,0);scene_80044024(h,&c,n);}
    else if(type==3u){face_set(&c,f->vertices[0],&a,f->vertices[2],&b,e0,e3,e2,f->edges[3]);scene_8004442C(h,&c,n);face_set(&c,f->vertices[3],&b,f->vertices[1],&a,e2,e3,e0,f->edges[1]);scene_8004442C(h,&c,n);}
    else if(type==4u){face_set(&c,f->vertices[0],&a,f->vertices[2],&d,e0,e3,e2,f->edges[3]);scene_8004442C(h,&c,n);face_set(&c,f->vertices[1],&b,&a,&d,e1,e2,e3,e0);scene_8004442C(h,&c,n);face_set(&c,&b,f->vertices[3],&d,0,e1,e2,e2,0);scene_80044024(h,&c,n);}
    else {e3=twice(e3);face_set(&c,f->vertices[0],&a,&e,&g,e0,e1,e0,e3);scene_8004442C(h,&c,n);face_set(&c,f->vertices[1],&b,&a,&g,e1,e2,e1,e0);scene_8004442C(h,&c,n);face_set(&c,f->vertices[2],&e,&d,&g,e3,e0,e3,e2);scene_8004442C(h,&c,n);face_set(&c,f->vertices[3],&d,&b,&g,e2,e3,e2,e1);scene_8004442C(h,&c,n);}
    return 0u;
}
