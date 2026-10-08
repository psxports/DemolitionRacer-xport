#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define R32(a) (*(uint32 *)psx_addr((a),4u))
#define R16(a) (*(uint16 *)psx_addr((a),2u))
#define R8(a) (*(uint8 *)psx_addr((a),1u))
extern uint32 game_800A8848(uint32 template_address,const VECTOR *position,const SVECTOR *rotation,const SVECTOR *scale,uint32 flags);
uint32 sub_80043450(uint32 x,uint32 z);
uint32 sub_8001685C(uint32 state);

static void car_effect_pending(uint32 target,uint32 first,uint32 second,uint32 third)
{
    /* TODO: Translate this complete original effect callee when reached */
    fprintf(stderr,"Untranslated car effect %08X(%08X,%08X,%08X)\n",target,first,second,third);
    abort();
}

static uint32 car_effect_facing(uint32 player)
{
    uint32 object=0x80066D14u+player*952u;
    uint32 x=R32(object+100u)-R32(0x8007A9D4u),z=R32(object+108u)-R32(0x8007A9DCu);
    uint32 rotation=R32(object+128u),magnitude=rotation,heading,delta;
    sint32 remainder;
    if((sint32)rotation<0)magnitude=0u-rotation;
    magnitude=(uint32)((sint32)magnitude>>8);
    remainder=(sint32)magnitude%4096;
    if((sint32)rotation>0)remainder=(sint32)(4096u-(uint32)remainder);
    if((sint32)z>0) {
        if((sint32)x>0)heading=sub_80043450(z,x);
        else heading=2048u-sub_80043450(z,0u-x);
    } else {
        if((sint32)x<0)heading=sub_80043450(0u-z,0u-x)+2048u;
        else heading=4096u-sub_80043450(0u-z,x);
    }
    delta=heading-(uint32)remainder;
    if(remainder<2048)return delta<2049u;
    return !(delta+2047u<2047u);
}

void game_800AFC88(uint32 player)
{
    uint32 object=0x80066D14u+player*952u;
    uint32 flags=R8(object+377u),first=R32(0x800C6B40u+player*8u),second=R32(0x800C6B44u+player*8u);
    if(!car_effect_facing(player))return;
    if(R8(object+19u)) {
        sub_80019C6C(player,flags&48u);
        if(flags&16u) {R8(first+120u)=55u;R8(first+121u)=55u;R8(first+122u)=55u;}
        if(flags&32u) {R8(second+120u)=55u;R8(second+121u)=55u;R8(second+122u)=55u;}
    } else {
        sub_80019C6C(player,0u);
        R8(first+120u)=0u;R8(first+121u)=0u;R8(first+122u)=0u;
        R8(second+120u)=0u;R8(second+121u)=0u;R8(second+122u)=0u;
    }
    sub_80019C6C(player,R8(object+21u)?0u:flags&3u);
}

void game_800AFE28(uint32 player)
{
    uint32 object=0x80066D14u+player*952u,model=R32(object),i,component;
    SVECTOR positions[2];
    for(i=0u;i<2u;++i) {
        positions[i].vx=(sint16)R16(model+96u+i*8u);
        positions[i].vy=(sint16)R16(model+98u+i*8u);
        positions[i].vz=(sint16)R16(model+100u+i*8u);positions[i].pad=0;
    }
    if(R8(0x80059A28u+player)!=2u) {
        uint32 car=R32(object+24u),vertices=car+R32(car+44u),bank=R32(0x80065210u)*8u;
        for(i=0u;i<2u;++i) {
            uint32 vertex=vertices+R32(0x800BF77Cu+bank+i*4u);
            sint16 *values=&positions[i].vx;
            for(component=0u;component<3u;++component)
                values[component]=(sint16)((uint16)values[component]+R16(vertex+component*2u)-R16(0x800C6BD0u+i*8u+component*2u));
        }
    }
    for(i=0u;i<2u;++i) {
        VECTOR output;
        gte_transform_raw(&positions[i],&output);
        R32(R32(0x800C6B40u+player*8u+i*4u)+92u)=(uint32)output.vx<<8;
        R32(R32(0x800C6B40u+player*8u+i*4u)+96u)=(uint32)output.vy<<8;
        R32(R32(0x800C6B40u+player*8u+i*4u)+100u)=(uint32)output.vz<<8;
    }
    R8(R32(0x800C6B40u+player*8u)+11u)&=127u;
    R8(R32(0x800C6B44u+player*8u)+11u)&=127u;
}

static sint16 car_effect_velocity(uint32 object,uint32 offset,uint32 random,uint32 mask,sint32 bias)
{
    uint32 value=(uint32)((sint32)R32(object+offset)>>4)*7u;
    if((sint32)value<0)value+=7u;
    return (sint16)((uint32)((sint32)value>>3)+(random&mask)+(uint32)bias);
}

void game_800AF0BC(uint32 player)
{
    uint32 object=0x80066D14u+player*952u,time,template_address,random;
    VECTOR position;
    SVECTOR velocity,scale;
    if((R32(object+376u)&0x30000u)!=0x20000u)return;
    memcpy(&scale,psx_addr(0x8008BB80u,8u),8u);
    ++R16(object+80u);
    gte_transform_raw((const SVECTOR *)psx_addr(0x800BF5D8u+R32(0x80065210u)*8u,8u),&position);
    position.vx=(sint32)((uint32)position.vx<<8);position.vy=(sint32)((uint32)position.vy<<8);position.vz=(sint32)((uint32)position.vz<<8);
    if((sint16)R16(object+80u)>=451) {
        if(randPSX()&15u)return;
        position.vx=(sint32)((uint32)position.vx-8192u+(uint32)((sint32)randPSX()%16384));
        position.vz=(sint32)((uint32)position.vz-8192u+(uint32)((sint32)randPSX()%16384));
        random=randPSX();velocity.vx=car_effect_velocity(object,112u,random,511u,-256);
        velocity.vy=(sint16)(((sint32)R32(object+116u)>>5)+800);
        random=randPSX();velocity.vz=car_effect_velocity(object,120u,random,511u,-256);velocity.pad=0;
        game_800A8848(0x800BF0D8u,&position,&velocity,&scale,32u);
        return;
    }
    time=R32(0x80064A30u);
    if(time/3u*3u==time-1u) {
        velocity.vx=car_effect_velocity(object,112u,0u,0u,0);
        velocity.vy=(sint16)(((sint32)R32(object+116u)>>5)+800);
        velocity.vz=car_effect_velocity(object,120u,0u,0u,0);velocity.pad=0;
        game_800A8848(0x800BF118u,&position,&velocity,&scale,12u);
    }
    time=R32(0x80064A30u);
    if(time/3u*3u!=time)return;
    random=randPSX();velocity.vx=car_effect_velocity(object,112u,random,127u,-64);
    velocity.vy=(sint16)(((sint32)R32(object+116u)>>5)+800);
    random=randPSX();velocity.vz=car_effect_velocity(object,120u,random,127u,-64);velocity.pad=0;
    template_address=0x800BF0F8u;
    game_800A8848(template_address,&position,&velocity,&scale,32u);
}

void game_800AEC08(uint32 player)
{
    sint32 state=(sint8)R8(0x800C6BC0u+player);
    uint32 object=0x80066D14u+player*952u,geometry,component;
    VECTOR position;
    SVECTOR velocity={0},scale;
    if(state>=3) {game_800AEDDC(player);return;}
    if(R32(0x800C6AFCu+player*4u)!=R32(0x80064A30u)+(R32(0x80064C14u)?3u:7u))return;
    memcpy(&scale,psx_addr(0x8008BB80u,8u),8u);
    velocity.vx=(sint16)((sint32)R32(object+112u)>>4);velocity.vy=210;
    velocity.vz=(sint16)((sint32)R32(object+120u)>>4);
    geometry=0x800BF618u+(R32(0x80065210u)*5u+(uint32)state)*8u;
    for(component=0;component<3u;++component) {
        uint32 value=(R32(object+168u+component*4u)+(uint32)(sint32)(sint16)R16(geometry+component*2u))<<8;
        if(component==0u)position.vx=(sint32)value;else if(component==1u)position.vy=(sint32)value;else position.vz=(sint32)value;
    }
    position.pad=0;
    game_800A8848(0x800BF498u,&position,&velocity,&scale,24u);
}

void game_800B24B4(uint32 object)
{
    sint32 strength=(sint32)R32(object+900u),other=(sint32)R32(object+896u),type;
    uint32 classification=0u;
    if(strength<other)strength=other;
    if(R8(object+938u))strength=32767;
    if(R8(object+940u)==1u || R8(object+942u)==15u)strength=0;
    if(strength)classification=sub_8001685C(object+88u);
    sub_8001D070(classification,(uint32)strength,R8(object+937u));
    game_800B1804(object,R32(object+900u));
    game_800B1A70(object,R32(object+896u));
    type=(sint16)R16(R32(object+4u)+118u);
    if(type==2 || type==3)game_800B1F98(object,R8(object+938u));
    if(type==1 || type==3)game_800B1D30(object,R8(object+938u));
}
