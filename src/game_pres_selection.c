#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define S32(a) (*(uint32 *)psx_addr((a), 4u))
#define S16(a) (*(uint16 *)psx_addr((a), 2u))
#define S8(a) (*(uint8 *)psx_addr((a), 1u))

void pres_800A0240(sint32 flags, uint32 entry);
void pres_8009CD44(void);
void pres_80090730(uint32 model, uint32 mode);
void pres_80090C08(uint32 selected);
void pres_80094394(void);

static void selection_entry_flags(sint32 flags, uint32 extra)
{
    pres_800A0240(flags, 0x800AD860u + (S32(0x800AD6B8u) + extra) * 68u);
}

static void selection_free_track(void)
{
    sub_8004261C(S32(0x800AD788u));
    sub_8004261C(S32(0x800AD798u));
    sub_8004261C(S32(0x800AD79Cu));
    sub_8004261C(S32(0x800AD7A0u));
    sub_8004261C(S32(0x800AD7A4u));
}

static void selection_projection(uint32 y)
{
    /* Original offset wrapper has no meaningful return */
    sub_8004FACC(255u, y);
    S32(0x80065154u) = 255u;
    S32(0x80065158u) = y;
    sub_8003BF48(144u);
    sub_8003BF7C(65536u);
}

void pres_80094BF4(void)
{
    uint32 mode, table, category, index;
    /* Original 75-byte local copy is never read */
    if ((sint32)S32(0x80059740u) >= 15)
        S32(0x80059740u) -= 15u;
    selection_projection(120u);
    mode = S32(0x800AD670u);
    S32(0x800AD850u) = 0u;
    S32(0x800AD858u) = 0u;
    if (mode == 2u)
        table = 0x800A3AD4u;
    else if (mode == 1u)
        table = 0x800A3B44u;
    else
        table = 0x800A3BC0u;
    S32(0x800A3A4Cu) = S32(table + S32(0x80064B8Cu) * 4u);
    S32(0x800A3A40u) = S32(0x80059738u) == 3u ? 1u : 0u;
    category = S32(0x800A3A40u);
    index = S8(0x8005A7D0u + S32(0x800AD670u) * 16u + category);
    S32(0x80059740u) = index;
    S32(0x800A3D60u) = S32(0x800A1D98u + S32(0x80059740u) * 4u);
    pres_80090C08(index);
    pres_80094394();
}

static void selection_texture_packets(uint32 texture)
{
    uint32 index, packet;
    S32(0x8006532Cu) = texture;
    for (index = 0u; index < 2u; ++index)
    {
        packet = 0x800789D8u + index * 40u;
        S8(packet + 3u) = 9u;
        S8(packet + 7u) = 0x2Du;
        texture = S32(0x8006532Cu);
        S8(packet + 12u) = S8(texture + 24u);
        S8(packet + 13u) = S8(texture + 25u);
        S8(packet + 20u) = (uint8)(S8(texture + 24u) + S8(texture + 22u) + 255u);
        S8(packet + 21u) = S8(texture + 25u);
        S8(packet + 28u) = S8(texture + 24u);
        S8(packet + 29u) = (uint8)(S8(texture + 25u) + S8(texture + 23u) + 255u);
        S8(packet + 36u) = (uint8)(S8(texture + 24u) + S8(texture + 22u) + 255u);
        S8(packet + 37u) = (uint8)(S8(texture + 25u) + S8(texture + 23u) + 255u);
        S16(packet + 8u) = 256u;
        S16(packet + 10u) = (uint16)(196u - S8(texture + 23u));
        S16(packet + 16u) = (uint16)(S8(texture + 22u) + 255u);
        S16(packet + 24u) = 256u;
        S16(packet + 26u) = 195u;
        S16(packet + 18u) = (uint16)(196u - S8(texture + 23u));
        S16(packet + 34u) = 195u;
        S16(packet + 32u) = (uint16)(S8(texture + 22u) + 255u);
        S16(packet + 22u) = S16(texture + 8u);
        S16(packet + 14u) = S16(texture + 10u);
    }
}

void pres_80095484(void)
{
    uint32 mode = S32(0x80059738u), index, packet, texture;
    if ((sint32)mode >= 5 && mode != 7u && mode != 11u)
    {
        if (mode == 8u)
            S32(0x80059744u) = 0u;
        else if (mode == 9u)
            S32(0x80059744u) = 4u;
        else if (mode == 10u)
            S32(0x80059744u) = 3u;
    }
    selection_projection(80u);
    pres_80090730(S32(0x80059744u), 0u);
    index = S32(0x80059744u) * 32u;
    sub_80019D80(0u, 0x800594CCu + index, 0x800594D0u + index);
    texture = sub_80039ADC(S32(0x8005A9D8u + S32(0x80059744u) * 4u));
    selection_texture_packets(texture);
    for (index = 0u; index < 2u; ++index)
    {
        packet = 0x80078A98u + index * 24u;
        S8(packet + 3u) = 5u;
        S8(packet + 7u) = 42u;
        S16(packet + 8u) = 134u;
        S16(packet + 10u) = 146u;
        S16(packet + 12u) = 238u;
        S16(packet + 14u) = 146u;
        S16(packet + 16u) = 134u;
        S16(packet + 18u) = 196u;
        S16(packet + 20u) = 238u;
        S16(packet + 22u) = 196u;
    }
    for (index = 0u; index < 8u; ++index)
    {
        packet = 0x80078B38u + index * 36u;
        S8(packet + 3u) = 8u;
        S8(packet + 7u) = 57u;
    }
}

void pres_80094E4C(uint32 object)
{
    uint32 selected = S32(0x80059740u), category, mode, index, flag;
    const uint8 *warnings = (const uint8 *)psx_addr(0x8008B58Cu, 15u);
    S32(0x800A3D60u) = S32(0x800A1D98u + selected * 4u);
    if (S16(0x80065524u) & 0x1000u)
    {
        sub_8001D01C(3u);
        if (S32(0x80059738u) != 2u && S32(0x8005973Cu) != 2u && S32(0x800AD858u) == 1u && S8(0x800596DCu + S32(0x80059740u)) == 0u)
        {
            S32(0x800AD858u) = 0u;
            S32(0x800AD850u) = 0u;
            selection_entry_flags(16, 2u);
            selection_entry_flags(16, 3u);
        }
        else
        {
            S32(0x800ABB20u) = 3u;
            selection_free_track();
            S32(0x800ABB30u) = 7u;
        }
    }
    else if ((S16(0x80065524u) & 0x4008u) && S8(0x800596CCu + selected) != 1u)
    {
        S32(0x800ABB20u) = 1u;
        sub_8001D01C(0u);
        if (S32(0x80059738u) == 2u || S32(0x8005973Cu) == 2u)
        {
            selection_free_track();
            S32(0x800ABB30u) = 4u;
        }
        else
        {
            selected = S32(0x80059740u);
            flag = S8(0x800596DCu + selected);
            if (flag == 1u || (S32(0x800AD858u) == 1u && flag == 0u) || warnings[selected] == 0u)
            {
                selection_free_track();
                S32(0x800ABB30u) = 4u;
                if (object == 0x800A3C8Cu)
                    S32(0x80059740u) += 15u;
            }
            else
            {
                S32(0x800AD858u) = 1u;
                S32(0x800AD850u) = 1u;
                selection_entry_flags(512, 2u);
                selection_entry_flags(512, 3u);
            }
        }
    }
    if ((S16(0x80065524u) & 128u) && (sint32)S32(0x800A3A40u) > 0 && S32(0x800AD858u) == 0u)
    {
        sub_8001D01C(2u);
        category = S32(0x800A3A40u) - 1u;
        mode = S32(0x800AD670u);
        S32(0x800A3A40u) = category;
        S32(0x80059740u) = S8(0x8005A7D0u + mode * 16u + category);
        pres_80090C08(S32(0x80059740u));
    }
    else if ((S16(0x80065524u) & 32u) && S8(0x8005A7D1u + S32(0x800AD670u) * 16u + S32(0x800A3A40u)) != 255u && S32(0x800AD858u) == 0u)
    {
        sub_8001D01C(2u);
        category = S32(0x800A3A40u) + 1u;
        mode = S32(0x800AD670u);
        S32(0x800A3A40u) = category;
        S32(0x80059740u) = S8(0x8005A7D0u + mode * 16u + category);
        pres_80090C08(S32(0x80059740u));
    }
    category = S32(0x800A3A40u);
    if (category == 0u)
    {
        selection_entry_flags(16, 0u);
        selection_entry_flags(8, 1u);
    }
    else if (S8(0x8005A7D1u + S32(0x800AD670u) * 16u + category) == 255u)
    {
        selection_entry_flags(8, 0u);
        selection_entry_flags(16, 1u);
    }
    else
    {
        selection_entry_flags(512, 0u);
        selection_entry_flags(512, 1u);
    }
    index = S32(0x800AD7A8u);
    pres_800A0240(S8(0x800596CCu + S32(0x80059740u)) == 0u ? 16 : 8, 0x800AD860u + index * 68u);
}

static void selection_link(uint32 packet, uint32 slot)
{
    S32(packet) = (S32(packet) & 0xFF000000u) | (S32(slot) & 0xFFFFFFu);
    S32(slot) = (S32(slot) & 0xFF000000u) | (packet & 0xFFFFFFu);
}

static void selection_reload_vehicle(void)
{
    uint32 offset;
    sub_8004261C(S32(0x800ABB10u));
    pres_80090730(S32(0x80059744u), 0u);
    offset = S32(0x80059744u) * 32u;
    sub_80019D80(0u, 0x800594CCu + offset, 0x800594D0u + offset);
    selection_texture_packets(sub_80039ADC(S32(0x8005A9D8u + S32(0x80059744u) * 4u)));
}

void pres_800957C0(uint32 object)
{
    const uint8 *locks;
    uint32 mode, selected, flags, index, packet, value, x, y, slot;
    selected = S32(0x80059744u);
    S32(0x800A40D4u) = S32(0x800A1EB8u + selected * 4u);
    mode = S32(0x80059738u);
    if ((sint32)mode < 5 || mode == 7u || mode == 11u)
        locks = (const uint8 *)psx_addr(0x800596ECu, 8u);
    else if (mode == 8u)
        locks = (const uint8 *)psx_addr(0x8008B59Cu, 8u);
    else if (mode == 9u)
        locks = (const uint8 *)psx_addr(0x8008B5A4u, 8u);
    else if (mode == 10u)
        locks = (const uint8 *)psx_addr(0x8008B5ACu, 8u);
    else
    {
        /* Original S2 is undefined for an unsupported game mode */
        fprintf(stderr, "Unsupported original vehicle selection mode %u\n", mode);
        abort();
    }
    pres_800A0240(locks[selected] == 0u ? 16 : 8, 0x800AD860u + S32(0x800AD7A8u) * 68u);
    S16(0x80066D58u) += 64u;
    S16(0x80066D5Au) += 64u;
    flags = S16(0x80065524u);
    if (flags & 0x1000u)
    {
        sub_8001D01C(3u);
        S32(0x800ABB20u) = 3u;
        sub_8004261C(S32(0x800ABB10u));
        S32(0x800ABB30u) = 3u;
    }
    else if ((flags & 0x8000u) && locks[S32(0x80059744u)] != 1u)
    {
        sub_8001D01C(2u);
        S32(0x800ABB20u) = 2u;
        sub_8004261C(S32(0x800ABB10u));
        pres_8009CD44();
        S32(0x800ABB48u) = 1u;
        S32(0x800ABB30u) = 9u;
    }
    else if ((S16(0x80065524u) & 0x4008u) && locks[S32(0x80059744u)] != 1u)
    {
        sub_8001D01C(0u);
        S32(0x800ABB20u) = 1u;
        sub_8004261C(S32(0x800ABB10u));
        pres_8009CD44();
        S32(0x800ABB48u) = 1u;
        S32(0x800ABB20u) = 7u;
    }
    else if (S32(0x800AD880u + (S32(0x800AD6B8u) + 2u) * 68u) == 4096u)
    {
        if ((S16(0x80065524u) & 128u) && (sint32)S32(0x80059744u) > 0)
        {
            sub_8001D01C(2u);
            --S32(0x80059744u);
            selection_reload_vehicle();
        }
        else if ((S16(0x80065524u) & 32u) && (sint32)S32(0x80059744u) < (sint32)(S32(0x800A1F28u) - 1u))
        {
            sub_8001D01C(2u);
            ++S32(0x80059744u);
            selection_reload_vehicle();
        }
    }
    selected = S32(0x80059744u);
    if (selected == 0u)
    {
        selection_entry_flags(16, 0u);
        selection_entry_flags(8, 1u);
    }
    else if (selected == S32(0x800A1F28u) - 1u)
    {
        selection_entry_flags(8, 0u);
        selection_entry_flags(16, 1u);
    }
    else
    {
        selection_entry_flags(512, 0u);
        selection_entry_flags(512, 1u);
    }
    pres_800A0240(locks[S32(0x80059744u)] == 0u ? 16 : 8, 0x800AD860u + S32(0x800AD7A8u) * 68u);
    packet = 0x800789D8u + S32(0x80065128u) * 40u;
    selection_link(packet, S32(S32(0x80065118u) + 112u) + 1024u);
    for (index = 0u; index < 4u; ++index)
    {
        packet = 0x80078B38u + index * 72u + S32(0x80065128u) * 36u;
        y = 149u + index * 12u;
        S16(packet + 8u) = 136u;
        S16(packet + 10u) = (uint16)y;
        value = S8(0x8005A9F8u + S32(0x80059744u) * 4u + index);
        S16(packet + 16u) = (uint16)(value * 10u + 146u);
        S16(packet + 18u) = (uint16)y;
        S16(packet + 24u) = 136u;
        S16(packet + 26u) = (uint16)(y + 8u);
        value = S8(0x8005A9F8u + S32(0x80059744u) * 4u + index);
        S16(packet + 32u) = (uint16)(value * 10u + 146u);
        S16(packet + 34u) = (uint16)(y + 8u);
        S8(packet + 4u) = 240u;
        S8(packet + 5u) = 96u;
        S8(packet + 6u) = 16u;
        S8(packet + 20u) = 240u;
        S8(packet + 21u) = 96u;
        S8(packet + 22u) = 16u;
        value = S8(0x8005A9F8u + S32(0x80059744u) * 4u + index);
        x = 240u - (value << 4);
        S8(packet + 12u) = (uint8)x;
        value = S8(0x8005A9F8u + S32(0x80059744u) * 4u + index);
        S8(packet + 13u) = (uint8)(96u + (value << 4));
        S8(packet + 14u) = 16u;
        value = S8(0x8005A9F8u + S32(0x80059744u) * 4u + index);
        S8(packet + 28u) = (uint8)(240u - (value << 4));
        value = S8(0x8005A9F8u + S32(0x80059744u) * 4u + index);
        S8(packet + 29u) = (uint8)(96u + (value << 4));
        S8(packet + 30u) = 16u;
        slot = S32(S32(0x80065118u) + 112u);
        selection_link(packet, slot);
    }
    packet = 0x80078A98u + S32(0x80065128u) * 24u;
    selection_link(packet, S32(S32(0x80065118u) + 112u));
}

static void selection_copy_geometry(uint32 source)
{
    uint32 index;
    for (index = 0u; index < 13u; ++index)
    {
        uint32 words[4];
        memcpy(words, psx_addr(source + index * 16u, 16u), 16u);
        memcpy(psx_addr(0x80066BB0u + index * 16u, 16u), words, 16u);
    }
    {
        uint32 words[3];
        uint16 tail;
        memcpy(words, psx_addr(source + 208u, 12u), 12u);
        tail = S16(source + 220u);
        memcpy(psx_addr(0x80066C80u, 12u), words, 12u);
        S16(0x80066C8Cu) = tail;
    }
}

void pres_80090730(uint32 model, uint32 mode)
{
    uint32 table = 0x800A202Cu + model * 4u;
    uint32 size, buffer, index, cursor, texture, chassis, x, y, model_size, allocation;
    uint32 tpage, clut, u, v;
    DrawSync(0);
    size = sub_80015150(S32(0x800ABAF8u) + S32(table));
    buffer = sub_80042544(size);
    sub_80015074(S32(0x800ABAF8u) + S32(table), buffer, (sint32)size);
    index = 1u;
    if (S32(buffer) != 0u)
    {
        cursor = buffer + 4u;
        do
        {
            S32(cursor) += buffer;
            ++index;
            cursor += 4u;
        } while (S32(buffer) >= index);
    }
    selection_copy_geometry(S32(buffer + 56u));
    texture = sub_80039ADC(0x800A2074u);
    tpage = S16(texture + 8u);
    x = (tpage << 6) & 1023u;
    y = ((tpage << 4) & 256u) + ((tpage >> 2) & 512u);
    S16(texture + 10u) = GetClut(512, 255);
    sub_8001B7B0(S32(buffer + 24u), x, y, 512u, 255u, 1u);
    DrawSync(0);
    S8(texture + 22u) = 128u;
    S8(texture + 23u) = 0u;
    chassis = sub_80039ADC(0x800A207Cu);
    sub_80017484(S32(buffer + 4u));
    sub_8001AB18(S32(buffer + 4u), chassis, texture);
    DrawSync(0);
    model_size = S32(buffer + 8u) - S32(buffer + 4u);
    allocation = sub_80042544(model_size);
    S32(0x800ABB10u + mode * 4u) = allocation;
    memcpy(psx_addr(allocation, model_size), psx_addr(S32(buffer + 4u), model_size), model_size);
    S8(0x800670BDu) = 0u;
    S32(0x80065254u) = 1u;
    selection_copy_geometry(S32(buffer + 56u));
    S32(0x80066D14u) = 0x80066BB0u;
    S32(0x80066D30u) = 0u;
    S32(0x80066D2Cu) = S32(0x800ABB10u + mode * 4u);
    for (index = 0u; index < 4u; ++index)
    {
        uint32 words[2];
        memcpy(words, psx_addr(S32(0x80066D14u) + 64u + index * 8u, 8u), 8u);
        memcpy(psx_addr(0x80066F84u + index * 8u, 8u), words, 8u);
    }
    sub_8001860C();
    sub_80018430(0u, S32(0x80066D14u));
    sub_80018430(1u, S32(0x80066D14u));
    sub_80018430(2u, S32(0x80066D14u));
    sub_80018430(3u, S32(0x80066D14u));
    texture = sub_80039ADC(0x800A2084u);
    tpage = S16(texture + 8u);
    u = S8(texture + 24u);
    v = S8(texture + 25u);
    clut = S16(texture + 10u);
    x = ((tpage << 6) & 1023u) | (u >> 2);
    y = (((tpage << 4) & 256u) + ((tpage >> 2) & 512u)) | v;
    sub_8001B7B0(S32(buffer + 48u), x, y, (clut & 63u) << 4, (clut >> 6) & 255u, 1u);
    DrawSync(0);
    sub_800197B8(1u);
    sub_80019C6C(0u, 207u);
    sub_8004261C(buffer);
}

void pres_80090C08(uint32 selected)
{
    uint32 entry = 0x800A1FD0u + selected * 4u;
    uint32 size, buffer, texture, clut, tpage, u, v;
    PSX_RECT rectangle;
    size = sub_80015150(S32(0x800ABB78u) + S32(entry));
    buffer = sub_80042544(size);
    sub_80015074(S32(0x800ABB78u) + S32(entry), buffer, 65536);
    texture = sub_80039ADC(0x800A208Cu);
    clut = S16(texture + 10u);
    rectangle.y = (sint16)(clut >> 6);
    rectangle.w = 256;
    rectangle.h = 1;
    rectangle.x = (sint16)((S16(texture + 10u) - ((clut >> 6) << 6)) << 4);
    LoadImagePSX(&rectangle, (uint32 *)psx_addr(buffer + 20u, 512u));
    DrawSync(0);
    u = S8(texture + 24u);
    tpage = S16(texture + 8u);
    rectangle.x = (sint16)((u >> 1) + ((tpage << 6) & 1023u));
    tpage = S16(texture + 8u);
    v = S8(texture + 25u);
    rectangle.w = 128;
    rectangle.y = (sint16)(v + ((tpage << 4) & 256u) + ((tpage >> 2) & 512u));
    rectangle.h = (sint16)S8(texture + 23u);
    LoadImagePSX(&rectangle, (uint32 *)psx_addr(buffer + 544u, (uint32)rectangle.h * 256u));
    DrawSync(0);
    sub_8004261C(buffer);
}

void pres_80094394(void)
{
    uint32 index, size, texture, u, v, bank, column, row, packet;
    S32(0x800AD788u) = sub_80042544(3120u);
    for (index = 0u; index < 4u; ++index)
    {
        size = S32(0x800A3FE0u + index * 4u) * S32(0x800A3FF0u + index * 4u);
        S32(0x800AD798u + index * 4u) = sub_80042544(size << 1);
    }
    texture = sub_80039ADC(0x800A4020u);
    u = S8(texture + 24u);
    v = S8(texture + 25u);
    for (bank = 0u; bank < 2u; ++bank)
    {
        for (index = 0u; index < 30u; ++index)
        {
            packet = S32(0x800AD788u) + (bank * 30u + index) * 52u;
            S16(packet + 26u) = S16(texture + 8u);
            S8(packet + 3u) = 12u;
            S8(packet + 7u) = 60u;
            S16(packet + 14u) = S16(texture + 10u);
        }
        for (row = 0u; row < 5u; ++row)
        {
            for (column = 0u; column < 6u; ++column)
            {
                uint32 left = u + column * 256u / 6u;
                uint32 right = u + (column + 1u) * 256u / 6u - 1u;
                uint32 top = v + row * 24u;
                uint32 bottom = top + 23u;
                packet = S32(0x800AD788u) + (bank * 30u + column * 5u + row) * 52u;
                S8(packet + 12u) = (uint8)left;
                S8(packet + 13u) = (uint8)top;
                S8(packet + 24u) = (uint8)right;
                S8(packet + 25u) = (uint8)top;
                S8(packet + 36u) = (uint8)left;
                S8(packet + 37u) = (uint8)bottom;
                S8(packet + 48u) = (uint8)right;
                S8(packet + 49u) = (uint8)bottom;
            }
        }
    }
}
