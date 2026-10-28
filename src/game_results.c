#include "draft_signatures.h"
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4))
#define R16(a) (*(uint16 *)psx_addr((a), 2))
#define R8(a) (*(uint8 *)psx_addr((a), 1))

extern uint32 sub_8001DE6C(void);
extern uint32 draft_agent3_missing_8003D914(uint32 mode);

static sint32 results_half(uint32 value)
{
    return (sint32)(value + (value >> 31)) >> 1;
}

uint32 sub_8002ABE4(void)
{
    static const uint32 packets[4] = {0x80078838, 0x80078970, 0x800788A0, 0x80078908};
    uint32 offset = R32(0x80065128) * 52;
    uint32 environment = R32(0x80065118);
    uint32 index, packet, table, result = 0;
    for (index = 0; index < 4; ++index)
    {
        packet = packets[index] + offset;
        table = R32(environment + 112);
        R32(packet) = (R32(packet) & 0xFF000000) | (R32(table) & 0xFFFFFF);
        table = R32(environment + 112);
        result = (R32(table) & 0xFF000000) | (packet & 0xFFFFFF);
        R32(table) = result;
    }
    return result;
}

uint32 sub_80024D60(uint32 font, uint32 initial_selection)
{
    uint32 messages[5], index, text, width, origin;
    for (index = 0; index < 5; ++index)
        messages[index] = R32(0x80011100 + index * 4);
    if (R8(0x80064EDC))
    {
        R8(0x80064EDD) = (uint8)initial_selection;
        R8(0x80064EDC) = 0;
    }
    sub_80021430(2);
    font &= 0xFF;
    width = sub_80020A14(font, messages[R32(0x80064B8C)]);
    text = messages[R32(0x80064B8C)];
    sub_80020AB8(font, results_half(320 - width), 120, (const char *)psx_addr(text, 1));
    sub_80021430(0);
    if (!R8(0x80064EDD))
        sub_80021430(1);
    width = sub_80020A14(font, R32(0x8005A200 + R32(0x80064B8C) * 4));
    text = R32(0x8005A200 + R32(0x80064B8C) * 4);
    origin = (uint32)results_half(320 - width) - (uint32)results_half(width);
    sub_80020AB8(font, (sint32)(origin - 4), 132, (const char *)psx_addr(text, 1));
    sub_80021430(0);
    if (R8(0x80064EDD) == 1)
        sub_80021430(1);
    text = R32(0x8005A214 + R32(0x80064B8C) * 4);
    sub_80020AB8(font, (sint32)(origin + width + 4), 132, (const char *)psx_addr(text, 1));
    /* Original 8002181C calls are empty */
    if (R16(0x80065524) & 0x80)
    {
        if (R8(0x80064EDD))
            --R8(0x80064EDD);
        R16(0x80065524) = 0;
    }
    if (R16(0x80065524) & 0x20)
    {
        if (!R8(0x80064EDD))
            ++R8(0x80064EDD);
        R16(0x80065524) = 0;
    }
    if (R16(0x80065524) & 0x4000)
    {
        uint32 selection = R8(0x80064EDD);
        R8(0x80064EDC) = 1;
        R16(0x80065524) = 0;
        return selection + 1;
    }
    return 0;
}

uint32 sub_80028390(void)
{
    static const uint32 messages[3] = {0x8005A40C, 0x8005A3F8, 0x8005A434};
    uint32 index, text, width, selection;
    for (index = 0; index < 3; ++index)
    {
        sub_80021430(R32(0x80065024) == index ? 1 : 2);
        width = sub_80020A14(5, R32(messages[index] + R32(0x80064B8C) * 4));
        text = R32(messages[index] + R32(0x80064B8C) * 4);
        sub_80020AB8(5, results_half(320 - width), 85 + (sint32)index * 12, (const char *)psx_addr(text, 1));
    }
    if (R32(0x80064BEC) != 1)
        return 0;
    if ((R16(0x80065524) & 0x10) && (sint32)R32(0x80065024) > 0)
        --R32(0x80065024);
    if ((R16(0x80065524) & 0x40) && (sint32)R32(0x80065024) < 2)
        ++R32(0x80065024);
    if (!(R16(0x80065524) & 0x4000))
        return 0;
    selection = R32(0x80065024);
    if (selection == 0 && R32(0x80064E5C) == 1)
    {
        R32(0x80078CA8) = R8(0x8005A791 + (R32(0x80059738) << 4) + R32(0x80065200)) == 255 ? 0 : 1;
        R32(0x80065244) = R32(0x80065200) + (R32(0x80064ED4) == 1 ? 1 : 0);
        sub_8002BDE8(0);
        R32(0x80064E68) = 12;
    }
    else if (selection == 1 && R32(0x80064E5C) == 1)
    {
        R32(0x80064E68) = 6;
        R32(0x80065020) = 1;
    }
    else if (selection == 2)
        return 1;
    return 0;
}

uint32 sub_8002AD28(void)
{
    PSX_RECT image_rect = {0, 0, 320, 256};
    PSX_RECT background_rect = {704, 0, 320, 256};
    char path[64];
    uint32 buffer, special = 0, value, result, saved = 0;
    sint32 random, divisor;
    sub_800399D4();
    sub_80039908();
    sub_800399D4();
    sub_80039908();
    buffer = sub_80042544(0x28800);
    R32(0x80065324) = buffer;
    VSync(0);
    DrawSync(0);
    draft_agent3_missing_8003D914(0);
    sub_80039718(0);
    R16(0x80081A9C) = (uint16)R32(0x80064BF0);
    R16(0x80081B10) = (uint16)R32(0x80064BF0);
    R16(0x80081A9E) = (uint16)R32(0x80064BF4);
    R16(0x80081B12) = (uint16)R32(0x80064BF4);
    sub_800399D4();
    sub_80039934();
    /* Original formatter receives the fixed PRES\\RESULTS.TIM literal */
    strcpy(path, (const char *)psx_addr(0x80011134, 1));
    sub_8004225C(path, buffer, 0x28800);
    R32(0x80065218) = R32(0x80065128);
    LoadImagePSX(&image_rect, (uint32 *)psx_addr(buffer + 20, 320 * 256 * 2));
    image_rect.y = 256;
    LoadImagePSX(&image_rect, (uint32 *)psx_addr(buffer + 20, 320 * 256 * 2));
    DrawSync(0);
    image_rect.y = 0;
    MoveImage(&image_rect, 704, 0);
    DrawSync(0);
    VSync(0);
    draft_agent3_missing_8003D914(1);
    random = (sint32)randPSX();
    divisor = (sint32)R32(0x80064D4C);
    if (!divisor)
        xport_mips_break(7);
    if (divisor == -1 && (uint32)random == 0x80000000)
        xport_mips_break(6);
    sub_80020430((uint32)(random % divisor));
    if (R32(0x80065110) == 1)
        R32(0x80065074) = 3;
    while (R32(0x80065074) != 666)
    {
        sub_8003A578();
        sub_800208E4();
        sub_800399D4();
        sub_80039908();
        VSync(0);
        sub_80039934();
        if (R32(0x80064BD8))
            MoveImage(&background_rect, 0, (sint32)(R32(0x80065128) << 8));
        switch (R32(0x80065074))
        {
            case 0:
                sub_800294BC();
                R32(0x80065074) = 1;
                /* Fall through */
            case 1:
                sub_8002ABE4();
                value = sub_800254DC();
                if (value == 1)
                    R32(0x80065074) = R32(0x8005973C) == 2 ? 4 : 3;
                else if (value == 2)
                    R32(0x80065074) = 666;
                break;
            case 3:
                value = sub_800286BC();
                if (!saved && !R32(0x80065110) && !R32(0x80064B5C))
                {
                    /* Capture league continuation after qualification and unlocks */
                    if (R32(0x80059738) >= 7 && R32(0x80059738) < 12)
                    {
                        R32(0x80078CA8) = R8(0x8005A791 + (R32(0x80059738) << 4) + R32(0x80065200)) == 255 ? 0 : 1;
                        R32(0x80065244) = R32(0x80065200) + (R32(0x80064ED4) == 1 ? 1 : 0);
                        sub_8002BDE8(0);
                    }
                    if (sub_800247DC(0) != 1)
                        draft_agent1_missing_diagnostic("Unable to save race progress\n");
                    saved = 1;
                }
                switch (value)
                {
                    case 1:
                        if (!R32(0x80064ED4))
                            R8(0x80064EA1) = 1;
                        R32(0x80065074) = 666;
                        break;
                    case 2:
                    case 13:
                        R8(0x80064EA0) = 1;
                        R32(0x80065074) = 666;
                        break;
                    case 3:
                    case 14:
                        R32(0x80064EC4) = 1;
                        R32(0x80065074) = 1;
                        break;
                    case 5:
                        R32(0x80065074) = 7;
                        break;
                    case 11:
                        R32(0x80065074) = 666;
                        break;
                    case 12:
                        R8(0x80064EA1) = 1;
                        R32(0x80065074) = 666;
                        break;
                }
                break;
            case 4:
                if (sub_800272F0() == 1)
                    R32(0x80065074) = 3;
                break;
            case 7:
                value = sub_80024D60(5, 1);
                if (value == 1)
                {
                    special = 1;
                    if (R32(0x80064BE0) == 0xFFFFFFFF)
                        R32(0x80064BE0) = R32(0x80059738) + 100;
                    R32(0x80065074) = 666;
                }
                else if (value == 2)
                    R32(0x80065074) = 3;
                break;
        }
        sub_800200E0();
        DrawSync(0);
        sub_80039A10();
        value = R32(0x80065074);
        ++R32(0x80064EBC);
        if (value == 666)
            break;
    }
    sub_8001DE6C();
    if (special)
        sub_8002BC5C(0x8005A624);
    if (R8(0x80064EA0) != 1 && R8(0x80064EA1) != 1 && !special && (sint32)R32(0x80059738) >= 7)
    {
        value = R32(0x80065200) + 1;
        R32(0x80065200) = value;
        value = R8(0x8005A790 + (R32(0x80059738) << 4) + value);
        if (value == 255)
        {
            if (R32(0x800656B8) == 1)
                sub_8002BC5C(0x8005A508);
        }
        else
        {
            R32(0x80064C04) = value + 1;
            R32(0x80064C08) = value + 1;
            R32(0x80059740) = value;
            R8(0x80064EA1) = 1;
        }
    }
    result = R32(0x80064A48);
    R32(0x80065074) = 0;
    R32(0x800656B4) = result;
    return result;
}
