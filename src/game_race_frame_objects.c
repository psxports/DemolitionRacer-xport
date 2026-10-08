#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define R32(a) (*(uint32 *)psx_addr((a),4u))
#define R16(a) (*(uint16 *)psx_addr((a),2u))
#define R8(a) (*(uint8 *)psx_addr((a),1u))
extern uint32 game_800B5790(uint32 track);
extern uint32 sub_80043450(uint32 x,uint32 z);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern void dr_race_normalize_vector(VECTOR *input,VECTOR *output);
extern void game_800A7FA8(uint32 packet,uint32 attributes);
uint32 sub_8002C188(uint32 state,uint32 position);
uint32 sub_8002DC38(uint32 state,uint32 position,uint32 output);

static void object_pending(uint32 target,uint32 object,uint32 second,uint32 third,uint32 fourth)
{
    /* TODO: Translate the full original object callee when reached */
    fprintf(stderr,"Untranslated frame object %08X(%08X,%08X,%08X,%08X)\n",target,object,second,third,fourth);
    abort();
}

static sint32 object_shift(uint32 value,uint32 shift)
{
    if((sint32)value<0)value+=(1u<<shift)-1u;
    return (sint32)value>>shift;
}

static sint32 object_div(uint32 value,uint32 denominator)
{
    if(!denominator)xport_mips_break(7u);
    if(value==0x80000000u && denominator==0xFFFFFFFFu)xport_mips_break(6u);
    return (sint32)value/(sint32)denominator;
}

static void object_link(uint32 packet,uint32 depth)
{
    uint32 environment=R32(0x80065118u),ot=R32(environment+112u)+depth*4u;
    R32(packet)=(R32(packet)&0xFF000000u)|(R32(ot)&0xFFFFFFu);
    ot=R32(environment+112u)+depth*4u;
    R32(ot)=(R32(ot)&0xFF000000u)|(packet&0xFFFFFFu);
}

void game_800AADF4(uint32 object)
{
    uint32 section_index=(uint32)(sint32)(sint16)R16(object),offset=section_index*24u;
    uint32 section=R32(0x800653D4u)+offset,count=R8(section+3u)&15u,segment=R8(object+8u);
    uint32 angle,first,second,magnitude,threshold,value,distance;
    sint32 cosine,sine;
    VECTOR normal;
    if(segment>=2u && (sint32)segment<(sint32)(count-2u))return;
    angle=(((uint32)((sint32)R32(object+88u)>>8)-game_800B5790(section_index))&2047u);
    if(angle>=1024u)angle=2047u-angle;
    cosine=draft_agent3_missing_800437EC(angle);
    sine=draft_agent3_missing_8004388C(angle);
    magnitude=(uint32)cosine*(uint32)(sint32)(sint16)R16(object+228u);
    section=R32(0x800653D4u)+offset;
    if(segment<2u) {
        first=R32(0x800653C4u)+R16(section+6u)*6u;
        second=R32(0x800653C4u)+R16(section+4u)*6u;
    } else {
        uint32 type=R8(section);
        first=R32(0x800653C4u)+(R16(section+4u)+count+R32(0x800BEFC8u+type*8u))*6u;
        second=R32(0x800653C4u)+(R16(section+6u)+count+R32(0x800BEFCCu+type*8u))*6u;
    }
    threshold=magnitude+(uint32)sine*(uint32)(sint32)(sint16)R16(object+232u);
    normal.vx=(sint32)((uint32)(sint32)(sint16)R16(second+4u)-(uint32)(sint32)(sint16)R16(first+4u));
    normal.vy=0;
    normal.vz=(sint32)((uint32)(sint32)(sint16)R16(first)-(uint32)(sint32)(sint16)R16(second));normal.pad=0;
    dr_race_normalize_vector(&normal,&normal);
    value=(uint32)((sint32)R32(object+12u)>>8)-(uint32)(sint32)(sint16)R16(first)-R32(section+12u);
    distance=value*(uint32)normal.vx;
    value=(uint32)((sint32)R32(object+20u)>>8)-(uint32)(sint32)(sint16)R16(first+4u)-R32(section+20u);
    distance+=value*(uint32)normal.vz;distance-=threshold;
    if((sint32)distance<0) {
        angle=sub_80043450((uint32)(sint32)(sint16)R16(second+4u)-(uint32)(sint32)(sint16)R16(first+4u),
            (uint32)(sint32)(sint16)R16(second)-(uint32)(sint32)(sint16)R16(first));
        game_800AAA94(object,object+12u,(sint32)angle,(sint32)(distance+4095u)>>12);
        game_800AB0D8(object);
    }
}

void game_800AE020(uint32 object)
{
    uint32 section=(uint32)(sint32)(sint16)R16(object),segment=R8(object+8u),index,mask=0u;
    sint32 lowest=0;
    if((sint32)R32(object+28u)>=0)return;
    for(index=0u;index<8u;++index) {
        uint32 state=0x800C4218u+index*10u,position=object+124u+index*12u,height=0x800C08D8u+index*4u;
        sint32 difference;
        R16(state)=(uint16)section;R8(state+8u)=(uint8)segment;
        sub_8002C188(state,position);sub_8002DC38(state,position,height);
        difference=(sint32)(R32(object+128u+index*12u)-R32(height));
        if(difference<0) {if(difference<lowest)lowest=difference;mask|=1u<<index;}
    }
    if(mask) {
        R32(object+16u)-=(uint32)lowest;game_800AB0D8(object);
        game_800ADCE8(object,mask);
    }
}

void game_800A9504(uint32 object)
{
    uint32 frame=R32(object+116u)+R8(object+131u)*32u,component;
    for(component=0;component<3u;++component) {
        uint32 sum=R16(object+104u+component*2u)+R16(object+110u+component*2u);
        R16(object+104u+component*2u)=(uint16)sum;
        R32(object+92u+component*4u)+=(uint32)((sint32)(sum<<16)>>12);
    }
    --R8(object+130u);
    if(!R8(object+130u)) {
        R8(object+131u)=R8(frame+25u);
        frame=R32(object+116u)+R8(object+131u)*32u;
        R8(object+130u)=R8(frame+24u);
        R16(object+124u)=(uint16)(R8(frame+26u)<<4);R16(object+126u)=(uint16)(R8(frame+27u)<<4);
        R8(object+130u)=R8(frame+24u);
        memcpy(psx_addr(object+120u,4u),psx_addr(frame+16u,4u),4u);
        game_800A7FA8(object+12u,object+116u);
    }
    for(component=0;component<3u;++component)
        R16(object+124u+component*2u)=(uint16)(R16(object+124u+component*2u)+(sint8)R8(frame+28u+component));
    for(component=0;component<3u;++component)
        R8(object+120u+component)=(uint8)(R8(object+120u+component)+R8(frame+20u+component));
}

void game_800A96C4(uint32 object)
{
    SVECTOR inputs[4];
    uint32 component,index,packet=object+12u+R32(0x80065128u)*40u;
    sint32 screen[3],depths[3],screen4,depth4,flags,area,area4,depth;
    uint32 center[3];
    for(component=0;component<3u;++component)center[component]=R32(object+92u+component*4u)-R32(0x80081BB8u+component*4u);
    for(index=0;index<4u;++index) {
        inputs[index].vx=(sint16)(center[0]+(index?R16(object+104u+(index-1u)*6u):0u));
        inputs[index].vy=(sint16)(center[1]+(index?R16(object+106u+(index-1u)*6u):0u));
        inputs[index].vz=(sint16)(center[2]+(index?R16(object+108u+(index-1u)*6u):0u));inputs[index].pad=0;
    }
    gte_project3_full_depth(inputs,screen,depths,&flags);
    if((uint32)flags&0x01C66000u)return;
    area=NormalClip(screen[0],screen[1],screen[2]);
    R32(packet+8u)=(uint32)screen[0];R32(packet+16u)=(uint32)screen[1];R32(packet+24u)=(uint32)screen[2];
    depth4=gte_project_full_depth(&inputs[3],&screen4,&flags);
    if((uint32)flags&0x01C66000u)return;
    area4=NormalClip(screen[1],screen[2],screen4);depth=AverageZ4(depths[0],depths[1],depths[2],depth4);
    if(area>0 && area4<0)return;
    if(depth<33 || depth>=(sint32)R32(0x800654F0u))return;
    R8(packet+4u)=R8(object+128u);R8(packet+5u)=R8(object+129u);R8(packet+6u)=R8(object+130u);
    R32(packet+32u)=(uint32)screen4;object_link(packet,(uint32)depth);
}

void game_800A9078(uint32 object)
{
    uint32 packet=object+12u+R32(0x80065128u)*40u,values[3],squared=0u,i;
    SVECTOR position;
    sint32 screen,flags,depth,width,height,cosine,sine,angle;
    sint32 horizontal_cos,horizontal_sin,vertical_cos,vertical_sin;
    uint32 x,y;
    for(i=0;i<3u;++i) {
        uint32 component=R32(object+92u+i*4u);
        values[i]=(uint32)object_shift(component,8u)-R32(0x80081BB8u+i*4u);
        component=(uint32)((sint32)values[i]>>6);squared+=component*component;
    }
    if((sint32)squared>0x100000)return;
    position.vx=(sint16)values[0];position.vy=(sint16)values[1];position.vz=(sint16)values[2];position.pad=0;
    depth=gte_project_full_depth(&position,&screen,&flags)>>2;
    if((uint32)flags&0x01C66000u)return;
    width=object_shift((uint32)object_shift((uint32)(sint32)(sint16)R16(object+124u),4u)*R32(0x8006516Cu),12u);
    height=object_shift((uint32)object_shift((uint32)(sint32)(sint16)R16(object+126u),4u)*R32(0x8006516Cu),12u);
    angle=(sint16)R16(object+128u);
    if(depth<33 || depth>=(sint32)R32(0x800654F0u))return;
    R8(packet+4u)=R8(object+120u);R8(packet+5u)=R8(object+121u);R8(packet+6u)=R8(object+122u);
    width=object_div((uint32)width*R32(0x80065150u),(uint32)depth);
    height=object_div((uint32)height*R32(0x80065150u),(uint32)depth);
    cosine=draft_agent3_missing_800437EC((uint32)angle);
    sine=draft_agent3_missing_8004388C((uint32)angle);
    horizontal_sin=object_shift((uint32)width*(uint32)sine,14u);
    horizontal_cos=object_shift((uint32)width*(uint32)cosine,14u);
    vertical_sin=object_shift((uint32)height*(uint32)sine,14u);
    vertical_cos=object_shift((uint32)height*(uint32)cosine,14u);
    x=(uint16)screen;y=(uint16)((uint32)screen>>16);
    if(R8(object+10u)==1u) {
        R16(packet+8u)=(uint16)(x-(uint32)horizontal_cos+(uint32)vertical_sin);
        R16(packet+10u)=(uint16)(y-(uint32)horizontal_sin-(uint32)vertical_cos);
        R16(packet+16u)=(uint16)(x+(uint32)horizontal_cos+(uint32)vertical_sin);
        R16(packet+18u)=(uint16)(y+(uint32)horizontal_sin-(uint32)vertical_cos);
        R16(packet+24u)=(uint16)(x-(uint32)horizontal_cos-(uint32)vertical_sin);
        R16(packet+26u)=(uint16)(y-(uint32)horizontal_sin+(uint32)vertical_cos);
        R16(packet+32u)=(uint16)(x+(uint32)horizontal_cos-(uint32)vertical_sin);
        R16(packet+34u)=(uint16)(y+(uint32)horizontal_sin+(uint32)vertical_cos);
    } else {
        R16(packet+8u)=(uint16)(x+(uint32)vertical_sin*2u-(uint32)horizontal_cos);
        R16(packet+10u)=(uint16)(y-(uint32)horizontal_sin-(uint32)vertical_cos*2u);
        R16(packet+16u)=(uint16)(x+(uint32)horizontal_cos+(uint32)vertical_sin*2u);
        R16(packet+18u)=(uint16)(y+(uint32)horizontal_sin-(uint32)vertical_cos*2u);
        R16(packet+24u)=(uint16)(x-(uint32)horizontal_cos);R16(packet+26u)=(uint16)(y-(uint32)horizontal_sin);
        R16(packet+32u)=(uint16)(x+(uint32)horizontal_cos);R16(packet+34u)=(uint16)(y+(uint32)horizontal_sin);
    }
    object_link(packet,(uint32)depth);
}
