#include "draft_signatures.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Full GAME A22AC glyph coordinates and proportional widths */
void game_800A22AC(uint32 *u, uint32 *v, uint32 character, uint32 *width, uint32 proportional)
{
    uint32 c = character & 255u;
    if (c >= 98u) { *u = (c - 97u) << 3; *v = 16u; }
    else if (c >= 65u) { *u = (c - 64u) << 3; *v = 8u; }
    else { *u = c << 3; *v = 0u; }
    if (proportional != 0u) {
        if (c == 73u) { *width = 5u; *u -= 1u; }
        else if (c == 49u || c == 58u) { *width = 5u; *u -= 2u; }
        else if (c == 32u) *width = 4u;
        else *width = 6u;
    } else {
        *width = 6u;
        if (c == 58u) *u -= 2u;
        if (c == 49u) *u -= 2u;
    }
}

static uint32 hud_divide(uint32 numerator, uint32 denominator)
{
    if (denominator == 0u || (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)) abort();
    return (uint32)((int32_t)numerator / (int32_t)denominator);
}

/* Texture coordinates use the original descriptor byte fields */
static void hud_uv(uint32 packet, uint32 texture, uint32 left, uint32 right, uint32 top, uint32 bottom)
{
    w_u8(packet + 12u, (uint8)(r_u8(texture + 24u) + left));
    w_u8(packet + 13u, (uint8)(r_u8(texture + 25u) + top));
    w_u8(packet + 20u, (uint8)(r_u8(texture + 24u) + right));
    w_u8(packet + 21u, (uint8)(r_u8(texture + 25u) + top));
    w_u8(packet + 28u, (uint8)(r_u8(texture + 24u) + left));
    w_u8(packet + 29u, (uint8)(r_u8(texture + 25u) + bottom));
    w_u8(packet + 36u, (uint8)(r_u8(texture + 24u) + right));
    w_u8(packet + 37u, (uint8)(r_u8(texture + 25u) + bottom));
}

static void hud_xy(uint32 packet, uint32 x, uint32 y, uint32 width, uint32 height)
{
    w_u16(packet + 8u, (uint16)x); w_u16(packet + 10u, (uint16)y);
    w_u16(packet + 16u, (uint16)(x + width)); w_u16(packet + 18u, (uint16)y);
    w_u16(packet + 24u, (uint16)x); w_u16(packet + 26u, (uint16)(y + height));
    w_u16(packet + 32u, (uint16)(x + width)); w_u16(packet + 34u, (uint16)(y + height));
}

static void hud_texture(uint32 packet, uint32 texture)
{
    w_u16(packet + 22u, r_u16(texture + 8u)); w_u16(packet + 14u, r_u16(texture + 10u));
}

static void hud_header(uint32 packet)
{
    w_u8(packet + 3u, 9u); w_u8(packet + 7u, 0x2Cu);
}

static void hud_rgb(uint32 packet, uint32 red, uint32 green, uint32 blue)
{
    w_u8(packet + 4u, (uint8)red); w_u8(packet + 5u, (uint8)green); w_u8(packet + 6u, (uint8)blue);
}

static uint32 hud_coordinate(uint32 layout, uint32 offset)
{
    return (uint32)(int32_t)(int16_t)r_u16(layout + offset);
}

static void hud_glyph(uint32 packet, uint32 texture, uint32 character, uint32 *x, uint32 y, uint32 flags, uint32 yellow)
{
    uint32 u, v, width;
    hud_header(packet);
    game_800A22AC(&u, &v, character, &width, 1u);
    hud_uv(packet, texture, u, u + width, v, v + 8u);
    *x += width;
    hud_xy(packet, *x, y, width, 8u);
    if (yellow != 0u) hud_rgb(packet, 128u, 128u, 0u);
    hud_texture(packet, texture);
    w_u8(packet + 7u, (uint8)((r_u8(packet + 7u) & 0xFCu) | flags));
}

/* Full original GAME 9FB80 initialization before the two display-buffer loops */
static void hud_initial_state(void)
{
    uint32 layout, state = r_u32(0x80064C14u), mode, first, last, destination, source;
    if (state != 0u) layout = r_u32(0x80064B68u) == 0u ? 0x800BEAC8u : 0x800BEBA0u;
    else layout = 0x800BE9F0u;
    w_u32(0x800C4030u, layout);
    w_u32(0x800C4004u, 0u); w_u32(0x800C4000u, 0u);
    w_u16(0x800C4082u, 0u); w_u16(0x800C4080u, 0u); w_u16(0x800C408Au, 0u); w_u16(0x800C4088u, 0u);
    w_u8(0x800C4090u, 127u);
    w_u32(0x800C0B10u, 0u); w_u32(0x800C0B14u, 0u);
    w_u32(0x800BEEA8u, 0u); w_u32(0x800BEEACu, 0u); w_u32(0x800BEEB0u, 0u);
    w_u32(0x800C0B18u, 0u); w_u32(0x800BEEB4u, 0u); w_u32(0x800BEEA4u, 0u);
    w_u32(0x800C0F3Cu, sub_80039ADC(0x8008B9D8u));
    w_u32(0x800C0F40u, sub_80039ADC(0x8008B9E0u));
    w_u32(0x800C0F44u, sub_80039ADC(0x8008B9E8u));
    w_u32(0x800C0F48u, sub_80039ADC(0x8008B9F0u));
    w_u32(0x800C0CB8u, sub_80039ADC(r_u32(0x80064BB8u) != 0u ? 0x8008B9F8u : 0x8008B9FCu));
    w_u32(0x800C0C64u, sub_80039ADC(0x8008BA00u));
    w_u32(0x800C0C10u, sub_80039ADC(0x8008BA08u));
    w_u32(0x800C0B6Cu, sub_80039ADC(0x8008BA10u));
    source = sub_80039ADC(0x8008BA14u);
    mode = r_u32(0x80064B68u); state = r_u32(0x80064C14u);
    first = state * (mode * 2u + 1u); last = state * (mode * 2u + 2u);
    w_u32(0x800C39CCu, source);
    destination = 0x800C4010u; source = 0x800BEC78u + first * 10u;
    if ((int32_t)last >= (int32_t)first) {
        do {
            uint32 rise, run;
            w_u16(destination, r_u16(source)); w_u16(destination + 2u, r_u16(source + 2u));
            w_u16(destination + 4u, r_u16(source + 4u)); w_u16(destination + 6u, r_u16(source + 6u));
            w_u16(destination + 12u, r_u16(source + 8u));
            rise = hud_coordinate(destination, 6u) - hud_coordinate(destination, 2u);
            run = hud_coordinate(destination, 4u) - hud_coordinate(destination, 0u);
            w_u32(destination + 8u, hud_divide(rise << 12, run));
            ++first; source += 10u; destination += 16u;
        } while ((int32_t)last >= (int32_t)first);
    }
}

/* Rectangle texture spans retain the original low-halfword narrowing */
static void hud_quad(uint32 packet, uint32 texture, uint32 x, uint32 y, uint32 width, uint32 height, uint32 left, uint32 right, uint32 bottom, uint32 flags)
{
    hud_header(packet); hud_uv(packet, texture, left, right, 0u, bottom);
    hud_xy(packet, x, y, width, height); hud_texture(packet, texture);
    w_u8(packet + 7u, (uint8)((r_u8(packet + 7u) & 0xFCu) | flags));
}

/* Full original GAME 9FB80 two-buffer HUD packet construction */
uint32 game_8009FB80(void)
{
    uint32 buffer, i, j, packet, texture, layout, x, y, string, count, color, mode;
    hud_initial_state();
    for (buffer = 0u; buffer < 2u; ++buffer) {
        uint32 shift = buffer * 40u;
        layout = r_u32(0x800C4030u) + buffer * 4u;
        for (i = 0u; i < 2u; ++i) {
            packet = 0x800C0B70u + buffer * 20u + i * 40u;
            w_u8(packet + 7u, 0x20u); w_u8(packet + 3u, 4u); hud_rgb(packet,255u,255u,0u);
            w_u8(packet + 7u, (uint8)(r_u8(packet + 7u) & 0xFDu));
            packet = 0x800C0BC0u + buffer * 20u + i * 40u;
            w_u8(packet + 7u, 0x20u); w_u8(packet + 3u, 4u); hud_rgb(packet,1u,1u,1u);
            w_u8(packet + 7u, (uint8)(r_u8(packet + 7u) & 0xFDu));
        }
        texture = r_u32(0x800C0F3Cu); x = hud_coordinate(layout,16u); y = hud_coordinate(layout,18u);
        for (i = 0u; i < 3u; ++i) hud_quad(0x800C0DFCu+shift+i*80u,texture,x+(2u-i)*9u,y,8u,r_u8(texture+23u),0u,18u,r_u8(texture+23u)+255u,3u);
        texture = r_u32(0x800C0CB8u);
        hud_quad(0x800C0DACu+shift,texture,hud_coordinate(layout,24u),hud_coordinate(layout,26u),r_u8(texture+22u)+65535u,r_u8(texture+23u)+65535u,0u,r_u8(texture+22u)+255u,r_u8(texture+23u)+255u,3u);
        texture = r_u32(0x800C0C64u);
        hud_quad(0x800C0C68u+shift,texture,hud_coordinate(layout,32u),hud_coordinate(layout,34u),12u,r_u8(texture+23u),0u,12u,r_u8(texture+23u),1u);
        texture = r_u32(0x800C0C10u);
        hud_quad(0x800C0C14u+shift,texture,hud_coordinate(layout,0u),hud_coordinate(layout,2u),r_u8(texture+22u),r_u8(texture+23u),0u,r_u8(texture+22u),r_u8(texture+23u),3u);
        texture = r_u32(0x800C0B6Cu);
        hud_quad(0x800C0B1Cu+shift,texture,hud_coordinate(layout,8u),hud_coordinate(layout,10u),r_u8(texture+22u),r_u8(texture+23u),0u,r_u8(texture+22u),r_u8(texture+23u),3u);
        texture = r_u32(0x800C0F3Cu); x = hud_coordinate(layout,80u); y = hud_coordinate(layout,82u);
        for(i=0u;i<2u;++i) hud_quad(0x800C25CCu+shift+i*80u,texture,x-i*8u,y,8u,8u,0u,18u,r_u8(texture+23u)+255u,1u);
        texture = r_u32(0x800C0F3Cu); x = hud_coordinate(layout,120u); y = hud_coordinate(layout,122u);
        mode = r_u32(0x80064BD4u) * 8u;
        packet = 0x800C28ECu+shift;
        hud_quad(packet,texture,x,y,8u,8u,mode,mode+7u,r_u8(texture+23u)+255u,0u); hud_rgb(packet,128u,128u,128u);
        x = hud_coordinate(layout,56u); y = hud_coordinate(layout,58u);
        for(i=0u;i<8u;++i) hud_quad(0x800C266Cu+shift+i*80u,texture,x+i*8u,y,8u,8u,0u,18u,r_u8(texture+23u)+255u,1u);
        x = hud_coordinate(layout,168u); y = hud_coordinate(layout,170u);
        for(i=0u;i<6u;++i) hud_quad(0x800C23ECu+shift+i*80u,texture,x-i*8u,y,8u,8u,0u,18u,r_u8(texture+23u)+255u,1u);
        x = hud_coordinate(layout,96u); y = hud_coordinate(layout,98u);
        for(i=0u;i<2u;++i) {
            texture = sub_80039ADC(0x8008B9E8u); w_u32(0x800C0F44u,texture);
            hud_quad(0x800C298Cu+shift+i*80u,texture,x-i*11u,y,13u,16u,0u,11u,r_u8(texture+23u)+255u,1u);
        }
        texture = r_u32(0x800C0F48u);
        color = r_u16(texture+10u);
        w_u16(0x80064C80u,(uint16)((color&63u)<<4));
        w_u16(0x80064C82u,(uint16)((int32_t)(color<<16)>>22));
        StoreImage((PSX_RECT*)psx_addr(0x80064C80u,8u),(uint32*)psx_addr(0x800C4038u,32u));
        for(i=0u;i<6u;++i) {
            packet=0x800C36ACu+shift+i*80u;hud_header(packet);
            w_u16(packet+14u,(uint16)color);w_u16(packet+22u,r_u16(texture+8u));
            w_u8(packet+7u,(uint8)((r_u8(packet+7u)|1u)&0xFDu));
        }
        packet=0x800C4098u+shift;w_u8(packet+7u,0x2Du);w_u8(packet+3u,9u);hud_rgb(packet,192u,192u,192u);
        for(i=0u;i<20u;++i){
            packet=0x800C1DACu+buffer*800u+i*40u;hud_header(packet);texture=r_u32(0x800C0F40u);
            w_u16(packet+22u,r_u16(texture+8u));w_u16(packet+14u,r_u16(texture+10u));
            w_u8(packet+7u,(uint8)((r_u8(packet+7u)|1u)&0xFDu));hud_rgb(packet,128u,128u,128u);
        }
        texture=r_u32(0x800C0F44u);packet=0x800C293Cu+shift;
        hud_quad(packet,texture,hud_coordinate(layout,136u),hud_coordinate(layout,138u),13u,16u,12u,23u,r_u8(texture+23u)+255u,0u);hud_rgb(packet,128u,128u,128u);
        texture=r_u32(0x800C0F40u);
        for(j=0u;j<8u;++j)for(i=0u;i<5u;++i){
            packet=0x800C2A2Cu+buffer*200u+j*400u+i*40u;hud_header(packet);hud_texture(packet,texture);
            w_u8(packet+7u,(uint8)((r_u8(packet+7u)|1u)&0xFDu));
        }
        for(i=0u;i<4u;++i){
            packet=0x800C388Cu+shift+i*80u;hud_header(packet);hud_texture(packet,texture);
            w_u8(packet+7u,(uint8)((r_u8(packet+7u)|1u)&0xFDu));
        }
        x=hud_coordinate(layout,208u);y=hud_coordinate(layout,210u);
        string=r_u32(0x800BECB0u+r_u32(0x80064B8Cu)*4u);
        count=(uint32)strlen((const char*)psx_addr(string,1u));
        if((int32_t)count>0)for(i=0u;i<count;++i)hud_glyph(0x800C0F4Cu+shift+i*80u,texture,r_u8(string+i),&x,y,0u,1u);
        x=hud_coordinate(layout,48u);y=hud_coordinate(layout,50u);string=r_u32(0x800BEDECu+r_u32(0x80064B8Cu)*4u);
        for(i=0u;i<4u;++i)hud_glyph(0x800C13ACu+shift+i*80u,texture,r_u8(string+i),&x,y,1u,0u);
        x=hud_coordinate(layout,88u);y=hud_coordinate(layout,90u);hud_glyph(0x800C1D0Cu+shift,texture,47u,&x,y,1u,0u);
        x=hud_coordinate(layout,128u);y=hud_coordinate(layout,130u);hud_glyph(0x800C1D5Cu+shift,texture,47u,&x,y,0u,0u);hud_rgb(0x800C1D5Cu+shift,128u,128u,128u);
        x=hud_coordinate(layout,160u);y=hud_coordinate(layout,162u);string=r_u32(0x800BEE14u+r_u32(0x80064B8Cu)*4u);
        for(i=0u;i<6u;++i)hud_glyph(0x800C14ECu+shift+i*80u,texture,r_u8(string+i),&x,y,1u,0u);
        x=hud_coordinate(layout,72u);y=hud_coordinate(layout,74u);mode=r_u32(0x80059738u);
        string=r_u32(((mode-3u)<2u?0x800BEE00u:0x800BEDD8u)+r_u32(0x80064B8Cu)*4u);
        for(i=0u;i<8u;++i)hud_glyph(0x800C16CCu+shift+i*80u,texture,r_u8(string+i),&x,y,1u,0u);
        x=hud_coordinate(layout,112u);y=hud_coordinate(layout,114u);string=r_u32(0x800BEE28u+r_u32(0x80064B8Cu)*4u);
        for(i=0u;i<3u;++i)hud_glyph(0x800C194Cu+shift+i*80u,texture,r_u8(string+i),&x,y,1u,0u);
        x=hud_coordinate(layout,152u);y=hud_coordinate(layout,154u);string=r_u32(0x800BEE3Cu+r_u32(0x80064B8Cu)*4u);
        for(i=0u;i<9u;++i){packet=0x800C1A3Cu+shift+i*80u;hud_glyph(packet,texture,r_u8(string+i),&x,y,0u,0u);hud_rgb(packet,128u,128u,128u);}
        for(i=0u;i<4u;++i){
            static const uint32 offsets[4]={144u,64u,176u,104u};
            static const uint32 widths[4]={36u,70u,70u,50u};
            static const uint32 heights[4]={16u,11u,11u,16u};
            packet=0x800C3AC0u+buffer*24u+i*48u;
            w_u8(packet+7u,0x28u);w_u8(packet+3u,5u);hud_rgb(packet,10u,10u,10u);w_u8(packet+7u,(uint8)(r_u8(packet+7u)|2u));
            x=hud_coordinate(layout,offsets[i]);y=hud_coordinate(layout,offsets[i]+2u);
            w_u16(packet+8u,(uint16)x);w_u16(packet+10u,(uint16)y);w_u16(packet+12u,(uint16)(x+widths[i]));w_u16(packet+14u,(uint16)y);
            w_u16(packet+16u,(uint16)x);w_u16(packet+18u,(uint16)(y+heights[i]));w_u16(packet+20u,(uint16)(x+widths[i]));w_u16(packet+22u,(uint16)(y+heights[i]));
        }
        texture=r_u32(0x800C39CCu);
        for(i=0u;i<3u;++i){
            packet=0x800C39D0u+shift+i*80u;hud_header(packet);
            hud_uv(packet,texture,0u,r_u8(texture+22u)+255u,0u,r_u8(texture+23u)+255u);
            hud_rgb(packet,128u,i==0u?128u:0u,i==0u?128u:0u);hud_texture(packet,texture);
            w_u8(packet+7u,(uint8)((r_u8(packet+7u)&0xFEu)|2u));
        }
    }
    texture=sub_80039ADC(0x8008BA18u);w_u32(0x800C3B80u,texture);
    for(i=0u;i<4u;++i){packet=0x800C3B88u+i*40u;hud_header(packet);hud_uv(packet,texture,0u,r_u8(texture+22u)+255u,0u,r_u8(texture+23u)+255u);hud_texture(packet,texture);w_u8(packet+7u,(uint8)((r_u8(packet+7u)|1u)&0xFDu));}
    texture=sub_80039ADC(0x8008BA1Cu);w_u32(0x800C3B84u,texture);
    for(i=0u;i<24u;++i){packet=0x800C3C28u+i*40u;hud_header(packet);hud_uv(packet,texture,0u,r_u8(texture+22u),0u,r_u8(texture+23u));hud_texture(packet,texture);w_u8(packet+7u,(uint8)((r_u8(packet+7u)&0xFEu)|2u));}
    for(i=0u;i<3u;++i)for(j=0u;j<4u;++j)hud_rgb(0x800C3C28u+i*40u+j*240u,240u,50u,50u);
    for(i=3u;i<6u;++i)for(j=0u;j<4u;++j)hud_rgb(0x800C3C28u+i*40u+j*240u,20u,130u,20u);
    for(i=0u;i<16u;++i)w_u16(0x800C4058u+i*2u,(uint16)(r_u16(0x800C4038u+i*2u)&0x83FFu));
    for(i=0u;i<5u;++i)w_u16(0x800BEED0u+i*2u,(uint16)((i+1u)*25u));
    return 0u;
}

