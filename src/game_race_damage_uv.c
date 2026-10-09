#include "draft_signatures.h"
/* Original signed UV comparisons use strict rectangle interiors */
uint32 sub_80033554(uint32 mask,sint32 x,sint32 y,sint32 side)
{
    uint32 index,selected,address;
    for(index=0u;index<13u;++index){
        selected=mask&(1u<<index);
        if(!selected)continue;
        if(side<0)selected<<=16;
        if(!(selected&0x1D5F1ABFu))continue;
        address=0x80066C56u+index*4u;
        if((sint32)r_u8(address)<x && (sint32)r_u8(address+1u)<y &&
           x<(sint32)r_u8(address+2u) && y<(sint32)r_u8(address+3u))return 1u;
    }
    if((mask&1u) && (sint32)r_u8(0x80066C8Au)<x &&
       (sint32)r_u8(0x80066C8Bu)<y && x<(sint32)r_u8(0x80066C8Cu))
        return (uint32)(y<(sint32)r_u8(0x80066C8Du));
    return 0u;
}
