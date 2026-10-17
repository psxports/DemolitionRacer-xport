#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void pres_80093910(uint32 style);
extern void pres_80093008(uint32 font, sint32 x, sint32 y, uint32 text);

static uint32 pres_records_columns(uint32 descriptor)
{
    uint32 columns = r_u8(descriptor + 6u);
    if (!columns)
        abort();
    return columns;
}

/* Native string carrier for the original 80092E20 glyph-width algorithm */
static uint32 pres_records_half_width(uint32 identifier, const char *text)
{
    uint32 font = 0x800A21D0u + (identifier & 15u) * 144u;
    uint32 index, line = 0u, maximum = 0u, length = (uint32)strlen(text);
    for (index = 0; (sint32)index < (sint32)length; ++index)
    {
        uint32 character = (uint8)text[index], glyph = character - 32u;
        if (character == '$')
            glyph = 84u;
        if (character == '%')
            glyph = 85u;
        if (character == '^')
            glyph = 86u;
        if (character == '&')
            glyph = 87u;
        if (character == 10u)
        {
            if ((sint32)maximum < (sint32)line)
                maximum = line;
            line = 0;
        }
        else
            line += r_u8(font + 16u + (glyph & 255u)) + (character != 32u);
    }
    if ((sint32)maximum < (sint32)line)
        maximum = line;
    return (uint32)((sint32)(maximum + (maximum >> 31)) >> 1);
}

/* Native string carrier for the complete original 80093008 packet algorithm */
static void pres_records_text(uint32 font, sint32 x, sint32 y, const char *text)
{
    uint32 origin = (uint32)x;
    uint32 cursor, baseline = (uint32)y;
    uint32 descriptor = 0x800A21D0u + (font & 15u) * 144u;
    uint32 index, length;
    if (font & 0x80u)
        origin -= pres_records_half_width(font, text);
    cursor = origin;
    length = (uint32)strlen(text);
    for (index = 0u; (int32_t)index < (int32_t)length; ++index)
    {
        uint32 character = (uint8)text[index];
        uint32 glyph = character - 32u;
        uint32 packet, width, texture, object, head, count, tag;
        if (character == '$')
            glyph = 84u;
        if (character == '%')
            glyph = 85u;
        if (character == '^')
            glyph = 86u;
        if (character == '&')
            glyph = 87u;
        if (character == 10u)
        {
            cursor = origin;
            baseline += r_u8(descriptor + 5u);
            continue;
        }
        if (character == 13u)
        {
            uint32 remaining = pres_records_half_width(font, text + index + 1u) * 2u;
            cursor = r_u32(0x800ABBF4u) - remaining;
            continue;
        }
        if (character == '}')
        {
            uint32 remaining = pres_records_half_width(font, text + index + 1u) * 2u;
            cursor = origin - remaining;
            continue;
        }
        if (character == 9u)
        {
            cursor = r_u32(0x800ABBF4u);
            continue;
        }
        if (character == 32u)
        {
            cursor += r_u8(descriptor + 16u + (glyph & 255u));
            continue;
        }
        glyph &= 255u;
        width = r_u8(descriptor + 16u + glyph);
        packet = r_u32(0x800ABBE8u) + r_u32(0x800ABBECu) * 52u;
        w_u16(packet + 8u, (uint16)cursor);
        w_u16(packet + 10u, (uint16)baseline);
        w_u16(packet + 22u, (uint16)baseline);
        w_u16(packet + 32u, (uint16)cursor);
        w_u16(packet + 20u, (uint16)(cursor + r_u8(descriptor + 7u) + width));
        w_u16(packet + 34u, (uint16)(baseline + r_u8(descriptor + 5u)));
        w_u16(packet + 44u, (uint16)(cursor + r_u8(descriptor + 7u) + width));
        w_u16(packet + 46u, (uint16)(baseline + r_u8(descriptor + 5u)));

        texture = r_u32(descriptor + 12u);
        w_u8(packet + 12u, (uint8)(r_u8(texture + 24u) + r_u8(descriptor + 4u) * (glyph % pres_records_columns(descriptor))));
        texture = r_u32(descriptor + 12u);
        w_u8(packet + 13u, (uint8)(r_u8(texture + 25u) + r_u8(descriptor + 5u) * (glyph / pres_records_columns(descriptor))));
        texture = r_u32(descriptor + 12u);
        w_u8(packet + 24u, (uint8)(r_u8(texture + 24u) + r_u8(descriptor + 4u) * (glyph % pres_records_columns(descriptor)) + r_u8(descriptor + 7u) + width));
        texture = r_u32(descriptor + 12u);
        w_u8(packet + 25u, (uint8)(r_u8(texture + 25u) + r_u8(descriptor + 5u) * (glyph / pres_records_columns(descriptor))));
        texture = r_u32(descriptor + 12u);
        w_u8(packet + 36u, (uint8)(r_u8(texture + 24u) + r_u8(descriptor + 4u) * (glyph % pres_records_columns(descriptor))));
        texture = r_u32(descriptor + 12u);
        {
            uint32 height = r_u8(descriptor + 5u);
            w_u8(packet + 37u, (uint8)(height + r_u8(texture + 25u) + height * (glyph / pres_records_columns(descriptor))));
        }
        texture = r_u32(descriptor + 12u);
        w_u8(packet + 48u, (uint8)(r_u8(texture + 24u) + r_u8(descriptor + 4u) * (glyph % pres_records_columns(descriptor)) + r_u8(descriptor + 7u) + width));
        texture = r_u32(descriptor + 12u);
        {
            uint32 height = r_u8(descriptor + 5u);
            w_u8(packet + 49u, (uint8)(height + r_u8(texture + 25u) + height * (glyph / pres_records_columns(descriptor))));
        }
        {
            uint32 vertex, component;
            for (vertex = 0u; vertex < 4u; ++vertex)
                for (component = 0u; component < 3u; ++component)
                    w_u8(packet + 4u + vertex * 12u + component, (uint8)r_u8(0x800ABC00u + vertex * 4u + component));
        }
        w_u16(packet + 26u, r_u16(r_u32(descriptor + 12u) + 8u));
        texture = r_u32(descriptor + 12u);
        object = r_u32(0x80065118u);
        w_u16(packet + 14u, r_u16(texture + 10u));
        head = r_u32(object + 112u);
        tag = r_u32(packet);
        w_u32(packet, (tag & 0xFF000000u) | (r_u32(head) & 0xFFFFFFu));
        head = r_u32(object + 112u);
        count = r_u32(0x800ABBECu);
        object = r_u32(0x800ABBF0u);
        cursor += 1u + width;
        tag = r_u32(head);
        w_u32(0x800ABBECu, count + 1u);
        w_u32(0x800ABBF0u, object + 1u);
        w_u32(head, (tag & 0xFF000000u) | (packet & 0xFFFFFFu));
    }
}

void dr_pres_text_native(uint32 font, sint32 x, sint32 y, const char *text)
{
    pres_records_text(font, x, y, text);
}

uint32 pres_8009017C(void)
{
    static const uint32 titles[5] = {0x800A1CEC, 0x800A1CF8, 0x800A1D00, 0x800A1D08, 0x800A1D1C};
    static const uint32 title_bytes[5] = {11, 6, 8, 18, 8};
    static const uint32 tables[5] = {0x8006649C, 0x800665DC, 0x8006671C, 0x8006685C, 0x800668BC};
    uint32 selection = r_u32(0x800A1CE0), row, y = 64, record, index;
    uint32 time, minutes, seconds, denominator;
    uint8 title[18];
    char text[72];
    w_u32(0x800A1CE8, r_u8(0x800A1CD8 + selection));
    if (selection < 5)
    {
        record = r_u32(0x800A980C);
        memcpy(title, psx_addr(titles[selection], title_bytes[selection]), title_bytes[selection]);
        for (index = 0; index < title_bytes[selection]; ++index)
            w_u8(record + 14 + index, title[index]);
        w_u32(0x800A1CE4, tables[selection]);
    }
    for (row = 0; (sint32)row < (sint32)r_u32(0x800A1CE8); ++row, y += 12)
    {
        pres_80093910(2);
        record = r_u32(0x800A1CE4);
        pres_80093008(0, 64, (sint32)y, r_u32(0x800A1D98 + r_u32(record + 28) * 4));
        selection = r_u32(0x800A1CE0);
        record = r_u32(0x800A1CE4);
        if (selection == 0 || selection == 3)
            snprintf(text, sizeof(text), "%d", (sint32)r_u32(record + 24));
        else
        {
            time = r_u32(record + 20) * 100;
            denominator = r_u32(0x80065124);
            denominator = (uint32)((sint32)(denominator + (denominator >> 31)) >> 1);
            if (!denominator || (time == 0x80000000 && denominator == 0xFFFFFFFF))
                abort();
            time = (uint32)((sint32)time / (sint32)denominator);
            minutes = (uint32)((sint32)time / 6000);
            time -= minutes * 6000;
            seconds = (uint32)((sint32)time / 100);
            time -= seconds * 100;
            /* The third field is the fifth argument in the original call */
            snprintf(text, sizeof(text), "%02d : %02d : %02d", (sint32)minutes, (sint32)seconds, (sint32)time);
        }
        pres_records_text(0, 256, (sint32)y, text);
        snprintf(text, sizeof(text), "%s", (const char *)psx_addr(r_u32(0x800A1CE4), 1));
        pres_records_text(0, 370, (sint32)y, text);
        w_u32(0x800A1CE4, r_u32(0x800A1CE4) + 32);
    }
    if ((r_u16(0x80065524) & 0x20) && r_u32(0x800A1CE0) < 4)
    {
        sub_8001D01C(2);
        w_u32(0x800A1CE0, r_u32(0x800A1CE0) + 1);
    }
    if ((r_u16(0x80065524) & 0x80) && (sint32)r_u32(0x800A1CE0) > 0)
    {
        sub_8001D01C(2);
        w_u32(0x800A1CE0, r_u32(0x800A1CE0) - 1);
    }
    if (!(r_u16(0x80065524) & 0x5000))
        return 0;
    sub_8001D01C(0);
    return 1;
}
