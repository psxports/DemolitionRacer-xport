#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4))
#define R16(a) (*(uint16 *)psx_addr((a), 2))
#define R8(a) (*(uint8 *)psx_addr((a), 1))

extern void pres_80093910(uint32 style);
extern uint32 pres_80092F64(uint32 font, uint32 text);
extern void pres_80093008(uint32 font, sint32 x, sint32 y, uint32 text);
extern uint32 sub_800246B0(uint32 slot);
extern uint32 sub_8002409C(uint32 load);

static sint32 card_half(uint32 value)
{
    return (sint32)(value + (value >> 31)) >> 1;
}

static void card_style(uint32 style)
{
    if (R32(0x80064E60))
        sub_80021430(style);
    else
        pres_80093910(style);
}

static uint32 card_width(uint32 font, uint32 text)
{
    return R32(0x80064E60) ? sub_80020A14(5, text) : pres_80092F64(font & 255, text);
}

static void card_text(uint32 font, uint32 x, sint32 y, uint32 text)
{
    if (R32(0x80064E60))
        sub_80020AB8(5, (sint32)x, y, (const char *)psx_addr(text, 1));
    else
        pres_80093008(font & 255, (sint32)x, y, text);
}

static void card_center(uint32 table, sint32 y)
{
    uint32 width = card_width(0, R32(table + R32(0x80064B8C) * 4));
    uint32 x = (uint32)card_half(R32(0x80064E58) - width);
    card_text(0, x, y, R32(table + R32(0x80064B8C) * 4));
}

uint32 sub_800218CC(void)
{
    uint32 width = card_width(0, R32(0x8005A380 + R32(0x80064B8C) * 4));
    uint32 x, packet, descriptor, environment, table;
    card_style(0);
    x = (uint32)card_half(R32(0x80064E58) - width);
    card_text(0, x, 212, R32(0x8005A380 + R32(0x80064B8C) * 4));
    if (R32(0x80064E60))
    {
        /* Original selects the resident style even before the route is reread */
        sub_80021430(8);
        x = (uint32)card_half(R32(0x80064E58) - width) + width + 4;
        card_text(0, x, 212, 0x80064E64);
    }
    else
    {
        packet = 0x80074590 + R32(0x80065128) * 40;
        descriptor = R32(0x800652F4);
        x = (uint32)card_half(R32(0x80064E58) - width) + width;
        R16(packet + 8) = (uint16)(x + 6);
        R16(packet + 10) = 214;
        R16(packet + 18) = 214;
        R16(packet + 24) = (uint16)(x + 6);
        R16(packet + 16) = (uint16)(x + R8(descriptor + 22) + 5);
        R16(packet + 26) = (uint16)(R8(descriptor + 23) + 213);
        R16(packet + 32) = (uint16)(x + R8(descriptor + 22) + 5);
        environment = R32(0x80065118);
        R16(packet + 34) = (uint16)(R8(descriptor + 23) + 213);
        table = R32(environment + 112);
        R32(packet) = (R32(packet) & 0xFF000000) | (R32(table) & 0xFFFFFF);
        table = R32(environment + 112);
        R32(table) = (R32(table) & 0xFF000000) | (packet & 0xFFFFFF);
    }
    if (!(R16(0x80065524) & 0x4000))
        return 0;
    R16(0x80065524) = 0;
    return 1;
}

uint32 sub_80021B24(uint32 font, uint32 initial_selection)
{
    uint32 width, origin, selection;
    if (R8(0x80064E66))
    {
        R8(0x80064E67) = (uint8)initial_selection;
        R8(0x80064E66) = 0;
    }
    card_style(0);
    if (!R8(0x80064E67))
        card_style(1);
    width = card_width(font, R32(0x8005A200 + R32(0x80064B8C) * 4));
    origin = (uint32)card_half(R32(0x80064E58) - width) - (uint32)card_half(width);
    card_text(font, origin - 4, 212, R32(0x8005A200 + R32(0x80064B8C) * 4));
    card_style(0);
    if (R8(0x80064E67) == 1)
        card_style(1);
    card_text(font, origin + width + 4, 212, R32(0x8005A214 + R32(0x80064B8C) * 4));
    if (R16(0x80065524) & 0x80)
    {
        if (R8(0x80064E67))
            --R8(0x80064E67);
        R16(0x80065524) = 0;
    }
    if (R16(0x80065524) & 0x20)
    {
        if (!R8(0x80064E67))
            ++R8(0x80064E67);
        R16(0x80065524) = 0;
    }
    if (!(R16(0x80065524) & 0x4000))
        return 0;
    selection = R8(0x80064E67);
    R8(0x80064E66) = 1;
    R16(0x80065524) = 0;
    return selection + 1;
}

uint32 sub_80023B58(uint32 slot)
{
    _card_info((sint32)(slot << 4));
    for (;;)
    {
        if (TestEvent((sint32)R32(0x800652FC)) == 1)
            return 0;
        if (TestEvent((sint32)R32(0x80065300)) == 1)
            return 1;
        if (TestEvent((sint32)R32(0x80065304)) == 1)
            return 2;
        if (TestEvent((sint32)R32(0x80065308)) == 1)
            return 3;
    }
}

uint32 sub_80023BD4(uint32 slot)
{
    char path[32];
    sint32 descriptor;
    snprintf(path, sizeof(path), "bu%d0:BASLUS-00969", (sint32)slot);
    descriptor = openPSX(path, 0x10200);
    if (descriptor < 0)
        return 0;
    closePSX(descriptor);
    return 1;
}

uint32 sub_800247DC(uint32 slot)
{
    char path[32], title[24];
    uint32 index, offset, words[4], written;
    sint32 descriptor;
    memcpy(title, psx_addr(0x80011074, 17), 17);
    snprintf(path, sizeof(path), "bu%d0:BASLUS-00969", (sint32)slot);
    memset(psx_addr(0x80076838, 0x2000), 0, 0x2000);
    R8(0x80076838) = 'S';
    R8(0x80076839) = 'C';
    R8(0x8007683A) = 0x11;
    R8(0x8007683B) = 1;
    for (index = 0; (sint32)index < (sint32)strlen(title); ++index)
    {
        R8(0x8007683C + index * 2) = title[index] == ' ' ? 0x81 : 0x82;
        R8(0x8007683D + index * 2) = title[index] == ' ' ? 0x40 : (uint8)(title[index] + 31);
    }
    for (offset = 0; offset < 32; offset += 16)
    {
        for (index = 0; index < 4; ++index)
            words[index] = R32(0x8005A45C + offset + index * 4);
        for (index = 0; index < 4; ++index)
            R32(0x80076898 + offset + index * 4) = words[index];
    }
    for (offset = 0; offset < 128; offset += 16)
    {
        for (index = 0; index < 4; ++index)
            words[index] = R32(0x8005A488 + offset + index * 4);
        for (index = 0; index < 4; ++index)
            R32(0x800768B8 + offset + index * 4) = words[index];
    }
    R32(0x80065320) = 0;
    R32(0x8006531C) = 0x80076A3C;
    sub_8002409C(0);
    for (index = 0; index < 7676; ++index)
        R32(0x80076A38) += R8(0x80076A3C + index);
    descriptor = openPSX(path, 2);
    if (descriptor < 0)
        return 0;
    written = (uint32)writePSX(descriptor, psx_addr(0x80076838, 0x2000), 0x2000);
    closePSX(descriptor);
    return written == 0x2000;
}

static uint32 card_format_pending(uint32 channel)
{
    /* TODO: Bind original 8004D7B8 numeric-channel format to the toolset */
    draft_agent1_missing_diagnostic("Unimplemented memory-card format, channel %u\n", channel);
    abort();
}

uint32 sub_80021DB8(void)
{
    uint32 state, value, counter;
    card_style(0);
    state = R32(0x80064E68);
    switch (state)
    {
        case 0:
            R32(0x80064E68) = 1;
            break;
        case 1:
            R32(0x80064E68) = 2;
            break;
        case 2:
        case 3:
            card_style(2);
            card_center(0x8005A3A8, 196);
            card_center(0x8005A3D0, 204);
            R32(0x80064E68) = state + 1;
            if (state == 2)
                R32(0x80064E5C) = 0;
            R32(0x80064BD8) = 0;
            break;
        case 4:
            if (sub_80024A30(R32(0x80064E74)))
            {
                R32(0x80064E5C) = 1;
                R32(0x80064E68) = 5;
            }
            else
            {
                R32(0x80064E5C) = 0;
                if (sub_80024A30(R32(0x80064E74)))
                {
                    R32(0x80064E68) = 5;
                    R32(0x80064E5C) = 1;
                }
                else
                {
                    R32(0x80064E68) = 29;
                    R32(0x80064E5C) = 0;
                }
            }
            if (R32(0x80064E6C) == 1)
                R32(0x80064E68) = 13;
            value = sub_80023B58(R32(0x80064E74));
            R32(0x800652F8) = value;
            R32(0x800652F0) = value;
            break;
        case 5:
            R32(0x80064E6C) = 0;
            break;
        case 6:
        case 13:
        case 20:
            card_center(state == 6 ? 0x8005A2A4 : state == 13 ? 0x8005A2F4 : 0x8005A290, 196);
            value = sub_80021B24(0, 1);
            if (value == 1)
                R32(0x80064E68) = state + 1;
            else if (value == 2)
                R32(0x80064E68) = 5;
            break;
        case 7:
            R32(0x80064E68) = sub_80023F78(R32(0x80064E74)) ? 8 : 34;
            break;
        case 8:
        case 9:
        case 15:
        case 16:
        case 23:
        case 24:
            card_style(2);
            card_center(state < 10 ? 0x8005A2B8 : state < 17 ? 0x8005A3E4 : 0x8005A254, state == 24 ? 164 : 196);
            card_center(0x8005A3D0, state == 24 ? 172 : 204);
            R32(0x80064E68) = state + 1;
            R32(0x80064BD8) = 0;
            break;
        case 10:
            R32(0x80064E68) = sub_800246B0(R32(0x80064E74)) == 1 ? 11 : 33;
            break;
        case 11:
            card_style(2);
            card_center(0x8005A2CC, 196);
            if (sub_800218CC())
                R32(0x80064E68) = 5;
            break;
        case 12:
            if (R32(0x80075708 + R32(0x80064E74) * 4396))
            {
                if (sub_80023F78(R32(0x80064E74)))
                    R32(0x80064E68) = 20;
                else if (sub_80023FDC(R32(0x80064E74)) == 1)
                    R32(0x80064E68) = 30;
                else
                    R32(0x80064E68) = 13;
            }
            else
            {
                R32(0x80064E6C) = 1;
                R32(0x80064E68) = 22;
            }
            break;
        case 14:
        case 21:
            R32(0x80064E68) = 15;
            break;
        case 17:
            if (sub_80023F78(R32(0x80064E74)) || sub_80023BD4(R32(0x80064E74)))
                R32(0x80064E68) = sub_800247DC(R32(0x80064E74)) == 1 ? 18 : 32;
            else
                R32(0x80064E68) = 31;
            break;
        case 18:
            card_style(2);
            card_center(0x8005A308, 196);
            if (sub_800218CC())
                R32(0x80064E68) = 19;
            break;
        case 19:
        case 28:
            R32(0x80064E68) = 0;
            break;
        case 22:
            card_style(2);
            card_center(0x8005A228, 196);
            card_style(0);
            card_center(0x8005A240, 204);
            value = sub_80021B24(0, 1);
            if (value == 1)
                R32(0x80064E68) = 23;
            else if (value == 2)
                R32(0x80064E68) = 5;
            break;
        case 25:
            R32(0x80064E68) = card_format_pending(R32(0x80064E74) << 4) == 1 ? 26 : 35;
            break;
        case 26:
            R32(0x80064E68) = 27;
            break;
        case 27:
            card_center(0x8005A268, 196);
            if (sub_800218CC())
                R32(0x80064E68) = 28;
            break;
        case 29:
            card_style(3);
            card_center(0x8005A330, 196);
            if (sub_800218CC())
                R32(0x80064E68) = 5;
            break;
        case 30:
        case 31:
        case 32:
        case 33:
        case 34:
        case 35:
            card_style(3);
            card_center(state == 30 ? 0x8005A31C : state == 31 ? 0x8005A358 : state == 32 ? 0x8005A36C : state == 33 ? 0x8005A2E0 : state == 34 ? 0x8005A394 : 0x8005A27C, 196);
            if (sub_800218CC())
                R32(0x80064E68) = 0;
            break;
        case 36:
            R32(0x80064E68) = 0;
            return 1;
    }
    counter = R32(0x80064E70) + 1;
    R32(0x80064E70) = counter;
    if ((sint32)counter % 60 == 0)
    {
        value = sub_80023B58(R32(0x80064E74));
        counter = R32(0x800652F8);
        R32(0x800652F0) = value;
        if (value != counter)
        {
            R32(0x800652F8) = value;
            R32(0x80064E68) = 0;
        }
    }
    return 0;
}
