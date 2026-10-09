#include "draft_signatures.h"
#include <string.h>
extern uint32 game_800A8848(uint32,const VECTOR*,const SVECTOR*,const SVECTOR*,uint32);
/* Original owner RNG delegates to the eleven-word 43780 leaf */
uint32 sub_8001B884(uint32 owner)
{
    uint32 address=0x80067050u+owner*952u;
    uint32 seed=r_u32(address)*0x01DF5E0Du+1u;
    w_u32(address,seed);
    return(seed<<24)|(seed>>8);
}
static sint32 pair_effect_shift(sint32 value,uint32 shift)
{return(sint32)((uint32)value+(value<0?(1u<<shift)-1u:0u))>>shift;}
/* Full original thirteen-entry effect position switch */
void game_800AF468(uint32 object,uint32 force)
{
    sint32 vertical[5],scale_y[3],scale_xz[3],count,index,intensity;
    uint32 i,position_address,mode,elapsed;
    VECTOR position;
    SVECTOR rotation={0,0,0,0},scale={0,0,0,0};
    for(i=0u;i<5u;++i)vertical[i]=(sint32)r_u32(0x8008BB88u+i*4u);
    for(i=0u;i<3u;++i){scale_y[i]=(sint32)r_u32(0x8008BB9Cu+i*4u);scale_xz[i]=(sint32)r_u32(0x8008BBA8u+i*4u);}
    /* Original velocity shift is arithmetic before multiplication */
    scale.vx=(sint16)pair_effect_shift((sint32)((uint32)((sint32)r_u32(object+112u)>>4)*7u),3u);
    scale.vz=(sint16)pair_effect_shift((sint32)((uint32)((sint32)r_u32(object+120u)>>4)*7u),3u);
    rotation=scale;
    elapsed=r_u32(0x80064A30u)-r_u32(0x800C6B3Cu);
    if(elapsed<10u)return;
    w_u32(0x800C6B3Cu,r_u32(0x80064A30u));
    force&=255u;
    if(r_u8(object+947u)<24u&&!force)return;
    mode=r_u32(object+268u);
    switch(mode){
    case 0:case 11:position_address=object+576u;break;
    case 2:case 3:position_address=object+588u;break;
    case 8:case 9:position_address=object+600u;break;
    case 5:case 6:position_address=object+612u;break;
    default:position_address=object+100u;break;
    }
    position.vx=(sint32)r_u32(position_address);position.vy=(sint32)r_u32(position_address+4u);position.vz=(sint32)r_u32(position_address+8u);
    intensity=(sint32)(r_u8(object+947u)*100u)/255;
    if(force){intensity=100;position.vx=(sint32)r_u32(object+100u);position.vy=(sint32)r_u32(object+104u);position.vz=(sint32)r_u32(object+108u);}
    count=intensity*6/100+4;
    for(index=0;index<count;++index){
        uint32 template_address;
        rotation.vy=(sint16)vertical[randPSX()%5u];
        scale.vy=(sint16)scale_y[randPSX()%3u];
        scale.vx=(sint16)scale_xz[randPSX()%3u];
        scale.vz=(sint16)scale_xz[randPSX()%3u];
        if(randPSX()&1u)scale.vx=(sint16)(0u-(uint32)(uint16)scale.vx);
        if(randPSX()&1u)scale.vz=(sint16)(0u-(uint32)(uint16)scale.vz);
        template_address=0x800C66A8u+(randPSX()%4u)*256u;
        game_800A8848(template_address,&position,&rotation,&scale,100u);
    }
}

