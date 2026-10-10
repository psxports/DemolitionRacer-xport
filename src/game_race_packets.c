#include "draft_signatures.h"
#include <string.h>

void game_800AA450(void)
{
    uint32 mode = r_u32(0x80059738u), i, sizes = r_u32(0x800651F0u);
    w_u8(0x800C42ACu, mode - 1u < 2u ? 0u : r_u32(0x8005975Cu) ? 3u : 2u);
    w_u32(0x800C4268u, 0u);
    for (i = 0; i < 16u; ++i)
        w_u32(0x800C426Cu + i * 4u, r_u32(sizes + 16u + i * 4u) - r_u32(sizes + 12u + i * 4u));
    for (i = 0; i < 30u; ++i)
        memset(psx_addr(0x800C4378u + i * 300u, 300u), 0, 300u);
    for (i = 0; i < 200u; ++i)
        w_u8(0x800C4377u - i, 0u);
}

/* Original JR RA and NOP leaf */
void game_800B3630(void)
{
}

void game_800BB4FC(void)
{
    uint32 i;
    for (i = 0; i < 5u; ++i)
        memset(psx_addr(0x800C8930u + i * 36u, 36u), 0, 36u);
    w_u16(0x800C89E4u, 0u);
}

void game_800A7CA8(uint32 count)
{
    uint32 allocation = sub_80042544(count * 136u), previous = 0x800C40ECu, i;
    w_u32(0x800C4178u, 0x800C4174u);
    w_u32(0x800C4174u, 0x800C4174u);
    w_u32(0x800C40F0u, allocation + count * 136u - 136u);
    w_u32(0x800C40ECu, allocation);
    w_u32(0x800C40E8u, allocation);
    w_u32(0x800C41FCu, count);
    for (i = 0; (sint32)i < (sint32)(count - 1u); ++i)
    {
        uint32 node = allocation + i * 136u;
        w_u32(node + 4u, previous);
        previous = node;
        w_u32(node, allocation + (i + 1u) * 136u);
    }
    w_u32(0x800C4208u, 0u);
    w_u32(0x800C4210u, count);
    w_u32(allocation + count * 136u - 136u, 0x800C40ECu);
    w_u32(allocation + count * 136u - 132u, previous);
    w_u32(0x800C4200u, r_u32(0x800C4200u) + 1u);
}

void game_800B3640(void)
{
    uint32 buffer, i;
    for (buffer = 0; buffer < 2u; ++buffer)
    {
        uint32 p = 0x800C7458u + buffer * 24u;
        uint32 width = r_u32(0x8006511Cu), height = r_u32(0x80065120u);
        w_u8(p + 3u, 5u);
        w_u8(p + 7u, 0x2au);
        w_u16(p + 8u, 0u);
        w_u16(p + 10u, 0u);
        w_u16(p + 12u, width);
        w_u16(p + 14u, 0u);
        w_u16(p + 16u, 0u);
        w_u16(p + 18u, height);
        w_u16(p + 20u, width);
        w_u16(p + 22u, height);
        SetDrawMode1(psx_addr(0x800C7410u + buffer * 8u, 8u), 0, 1, 32u);
        for (i = 0; i < 9u; ++i)
        {
            uint32 record = 0x800C7108u + i * 84u;
            uint32 texture = sub_80039ADC(r_u32(0x800BF874u + i * 4u));
            uint32 width_tex = r_u8(texture + 22u), height_tex = r_u8(texture + 23u);
            uint32 u = r_u8(texture + 24u), v = r_u8(texture + 25u);
            p = record + 4u + buffer * 40u;
            w_u8(p + 3u, 9u);
            w_u8(p + 7u, 0x2eu);
            w_u8(p + 12u, u + 1u);
            w_u8(p + 13u, v + 1u);
            w_u8(p + 20u, u + width_tex - 1u);
            w_u8(p + 21u, v + 1u);
            w_u8(p + 28u, u + 1u);
            w_u8(p + 29u, v + height_tex - 1u);
            w_u8(p + 36u, u + width_tex - 1u);
            w_u8(p + 37u, v + height_tex - 1u);
            w_u8(p + 4u, 128u);
            w_u8(p + 5u, 128u);
            w_u8(p + 6u, 128u);
            w_u16(p + 22u, r_u16(texture + 8u));
            w_u16(p + 14u, r_u16(texture + 10u));
            w_u16(record, width_tex - 2u);
            w_u16(record + 2u, height_tex - 2u);
        }
    }
}

void game_800910F0(void)
{
    uint32 i;
    for (i = 0; i < 64u; ++i)
        memset(psx_addr(0x800C03D0u + i * 20u, 20u), 0, 20u);
    for (i = 0; i < 1024u; ++i)
        w_u8(0x800C03BFu - i, 255u);
    w_u32(0x800C03C8u, 0u);
    for (i = 0; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        uint32 index = r_u32(0x800C03C8u);
        w_u8(0x800C03E1u + index * 20u, 0u);
        w_u8(0x800C03E0u + r_u32(0x800C03C8u) * 20u, 255u);
        w_u32(0x800C03C8u, r_u32(0x800C03C8u) + 1u);
    }
    w_u32(0x800C08D0u, r_u32(0x800C03C8u));
    for (i = 0; i < 30u; ++i)
    {
        uint32 index = r_u32(0x800C03C8u);
        w_u8(0x800C03E1u + index * 20u, 1u);
        w_u8(0x800C03E0u + r_u32(0x800C03C8u) * 20u, 255u);
        w_u32(0x800C03C8u, r_u32(0x800C03C8u) + 1u);
    }
}

void game_800B0068(void)
{
    uint32 texture, allocation, offset, i;
    w_u32(0x800C6BE0u, 0u);
    texture = sub_80039ADC(0x8008BBF0u);
    allocation = sub_80042544(0x2e40u);
    w_u32(0x800C6BE4u, allocation);
    w_u32(0x800C6C20u, 0u);
    for (i = 0; i < 8u; ++i)
        w_u8(0x800C6BEAu + 49u - i * 7u, 0u);
    for (offset = 0; offset < 0x2e40u; offset += 148u)
    {
        uint32 p = allocation + offset, u = r_u8(texture + 24u), v = r_u8(texture + 25u);
        uint32 width = r_u8(texture + 22u), height = r_u8(texture + 23u);
        w_u8(p + 112u, 0u);
        w_u8(p + 35u, 9u);
        w_u16(p, (uint16)-300);
        w_u16(p + 2u, 0u);
        w_u16(p + 4u, 300u);
        w_u16(p + 8u, 300u);
        w_u16(p + 10u, 0u);
        w_u16(p + 12u, 300u);
        w_u16(p + 16u, (uint16)-300);
        w_u16(p + 18u, 0u);
        w_u16(p + 20u, (uint16)-300);
        w_u16(p + 24u, 800u);
        w_u16(p + 26u, 0u);
        w_u16(p + 28u, (uint16)-300);
        w_u8(p + 39u, 0x2eu);
        w_u8(p + 36u, 26u);
        w_u8(p + 37u, 26u);
        w_u8(p + 38u, 26u);
        w_u8(p + 44u, u);
        w_u8(p + 45u, v);
        w_u8(p + 52u, u + width - 1u);
        w_u8(p + 53u, v);
        w_u8(p + 60u, u);
        w_u8(p + 61u, v + height - 1u);
        w_u8(p + 68u, u + width - 1u);
        w_u8(p + 69u, v + height - 1u);
        w_u16(p + 54u, r_u16(texture + 8u));
        w_u16(p + 46u, r_u16(texture + 10u));
        for (i = 0; i < 40u; i += 4u)
            w_u32(p + 72u + i, r_u32(p + 32u + i));
        w_u32(p + 132u, 0xffffffffu);
        w_u32(p + 136u, 0xffffffffu);
        w_u32(p + 140u, 0xffffffffu);
        w_u32(p + 144u, 0xffffffffu);
    }
}
