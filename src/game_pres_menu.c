#include "draft_signatures.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

extern void pres_800939C0(void);
extern void pres_80092970(void);

/* Full PRES CFG 80091040 through 8009127C */
void pres_80091040(void)
{
    uint32 columns = r_u32(0x800A20BCu);
    uint32 rows = r_u32(0x800A20C0u);
    uint32 cells = sub_80042544(columns * rows);
    uint32 counts, polygons;
    uint32 x, y, index;
    w_u32(0x800A20B4u, cells);
    counts = sub_80042544(r_u32(0x800A20C4u));
    rows = r_u32(0x800A20C0u);
    columns = r_u32(0x800A20BCu);
    w_u32(0x800A20B8u, counts);
    polygons = sub_80042544(columns * rows * 48u);
    w_u32(0x800ABB50u, polygons);

    for (x = 0u; (int32_t)x < (int32_t)r_u32(0x800A20BCu); ++x)
        for (y = 0u; (int32_t)y < (int32_t)r_u32(0x800A20C0u); ++y)
            w_u8(r_u32(0x800A20B4u) + x + y * r_u32(0x800A20BCu), 0u);
    for (x = 0u; (int32_t)x < (int32_t)r_u32(0x800A20C4u); ++x)
    {
        uint32 destination = r_u32(0x800A20B8u) + x;
        uint32 value = r_u32(0x800A20C8u);
        w_u8(destination, (uint8)value);
    }
    polygons = r_u32(0x800ABB50u);
    for (index = 0u; (int32_t)index < (int32_t)(r_u32(0x800A20BCu) * r_u32(0x800A20C0u) * 2u); ++index)
    {
        uint32 packet = polygons + index * 24u;
        w_u8(packet + 3u, 5u);
        w_u8(packet + 7u, 0x28u);
        w_u8(packet + 4u, 0x40u);
        w_u8(packet + 5u, 0x40u);
        w_u8(packet + 6u, 0x40u);
    }
    for (index = 0u; index < 2u; ++index)
    {
        uint32 first = 0x800ABAD0u + index * 20u;
        uint32 second = 0x800ABBC0u + index * 20u;
        w_u8(first + 3u, 4u);
        w_u8(first + 7u, 0x20u);
        w_u8(first + 4u, 0x10u);
        w_u8(first + 5u, 0xF0u);
        w_u8(first + 6u, 0x10u);
        w_u8(second + 3u, 4u);
        w_u8(second + 7u, 0x20u);
        w_u8(second + 4u, 0x10u);
        w_u8(second + 5u, 0xF0u);
        w_u8(second + 6u, 0x10u);
    }
    w_u32(0x800A20D4u, 0u);
}

/* Full PRES CFG 80092DD4 through 80092E1C */
void pres_80092DD4(void)
{
    uint32 buffer = r_u32(0x80065128u);
    w_u32(0x800ABBF0u, 0u);
    w_u32(0x800ABBECu, buffer * 500u);
    pres_800939C0();
}

/* Full PRES CFG 80092854 through 8009296C */
void pres_80092854(void)
{
    uint32 start;
    for (start = 0u; start < 8u; ++start)
    {
        if (r_u8(0x800ABB00u + (start & 15u)) != 1u)
            continue;
        if (r_u8(0x800ABB00u + ((start + 1u) & 15u)) != 1u)
            continue;
        if (r_u8(0x800ABB00u + ((start + 2u) & 15u)) != 2u)
            continue;
        if (r_u8(0x800ABB00u + ((start + 3u) & 15u)) != 2u)
            continue;
        if (r_u8(0x800ABB00u + ((start + 4u) & 15u)) != 4u)
            continue;
        if (r_u8(0x800ABB00u + ((start + 5u) & 15u)) != 4u)
            continue;
        if (r_u8(0x800ABB00u + ((start + 6u) & 15u)) != 3u)
            continue;
        if (r_u8(0x800ABB00u + ((start + 7u) & 15u)) != 3u)
            continue;
        pres_80092970();
        break;
    }
}

/* Full PRES CFG 80092970 through 80092A4C */
void pres_80092970(void)
{
    uint32 index;
    for (index = 0u; index < 15u; ++index)
    {
        w_u8(0x800596DCu + index, 0u);
        w_u8(0x800596CCu + index, 0u);
    }
    for (index = 0u; index < 8u; ++index)
        w_u8(0x800596F3u - index, 0u);
    for (index = 0u; index < 7u; ++index)
        w_u8(0x800596FAu - index, 0u);
    w_u32(0x80064B28u, 0u);
    w_u32(0x80064B2Cu, 0u);
    w_u32(0x80064B30u, 0u);
    w_u32(0x80064B34u, 0u);
    w_u32(0x80064B38u, 0u);
    for (index = 0u; index < 16u; ++index)
        w_u8(0x800ABB0Fu - index, 0u);
    sub_8001D01C(1u);
    sub_8001D01C(1u);
    sub_8001D01C(1u);
    sub_8001D01C(1u);
}

/* Exact original SDK 8004D578: normal vector, RGB, NCCS and RGB2 store */
static void pres_normal_color(uint32 normal, uint32 color, uint32 destination)
{
    w_u32(destination, gte_normal_color_col((const SVECTOR *)psx_addr(normal, 8u), r_u32(color)));
}

/* Full PRES CFG 800939C0 through 80093CBC */
void pres_800939C0(void)
{
    MATRIX matrix;
    uint16 angle;
    uint32 index;
    memcpy(&matrix, psx_addr(0x8008B520u, 32u), 32u);
    sub_8003C0DC(0u, (const sint16 *)psx_addr(0x800A239Cu, 6u), 110u, 0u, 0u);
    sub_8003C0DC(1u, (const sint16 *)psx_addr(0x800A23A4u, 6u), 110u, 0u, 110u);
    sub_8003C0DC(2u, (const sint16 *)psx_addr(0x800A23ACu, 6u), 0u, 0u, 110u);
    sub_8003C0B4(90u, 50u, 50u);
    angle = (uint16)(r_u16(0x800A23B4u) + 16u);
    w_u16(0x800A23B4u, angle);
    RotMatrixX((sint16)angle, &matrix);
    RotMatrixY((sint16)r_u16(0x800A23B6u), &matrix);
    angle = (uint16)(r_u16(0x800A23B8u) + 32u);
    w_u16(0x800A23B8u, angle);
    RotMatrixZ((sint16)angle, &matrix);
    SetRotMatrix(&matrix);
    /* The original explicitly sends the first three matrix words to TRX, TRY and TRZ */
    {
        uint32 translation[3];
        memcpy(translation, &matrix, 12u);
        xport_gte_write_control(5u, translation[0]);
        xport_gte_write_control(6u, translation[1]);
        xport_gte_write_control(7u, translation[2]);
    }
    sub_8003C164(&matrix);
    for (index = 0u; index < 4u; ++index)
        pres_normal_color(0x800A23BCu + index * 8u, 0x800A2140u + index * 4u, 0x800A2150u + index * 4u);
    sub_8003C0B4(90u, 90u, 50u);
    sub_8003C164(&matrix);
    for (index = 0u; index < 4u; ++index)
        pres_normal_color(0x800A23BCu + index * 8u, 0x800A2160u + index * 4u, 0x800A21A0u + index * 4u);
    sub_8003C0B4(50u, 50u, 100u);
    sub_8003C164(&matrix);
    sub_8003C0DC(0u, (const sint16 *)psx_addr(0x800A239Cu, 6u), 0u, 50u, 50u);
    sub_8003C0DC(1u, (const sint16 *)psx_addr(0x800A23A4u, 6u), 0u, 0u, 110u);
    for (index = 0u; index < 4u; ++index)
        pres_normal_color(0x800A23BCu + index * 8u, 0x800A2140u + index * 4u, 0x800A2180u + index * 4u);
    sub_8003C0B4(50u, 50u, 50u);
}

extern void pres_80093910(uint32 style);
extern uint32 pres_80092F64(uint32 font, uint32 guest_text);
extern void pres_80093008(uint32 font, sint32 x, sint32 y, uint32 guest_text);
extern void pres_800A0240(sint32 flags, uint32 object);
extern uint32 sub_800246B0(uint32 slot);

static uint32 pres_message_pointer(uint32 table)
{
    return r_u32(table + r_u32(0x80064B8Cu) * 4u);
}

static void pres_message_center(uint32 font, sint32 y, uint32 table)
{
    uint32 width = pres_80092F64(font, pres_message_pointer(table));
    uint32 remaining = 512u - width;
    uint32 text = pres_message_pointer(table);
    sint32 x = (sint32)(remaining + (remaining >> 31)) >> 1;
    pres_80093008(font, x, y, text);
}

static void pres_message_row(sint32 y, uint32 label_table, uint32 value_word, uint32 value_table)
{
    pres_80093008(1u, 48, y, pres_message_pointer(label_table));
    pres_80093008(1u, 216, y, value_table ? pres_message_pointer(value_table) : r_u32(value_word));
}

/* Full PRES CFG 80098814 through 800991D8 */
void pres_80098814(uint32 object)
{
    uint32 mode;
    if ((int32_t)r_u32(0x80064BE0u) < 100)
    {
        pres_80093910(1u);
        pres_message_center(1u, 14, 0x800A6FCCu);
        pres_80093910(0u);
        pres_message_center(2u, 42, 0x800A6ED0u);
    }
    else
    {
        pres_80093910(1u);
        pres_message_center(1u, 14, 0x800A7028u);
        pres_80093910(0u);
        pres_message_center(2u, 84, 0x800A7064u);
    }
    pres_80093910(2u);
    mode = r_u32(0x80064BE0u);
    switch (mode)
    {
        case 8u:
            pres_message_row(66, 0x800A6F8Cu, 0u, 0x800A70ECu);
            pres_message_row(84, 0x800A6F8Cu, 0x800A1F30u, 0u);
            pres_message_row(102, 0x800A6F14u, 0x800A1EC8u, 0u);
            pres_message_row(120, 0x800A6F14u, 0x800A1ECCu, 0u);
            pres_message_row(138, 0x800A6F50u, 0x800A1DB0u, 0u);
            break;
        case 9u:
            pres_message_row(66, 0x800A6F8Cu, 0u, 0x800A7130u);
            pres_message_row(84, 0x800A6F8Cu, 0x800A1F34u, 0u);
            pres_message_row(102, 0x800A6F14u, 0x800A1EC4u, 0u);
            pres_message_row(120, 0x800A6F50u, 0x800A1DD0u, 0u);
            pres_message_row(138, 0x800A6F50u, 0x800A1DB4u, 0u);
            pres_message_row(156, 0x800A6F50u, 0x800A1DBCu, 0u);
            pres_message_row(174, 0x800A6F50u, 0x800A1DC8u, 0u);
            break;
        case 10u:
            pres_message_row(66, 0x800A6F8Cu, 0u, 0x800A7170u);
            pres_message_row(84, 0x800A6F50u, 0x800A1D9Cu, 0u);
            pres_message_row(102, 0x800A6F50u, 0x800A1DC4u, 0u);
            pres_message_row(120, 0x800A6F50u, 0x800A1DCCu, 0u);
            break;
        case 11u:
            pres_80093008(1u, 48, 66, pres_message_pointer(0x800A71FCu));
            pres_message_row(84, 0x800A6F14u, 0x800A1ED4u, 0u);
            break;
        case 7u:
            pres_message_row(66, 0x800A6F8Cu, 0x800A1F3Cu, 0u);
            pres_message_row(84, 0x800A6F14u, 0x800A1ED0u, 0u);
            break;
        case 108u:
            pres_message_center(1u, 102, 0x800A70A8u);
            break;
        case 109u:
            pres_message_center(1u, 102, 0x800A70ECu);
            break;
        case 110u:
            pres_message_center(1u, 102, 0x800A7130u);
            break;
        case 111u:
            pres_message_center(1u, 102, 0x800A7170u);
            break;
        case 107u:
            pres_message_center(1u, 102, 0x800A71B8u);
            break;
        default:
            break;
    }
    if (r_u16(0x80065524u) & 0x4008u)
    {
        w_u32(0x800ABB20u, 1u);
        sub_8001D01C(0u);
        w_u32(0x80064BE0u, 0xFFFFFFFFu);
        w_u32(0x800ABB58u, 4u);
        w_u32(0x800ABB30u, 0u);
        sub_8001D01C(1u);
    }
}

static void pres_card_hide_four(void)
{
    uint32 offset;
    for (offset = 0u; offset < 272u; offset += 68u)
        pres_800A0240(16u, 0x800AD860u + offset + r_u32(0x800AD6B8u) * 68u);
}

/* Full PRES CFG 80093D1C through 80094140 */
void pres_80093D1C(uint32 object)
{
    uint32 state;
    w_u16(0x800AD896u + r_u32(0x800AD7A8u) * 68u, r_u32(0x8006553Cu) == 255u ? 1u : 0u);
    state = r_u32(0x80064BDCu);
    if (state == 0u)
    {
        uint32 card_one = r_u32(0x80075704u);
        uint32 card_two = r_u32(0x80075708u);
        if (card_one == 1u && card_two == 1u && sub_80023F78(0u) != 0u)
        {
            w_u32(0x80064BDCu, card_two);
            pres_800A0240(512u, 0x800AD860u + r_u32(0x800AD6B8u) * 68u);
        }
        else
        {
            uint32 countdown = r_u32(0x800A31D4u);
            if (countdown == 0u)
            {
                w_u32(0x80064BDCu, 3u);
                pres_card_hide_four();
            }
            else
            {
                w_u32(0x800A31D4u, countdown - 1u);
                if (r_u32(0x80075704u) == 0u)
                {
                    pres_800A0240(512u, 0x800AD8A4u + r_u32(0x800AD6B8u) * 68u);
                    pres_800A0240(512u, 0x800AD92Cu + r_u32(0x800AD6B8u) * 68u);
                }
                else if (sub_80023FDC(0u) == 1)
                {
                    pres_800A0240(512u, 0x800AD8E8u + r_u32(0x800AD6B8u) * 68u);
                    pres_800A0240(512u, 0x800AD92Cu + r_u32(0x800AD6B8u) * 68u);
                }
            }
        }
    }
    else if (state == 1u)
    {
        w_u32(0x80064BDCu, 2u);
    }
    else if (state == 2u)
    {
        sub_800246B0(0u);
        w_u32(0x80064BDCu, 3u);
        pres_800A0240(16u, 0x800AD860u + r_u32(0x800AD6B8u) * 68u);
    }
    if ((r_u16(0x80065524u) & 0x4008u) != 0u && r_u32(0x800ABB48u) == 0u)
    {
        w_u32(0x80064BDCu, 3u);
        pres_card_hide_four();
        w_u32(0x800ABB20u, 1u);
        if (object == 0x800A2770u)
            w_u32(0x800ABB30u, 2u);
        else if (object == 0x800A25D4u)
            w_u32(0x800ABB30u, 7u);
        else if (object == 0x800A2658u && (sint16)r_u16(0x800AD896u + r_u32(0x800AD7A8u) * 68u) != 1)
            w_u32(0x800ABB30u, 7u);
        else if (object == 0x800A26D4u)
            w_u32(0x800ABB30u, 11u);
        sub_8001D01C(0u);
    }
}

extern uint32 pres_80092E20(uint32 font, uint32 guest_text);

/* Full PRES CFG 80093910 through 800939A4 */
void pres_80093910(uint32 style)
{
    uint32 source = 0x800A2140u + style * 16u;
    uint32 first = r_u32(source);
    uint32 second = r_u32(source + 4u);
    uint32 third = r_u32(source + 8u);
    w_u32(0x800ABC00u, first);
    w_u32(0x800ABC04u, second);
    w_u32(0x800ABC08u, third);
    w_u32(0x800ABC0Cu, r_u32(source + 12u));
}

/* Full PRES CFG 80092F64 through 80093004 */
uint32 pres_80092F64(uint32 font, uint32 guest_text)
{
    uint32 descriptor = 0x800A21D0u + font * 144u;
    uint32 width = 0u, index = 0u;
    while ((int32_t)index < (int32_t)(uint32)strlen((const char *)psx_addr(guest_text, 1u)))
    {
        uint32 character = (r_u8(guest_text + index) - 32u) & 255u;
        width += r_u8(descriptor + 16u + character);
        ++index;
    }
    return width;
}

static uint32 pres_font_columns(uint32 descriptor)
{
    uint32 columns = r_u8(descriptor + 6u);
    if (columns == 0u)
        abort();
    return columns;
}

/* Full PRES CFG 80093008 through 800935B4 */
void pres_80093008(uint32 font, sint32 x, sint32 y, uint32 guest_text)
{
    uint32 origin = (uint32)x;
    uint32 cursor, baseline = (uint32)y;
    uint32 descriptor = 0x800A21D0u + (font & 15u) * 144u;
    uint32 index, length;
    if (font & 0x80u)
        origin -= pres_80092E20(font, guest_text);
    cursor = origin;
    length = (uint32)strlen((const char *)psx_addr(guest_text, 1u));
    for (index = 0u; (int32_t)index < (int32_t)length; ++index)
    {
        uint32 character = r_u8(guest_text + index);
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
            uint32 remaining = pres_80092E20(font, guest_text + index + 1u) * 2u;
            cursor = r_u32(0x800ABBF4u) - remaining;
            continue;
        }
        if (character == '}')
        {
            uint32 remaining = pres_80092E20(font, guest_text + index + 1u) * 2u;
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
        w_u8(packet + 12u, (uint8)(r_u8(texture + 24u) + r_u8(descriptor + 4u) * (glyph % pres_font_columns(descriptor))));
        texture = r_u32(descriptor + 12u);
        w_u8(packet + 13u, (uint8)(r_u8(texture + 25u) + r_u8(descriptor + 5u) * (glyph / pres_font_columns(descriptor))));
        texture = r_u32(descriptor + 12u);
        w_u8(packet + 24u, (uint8)(r_u8(texture + 24u) + r_u8(descriptor + 4u) * (glyph % pres_font_columns(descriptor)) + r_u8(descriptor + 7u) + width));
        texture = r_u32(descriptor + 12u);
        w_u8(packet + 25u, (uint8)(r_u8(texture + 25u) + r_u8(descriptor + 5u) * (glyph / pres_font_columns(descriptor))));
        texture = r_u32(descriptor + 12u);
        w_u8(packet + 36u, (uint8)(r_u8(texture + 24u) + r_u8(descriptor + 4u) * (glyph % pres_font_columns(descriptor))));
        texture = r_u32(descriptor + 12u);
        {
            uint32 height = r_u8(descriptor + 5u);
            w_u8(packet + 37u, (uint8)(height + r_u8(texture + 25u) + height * (glyph / pres_font_columns(descriptor))));
        }
        texture = r_u32(descriptor + 12u);
        w_u8(packet + 48u, (uint8)(r_u8(texture + 24u) + r_u8(descriptor + 4u) * (glyph % pres_font_columns(descriptor)) + r_u8(descriptor + 7u) + width));
        texture = r_u32(descriptor + 12u);
        {
            uint32 height = r_u8(descriptor + 5u);
            w_u8(packet + 49u, (uint8)(height + r_u8(texture + 25u) + height * (glyph / pres_font_columns(descriptor))));
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

extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);

static uint32 pres_signed_quotient(uint32 numerator, uint32 denominator)
{
    if (denominator == 0u || (numerator == 0x80000000u && denominator == 0xFFFFFFFFu))
        abort();
    return (uint32)((int32_t)numerator / (int32_t)denominator);
}

static void pres_link_packet(uint32 packet, uint32 object, uint32 depth_offset)
{
    uint32 head = r_u32(object + 112u) + depth_offset;
    uint32 tag = r_u32(packet);
    w_u32(packet, (tag & 0xFF000000u) | (r_u32(head) & 0xFFFFFFu));
    head = r_u32(object + 112u) + depth_offset;
    tag = r_u32(head);
    w_u32(head, (tag & 0xFF000000u) | (packet & 0xFFFFFFu));
}

/* Full PRES CFG 80094604 through 80094B04 */
void pres_80094604(sint32 x, sint32 y)
{
    uint16 grid_x[6][7], grid_y[6][7];
    uint8 shades[2][6][7];
    uint32 column, row;
    uint32 packet = r_u32(0x800AD788u) + r_u32(0x80065128u) * 1560u;
    uint32 object = 0u;
    w_u32(0x800ABC10u, r_u32(0x800ABC10u) + 1u);
    for (column = 0u; column < 7u; ++column)
    {
        for (row = 0u; row < 6u; ++row)
        {
            uint32 wave = (uint32)draft_agent3_missing_8004388C(r_u32(0x800ABC10u) * 72u + column * 768u + row * 768u);
            uint32 divisor, numerator, brightness;
            if ((int32_t)wave < 0)
                wave += 63u;
            wave = (uint32)((int32_t)wave >> 6);
            divisor = wave + 1280u;
            numerator = (uint32)draft_agent3_missing_800437EC(r_u32(0x800ABC10u) * 80u + column * 512u + row * 1024u);
            numerator += (column * 2u - 6u) * 28000u;
            grid_x[row][column] = (uint16)(pres_signed_quotient(numerator, divisor) + (uint32)x);
            numerator = (uint32)draft_agent3_missing_800437EC(r_u32(0x800ABC10u) * 96u + column * 768u + row * 384u);
            numerator += 0xFFFEC780u + row * 32000u;
            grid_y[row][column] = (uint16)(pres_signed_quotient(numerator, divisor) + 120u);
            brightness = (80u - wave) & 255u;
            shades[1][row][column] = (uint8)brightness;
            shades[0][row][column] = (uint8)(brightness >> 1);
        }
    }
    for (column = 0u; column < 6u; ++column)
    {
        for (row = 0u; row < 5u; ++row)
        {
            uint32 parity = (column + row) & 1u;
            uint32 vertex, component;
            uint8 colors[4];
            w_u16(packet + 8u, grid_x[row][column]);
            w_u16(packet + 10u, grid_y[row][column]);
            w_u16(packet + 20u, grid_x[row][column + 1u]);
            w_u16(packet + 22u, grid_y[row][column + 1u]);
            w_u16(packet + 32u, grid_x[row + 1u][column]);
            w_u16(packet + 34u, grid_y[row + 1u][column]);
            w_u16(packet + 44u, grid_x[row + 1u][column + 1u]);
            w_u16(packet + 46u, grid_y[row + 1u][column + 1u]);
            colors[0] = (uint8)(80u + shades[parity][row][column]);
            colors[1] = (uint8)(80u + shades[parity][row][column + 1u]);
            colors[2] = (uint8)(80u + shades[parity][row + 1u][column]);
            colors[3] = (uint8)(80u + shades[parity][row + 1u][column + 1u]);
            for (vertex = 0u; vertex < 4u; ++vertex)
                for (component = 0u; component < 3u; ++component)
                    w_u8(packet + 4u + vertex * 12u + component, colors[vertex]);
            object = r_u32(0x80065118u);
            pres_link_packet(packet, object, 800u);
            packet += 52u;
        }
    }
    packet = 0x800AD838u + r_u32(0x80065128u) * 12u;
    pres_link_packet(packet, object, 800u);
}

static void pres_line_colors(uint32 base, uint32 color)
{
    w_u8(base + r_u32(0x80065128u) * 20u + 4u, (uint8)color);
    w_u8(base + r_u32(0x80065128u) * 20u + 5u, (uint8)color);
    w_u8(base + r_u32(0x80065128u) * 20u + 6u, (uint8)color);
    w_u8(base + r_u32(0x80065128u) * 20u + 12u, (uint8)color);
    w_u8(base + r_u32(0x80065128u) * 20u + 13u, (uint8)color);
    w_u8(base + r_u32(0x80065128u) * 20u + 14u, (uint8)color);
}

/* Full PRES CFG 8009D56C through 8009DA04 */
void pres_8009D56C(void)
{
    uint32 settings = r_u32(0x80065330u) + (r_u32(0x800A5A08u) == 0u ? 8u : 20u);
    uint32 flashing = r_u32(0x800ABB28u) << 3;
    uint32 color = r_u32(0x800A5A04u) == 0u ? flashing : 255u;
    uint32 packet, position, object;
    pres_line_colors(0x800AD7B8u, color);
    position = r_u32(settings) + 72u;
    packet = 0x800AD7B8u + r_u32(0x80065128u) * 20u;
    object = r_u32(0x80065118u);
    w_u16(packet + 8u, 24u);
    w_u16(packet + 10u, (uint16)position);
    w_u16(packet + 16u, 56u);
    w_u16(packet + 18u, (uint16)position);
    pres_link_packet(packet, object, 0u);
    color = r_u32(0x800A5A04u) == 1u ? flashing : 255u;
    pres_line_colors(0x800AD7CCu, color);
    position = r_u32(settings + 8u) + 128u;
    packet = 0x800AD7CCu + r_u32(0x80065128u) * 20u;
    object = r_u32(0x80065118u);
    w_u16(packet + 8u, 64u);
    w_u16(packet + 10u, (uint16)position);
    w_u16(packet + 16u, 103u);
    w_u16(packet + 18u, (uint16)position);
    pres_link_packet(packet, object, 0u);
    pres_line_colors(0x800AD7E0u, color);
    packet = 0x800AD7E0u + r_u32(0x80065128u) * 20u;
    position = r_u32(settings + 4u) + 64u;
    w_u16(packet + 8u, (uint16)position);
    w_u16(packet + 10u, 128u);
    object = r_u32(0x80065118u);
    w_u16(packet + 18u, 167u);
    position = r_u32(settings + 4u) + 64u;
    w_u16(packet + 16u, (uint16)position);
    pres_link_packet(packet, object, 0u);
}

/* Full PRES MIPS CFG 8009A4A4 through 8009A8C8 */
void pres_8009A4A4(void)
{
    uint32 state = r_u32(0x800AD678u);
    uint32 buffer = r_u32(0x80059748u) * 4u;
    uint32 source, texture, index;
    if (state == 1u)
    {
        texture = r_u32(0x800A8508u + buffer);
        source = 0x8005977Au;
        w_u32(0x80065204u, 0x8006648Cu);
        w_u32(0x800651FCu, 0x80065228u);
    }
    else
    {
        texture = r_u32((state == 0u ? 0x800A8470u : 0x800A8404u) + buffer);
        source = 0x80059770u;
        w_u32(0x80065204u, 0x8006647Cu);
        w_u32(0x800651FCu, 0x8006521Cu);
    }
    w_u32(0x800A8384u + buffer, texture);
    w_u32(r_u32(0x800651FCu), 0u);
    for (index = 0u; index < 8u; ++index)
        w_u8(r_u32(0x800A9488u + index * 4u), index == 0u ? 65u : 32u);
    for (index = 0u; index < 8u; ++index)
        w_u32(0x800A80D4u + index * 68u + r_u32(0x80059748u) * 4u, r_u32(0x800A9488u + index * 4u));
    if (r_u8(source) == 0u)
    {
        for (index = 0u; index < 8u; ++index)
            w_u8(r_u32(0x80065204u) + index, 32u);
        w_u32(0x800AD668u, 0u);
        w_u32(0x800AD560u, 0u);
        return;
    }
    while (r_u8(source + r_u32(r_u32(0x800651FCu))) != 0u)
    {
        uint32 cursor_pointer = r_u32(0x800651FCu);
        uint32 cursor = r_u32(cursor_pointer);
        uint32 node, text;
        if ((int32_t)cursor >= 8)
            break;
        node = r_u32(0x800ABC1Cu + (r_u32(0x800AD7A8u) + cursor) * 48u);
        text = r_u32(node + r_u32(0x80059748u) * 4u + 8u);
        w_u8(text, r_u8(source + cursor));
        cursor_pointer = r_u32(0x800651FCu);
        cursor = r_u32(cursor_pointer);
        node = r_u32(0x800AD860u + (r_u32(0x800AD6B8u) + cursor) * 68u);
        w_u32(node + 4u, 0x800A7FFCu);
        w_u32(cursor_pointer, r_u32(cursor_pointer) + 1u);
    }
    w_u32(0x800AD668u, 29u);
    w_u32(0x800AD560u, 29u);
}

static void pres_name_fill_empty(void)
{
    uint32 index;
    for (index = 0u; index < 8u; ++index)
    {
        uint32 byte = r_u32(0x80065204u) + index;
        if (r_u8(byte) == 0u)
            w_u8(byte, 32u);
    }
}

static uint32 pres_name_descriptor(uint32 base, uint32 cursor)
{
    return 0x800AD860u + (r_u32(base) + cursor) * 68u;
}

static uint32 pres_name_text(uint32 cursor)
{
    uint32 node = r_u32(0x800ABC1Cu + (r_u32(0x800AD7A8u) + cursor) * 48u);
    return r_u32(node + r_u32(0x80059748u) * 4u + 8u);
}

/* Full PRES MIPS CFG 8009A8CC through 8009AE04 */
void pres_8009A8CC(uint32 object)
{
    uint8 characters[31];
    uint32 buttons, cursor_pointer, cursor;
    uint8 character;
    memcpy(characters, psx_addr(0x800A94F0u, 31u), 31u);
    if (r_u32(0x800AD678u) == 1u)
        w_u32(0x800A23E0u, 1u);
    buttons = r_u16(0x80065524u + r_u32(0x800A23E0u) * 2u);
    if ((buttons & 0x1000u) != 0u)
    {
        sub_8001D01C(3u);
        pres_name_fill_empty();
        w_u32(0x800ABB20u, 3u);
        w_u32(0x800ABB30u, 0u);
        return;
    }
    if ((buttons & 0x4000u) != 0u)
    {
        sub_8001D01C(0u);
        character = characters[r_u32(0x800AD668u)];
        if (character == 'b')
        {
            pres_name_fill_empty();
            w_u32(0x800ABB20u, 1u);
            w_u32(0x800ABB30u, 0u);
            sub_8001D01C(1u);
            return;
        }
        cursor_pointer = r_u32(0x800651FCu);
        cursor = r_u32(cursor_pointer);
        if (character == 'a' && cursor == 8u)
        {
            w_u32(cursor_pointer, 7u);
            w_u8(r_u32(0x80065204u) + r_u32(r_u32(0x800651FCu)), 32u);
        }
        else if (character != 'a' && (int32_t)cursor < 8)
        {
            pres_800A0240(0x200, pres_name_descriptor(0x800AD7A8u, cursor));
            cursor_pointer = r_u32(0x800651FCu);
            cursor = r_u32(cursor_pointer);
            w_u32(r_u32(pres_name_descriptor(0x800AD6B8u, cursor + 1u)) + 4u, 0x800A7FFCu);
            w_u8(r_u32(0x80065204u) + cursor, characters[r_u32(0x800AD668u)]);
            cursor_pointer = r_u32(0x800651FCu);
            w_u32(cursor_pointer, r_u32(cursor_pointer) + 1u);
        }
        else if (cursor != 0u && characters[r_u32(0x800AD668u)] == 'a')
        {
            w_u32(r_u32(pres_name_descriptor(0x800AD6B8u, cursor)) + 4u, 0x800A801Cu);
            w_u8(pres_name_text(r_u32(cursor_pointer)), 32u);
            cursor_pointer = r_u32(0x800651FCu);
            w_u32(cursor_pointer, r_u32(cursor_pointer) - 1u);
            w_u8(r_u32(0x80065204u) + r_u32(r_u32(0x800651FCu)), 32u);
        }
        if (r_u32(r_u32(0x800651FCu)) == 8u)
            w_u32(0x800AD668u, 29u);
    }
    character = characters[r_u32(0x800AD668u)];
    if (character == 'a' || character == 'b')
        character = 64u;
    cursor_pointer = r_u32(0x800651FCu);
    cursor = r_u32(cursor_pointer);
    if (cursor == 8u)
        return;
    if (character != r_u8(0x800AD70Fu))
    {
        w_u8(0x800AD70Fu, character);
        w_u16(pres_name_descriptor(0x800AD7A8u, r_u32(cursor_pointer)) + 48u, 24u);
    }
    w_u8(pres_name_text(r_u32(r_u32(0x800651FCu))), character);
    pres_800A0240(8, pres_name_descriptor(0x800AD7A8u, r_u32(r_u32(0x800651FCu))));
}

/* Full PRES MIPS CFG 80090D44 through 80090E98 */
void pres_80090D44(uint32 slot, uint32 selection)
{
    uint32 table = 0x800A1F9Cu + selection * 4u;
    uint32 source = r_u32(0x800ABB60u) + r_u32(table);
    uint32 bytes = sub_80015150(source);
    uint32 image = sub_80042544(bytes);
    uint32 texture, clut, page;
    PSX_RECT rectangle;
    sub_80015074(r_u32(0x800ABB60u) + r_u32(table), image, 65536);
    texture = sub_80039ADC(slot != 0u ? 0x800A209Cu : 0x800A2094u);
    clut = r_u16(texture + 10u);
    rectangle.y = (sint16)(clut >> 6u);
    rectangle.w = 256;
    rectangle.h = 1;
    rectangle.x = (sint16)((clut - ((clut >> 6u) << 6u)) << 4u);
    LoadImagePSX(&rectangle, (uint32 *)psx_addr(image + 20u, 512u));
    DrawSync(0);
    page = r_u16(texture + 8u);
    rectangle.x = (sint16)((r_u8(texture + 24u) >> 1u) + ((page << 6u) & 0x3FFu));
    page = r_u16(texture + 8u);
    rectangle.w = 128;
    rectangle.y = (sint16)(r_u8(texture + 25u) + ((page << 4u) & 0x100u) + ((page >> 2u) & 0x200u));
    rectangle.h = (sint16)r_u8(texture + 23u);
    LoadImagePSX(&rectangle, (uint32 *)psx_addr(image + 544u, 1u));
    DrawSync(0);
    sub_8004261C(image);
}

/* Full PRES MIPS CFG 80096E38 through 80096EC8 */
void pres_80096E38(void)
{
    uint32 selection;
    if ((int32_t)r_u32(0x80059738u) >= 5)
        w_u32(0x80059738u, 0u);
    selection = r_u32(0x80059738u);
    w_u32(0x800A5158u, r_u32(0x800A1F2Cu + selection * 4u));
    pres_80090D44(0u, selection);
    selection = r_u32(0x80059738u);
    if (selection != r_u32(0x800A1F78u) - 1u)
        pres_80090D44(1u, selection + 1u);
    w_u32(0x800AD6B0u, 0u);
}

static uint32 pres_mode_preview_entry(uint32 index)
{
    return 0x800AD860u + (r_u32(0x800AD6B8u) + index) * 68u;
}

static void pres_mode_slide(uint32 direction)
{
    uint32 toggle = r_u32(0x800AD6B0u);
    uint32 entry = pres_mode_preview_entry(toggle == 0u ? 4u : 2u);
    uint32 node = r_u32(entry);
    uint32 index, packed;
    uint32 shift = direction == 0xFFFFFFFFu ? 512u : 0u - 512u;
    w_u32(node + 16u, direction == 0xFFFFFFFFu ? 0x0002FEFFu : 0x000202FFu);
    pres_80090D44(toggle == 0u ? 1u : 0u, r_u32(0x80059738u) + direction);
    for (index = 2u; index <= 4u; index += 2u)
    {
        entry = pres_mode_preview_entry(index);
        node = r_u32(entry);
        w_u32(entry + 32u, 0u);
        w_u32(node + 24u, r_u32(node + 16u));
        node = r_u32(entry);
        packed = r_u32(node + 16u);
        w_u32(node + 16u, (packed & 0xFFFF0000u) | ((packed + shift) & 0xFFFFu));
    }
    toggle = r_u32(0x800AD6B0u);
    w_u32(0x800AD6B0u, toggle == 0u ? 1u : 0u);
    w_u32(0x80059738u, r_u32(0x80059738u) + direction);
}

/* Full PRES MIPS CFG 80096ECC through 80097488 */
void pres_80096ECC(uint32 object)
{
    uint32 buttons = r_u16(0x80065524u);
    uint32 selected;
    if ((buttons & 0x1000u) != 0u)
    {
        sub_8001D01C(3u);
        w_u32(0x800ABB30u, 0u);
        w_u32(0x800ABB20u, 3u);
    }
    else if ((buttons & 0x4008u) != 0u && r_u8(0x800596F4u + r_u32(0x80059738u)) != 1u)
    {
        sub_8001D01C(0u);
        w_u32(0x800ABB20u, 1u);
        w_u32(0x800ABB30u, 3u);
        sub_8001D01C(1u);
    }
    else if (r_u32(pres_mode_preview_entry(2u) + 32u) == 4096u)
    {
        if ((r_u16(0x80065524u) & 0x80u) != 0u && (int32_t)r_u32(0x80059738u) > 0)
        {
            sub_8001D01C(2u);
            pres_mode_slide(0xFFFFFFFFu);
        }
        else if ((r_u16(0x80065524u) & 0x20u) != 0u && (int32_t)r_u32(0x80059738u) < (int32_t)(r_u32(0x800A1F78u) - 1u))
        {
            sub_8001D01C(2u);
            pres_mode_slide(1u);
        }
    }
    selected = r_u32(0x80059738u);
    if (selected == 0u)
    {
        pres_800A0240(16, pres_mode_preview_entry(0u));
        pres_800A0240(8, pres_mode_preview_entry(1u));
    }
    else if (selected == r_u32(0x800A1F78u) - 1u)
    {
        pres_800A0240(8, pres_mode_preview_entry(0u));
        pres_800A0240(16, pres_mode_preview_entry(1u));
    }
    else
    {
        pres_800A0240(512, pres_mode_preview_entry(0u));
        pres_800A0240(512, pres_mode_preview_entry(1u));
    }
    selected = r_u32(0x80059738u);
    w_u32(0x800A5158u, r_u32(0x800A1F2Cu + selected * 4u));
    pres_800A0240(r_u8(0x800596F4u + selected) == 0u ? 16 : 8, 0x800AD860u + r_u32(0x800AD7A8u) * 68u);
}

/* Full PRES MIPS CFG 80097540 through 800977B0 */
void pres_80097540(uint32 color)
{
    uint32 vertex, channel;
    for (vertex = 0u; vertex < 4u; ++vertex)
    {
        for (channel = 0u; channel < 3u; ++channel)
        {
            uint32 entry = 0x800AD860u + r_u32(0x800AD6B8u) * 68u;
            uint32 node = r_u32(entry);
            uint32 component = r_u8(color + channel);
            uint32 packet = r_u32(node + 4u);
            w_u8(packet + 16u + vertex * 4u + channel, (uint8)(component << 2u));
        }
    }
}

/* Full PRES MIPS CFG 80090E9C through 80090FEC */
void pres_80090E9C(uint32 selection)
{
    uint32 base = r_u32(0x800ABB70u);
    uint32 image = r_u32(0x800ABB70u) + r_u32(base + selection * 8u + 4u) + 2048u;
    uint32 texture = sub_80039ADC(0x800A20A4u);
    uint32 clut, page;
    sint16 destination_x, destination_y;
    PSX_RECT rectangle;
    sub_80039ADC(0x800A20ACu);
    clut = r_u16(texture + 10u);
    rectangle.y = (sint16)(clut >> 6u);
    rectangle.w = 256;
    rectangle.h = 1;
    rectangle.x = (sint16)((clut - ((clut >> 6u) << 6u)) << 4u);
    LoadImagePSX(&rectangle, (uint32 *)psx_addr(image + 20u, 512u));
    destination_x = rectangle.x;
    destination_y = rectangle.y;
    rectangle.w = 64;
    rectangle.h = 1;
    rectangle.y = (sint16)r_u32(0x80064C74u);
    rectangle.x = (sint16)r_u32(0x80064C70u);
    MoveImage(&rectangle, destination_x, destination_y);
    DrawSync(0);
    page = r_u16(texture + 8u);
    rectangle.x = (sint16)((r_u8(texture + 24u) >> 1u) + ((page << 6u) & 0x3FFu));
    page = r_u16(texture + 8u);
    rectangle.y = (sint16)(r_u8(texture + 25u) + ((page << 4u) & 0x100u) + ((page >> 2u) & 0x200u));
    rectangle.w = (sint16)(r_u8(texture + 22u) >> 1u);
    rectangle.h = (sint16)r_u8(texture + 23u);
    LoadImagePSX(&rectangle, (uint32 *)psx_addr(image + 544u, 1u));
    DrawSync(0);
}

extern void pres_80090730(uint32 model, uint32 mode);

static void pres_paint_copy32(uint32 destination, uint32 source)
{
    uint32 offset;
    for (offset = 0u; offset < 24u; offset += 12u)
    {
        uint32 first = r_u32(source + offset);
        uint32 second = r_u32(source + offset + 4u);
        uint32 third = r_u32(source + offset + 8u);
        w_u32(destination + offset, first);
        w_u32(destination + offset + 4u, second);
        w_u32(destination + offset + 8u, third);
    }
    {
        uint32 first = r_u32(source + 24u), second = r_u32(source + 28u);
        w_u32(destination + 24u, first);
        w_u32(destination + 28u, second);
    }
}

/* Full PRES MIPS CFG 800977B4 through 80097A74 */
void pres_800977B4(void)
{
    uint32 paint = r_u32(0x80065330u);
    uint32 index, phase, choice;
    uint8 mixed[3];
    pres_paint_copy32(0x800AD690u, paint);
    w_u32(0x800AD7B0u, sub_80042544(0x1400u));
    for (index = 0u; index < 248u; ++index)
    {
        w_u8(r_u32(0x800AD7B0u) + index * 20u + 3u, 4u);
        w_u8(r_u32(0x800AD7B0u) + index * 20u + 7u, 0x50u);
    }
    for (index = 0u; index < 6u; ++index)
    {
        w_u8(0x800AD7B8u + index * 20u + 3u, 4u);
        w_u8(0x800AD7B8u + index * 20u + 7u, 0x50u);
    }
    w_u32(0x800A5A04u, 2u);
    sub_8003BF0C(255u, 120u, 2u);
    sub_8003BF48(144u);
    sub_8003BF7C(65536u);
    pres_80090730(r_u32(0x80059744u), 0u);
    choice = r_u32(0x800A5A08u);
    paint = r_u32(0x80065330u);
    if (choice == 1u)
    {
        sub_80019DA0(r_u32(0x800ABBB8u) + 20u, (uint8 *)psx_addr(0x80078AE4u, 3u), mixed, r_u32(paint + 20u), r_u32(paint + 24u), r_u32(paint + 28u));
        sub_80019D80(0u, r_u32(0x80065330u), r_u32(0x80065330u) + 4u);
        pres_80097540(r_u32(0x80065330u) + 4u);
    }
    else
    {
        sub_80019DA0(r_u32(0x800ABBB8u) + 20u, (uint8 *)psx_addr(0x80078AE4u, 3u), mixed, r_u32(paint + 8u), r_u32(paint + 12u), r_u32(paint + 16u));
        sub_80019D80(0u, r_u32(0x80065330u), r_u32(0x80065330u) + 4u);
        pres_80097540(r_u32(0x80065330u));
    }
    phase = r_u32(0x800AD680u);
    if (phase <= 2u)
    {
        w_u32(0x800A5A14u, phase == 0u ? 0x800A63A4u : phase == 1u ? 0x800A63B0u : 0x800A63C4u);
        pres_80090E9C(r_u32(0x8005978Cu));
        if (phase == 2u)
            w_u8(0x80078A78u + r_u32(0x800ABB80u), 4u);
    }
    else if (phase == 3u)
    {
        w_u32(0x800A5A14u, 0x800A63DCu);
        pres_80090E9C(r_u32(0x80059790u));
    }
}

static uint32 pres_paint_entry(uint32 offset)
{
    return 0x800AD860u + (r_u32(0x800AD7A8u) + offset) * 68u;
}

static uint16 pres_paint_buttons(uint32 address)
{
    return r_u16(address + r_u32(0x800A23E0u) * 2u);
}

/* Full PRES MIPS CFG 80097A78 through 800983F0 */
void pres_80097A78(uint32 object)
{
    uint32 phase = r_u32(0x800AD680u);
    uint32 selection = phase == 3u ? 0x80059790u : 0x8005978Cu;
    uint32 paint = r_u32(0x80065330u);
    uint32 parameters, base_color, buttons, mode, choice, index;
    uint8 mixed[3];
    /* The original leaves the selection pointer undefined outside these modes */
    if (phase > 3u)
        abort();
    if (phase == 3u)
        w_u32(0x800A23E0u, 1u);
    choice = r_u32(0x800A5A08u);
    parameters = paint + (choice == 0u ? 8u : 20u);
    base_color = paint + (choice == 0u ? 0u : 4u);
    w_u16(0x80066D58u, (uint16)(r_u16(0x80066D58u) + 80u));
    w_u16(0x80066D5Au, (uint16)(r_u16(0x80066D5Au) + 80u));
    if (object == 0x800A5A94u)
        w_u32(0x800A5A08u, 0u);
    else if (object == 0x800A5AF4u)
        w_u32(0x800A5A08u, 1u);
    else if (object == 0x800A5C34u)
    {
        w_u32(0x800A5A08u, 2u);
        pres_80090E9C(r_u32(selection));
    }
    buttons = pres_paint_buttons(0x80065524u);
    if ((buttons & 0x1000u) != 0u && r_u32(0x800A5A04u) == 2u)
    {
        sub_8001D01C(3u);
        w_u32(0x800ABB20u, 3u);
        w_u32(0x800ABB30u, 0u);
        sub_8004261C(r_u32(0x800ABB10u));
        sub_8004261C(r_u32(0x800AD7B0u));
    }
    else if ((pres_paint_buttons(0x80065524u) & 0x4008u) != 0u)
    {
        sub_8001D01C(0u);
        if (object == 0x800A5BC8u)
        {
            pres_paint_copy32(r_u32(0x80065330u), 0x800AD690u);
        }
        else if (object == 0x800A5C8Cu)
        {
            w_u32(0x800ABB20u, 1u);
            sub_8004261C(r_u32(0x800ABB10u));
            sub_8004261C(r_u32(0x800AD7B0u));
            w_u32(0x800ABB30u, 4u);
            w_u32(0x800A5A04u, 2u);
        }
        else
        {
            mode = r_u32(0x800A5A04u);
            if (mode == 2u)
            {
                pres_800A0240(512, pres_paint_entry(r_u32(0x800A5A08u) == 2u ? 0xFFFFFFFEu : 0xFFFFFFFFu));
                w_u32(0x800A5A04u, 0u);
                if (object == 0x800A5A94u)
                    w_u16(pres_paint_entry(0u) + 54u, 1u);
                else if (object == 0x800A5AF4u)
                    w_u16(pres_paint_entry(1u) + 54u, 1u);
                else if (object == 0x800A5C34u)
                    w_u16(pres_paint_entry(3u) + 54u, 1u);
            }
            else
            {
                pres_800A0240(16, pres_paint_entry(0xFFFFFFFFu));
                pres_800A0240(16, pres_paint_entry(0xFFFFFFFEu));
                w_u32(0x800A5A04u, 2u);
                if (object == 0x800A5A94u)
                    w_u16(pres_paint_entry(0u) + 54u, 0u);
                else if (object == 0x800A5AF4u)
                    w_u16(pres_paint_entry(1u) + 54u, 0u);
                else if (object == 0x800A5C34u)
                    w_u16(pres_paint_entry(3u) + 54u, 0u);
            }
        }
    }
    choice = r_u32(0x800A5A08u);
    if (choice == 2u)
    {
        buttons = pres_paint_buttons(0x80065524u);
        if ((buttons & 0x80u) != 0u && (int32_t)r_u32(selection) > 0)
        {
            sub_8001D01C(2u);
            index = r_u32(selection) - 1u;
            w_u32(selection, index);
            pres_80090E9C(index);
        }
        else if ((pres_paint_buttons(0x80065524u) & 0x20u) != 0u && (int32_t)r_u32(selection) < 47)
        {
            sub_8001D01C(2u);
            index = r_u32(selection) + 1u;
            w_u32(selection, index);
            pres_80090E9C(index);
        }
    }
    else if (r_u32(0x800A5A04u) != 2u)
    {
        buttons = pres_paint_buttons(0x80065524u);
        if ((buttons & 0x800u) != 0u)
            w_u32(0x800A5A04u, 1u);
        if ((buttons & 0x400u) != 0u)
            w_u32(0x800A5A04u, 0u);
        buttons = pres_paint_buttons(0x80065520u);
        if ((buttons & 0x10u) != 0u)
        {
            sub_8001D01C(2u);
            mode = r_u32(0x800A5A04u);
            if (mode == 0u && (int32_t)r_u32(parameters) > 0)
                w_u32(parameters, r_u32(parameters) - 1u);
            else if (mode == 1u && (int32_t)r_u32(parameters + 8u) > 0)
                w_u32(parameters + 8u, r_u32(parameters + 8u) - 1u);
        }
        else if ((buttons & 0x40u) != 0u)
        {
            sub_8001D01C(2u);
            mode = r_u32(0x800A5A04u);
            if (mode == 0u && (int32_t)r_u32(parameters) < 95)
                w_u32(parameters, r_u32(parameters) + 1u);
            else if (mode == 1u && (int32_t)r_u32(parameters + 8u) < 39)
                w_u32(parameters + 8u, r_u32(parameters + 8u) + 1u);
        }
        else if ((buttons & 0x80u) != 0u)
        {
            sub_8001D01C(2u);
            if (r_u32(0x800A5A04u) == 1u && (int32_t)r_u32(parameters + 4u) > 0)
                w_u32(parameters + 4u, r_u32(parameters + 4u) - 1u);
        }
        else if ((buttons & 0x20u) != 0u)
        {
            sub_8001D01C(2u);
            if (r_u32(0x800A5A04u) == 1u && (int32_t)r_u32(parameters + 4u) < 39)
                w_u32(parameters + 4u, r_u32(parameters + 4u) + 1u);
        }
    }
    sub_80019DA0(r_u32(0x800ABBB8u) + 20u, (uint8 *)psx_addr(0x80078AE4u, 3u), mixed, r_u32(parameters), r_u32(parameters + 4u), r_u32(parameters + 8u));
    w_u8(base_color, mixed[0] >> 3u);
    w_u8(base_color + 1u, mixed[1] >> 3u);
    w_u8(base_color + 2u, mixed[2] >> 3u);
    pres_80097540(base_color);
    paint = r_u32(0x80065330u);
    sub_80019D80(0u, paint, paint + 4u);
    if ((sint16)r_u16(pres_paint_entry(3u) + 54u) == 1)
    {
        index = r_u32(selection);
        if (index == 0u)
        {
            pres_800A0240(16, pres_paint_entry(5u));
            pres_800A0240(8, pres_paint_entry(6u));
        }
        else if (index == 47u)
        {
            pres_800A0240(8, pres_paint_entry(5u));
            pres_800A0240(16, pres_paint_entry(6u));
        }
        else
        {
            pres_800A0240(512, pres_paint_entry(5u));
            pres_800A0240(512, pres_paint_entry(6u));
        }
        pres_800A0240(512, pres_paint_entry(0xFFFFFFFCu));
        pres_800A0240(512, pres_paint_entry(0xFFFFFFFDu));
    }
    else
    {
        pres_800A0240(16, pres_paint_entry(0xFFFFFFFCu));
        pres_800A0240(16, pres_paint_entry(0xFFFFFFFDu));
        pres_800A0240(16, pres_paint_entry(5u));
        pres_800A0240(16, pres_paint_entry(6u));
    }
}
