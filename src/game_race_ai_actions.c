#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
extern uint32 game_800B5790(uint32);
extern uint32 sub_80043450(uint32,uint32);

static sint32 act_add(sint32 a,sint32 b){return (sint32)((uint32)a+(uint32)b);}
static sint32 act_sub(sint32 a,sint32 b){return (sint32)((uint32)a-(uint32)b);}
static sint32 act_neg(sint32 a){return (sint32)(0u-(uint32)a);}
static sint32 act_half(uint32 a){return (sint16)r_u16(a);}
static sint32 act_read(uint32 a){return (sint32)r_u32(a);}
static sint32 act_shift(sint32 a,uint32 n){return act_add(a,a<0?(sint32)((1u<<n)-1u):0)>>n;}
static sint32 act_wrap(sint32 a){return (sint32)(((uint32)a+2048u)&4095u)-2048;}
static uint32 act_object(uint32 car){return 0x80066D14u+car*952u;}
static uint32 act_record(uint32 car){return 0x800C7698u+car*104u;}
static uint32 act_random(uint32 car)
{return sub_8001B884(car);}
static uint32 act_mod(uint32 value,uint32 divisor)
{if(!divisor)xport_mips_break(7u);return value%divisor;}
static sint32 act_remainder(sint32 value,sint32 divisor)
{if(!divisor)xport_mips_break(7u);if(value==INT_MIN&&divisor==-1)xport_mips_break(6u);return value%divisor;}
static void act_target(sint32 segment,uint32 lane,VECTOR *position,sint32 offset)
{
    game_800B5F90(segment,lane,position,offset);
}
static void act_stuck(uint32 object,uint32 ai)
{
    uint16 timer;
    if(act_read(object+888u)>=97)return;
    if(!r_u8(object+941u) && (sint8)r_u8(object+946u)<0)return;
    timer=(uint16)(r_u16(ai+60u)+8u);w_u16(ai+60u,timer);
    if((sint16)timer>=25) {
        w_u8(ai+95u,1u);w_u8(ai+98u,1u);
        w_u16(ai+60u,r_u8(object+941u)?4u:90u);
    }
}
static void act_route(uint32 car,uint32 ai)
{
    if(r_u8(ai+99u)==255u)game_800B5418(car);else game_800B54B8(car);
}
static void act_heading(uint32 car,uint32 ai,const VECTOR *position,uint32 mode)
{
    uint32 object=act_object(r_u8(ai+97u));
    sint32 x=act_sub(position->vx,act_read(object+100u))>>8;
    sint32 z=act_sub(position->vz,act_read(object+108u))>>8;
    sint32 angle=(sint32)sub_80043450((uint32)x,(uint32)z);
    angle=act_wrap(act_sub(angle,act_add(act_read(object+128u),act_read(object+8u))>>8));
    w_u16(ai+52u,(uint16)angle);
    if(mode) {
        angle=act_half(ai+52u);if(angle>256)w_u16(ai+52u,256u);
        if(act_half(ai+52u)<-256)w_u16(ai+52u,(uint16)-256);
    }
    game_800B60F8(car,(sint32)mode);
}
static void act_route_heading(uint32 car,uint32 ai)
{
    VECTOR position;
    act_target(act_half(ai+62u),r_u8(r_u32(ai)+((uint32)act_half(ai+64u)*3u)),&position,act_half(ai+26u));
    act_heading(car,ai,&position,0u);
}

/* Full B6C50 action including both original timer decrements */
void game_800B6C50(uint32 car)
{
    uint32 ai=act_record(car),object=act_object(car);
    sint32 timer=act_half(ai+60u);
    if(!timer)w_u8(ai+95u,0u);else w_u16(ai+60u,(uint16)act_sub(timer,1));
    timer=act_half(ai+60u);if(timer>0)w_u16(ai+60u,(uint16)act_sub(timer,1));
    act_stuck(object,ai);
    if(act_half(ai+60u)<150) {
        game_800B4F18(ai);act_route(car,ai);
    } else game_800B4F18(ai);
    act_route_heading(car,ai);
}

/* Full B6294 reverse heading action */
void game_800B6294(uint32 car)
{
    uint32 object=act_object(car),ai=act_record(car);
    sint32 wrapped,heading,timer,velocity,magnitude,throttle,steering;
    heading=act_sub(act_read(object+128u)>>8,(sint32)game_800B5790((uint32)act_half(object+88u)));
    wrapped=(sint32)(((uint32)heading+2048u)&4095u);heading=wrapped-2048;
    timer=act_half(ai+60u);
    if(!timer) {
        if(r_u8(object+941u)){w_u16(ai+60u,4u);w_u8(ai+98u,r_u8(ai+98u)^1u);}
        if((uint32)wrapped-1749u<599u)w_u8(ai+95u,0u);
    } else w_u16(ai+60u,(uint16)act_sub(timer,1));
    velocity=act_shift(act_read(object+888u),6u);magnitude=velocity<0?act_neg(velocity):velocity;
    throttle=act_sub(256,magnitude);if(throttle<0)throttle=0;if(throttle>256)throttle=256;
    if(!r_u8(ai+98u)) {
        w_u16(object+16u,(uint16)throttle);steering=(sint32)((uint32)act_neg(heading)<<7);
    } else {
        w_u16(object+16u,(uint16)act_neg(throttle));steering=(sint32)((uint32)heading<<7);
    }
    w_u8(object+19u,0u);w_u8(object+21u,0u);
    if(steering>98304)steering=98304;if(steering<-98304)steering=-98304;
    w_u32(object+8u,(uint32)steering);w_u16(ai+52u,0u);
}

/* Full B6498 four-phase AI action selection */
void game_800B6498(uint32 car)
{
    uint32 ai=act_record(car),object=act_object(car),phase,value,quotient;
    sint32 timer=act_half(ai+76u);
    if(timer){w_u16(ai+76u,(uint16)act_sub(timer,1));return;}
    if((sint8)r_u8(object+946u)>=0 && r_u8(object+947u)>=5u)w_u16(ai+76u,r_u16(ai+74u));
    timer=act_half(ai+60u);if(timer>0)w_u16(ai+60u,(uint16)act_sub(timer,1));
    act_stuck(object,ai);phase=(r_u32(0x80064A30u)+car)&3u;
    switch(phase) {
    case 0u:
        value=act_random(car);
        if(!act_mod(value,(uint32)act_half(ai+72u))) {
            game_800B52F0(ai);
            if(r_u8(ai+94u)!=car)w_u8(ai+95u,2u);
        }
        break;
    case 1u:game_800B4F18(ai);break;
    case 2u:
        value=act_random(car);
        if(!act_mod(value,(uint32)act_half(ai+78u))) {
            value=act_random(car);quotient=(uint32)(((uint64)value*0x10624DD3u)>>32)>>8;
            w_u16(ai+26u,(uint16)(r_u16(ai+26u)+value-quotient*4000u+0xF830u));
        }
        value=act_random(car);quotient=(uint32)(((uint64)value*0x1B4E81B5u)>>32)>>4;
        if(value==quotient*150u)w_u16(ai+84u,r_u16(ai+80u));
        break;
    case 3u:
        value=act_random(car);
        if(!act_mod(value,r_u8(ai+91u))) {
            game_800B52F0(ai);
            if(!r_u8(0x80059A28u+r_u8(ai+94u)))w_u8(ai+95u,2u);
        }
        break;
    }
    act_route(car,ai);act_route_heading(car,ai);
}

/* Full B68B0 pursuit action and bounded steering */
void game_800B68B0(uint32 car)
{
    uint32 ai=act_record(car),object=act_object(car),target;
    sint32 timer=act_half(ai+76u),other,period,distance,remainder,x,z,angle,wrapped;
    VECTOR position;
    if(timer){w_u16(ai+76u,(uint16)act_sub(timer,1));return;}
    other=(sint8)r_u8(object+946u);
    if(other>=0 && other!=(sint32)r_u8(ai+94u) && r_u8(object+947u)>=5u)w_u16(ai+76u,r_u16(ai+74u));
    timer=act_half(ai+60u);if(timer>0)w_u16(ai+60u,(uint16)act_sub(timer,1));
    act_stuck(object,ai);
    target=act_object(r_u8(ai+94u));period=act_read(0x800653A4u);
    distance=act_sub(act_half(target+92u),act_half(object+92u));
    distance=act_add(distance,act_shift(act_add(act_add(period,period),period),1u));
    remainder=act_sub(act_remainder(distance,period),act_shift(period,1u));
    position.vx=act_read(target+100u);position.vy=act_read(target+104u);position.vz=act_read(target+108u);
    x=(sint16)act_shift(act_sub(position.vx,act_read(object+100u)),8u);
    z=(sint16)act_shift(act_sub(position.vz,act_read(object+108u)),8u);
    angle=(sint32)sub_80043450((uint32)z,(uint32)x);
    wrapped=act_wrap(act_sub(angle,act_shift(act_read(object+128u),8u)));
    if((uint32)remainder>=11u || (wrapped<0?act_neg(wrapped):wrapped)>512) {
        w_u8(ai+95u,0u);return;
    }
    act_heading(car,ai,&position,32u);
}
