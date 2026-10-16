#include "draft_signatures.h"
#include <stdio.h>
#include <string.h>

extern uint32 dr_texture_lookup(const char *name);

static void race_clut_location(uint32 texture, uint32 destination)
{
    uint32 clut = r_u16(texture + 8u);
    w_u16(destination, (r_u8(texture + 24u) >> 1) + ((clut << 6) & 1023u));
    w_u16(destination + 2u, r_u8(texture + 25u) + ((clut << 4) & 256u) + ((clut >> 2) & 512u));
}

void game_800BCB60(void)
{
    uint32 i, channel;
    w_u32(0x800BFBE0u, 0u);
    w_u32(0x800BFBE4u, 0u);
    for (i = 0; i < 16u; ++i)
    {
        uint32 components[3];
        for (channel = 0; channel < 3u; ++channel)
        {
            uint32 first = r_u32(0x800BFBC8u + channel * 4u);
            uint32 product = (r_u32(0x800BFBD4u + channel * 4u) - first) * i;
            components[channel] = first + (uint32)((sint32)product / 16);
        }
        w_u16(0x800C8B80u + i * 2u, ((components[0] >> 3) & 31u) | ((components[1] >> 3) & 31u) << 5 | ((components[2] << 7) & 0x7c00u));
    }
    for (i = 0; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        PSX_RECT rectangle;
        rectangle.x = (sint16)(r_u32(0x80064C70u) + 192u);
        rectangle.y = (sint16)(r_u32(0x80064C74u) + i);
        rectangle.w = 16;
        rectangle.h = 1;
        LoadImagePSX(&rectangle, (uint32 *)psx_addr(0x800C8B80u, 32u));
        DrawSync(0);
    }
    for (i = 0; i < 4u; ++i)
    {
        char name[16];
        strcpy(name, (const char *)psx_addr(0x8008BF88u, 1u));
        if (i > 0u)
            snprintf(name + 5, sizeof(name) - 5u, (const char *)psx_addr(0x8008BF90u, 1u), (int)i + 1);
        race_clut_location(dr_texture_lookup(name), 0x800C8BA4u + i * 8u);
    }
    race_clut_location(sub_80039ADC(0x8008BF94u), 0x800C8BC4u);
}
