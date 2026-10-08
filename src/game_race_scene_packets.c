#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>

#include "game_race_scene_packets.h"
uint32 scene_80044B68(SceneSubdivisionHeader*,SceneSubdivisionFace*,uint32);
uint32 scene_80044F70(SceneSubdivisionHeader*,SceneSubdivisionFace*,uint32);

uint32 scene_8004FDDC(uint32 packet);

static uint32 scene_packet_pending(uint32 target, uint32 packet)
{
    fprintf(stderr,"TODO Scene packet helper %08X packet=%08X\n",target,packet);
    abort();
    return 0u;
}

static void scene_packet_vertex(SceneSubdivisionVertex *vertex,uint32 address,
    uint32 dx,uint32 dy,uint32 dz)
{
    vertex->x=(sint16)(r_u16(address)+dx);
    vertex->y=(sint16)(r_u16(address+2u)+dy);
    vertex->z=(sint16)(r_u16(address+4u)+dz);
}

static void scene_packet_load_vertex(const SceneSubdivisionVertex *vertex,uint32 slot)
{
    xport_gte_write_data(slot,(uint16)vertex->x|((uint32)(uint16)vertex->y<<16));
    xport_gte_write_data(slot+1u,(uint16)vertex->z);
}

static sint16 scene_packet_clip(uint32 screen)
{
    sint32 x=(sint16)screen, y=(sint16)(screen>>16);
    uint32 result=x<0?1u:0u;
    if(x>=(sint32)r_u32(0x8006511Cu)) result|=2u;
    if(y<0) result|=4u;
    if(y>=(sint32)r_u32(0x80065120u)) result|=8u;
    return (sint16)result;
}

uint32 scene_800456AC(uint32 packet)
{
    uint32 stream,vertices,ordering,base,dx,dy,dz;
    sint32 maximum;
    uint32 i;
    if((sint32)r_u32(0x80065548u)>=(sint16)r_u16(packet+4u))
        return scene_8004FDDC(packet);
    for(i=0u;i<3u;++i) xport_gte_write_control(5u+i,r_u32(0x800123FCu+i*4u));
    dx=r_u32(packet+16u)-r_u32(0x80081BB8u);
    dy=r_u32(packet+20u)-r_u32(0x80081BBCu);
    dz=r_u32(packet+24u)-r_u32(0x80081BC0u);
    stream=packet+r_u32(packet+48u);
    vertices=packet+r_u32(packet+44u);
    base=r_u32(r_u32(0x80065118u)+0x70u);
    ordering=base+(uint32)((sint16)r_u16(packet+12u)>>((r_u32(0x80065134u)-r_u32(0x80065170u))&31u))*4u;
    maximum=(sint32)r_u32(0x8006514Cu)>>4;
    if(!r_u8(stream+3u)) return base;
    while(r_u8(stream+3u)) {
        sint32 count=(sint16)r_u16(stream),index;
        uint32 kind=r_u8(stream+2u);
        stream+=4u;
        for(index=0;index<count;++index) {
            SceneSubdivisionVertex vertex[4]={{0}};
            SceneSubdivisionFace face={{&vertex[0],&vertex[1],&vertex[2],&vertex[3]},{0}};
            SceneSubdivisionHeader header={0};
            uint32 quad=kind&1u, shaded=kind>>1;
            uint32 vertex_count=quad?4u:3u;
            uint32 stride=kind==0u?20u:kind==3u?28u:24u;
            uint32 output=r_u32(0x80065570u),screen[4]={0},colors[4]={0};
            uint32 screen_offset[4],color_offset[4],uv_offset[4],threshold,command,words,bytes;
            sint32 area,depth;
            if(kind>3u) break;
            for(i=0u;i<vertex_count;++i)
                scene_packet_vertex(&vertex[i],vertices+(uint32)r_u8(stream+(quad?12u:10u)+i)*8u,dx,dy,dz);
            for(i=0u;i<3u;++i) scene_packet_load_vertex(&vertex[i],i*2u);
            xport_gte_execute(0x280030u);
            xport_gte_execute(0x1400006u);
            area=(sint32)xport_gte_read_data(24u);
            if(!quad && area>0) { stream+=stride; continue; }
            for(i=0u;i<vertex_count;++i) {
                screen_offset[i]=8u+i*(shaded?12u:8u);
                color_offset[i]=4u+i*12u;
                uv_offset[i]=12u+i*(shaded?12u:8u);
            }
            if(!quad) xport_gte_execute(0x158002Du);
            for(i=0u;i<3u;++i) {
                screen[i]=xport_gte_read_data(12u+i);
                w_u32(output+screen_offset[i],screen[i]);
            }
            if(quad) {
                scene_packet_load_vertex(&vertex[3],0u);
                xport_gte_execute(0x180001u);
                xport_gte_execute(0x1400006u);
                if(area>0 && (sint32)xport_gte_read_data(24u)<0) { stream+=stride; continue; }
                xport_gte_execute(0x168002Eu);
                screen[3]=xport_gte_read_data(14u);
                w_u32(output+screen_offset[3],screen[3]);
            }
            depth=(sint32)xport_gte_read_data(7u);
            if(depth<0 || depth>=maximum) { stream+=stride; continue; }
            if(!shaded) {
                uint32 color_index=quad?20u:16u;
                colors[0]=r_u32(0x800835D4u+(uint32)r_u8(stream+color_index)*4u)
                    |((uint32)r_u8(stream+color_index+2u)<<24);
                threshold=r_u8(stream+color_index+1u);
            } else {
                uint32 color_index=quad?20u:16u;
                command=((uint32)r_u8(stream+(quad?25u:20u))&3u)<<24;
                for(i=0u;i<vertex_count;++i)
                    colors[i]=r_u32(0x800835D4u+(uint32)r_u8(stream+color_index+i)*4u)|command;
                
                threshold=r_u8(stream+(quad?24u:19u));
            }
            if(depth<(sint32)(threshold*4u)) {
                uint32 emitted;
                header.ordering=ordering;
                header.color=quad?colors[0]&0x03FFFFFFu:colors[0];
                header.tpage=r_u16(stream+6u); header.clut=r_u16(stream+2u);
                for(i=0u;i<4u;++i) face.edges[i]=(sint16)r_u8(stream+(quad?16u:13u)+i);
                for(i=0u;i<vertex_count;++i) {
                    sint32 z=(sint32)xport_gte_read_data((quad?16u:17u)+i);
                    vertex[i].depth=(sint16)(z<128?128:z>2048?2048:z);
                    vertex[i].uv=(sint16)r_u16(stream+(i<2u?i*4u:8u+(i-2u)*2u));
                    vertex[i].clip=scene_packet_clip(screen[i]);
                    if(shaded) vertex[i].color=colors[i];
                }
                emitted=kind==0u?scene_80044024(&header,&face,0u):kind==1u?scene_8004442C(&header,&face,0u)
                    :kind==2u?scene_80044B68(&header,&face,0u):scene_80044F70(&header,&face,0u);
                if(!emitted) { stream+=stride; continue; }
            }
            if(shaded) for(i=0u;i<vertex_count;++i) w_u32(output+color_offset[i],colors[i]|(!i?(quad?0x3C000000u:0x34000000u):0u));
            else w_u32(output+4u,colors[0]);
            w_u32(output+uv_offset[0],r_u32(stream));
            w_u32(output+uv_offset[1],r_u32(stream+4u));
            w_u16(output+uv_offset[2],r_u16(stream+8u));
            if(quad) w_u16(output+uv_offset[3],r_u16(stream+10u));
            words=kind==0u?7u:kind==3u?12u:9u; bytes=(words+1u)*4u;
            w_u32(output,words<<24);
            {
                uint32 bucket=ordering+((uint32)(depth>>((r_u32(0x80065134u)-r_u32(0x80065170u))&31u))*4u);
                w_u32(output,(r_u32(bucket)&0x00FFFFFFu)|(words<<24));
                base=r_u32(bucket)&0xFF000000u;
                w_u32(0x80065570u,r_u32(0x80065570u)+bytes);
                w_u32(bucket,base|(output&0x00FFFFFFu));
            }
            stream+=stride;
        }
    }
    return 0u;
}

uint32 scene_packet_far_core(uint32 packet,sint32 relative_x,sint32 relative_y,sint32 relative_z)
{
    uint32 stream=packet+r_u32(packet+48u),vertex_base=packet+r_u32(packet+44u);
    uint32 output=r_u32(0x80065570u),i,result,ordering;
    uint32 dx=(uint32)relative_x,dy=(uint32)relative_y,dz=(uint32)relative_z;
    uint32 shift=(r_u32(0x80065134u)-r_u32(0x80065170u))&31u;
    sint32 maximum=(sint32)r_u32(0x8006554Cu);
    result=(uint32)((sint16)r_u16(packet+12u)>>shift)*4u;
    ordering=result+r_u32(r_u32(0x80065118u)+0x70u);
    for(;;) {
        sint32 count=(sint16)r_u16(stream),index;
        uint32 selector=r_u8(stream+2u),target;
        if(!count) break;
        target=r_u32(0x8004FCACu+selector*4u);
        result=target; stream+=4u;
        if(target!=0x8004FE90u && target!=0x8004FFDCu && target!=0x800501A8u && target!=0x80050348u)
            return scene_packet_pending(target,packet);
        for(index=0;index<count;++index) {
            uint32 kind=target==0x8004FE90u?0u:target==0x8004FFDCu?1u:target==0x800501A8u?2u:3u;
            uint32 quad=kind&1u,shaded=kind>>1,vertex_count=quad?4u:3u;
            uint32 stride=kind==0u?20u:kind==3u?28u:24u;
            uint32 coordinate_stride=shaded?12u:8u;
            uint32 words=kind==0u?7u:kind==3u?12u:9u;
            uint32 colors[4]={0};
            sint32 area,depth;
            SceneSubdivisionVertex vertex[4]={{0}};
            for(i=0u;i<vertex_count;++i)
                scene_packet_vertex(&vertex[i],vertex_base+(uint32)r_u8(stream+(quad?12u:10u)+i)*8u,dx,dy,dz);
            for(i=0u;i<3u;++i) scene_packet_load_vertex(&vertex[i],i*2u);
            if(shaded) {
                uint32 color_index=quad?20u:16u;
                for(i=0u;i<vertex_count;++i) {
                    colors[i]=r_u32(0x800835D4u+(uint32)r_u8(stream+color_index+i)*4u);
                    if(!i) colors[i]|=(uint32)r_u8(stream+(quad?25u:20u))<<24;
                    w_u32(output+4u+i*12u,colors[i]);
                }
                w_u32(output+12u,r_u32(stream));
                w_u32(output+24u,r_u32(stream+4u));
                w_u16(output+36u,r_u16(stream+8u));
                if(quad) w_u16(output+48u,r_u16(stream+10u));
            }
            xport_gte_execute(0x280030u);
            if(quad) {
                /* Original pipeline prefetches the next face before testing this face */
                for(i=0u;i<3u;++i) {
                    (void)r_u8(stream+stride+12u+i);
                }
            }
            xport_gte_execute(0x1400006u);
            area=(sint32)xport_gte_read_data(24u);
            if(!shaded && !quad) {
                colors[0]=r_u32(0x800835D4u+(uint32)r_u8(stream+16u)*4u)|((uint32)r_u8(stream+18u)<<24);
                w_u32(output+4u,colors[0]);
                w_u32(output+12u,r_u32(stream));
                result=(uint32)area;
            }
            if(quad) {
                if(!shaded) {
                    w_u32(output+12u,r_u32(stream));
                    w_u32(output+20u,r_u32(stream+4u));
                }
                for(i=0u;i<3u;++i) w_u32(output+8u+i*coordinate_stride,xport_gte_read_data(12u+i));
                scene_packet_load_vertex(&vertex[3],0u);
                result=shaded?0x0C000000u:vertex_base+(uint32)r_u8(stream+15u)*8u;
                xport_gte_execute(0x180001u);
                if(!shaded) {
                    colors[0]=r_u32(0x800835D4u+(uint32)r_u8(stream+20u)*4u)|((uint32)r_u8(stream+22u)<<24);
                    w_u32(output+4u,colors[0]);
                }
                if(area>0) {
                    sint32 second_area;
                    xport_gte_execute(0x1400006u);
                    second_area=(sint32)xport_gte_read_data(24u);
                    xport_gte_execute(0x168002Eu);
                    if(second_area<0) { stream+=stride; continue; }
                } else {
                    xport_gte_execute(0x168002Eu);
                }
                depth=(sint32)xport_gte_read_data(7u);
                w_u32(output+8u+3u*coordinate_stride,xport_gte_read_data(14u));
                if(depth<=0 || depth>=maximum) { stream+=stride; continue; }
                if(!shaded) {
                    w_u16(output+28u,r_u16(stream+8u));
                    w_u16(output+36u,r_u16(stream+10u));
                    result=0x09000000u;
                }
            } else {
                /* AVSZ3 executes in the original positive-area branch delay slot */
                xport_gte_execute(0x158002Du);
                if(area>0) { stream+=stride; continue; }
                if(!shaded) {
                    w_u32(output+20u,r_u32(stream+4u));
                    w_u16(output+28u,r_u16(stream+8u));
                }
                depth=(sint32)xport_gte_read_data(7u);
                result=words<<24;
                if(depth<=0) { stream+=stride; continue; }
                /* First SXY store is the maximum-depth branch delay slot */
                w_u32(output+8u,xport_gte_read_data(12u));
                if(depth>=maximum) { stream+=stride; continue; }
                w_u32(output+8u+coordinate_stride,xport_gte_read_data(13u));
                w_u32(output+8u+2u*coordinate_stride,xport_gte_read_data(14u));
            }
            {
                uint32 bucket=ordering+(uint32)(depth>>shift)*4u;
                uint32 previous=r_u32(bucket);
                w_u32(bucket,output&0x00FFFFFFu);
                result=(words<<24)|(previous&0x00FFFFFFu);
                w_u32(output,result);
                output+=(words+1u)*4u;
            }
            stream+=stride;
        }
    }
    w_u32(0x80065570u,output);
    return result;
}

uint32 scene_8004FDDC(uint32 packet)
{
    uint32 i;
    for(i=0u;i<3u;++i) xport_gte_write_control(5u+i,0u);
    return scene_packet_far_core(packet,
        (sint32)(r_u32(packet+16u)-r_u32(0x80081BB8u)),
        (sint32)(r_u32(packet+20u)-r_u32(0x80081BBCu)),
        (sint32)(r_u32(packet+24u)-r_u32(0x80081BC0u)));
}
