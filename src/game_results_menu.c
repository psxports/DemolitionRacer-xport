#include "draft_signatures.h"
#include <stdio.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4))
#define R16(a) (*(uint16 *)psx_addr((a), 2))
#define R8(a) (*(uint8 *)psx_addr((a), 1))

static sint32 results_menu_half(uint32 value)
{
    return (sint32)(value + (value >> 31)) >> 1;
}

static void results_menu_center(const char *text, sint32 y)
{
    uint32 width = 0, index = 0;
    /* Adapt complete 80020A14 measurement to a native text pointer */
    while (text[index])
    {
        width += R8(0x80059DD8 + 5 * 144 + (uint8)((uint8)text[index] - 32) + 16);
        ++index;
    }
    width += index;
    sub_80020AB8(5, results_menu_half(320 - width), y, text);
}

static void results_menu_ordinal(char *text, size_t capacity, uint32 number, uint32 position, uint32 language)
{
    uint32 suffix = 0;
    snprintf(text, capacity, "%d", (sint32)number);
    switch (language)
    {
        case 0:
            suffix = position == 0 ? 0x80064F08 : position == 1 ? 0x80064F0C : position == 2 ? 0x80064F10 : 0x80064F14;
            break;
        case 1:
            suffix = position == 0 ? 0x80064F18 : 0x80064F1C;
            break;
        case 2:
        case 4:
            suffix = 0x80064F20;
            break;
        case 3:
            suffix = 0x80064F24;
            break;
    }
    if (suffix)
        strcat(text, (const char *)psx_addr(suffix, 1));
}

static void results_unlock(uint32 mode)
{
    switch (mode)
    {
        case 7:
            R8(0x800596F2) = 0;
            R8(0x800596F8) = 0;
            break;
        case 8:
            R32(0x80064B30) = 0;
            R8(0x800596F0) = 0;
            R8(0x800596F1) = 0;
            R8(0x800596D2) = 0;
            R8(0x800596F5) = 0;
            break;
        case 9:
            R32(0x80064B34) = 0;
            R8(0x800596F6) = 0;
            R8(0x800596EF) = 0;
            R8(0x800596DA) = 0;
            R8(0x800596D3) = 0;
            R8(0x800596D5) = 0;
            R8(0x800596D8) = 0;
            break;
        case 10:
            R32(0x80064B38) = 0;
            R8(0x800596CD) = 0;
            R8(0x800596D7) = 0;
            R8(0x800596D9) = 0;
            break;
        case 11:
            R8(0x800596F3) = 0;
            R8(0x800596EA) = 0;
            R8(0x800596E3) = 0;
            R8(0x800596E5) = 0;
            R8(0x800596E8) = 0;
            R8(0x800596DC) = 0;
            R8(0x800596DE) = 0;
            R8(0x800596DF) = 0;
            R8(0x800596E6) = 0;
            R8(0x800596DD) = 0;
            R8(0x800596E7) = 0;
            R8(0x800596E9) = 0;
            break;
    }
}

uint32 sub_800286BC(void)
{
    static const uint32 tables[11] = {0x800117FC, 0x8001183C, 0x8001185C, 0x80011890, 0x800118B0, 0x80011178, 0x800118E0, 0x80011950, 0x800119BC, 0x80011A20, 0x80011A34};
    static const uint8 ordinary[4] = {5, 7, 6, 8};
    static const uint8 success[4] = {5, 6, 8, 10};
    static const uint8 failure[4] = {4, 6, 8, 10};
    uint32 labels[11][5], table, language, threshold = 0, count = 4, base = 10, index, mode, position;
    const uint8 *items = ordinary;
    char text[64];
    size_t length;
    for (table = 0; table < 11; ++table)
        for (language = 0; language < 5; ++language)
            labels[table][language] = R32(tables[table] + language * 4);
    if (R32(0x80064ED8) == 1)
    {
        R8(0x80065028) = 0;
        R32(0x80064ED8) = 0;
    }
    mode = R32(0x80059738);
    if (mode == 8)
        threshold = 4;
    else if (mode == 9)
        threshold = 2;
    if ((sint32)mode >= 7 && (sint32)mode < 12)
    {
        R32(0x80064ED4) = (sint32)threshold >= (sint32)R32(0x80064ED0);
        if (R32(0x80064ED4) == 1)
        {
            if (R8(0x8005A791 + (R32(0x80059738) << 4) + R32(0x80065200)) == 255)
            {
                mode = R32(0x80059738);
                R32(0x80064BE0) = mode;
                results_unlock(mode);
            }
        }
        if (R32(0x80064ED4) == 1)
        {
            sub_80021430(5);
            position = labels[0][R32(0x80064B8C)];
            {
                uint32 width = sub_80020A14(5, position);
                sub_80020AB8(5, results_menu_half(320 - width), 32, (const char *)psx_addr(labels[0][R32(0x80064B8C)], 1));
            }
            items = success;
        }
        else
        {
            sub_80021430(0);
            position = labels[1][R32(0x80064B8C)];
            {
                uint32 width = sub_80020A14(5, position);
                sub_80020AB8(5, results_menu_half(320 - width), 32, (const char *)psx_addr(labels[1][R32(0x80064B8C)], 1));
            }
            items = failure;
        }
        length = (size_t)snprintf(text, sizeof(text), "%s ", (const char *)psx_addr(labels[2][R32(0x80064B8C)], 1));
        position = R32(0x80064ED0);
        results_menu_ordinal(text + length, sizeof(text) - length, position + 1, position, R32(0x80064B8C));
        results_menu_center(text, 44);
        length = (size_t)snprintf(text, sizeof(text), "%s ", (const char *)psx_addr(labels[3][R32(0x80064B8C)], 1));
        results_menu_ordinal(text + length, sizeof(text) - length, threshold | 1, threshold, R32(0x80064B8C));
        results_menu_center(text, 56);
        count = 4;
        base = 0;
    }
    for (index = 0; index < count; ++index)
    {
        sub_80021430(R8(0x80065028) == index ? 1 : 2);
        strcpy(text, (const char *)psx_addr(labels[items[index]][R32(0x80064B8C)], 1));
        results_menu_center(text, 85 + (sint32)index * 14);
    }
    if ((R16(0x80065524) & 0x40) && R8(0x80065028) < count - 1)
        ++R8(0x80065028);
    if ((R16(0x80065524) & 0x10) && R8(0x80065028))
        --R8(0x80065028);
    if (R16(0x80065524) & 0x4000)
    {
        /* Preserve command IDs after removing the league LOAD/SAVE item */
        index = R8(0x80065028);
        return base + index + 1 + (base == 0 && index >= 3);
    }
    return base;
}
