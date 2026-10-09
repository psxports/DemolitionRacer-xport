#include "draft_signatures.h"
#include <limits.h>
extern uint32 game_800B752C(uint32);
extern uint32 game_800B5EA4(uint32,uint32,uint32);
static sint32 route_add(sint32 a,sint32 b){return (sint32)((uint32)a+(uint32)b);}
static sint32 route_sub(sint32 a,sint32 b){return (sint32)((uint32)a-(uint32)b);}
static sint32 route_half(uint32 a){return (sint16)r_u16(a);}
static sint32 route_read(uint32 a){return (sint32)r_u32(a);}
static sint32 route_half_value(sint32 a){return route_add(a,(sint32)((uint32)a>>31))>>1;}
static sint32 route_div(sint32 a,sint32 b)
{if(!b)xport_mips_break(7u);if(a==INT_MIN&&b==-1)xport_mips_break(6u);return a/b;}
static sint32 route_mod(sint32 a,sint32 b)
{if(!b)xport_mips_break(7u);if(a==INT_MIN&&b==-1)xport_mips_break(6u);return a%b;}
static uint32 route_object(uint32 car){return 0x80066D14u+car*952u;}
static uint32 route_ai(uint32 car){return 0x800C7698u+car*104u;}
static sint32 route_distance(sint32 difference,sint32 period)
{
    sint32 offset=route_half_value(route_add(route_add(period,period),period));
    return route_sub(route_mod(route_add(difference,offset),period),route_half_value(period));
}

/* Full B4F18 scans nearby route records and chooses a passing side */
void game_800B4F18(uint32 ai)
{
    uint32 start=r_u8(ai+86u),cursor=start,owner=route_object(r_u8(ai+97u));
    uint32 other_ai,other_object,other_car,track,model,classification;
    sint32 period,difference,ratio,own_speed,other_speed,offset,left,right,center,first,second;
    w_u8(ai+99u,255u);
    for(;;) {
        cursor+=1u;if(cursor==r_u32(0x80065254u))cursor=0u;
        if(cursor==start)return;
        other_car=(uint32)route_half(0x800C7D18u+cursor*4u);other_ai=route_ai(other_car);
        if(r_u8(ai+96u)!=r_u8(other_ai+96u))continue;
        /* Original sample index uses cursor rather than mapped car */
        other_object=route_object(cursor);
        period=route_read(0x800653A4u);
        difference=route_sub(route_half(other_object+90u),route_half(owner+90u));
        if((uint32)route_distance(difference,period)>=20u)return;
        own_speed=route_half(ai+54u);other_speed=route_half(other_ai+54u);
        if(own_speed==other_speed)ratio=192000;
        else {
            difference=route_sub(route_half(route_object(r_u8(other_ai+97u))+88u),route_half(route_object(r_u8(ai+97u))+88u));
            ratio=route_div((sint32)((uint32)difference*1500u),route_sub(own_speed,other_speed));
        }
        if((uint32)ratio>=301u)continue;
        if(r_u32(ai)!=r_u32(other_ai)) {
            track=r_u32(other_ai);
            if(track==r_u32(0x800C7694u)) {
                w_u32(ai,track);w_u16(ai+26u,r_u16(ai+38u));
            } else {
                w_u16(ai+26u,r_u16(ai+36u));w_u32(ai,r_u32(0x800C7690u));
            }
        }
        if(r_u32(ai)==r_u32(0x800C7694u)) {
            w_u16(ai+50u,r_u16(ai+44u));w_u16(ai+48u,r_u16(ai+46u));
        } else {w_u16(ai+50u,r_u16(ai+40u));w_u16(ai+48u,r_u16(ai+42u));}
        classification=game_800B752C(ai);
        if(classification==1u || classification==2u) {
            track=r_u32(other_ai)+(uint32)route_half(other_object+90u)*3u;
            offset=(sint32)game_800B5EA4((uint32)route_half(other_object+88u),classification==1u?256u:0u,r_u8(track));
            model=r_u32(owner);
            offset=route_sub(route_add(offset,route_half(model+(classification==1u?0u:8u))),128);
        } else offset=route_half(ai+26u);
        model=r_u32(owner);left=route_add(route_half(model),route_sub(offset,128));
        first=route_half(other_ai+48u);
        if(first<left)continue;
        right=route_add(route_half(model+8u),route_add(offset,128));second=route_half(other_ai+50u);
        if(right<second)continue;
        center=route_half_value(route_add(first,second));
        first=route_sub(left,center);if(first<0)first=route_sub(center,left);
        second=route_sub(right,center);if(second<0)second=route_sub(center,right);
        w_u8(ai+99u,(uint8)other_car);w_u8(ai+100u,(uint8)(first>=second));return;
    }
}

/* Full B5418 eases route offset toward zero */
void game_800B5418(uint32 car)
{
    uint32 ai=route_ai(car);sint32 value=route_half(ai+26u);
    if(value<0) {
        value=route_add(value,20);w_u16(ai+26u,(uint16)value);
        if((sint16)value<=0)return;
        w_u16(ai+26u,0u);value=route_half(ai+26u);
    }
    if(value>0) {
        value=route_sub(value,20);w_u16(ai+26u,(uint16)value);
        if((sint16)value<0)w_u16(ai+26u,0u);
    }
}

/* Full B54B8 adjusts offset relative to the selected nearby car */
void game_800B54B8(uint32 car)
{
    uint32 ai=route_ai(car),target=route_object(r_u8(ai+99u));
    uint32 progress=r_u8(target+96u),side=r_u8(ai+100u),section;
    sint32 difference,period,amount,high;
    if(!progress)side=0u;
    else {
        section=r_u32(0x800653D4u)+(uint32)route_half(target+88u)*24u;
        if(progress==(r_u8(section+3u)&15u))side=1u;
    }
    difference=route_sub(route_half(target+90u),route_half(route_object(car)+90u));
    period=route_read(0x800653A4u);difference=route_distance(difference,period);
    amount=(sint32)((uint32)route_sub(20,difference)<<7);
    high=(sint32)(((int64_t)amount*0x66666667)>>32);
    amount=route_sub(high>>3,amount>>31);
    w_u16(ai+26u,(uint16)(side==1u?r_u16(ai+26u)-(uint32)amount:r_u16(ai+26u)+(uint32)amount));
}
