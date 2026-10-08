#include "draft_signatures.h"
static void hud_panel_link(uint32 packet)
{
    uint32 owner=r_u32(0x80065118u),head=r_u32(owner+0x70u);
    w_u32(packet,(r_u32(packet)&0xFF000000u)|(r_u32(head+4u)&0xFFFFFFu));
    head=r_u32(owner+0x70u);
    w_u32(head+4u,(r_u32(head+4u)&0xFF000000u)|(packet&0xFFFFFFu));
}
static void hud_panel_digit(uint32 packet,uint32 texture,uint32 digit,uint32 step,uint32 width,uint32 height)
{
    uint32 u=r_u8(texture+24u)+digit*step,v=r_u8(texture+25u);
    w_u8(packet+12u,(uint8)u); w_u8(packet+13u,(uint8)v);
    w_u8(packet+20u,(uint8)(u+width)); w_u8(packet+21u,(uint8)v);
    w_u8(packet+28u,(uint8)u); w_u8(packet+29u,(uint8)(v+height));
    w_u8(packet+36u,(uint8)(u+width)); w_u8(packet+37u,(uint8)(v+height));
}
void game_800A315C(uint32 player,uint32 port,uint32 flag)
{
    uint32 count=r_u32(0x80065254u),rank,buffer=r_u32(0x80065128u),i;
    (void)port;
    if(!flag) { count-=r_u32(0x80064A34u); rank=r_u8(player+0x3B0u)+1u; }
    else rank=count-r_u32(0x80064A34u);
    for(i=0u;i<8u;++i) hud_panel_link(0x800C16CCu+buffer*40u+i*80u);
    for(i=0u;i<2u;++i) {
        uint32 quotient=(uint32)((sint32)count/10),digit=count-quotient*10u;
        uint32 texture=r_u32(0x800C0F3Cu),packet=0x800C25CCu+buffer*40u+i*80u;
        hud_panel_digit(packet,texture,digit,8u,7u,r_u8(texture+23u));
        hud_panel_link(packet); count=quotient;
    }
    for(i=0u;i<2u;++i) {
        uint32 quotient=(uint32)((sint32)rank/10),digit=rank-quotient*10u;
        uint32 texture=r_u32(0x800C0F44u),packet=0x800C298Cu+buffer*40u+i*80u;
        hud_panel_digit(packet,texture,digit,12u,12u,16u);
        hud_panel_link(packet); rank=quotient;
    }
    hud_panel_link(0x800C1D0Cu+buffer*40u);
    hud_panel_link(0x800C3B50u+buffer*24u);
}
#include <string.h>
extern uint32 draft_agent3_missing_8003DC58(uint32 rectangle,const void *pixels);
static void hud_panel_xy(uint32 packet,uint32 x,uint32 y,uint32 width,uint32 height)
{
    w_u16(packet+8u,(uint16)x); w_u16(packet+10u,(uint16)y);
    w_u16(packet+16u,(uint16)(x+width)); w_u16(packet+18u,(uint16)y);
    w_u16(packet+24u,(uint16)x); w_u16(packet+26u,(uint16)(y+height));
    w_u16(packet+32u,(uint16)(x+width)); w_u16(packet+34u,(uint16)(y+height));
}
static const char *hud_panel_message(sint32 kind)
{
    return (const char*)psx_addr(r_u32(0x800BECC0u+(uint32)kind*20u+r_u32(0x80064B8Cu)*4u),1u);
}
void game_800A4D28(uint32 player,uint32 port)
{
    sint32 value=(sint16)r_u16(player+0x186u),kind=(sint8)r_u8(player+0x188u);
    uint32 state=0x800BEEB8u+port*12u,layout=0x800C4010u+port*16u,i;
    if(!value) return;
    if(kind>=0) {
        w_u8(player+0x188u,255u);
        draft_agent3_missing_8003DC58(0x80064C80u,psx_addr(kind==13?0x800C4058u:0x800C4038u,32u));
        w_u8(0x800C4078u+port,(uint8)kind);
    } else kind=r_u8(0x800C4078u+port);
    if(r_u32(state)!=r_u32(player+0x180u)) {
        w_u32(state+4u,(uint32)(sint32)(sint16)r_u16(layout));
        w_u32(state+8u,(uint32)(sint32)(sint16)r_u16(layout+2u));
        w_u32(state,r_u32(player+0x180u));
        if(value<0) {
            uint32 texture=sub_80039ADC(r_u32(0x800BEE8Cu+(uint32)(-6-value)*4u));
            for(i=0u;(sint32)i<(sint32)(2u-r_u32(0x80064C14u));++i) {
                uint32 packet=0x800C4098u+(i+port)*40u;
                w_u16(packet+14u,r_u16(texture+10u)); w_u16(packet+22u,r_u16(texture+8u));
                hud_panel_digit(packet,texture,0u,0u,48u,48u);
                w_u8(packet+7u,(uint8)((r_u8(packet+7u)|1u)&253u));
            }
        } else if(kind>=0) {
            const char *text=hud_panel_message(kind); uint32 length=(uint32)strlen(text);
            for(i=0u;(sint32)i<(sint32)length;++i) {
                uint32 character=(uint8)text[length-1u-i],u,v,width,texture=r_u32(0x800C0F40u),packet;
                if(character>=98u) { u=(character-97u)*8u;v=16u; }
                else if(character>=65u) { u=(character-64u)*8u;v=8u; }
                else {u=character*8u;v=0u;}
                if(character==73u) {width=5u;--u;}
                else if(character==49u||character==58u) {width=5u;u-=2u;}
                else width=character==32u?4u:6u;
                packet=0x800C1DACu+port*800u+i*40u;
                w_u8(packet+12u,(uint8)(r_u8(texture+24u)+u)); w_u8(packet+13u,(uint8)(r_u8(texture+25u)+v));
                w_u8(packet+20u,(uint8)(r_u8(texture+24u)+u+width)); w_u8(packet+21u,(uint8)(r_u8(texture+25u)+v));
                w_u8(packet+28u,(uint8)(r_u8(texture+24u)+u)); w_u8(packet+29u,(uint8)(r_u8(texture+25u)+v+8u));
                w_u8(packet+36u,(uint8)(r_u8(texture+24u)+u+width)); w_u8(packet+37u,(uint8)(r_u8(texture+25u)+v+8u));
                if(!r_u32(0x80064C14u)) {
                    uint32 mirror=0x800C20CCu+i*40u;
                    w_u8(mirror+12u,(uint8)(r_u8(texture+24u)+u));w_u8(mirror+13u,(uint8)(r_u8(texture+25u)+v));
                    w_u8(mirror+20u,(uint8)(r_u8(texture+24u)+u+width));w_u8(mirror+21u,(uint8)(r_u8(texture+25u)+v));
                    w_u8(mirror+28u,(uint8)(r_u8(texture+24u)+u));w_u8(mirror+29u,(uint8)(r_u8(texture+25u)+v+8u));
                    w_u8(mirror+36u,(uint8)(r_u8(texture+24u)+u+width));w_u8(mirror+37u,(uint8)(r_u8(texture+25u)+v+8u));
                }
            }
        }
    }
    if((sint32)r_u32(state+4u)<(sint16)r_u16(layout+4u)) {
        uint32 x=r_u32(state+4u)+(uint32)(sint32)(sint16)r_u16(layout+12u);
        w_u32(state+4u,x);
        if((sint16)r_u16(layout+4u)<(sint32)x) w_u32(state+4u,(uint32)(sint32)(sint16)r_u16(layout+4u));
    }
    if((sint16)r_u16(layout+6u)<(sint32)r_u32(state+8u)) {
        uint32 product=r_u32(layout+8u)*(r_u32(state+4u)-(uint32)(sint32)(sint16)r_u16(layout));
        sint32 y=(sint32)((uint32)((sint32)product/4096)+(uint32)(sint32)(sint16)r_u16(layout+2u));
        w_u32(state+8u,(uint32)y);
        if(y<(sint16)r_u16(layout+6u)) w_u32(state+8u,(uint32)(sint32)(sint16)r_u16(layout+6u));
    }
    if(value<0) {
        uint32 packet=0x800C4098u+r_u32(0x80065128u)*40u;
        hud_panel_xy(packet,r_u32(state+4u)-26u,r_u32(state+8u)+2u,48u,48u);
        hud_panel_link(packet);return;
    } else {
        uint32 base=port?3u:0u,spacing=0u,number=(uint32)value;
        for(i=base;i<base+3u;++i) {
            uint32 quotient=(uint32)((sint32)number/10),digit=number-quotient*10u,packet,texture;
            number=quotient;
            if(i!=base) spacing+=!digit?22u:(digit==1u||digit==3u||digit==5u)?17u:20u;
            if((i==base+2u && value<100)||(i==base+1u && value<10)) break;
            packet=0x800C36ACu+r_u32(0x80065128u)*40u+i*80u;
            hud_panel_xy(packet,r_u32(state+4u)-spacing,r_u32(state+8u),25u,28u);
            texture=r_u32(0x800C0F48u);
            hud_panel_digit(packet,texture,digit,25u,24u,28u);
            hud_panel_link(packet);
        }
    }
    {
        uint32 spacing=0u;
        for(i=0u;i<(uint32)strlen(hud_panel_message(kind));++i) {
            uint32 packet=0x800C1DACu+r_u32(0x80065128u)*800u+i*40u;
            uint32 width=(uint32)r_u8(packet+20u)-(uint32)r_u8(packet+12u);
            spacing+=width;
            hud_panel_xy(packet,r_u32(state+4u)-spacing+18u,r_u32(state+8u)+27u,width,8u);
            hud_panel_link(packet);
        }
    }
}
void game_800A5864(uint32 player,uint32 port)
{
    uint32 number=(uint32)(sint32)(sint16)r_u16(player+0x184u),buffer=r_u32(0x80065128u),i;
    for(i=0u;i<6u;++i) hud_panel_link(0x800C14ECu+buffer*40u+i*80u);
    for(i=0u;i<6u;++i) {
        uint32 quotient=(uint32)((sint32)number/10),digit=number-quotient*10u;
        uint32 texture=r_u32(0x800C0F3Cu),packet=0x800C23ECu+buffer*40u+i*80u;
        hud_panel_digit(packet,texture,digit,8u,7u,r_u8(texture+23u));
        hud_panel_link(packet); number=quotient;
    }
    hud_panel_link(0x800C3B20u+buffer*24u);
    game_800A4D28(player,port);
}
