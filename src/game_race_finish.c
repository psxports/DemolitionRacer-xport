#include "draft_signatures.h"
#define FINISH_GP 0x80064A10u
uint32 sub_8001372C(void)
{
    uint32 mode=r_u32(0x80059738u),count,finished=0u,i;
    if(r_u32(0x8005973Cu)==1u){
        if(mode>=12u)return 0u;
        switch(mode){
            case 0u:case 8u:case 9u:case 10u:case 11u:
                if(r_u8(0x80066E8Eu)&1u){w_u32(FINISH_GP+56u,1u);w_u32(FINISH_GP+52u,2u);}
                if(r_u8(0x800670C5u)==1u){w_u32(FINISH_GP+56u,1u);w_u32(FINISH_GP+52u,1u);}return 1u;
            case 1u:case 2u:
                if(r_u8(0x80066E8Eu)&1u){w_u32(FINISH_GP+56u,0u);w_u32(FINISH_GP+52u,2u);}
                if(r_u8(0x800670C5u)==1u){w_u32(FINISH_GP+56u,r_u8(0x800670C4u)?0u:1u);w_u32(FINISH_GP+52u,1u);}return 1u;
            case 4u:
                if(!(r_u8(0x80066E8Eu)&1u))return 1u;
                w_u32(FINISH_GP+56u,1u);w_u32(FINISH_GP+52u,2u);return 2u;
            case 3u:case 7u:
                if(r_u8(0x80066E8Eu)&1u){w_u32(FINISH_GP+56u,0u);w_u32(FINISH_GP+52u,2u);return 2u;}break;
            default:
                /* Original jump-table target remains the unused V0 carrier */
                return 0x80013A90u;
        }
    }else{
        if(mode-1u>=6u)return 0u;
        switch(mode){
            case 1u:case 2u:case 5u:case 6u:
                if(((r_u8(0x80066E8Eu)&1u)||r_u8(0x800670C5u)==1u)&&((r_u8(0x80067246u)&1u)||r_u8(0x8006747Du)==1u)){
                    w_u32(FINISH_GP+56u,1u);w_u32(FINISH_GP+52u,1u);
                }return 1u;
            case 4u:
                if(!(r_u8(0x80066E8Eu)&1u))return 0u;
                if(!(r_u8(0x80067246u)&1u))return 1u;
                w_u32(FINISH_GP+56u,1u);w_u32(FINISH_GP+52u,2u);return 2u;
            case 3u:
                if((r_u8(0x80066E8Eu)&1u)&&(r_u8(0x80067246u)&1u)){
                    w_u32(FINISH_GP+56u,0u);w_u32(FINISH_GP+52u,2u);return 2u;
                }break;
        }
    }
    count=r_u32(0x80065254u);
    for(i=0u;(sint32)i<(sint32)count;++i)if(r_u8(0x80066E8Eu+i*952u)&1u)++finished;
    if(finished==count-1u){w_u32(FINISH_GP+56u,1u);w_u32(FINISH_GP+52u,1u);}return 1u;
}
void sub_80013364(void)
{
    uint32 state=r_u32(FINISH_GP+52u),progress,packet,i,texture;
    if(!state)return;
    if(r_u32(0x8005973Cu)==2u){w_u32(FINISH_GP+48u,1u);return;}
    if(state==2u&&r_u32(FINISH_GP+64u)){
        w_u32(FINISH_GP+64u,r_u32(FINISH_GP+64u)-1u);return;
    }
    progress=r_u32(FINISH_GP+100u);
    if((sint32)progress<64)w_u32(FINISH_GP+100u,progress+2u);
    else if(r_u16(0x80065524u)&0x4000u)w_u32(FINISH_GP+72u,1u);
    else{
        uint32 timer=r_u32(FINISH_GP+60u)-1u;w_u32(FINISH_GP+60u,timer);
        if(timer==0xFFFFFFFFu)w_u32(FINISH_GP+72u,1u);
    }
    packet=r_u32(FINISH_GP+0x7B0u)+r_u32(0x80065128u)*800u;
    for(i=0u;i<20u;++i,packet+=40u){
        uint32 x=r_u16(0x8006564Cu+i*4u),y=r_u16(0x8006564Eu+i*4u),u0,u1,v0,v1,x0,x1,ot;
        texture=r_u32(FINISH_GP+0x7C4u);progress=r_u32(FINISH_GP+100u);
        v0=r_u8(texture+25u);v1=v0+r_u8(texture+23u);
        if(i&1u){u0=r_u8(texture+24u);u1=u0+progress;x0=x+64u-progress;x1=x+64u;}
        else{u0=r_u8(texture+24u)-(progress-64u);u1=r_u8(texture+24u)+64u;x0=x;x1=x+progress;}
        /* Original FT4 layout retains packet colors, opcode, CLUT and texture page */
        w_u8(packet+12u,(uint8)u0);w_u8(packet+13u,(uint8)v0);
        w_u8(packet+20u,(uint8)u1);w_u8(packet+21u,(uint8)v0);
        w_u8(packet+28u,(uint8)u0);w_u8(packet+29u,(uint8)v1);
        w_u8(packet+36u,(uint8)u1);w_u8(packet+37u,(uint8)v1);
        w_u16(packet+8u,(uint16)x0);w_u16(packet+10u,(uint16)y);
        w_u16(packet+16u,(uint16)x1);w_u16(packet+18u,(uint16)y);
        w_u16(packet+24u,(uint16)x0);w_u16(packet+26u,(uint16)(y+64u));
        w_u16(packet+32u,(uint16)x1);w_u16(packet+34u,(uint16)(y+64u));
        ot=r_u32(r_u32(0x80065118u)+112u);
        w_u32(packet,(r_u32(packet)&0xFF000000u)|(r_u32(ot)&0xFFFFFFu));
        w_u32(ot,(r_u32(ot)&0xFF000000u)|(packet&0xFFFFFFu));
    }
}
