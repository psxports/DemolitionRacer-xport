#include "draft_signatures.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#define TEXT_GP 0x80064A10u
static uint32 race_text_width(uint32 font,const unsigned char *text)
{
    uint32 line=0u,maximum=0u;
    while(*text) {
        uint32 character=*text++;
        if(character==10u) { if((sint32)maximum<(sint32)line) maximum=line; line=0u; }
        else line+=r_u8(font+16u+(uint8)(character-32u))+(character==32u?0u:1u);
    }
    if((sint32)maximum<(sint32)line) maximum=line;
    return maximum;
}
uint32 sub_80020AB8(uint32 font,sint32 x,sint32 y,const char *text)
{
    uint32 descriptor=0x80059DD8u+(font&15u)*144u,length,result,cursor=(uint32)x,index;
    w_u32(TEXT_GP+0x8C8u,descriptor);
    length=(uint32)strlen(text); result=length;
    if(font!=r_u32(TEXT_GP+0x368u)) {
        uint32 texture;
        w_u32(TEXT_GP+0x368u,font);
        w_u32(TEXT_GP+0x36Cu,r_u8(descriptor+5u));
        texture=r_u32(descriptor+12u);
        w_u32(TEXT_GP+0x370u,r_u8(texture+24u));
        texture=r_u32(descriptor+12u);
        w_u32(TEXT_GP+0x374u,r_u8(texture+25u));
        texture=r_u32(descriptor+12u);
        w_u32(TEXT_GP+0x378u,r_u8(descriptor+6u));
        w_u32(TEXT_GP+0x380u,r_u16(texture+8u));
        result=r_u16(r_u32(descriptor+12u)+10u);
        w_u32(TEXT_GP+0x384u,result);
        w_u32(TEXT_GP+0x37Cu,r_u8(descriptor+4u));
    }
    for(index=0u;(sint32)index<(sint32)length;++index) {
        uint32 character=(uint8)text[index],glyph=(uint8)(character-32u);
        if(character==13u || character==125u) {
            uint32 width=race_text_width(descriptor,(const unsigned char*)text+index+1u);
            uint32 even=(uint32)((sint32)(width+(width>>31))>>1)<<1;
            cursor=(character==13u?r_u32(TEXT_GP+0x8D8u):(uint32)x)-even;
        } else if(character==9u) cursor=r_u32(TEXT_GP+0x8D8u);
        else if(character==10u) { cursor=(uint32)x; y=(sint32)((uint32)y+r_u8(r_u32(TEXT_GP+0x8C8u)+5u)); }
        else if(character==32u) cursor+=r_u8(r_u32(TEXT_GP+0x8C8u)+16u+glyph);
        else {
            uint32 columns=r_u32(TEXT_GP+0x378u),width,height,packet,u,v,head,old,lower;
            if(!columns) abort();
            width=r_u8(r_u32(TEXT_GP+0x8C8u)+16u+glyph);
            height=r_u32(TEXT_GP+0x36Cu);
            packet=r_u32(TEXT_GP+0x8CCu)+40u*r_u32(TEXT_GP+0x8D0u);
            u=r_u32(TEXT_GP+0x370u)+(glyph%columns)*r_u32(TEXT_GP+0x37Cu);
            v=r_u32(TEXT_GP+0x374u)+(glyph/columns)*height;
            w_u8(packet+12u,(uint8)u); w_u8(packet+13u,(uint8)v);
            w_u8(packet+20u,(uint8)(u+width)); w_u8(packet+21u,(uint8)v);
            w_u8(packet+28u,(uint8)u);
            lower=(uint32)y+height;
            w_u16(packet+8u,(uint16)cursor); w_u16(packet+10u,(uint16)y);
            w_u16(packet+18u,(uint16)y); w_u16(packet+24u,(uint16)cursor);
            w_u16(packet+26u,(uint16)lower); w_u16(packet+34u,(uint16)lower);
            w_u16(packet+16u,(uint16)(cursor+width)); w_u16(packet+32u,(uint16)(cursor+width));
            w_u8(packet+29u,(uint8)(v+height)); w_u8(packet+36u,(uint8)(u+width)); w_u8(packet+37u,(uint8)(v+height));
            w_u16(packet+22u,(uint16)r_u32(TEXT_GP+0x380u)); w_u16(packet+14u,(uint16)r_u32(TEXT_GP+0x384u));
            w_u8(packet+4u,r_u8(0x80074580u)); w_u8(packet+5u,r_u8(0x80074581u)); w_u8(packet+6u,r_u8(0x80074582u));
            head=r_u32(r_u32(0x80065118u)+0x70u);
            w_u32(packet,(r_u32(packet)&0xFF000000u)|(r_u32(head)&0xFFFFFFu));
            head=r_u32(r_u32(0x80065118u)+0x70u);
            old=r_u32(head);
            cursor+=1u+width;
            w_u32(TEXT_GP+0x8D0u,r_u32(TEXT_GP+0x8D0u)+1u);
            w_u32(TEXT_GP+0x8D4u,r_u32(TEXT_GP+0x8D4u)+1u);
            w_u32(head,(old&0xFF000000u)|(packet&0xFFFFFFu));
        }
        result=index+1u<length?1u:0u;
    }
    return result;
}
