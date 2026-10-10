#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
extern uint32 sub_8003AAC8(uint32 player, uint32 first, uint32 second);
extern void draft_agent3_missing_8003DD18(const sint16 *, uint32, uint32);
extern uint32 draft_agent3_missing_8003D9AC(uint32);

static void pause_pending(uint32 target)
{
    /* TODO: Translate the complete pause dependency before invoking it */
    fprintf(stderr, "Untranslated pause dependency %08X\n", target);
    abort();
}

static void pause_packet(uint32 packet, uint32 texture, uint32 x, uint32 y)
{
    uint32 width = R8(texture + 22u), height = R8(texture + 23u);
    uint8 u = R8(texture + 24u), v = R8(texture + 25u);
    R8(packet + 3u) = 9u;
    R8(packet + 7u) = 44u;
    R8(packet + 4u) = 128u;
    R8(packet + 5u) = 128u;
    R8(packet + 6u) = 128u;
    R8(packet + 12u) = u;
    R8(packet + 13u) = v;
    R8(packet + 20u) = (uint8)(u + width - 1u);
    R8(packet + 21u) = v;
    R8(packet + 28u) = u;
    R8(packet + 29u) = (uint8)(v + height - 1u);
    R8(packet + 36u) = (uint8)(u + width - 1u);
    R8(packet + 37u) = (uint8)(v + height - 1u);
    R16(packet + 8u) = (uint16)x;
    R16(packet + 10u) = (uint16)y;
    R16(packet + 16u) = (uint16)(x + width + 65535u);
    R16(packet + 18u) = (uint16)y;
    R16(packet + 24u) = (uint16)x;
    R16(packet + 26u) = (uint16)(y + height + 65535u);
    R16(packet + 32u) = (uint16)(x + width + 65535u);
    R16(packet + 34u) = (uint16)(y + height + 65535u);
    R16(packet + 22u) = R16(texture + 8u);
    R16(packet + 14u) = R16(texture + 10u);
}

void game_800BA7B4(void)
{
    sint16 rectangle[4];
    uint32 buffer, index, slot, texture;
    memcpy(rectangle, psx_addr(0x8008BF08u, 8u), 8u);
    R8(0x800BFAE4u) = (uint8)(R32(0x80065538u) == 255u);
    if (R32(0x80064C14u))
        R8(0x800BFAE5u) = (uint8)(R32(0x8006553Cu) == 255u);
    if (R16(0x80065524u) & 8u)
        R16(0x800BFBBcu) = 0u;
    else if ((R16(0x80065526u) & 8u) && R32(0x8005973Cu) == 2u)
        R16(0x800BFBBcu) = 1u;
    else if (R8(0x800BFAE4u))
        R16(0x800BFBBcu) = 0u;
    else if (R8(0x800BFAE5u))
        R16(0x800BFBBcu) = 1u;
    else
        return;
    buffer = R32(0x80065128u);
    R32(0x8006511Cu) = 320u;
    rectangle[2] = 320;
    rectangle[1] = (sint16)((buffer ^ 1u) << 8);
    R32(0x800BFAE0u) = 0u;
    R32(0x800BFAECu) = 0u;
    R32(0x800BFAE8u) = 0u;
    R32(0x800C7ED0u) = 1u;
    draft_agent3_missing_8003DD18(rectangle, 0u, buffer << 8);
    sub_8003AAC8(0u, 0u, 0u);
    sub_8003AAC8(1u, 0u, 0u);
    sub_8001E328();
    sub_8002037C();
    if (R32(0x8005973Cu) == 2u)
        pause_pending(0x80049E64u);
    R32(0x800C7ED4u) = sub_80039ADC(0x8008BF10u);
    R32(0x800C7EDCu) = sub_80039ADC(0x8008BF18u);
    R32(0x800C7ED8u) = sub_80039ADC(0x8008BF20u);
    texture = R32(0x800C7ED4u);
    for (buffer = 0u; buffer < 2u; ++buffer)
        pause_packet(0x800C7EE0u + buffer * 40u, texture, (uint32)((320 - (sint32)R8(texture + 22u)) / 2), (uint32)((240 - (sint32)R8(texture + 23u)) / 2));
    for (index = 0u; index < 8u; ++index)
        for (buffer = 0u; buffer < 2u; ++buffer)
        {
            slot = index * 80u + buffer * 40u;
            pause_packet(0x800C7F30u + slot, R32(0x800C7EDCu), 0u, 0u);
            pause_packet(0x800C8430u + slot, R32(0x800C7EDCu), 0u, 0u);
            pause_packet(0x800C81B0u + slot, R32(0x800C7ED8u), 0u, 0u);
            pause_packet(0x800C86B0u + slot, R32(0x800C7ED8u), 0u, 0u);
        }
    while (R32(0x800C7ED0u) == 1u)
    {
        uint32 selected;
        sub_8003A578();
        sub_800208E4();
        sub_800399D4();
        sub_80039908();
        pause_pending(0x800B9E38u);
        if (R8(0x800BFAE4u) && R32(0x80065538u) != 255u)
            R8(0x800BFAE4u) = 0u;
        if (R8(0x800BFAE5u) && R32(0x8006553Cu) != 255u)
            R8(0x800BFAE5u) = 0u;
        R8(0x800BFAE4u) = (uint8)(R32(0x80065538u) == 255u);
        if (R8(0x800BFAE6u))
            R8(0x800BFAE5u) = (uint8)(R32(0x8006553Cu) == 255u);
        selected = 0x80065524u + (uint32)(sint32)(sint16)R16(0x800BFBBcu) * 2u;
        if (R16(selected) & 0x4000u)
        {
            uint32 item = R32(0x800BFAE8u);
            if (R32(0x800BFAECu) == 1u)
            {
                if (!item)
                {
                    if (R32(0x800BFAF0u))
                    {
                        R32(0x800BFAF0u) = 0u;
                        R8(0x80064EA1u) = 1u;
                    }
                    R32(0x800C7ED0u) = 0u;
                    R32(0x8006528Cu) = 1u;
                    R32(0x800651D0u) = 1u;
                }
                else
                {
                    R32(0x800BFAF0u) = 0u;
                    R32(0x800BFAECu) = 0u;
                    if (R8(0x800BFAE7u))
                    {
                        R8(0x800BFAE7u) = 0u;
                        R32(0x800BFAE8u) = 4u;
                    }
                    else
                        R32(0x800BFAE8u) = 3u;
                }
            }
            else
            {
                if (!item)
                    R32(0x800C7ED0u) = 0u;
                if (item == 3u)
                {
                    R32(0x800BFAF0u) = 1u;
                    R32(0x800BFAECu) = 1u;
                    R32(0x800BFAE8u) = 1u;
                }
                if (R32(0x800BFAE8u) == 4u)
                {
                    R32(0x800BFAECu) = 1u;
                    R32(0x800BFAE8u) = 1u;
                    R32(0x80064A2Cu) = 1u;
                    R8(0x800BFAE7u) = 1u;
                }
            }
        }
        VSync(2);
        sub_80039934();
        sub_80039A10();
        selected = 0x80065524u + (uint32)(sint32)(sint16)R16(0x800BFBBcu) * 2u;
        if (R16(selected) & 8u)
            R32(0x800C7ED0u) = 0u;
    }
    VSync(0);
    draft_agent3_missing_8003D9AC(0u);
    sub_8001E450();
    sub_800203A4();
    R16(0x80065524u) = 0u;
    R16(0x80065526u) = 0u;
    R16(0x80065520u) = 0u;
    R16(0x80065522u) = 0u;
    if (R32(0x80064C14u))
        sub_8001B9A4(R32(0x80064B68u));
}