#include "draft_signatures.h"
#include <stdio.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4))
#define R16(a) (*(uint16 *)psx_addr((a), 2))
#define R8(a) (*(uint8 *)psx_addr((a), 1))

static int sub_80025320(const void *first, const void *second)
{
    sint32 a = *(const sint32 *)((const uint8 *)first + 52);
    sint32 b = *(const sint32 *)((const uint8 *)second + 52);
    return a < b ? -1 : a != b;
}

uint32 sub_80025410(uint32 track)
{
    static const uint8 indices[29] = {0, 1, 2, 3, 0, 0, 0, 4, 0, 5, 6, 7, 8, 9, 0, 0, 1, 2, 3, 0, 0, 0, 4, 0, 5, 6, 7, 8, 9};
    sint32 mode = (sint32)R32(0x80059738);
    --track;
    if (mode < 0)
        return 0;
    if (mode < 3)
        return track < 29 ? indices[track] : 0;
    if (mode < 5)
        return track == 8 ? 1 : track == 14 ? 2 : 0;
    return 0;
}

static sint32 results_draw_half(uint32 value)
{
    return (sint32)(value + (value >> 31)) >> 1;
}

static void results_draw_text(sint32 x, sint32 y, uint32 text)
{
    sub_80020AB8(5, x, y, (const char *)psx_addr(text, 1));
}

static void results_draw_ordinal(uint32 destination, uint32 position)
{
    uint32 suffix = 0, language = R32(0x80064B8C);
    char number[12];
    snprintf(number, sizeof(number), "%d", (sint32)(position + 1));
    strcpy((char *)psx_addr(destination, strlen(number) + 1u), number);
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
        strcat((char *)psx_addr(destination, 1), (const char *)psx_addr(suffix, 1));
}

static void results_scroll(void)
{
    if ((R16(0x80065520) & 0x10) && (sint32)R32(0x80064EA4) < 80)
        ++R32(0x80064EA4);
    if ((R16(0x80065520) & 0x40) && (sint32)(116 - R32(0x80065254) * 28) < (sint32)R32(0x80064EA4))
        --R32(0x80064EA4);
}

static void results_continue(uint32 labels[14][5])
{
    uint32 text, width;
    sub_80021430(1);
    width = sub_80020A14(5, labels[0][R32(0x80064B8C)]);
    results_draw_text(results_draw_half(320 - width), 176, labels[0][R32(0x80064B8C)]);
    sub_80021430(0);
    text = R32(0x80011120 + R32(0x80064B8C) * 4);
    width = sub_80020A14(5, text);
    text = R32(0x80011120 + R32(0x80064B8C) * 4);
    results_draw_text(results_draw_half(320 - width), 200, text);
    sub_80021430(8);
    results_draw_text((sint32)((uint32)results_draw_half(320 - width) + width + 4), 200, 0x80064F00);
}

static uint32 results_record(uint32 name_base, uint32 score_base, uint32 row_offset, uint32 lower)
{
    uint32 record = score_base + sub_80025410(R32(0x80064C08)) * 32;
    uint32 score = R32(0x800656BC + row_offset);
    if (lower ? (sint32)R32(record) < (sint32)score : (sint32)score < (sint32)R32(record))
        return 0;
    record = name_base + sub_80025410(R32(0x80064C08)) * 32;
    strcpy((char *)psx_addr(record, 1), (const char *)psx_addr(0x80059770, 1));
    record = score_base + sub_80025410(R32(0x80064C08)) * 32;
    R32(record) = R32(0x800656BC + R32(0x80064EB0) * 72 + row_offset);
    return 1;
}

static void results_time(char *text, size_t capacity, uint32 ticks)
{
    uint32 numerator = ticks * 100;
    sint32 divisor = results_draw_half(R32(0x80065124));
    sint32 time;
    if (!divisor)
        xport_mips_break(7);
    if (divisor == -1 && numerator == 0x80000000)
        xport_mips_break(6);
    time = (sint32)numerator / divisor;
    snprintf(text, capacity, "%02d : %02d : %02d", time / 6000, time % 6000 / 100, time % 6000 % 100);
}

static void results_slide(uint32 points_mode)
{
    if (R16(0x80065524) & 0x4000)
        R32(0x80064EA4) = R32(0x80064EA8);
    if ((sint32)R32(0x80064EA4) < (sint32)R32(0x80064EA8))
        ++R32(0x80064EA4);
    else
        R8(0x80064EC0) = points_mode && R32(0x80064EC4) ? 4 : 1;
}

uint32 sub_800254DC(void)
{
    static const uint32 tables[14] = {0x80011178, 0x8001118C, 0x800111A0, 0x800111B4, 0x800111C8, 0x800111EC, 0x80011268, 0x800112E0, 0x800112F4, 0x80011308, 0x80011328, 0x8001133C, 0x80011350, 0x80011364};
    uint32 labels[14][5], table, language, row, mode, value, second, amount, completed;
    uint32 done = 0, new_record = 0;
    sint32 index, y;
    char text[64];
    for (table = 0; table < 14; ++table)
        for (language = 0; language < 5; ++language)
            labels[table][language] = R32(tables[table] + language * 4);
    sub_80021430(3);
    if (!R32(0x80064EC4))
        strcpy(text, (const char *)psx_addr(labels[6][R32(0x80064B8C)], 1));
    else if ((sint32)R32(0x80064EBC) % 16 < 8)
        strcpy(text, (const char *)psx_addr(labels[7][R32(0x80064B8C)], 1));
    else
        text[0] = 0;
    sub_80020AB8(5, 24, 16, text);
    y = (sint32)R32(0x80064EA4);
    mode = R32(0x80059738);
    if (mode < 12 && mode != 1 && mode != 2 && mode != 4)
    {
        for (index = 0; index < (sint32)R32(0x80065254); ++index)
        {
            row = 0x800656BC + (uint32)index * 72;
            if (y >= 56 && y < 176 - R8(0x80065328))
            {
                sub_80021430(R32(0x80064EC4) ? 5 : 3);
                results_draw_text(24, 32, labels[1][R32(0x80064B8C)]);
                sub_80021430(5);
                results_draw_text(60, 32, labels[2][R32(0x80064B8C)]);
                results_draw_text(136, 32, labels[3][R32(0x80064B8C)]);
                results_draw_text(172, 32, labels[8][R32(0x80064B8C)]);
                results_draw_text(172, 44, labels[9][R32(0x80064B8C)]);
                results_draw_text(222, 32, labels[10][R32(0x80064B8C)]);
                results_draw_text(222, 44, labels[11][R32(0x80064B8C)]);
                sub_80021430(R32(0x80064EC4) == 1 ? 3 : 5);
                results_draw_text(272, 32, labels[12][R32(0x80064B8C)]);
                results_draw_text(272, 44, labels[13][R32(0x80064B8C)]);
                sub_80021430(R32(0x80064EC4) ? 2 : 3);
                results_draw_text(24, y, row + 1);
                sub_80021430((uint32)index == R32(0x80064EB0) || (R32(0x8005973C) == 2 && (uint32)index == R32(0x80064EB4)) ? 1 : 2);
                results_draw_text(60, y, row + 9);
                if (R32(0x80059738) == 3 || R32(0x80059738) == 7 || R8(row + 60) != 1)
                {
                    snprintf(text, sizeof(text), "%2d X", (sint32)R32(row + 32));
                    sub_80020AB8(5, 136, y, text);
                    snprintf(text, sizeof(text), "%4d =", (sint32)R32(row + 36));
                    sub_80020AB8(5, 172, y, text);
                    snprintf(text, sizeof(text), "%5d", (sint32)R32(row + 44));
                    sub_80020AB8(5, 222, y, text);
                }
                else
                    results_draw_text(136, y, 0x80064FFC);
                if (R32(0x80064EC4))
                    sub_80021430(3);
                results_draw_text(272, y, row + 62);
            }
            y = (sint32)((uint32)y + 16 + R8(0x80065328));
            if (y >= 177)
                break;
        }
        switch (R8(0x80064EC0))
        {
            case 0:
                results_slide(1);
                break;
            case 1:
                if (R16(0x80065524) & 0x4000)
                    for (index = 0; index < (sint32)R32(0x80065254); ++index)
                        R32(0x800656E8 + (uint32)index * 72) = R32(0x800656E4 + (uint32)index * 72);
                amount = R32(0x80064EAC);
                for (index = 0; index < (sint32)R32(0x80065254); ++index)
                {
                    row = 0x800656BC + (uint32)index * 72;
                    value = R32(row + 40);
                    if ((sint32)R32(row + 44) < (sint32)(value - amount))
                        R32(row + 44) += amount;
                    else
                        R32(row + 44) = value;
                }
                completed = 0;
                for (index = 0; index < (sint32)R32(0x80065254); ++index)
                    if (R32(0x800656E8 + (uint32)index * 72) == R32(0x800656E4 + (uint32)index * 72))
                        ++completed;
                if (completed == R32(0x80065254))
                    R8(0x80064EC0) = 2;
                break;
            case 2:
                if (R16(0x80065524) & 0x4000)
                {
                    for (index = 0; index < (sint32)R32(0x80065254); ++index)
                    {
                        row = 0x800656BC + (uint32)index * 72;
                        results_draw_ordinal(row + 62, R32(row + 52));
                    }
                    R32(0x80064EB8) = R32(0x80065254);
                }
                if (!(R32(0x80064EBC) & 15))
                {
                    /* Original also writes the row at count before advancing */
                    row = 0x800656BC + R32(0x80064EB8) * 72;
                    results_draw_ordinal(row + 62, R32(row + 52));
                    if ((sint32)R32(0x80064EB8) < (sint32)R32(0x80065254))
                        ++R32(0x80064EB8);
                    else
                        R8(0x80064EC0) = 3;
                }
                break;
            case 3:
                qsortPSX(psx_addr(0x800656BC, 1), R32(0x80065254), 72, sub_80025320);
                R8(0x80064EC0) = 0;
                R32(0x80064EA4) = 0 - R32(0x80065254) * 28;
                value = R32(0x80065254);
                for (index = 0; index < (sint32)value; ++index)
                {
                    row = 0x800656BC + (uint32)index * 72;
                    if (R32(0x80059738) == 3 || R32(0x80059738) == 7)
                    {
                        if (!R8(row))
                        {
                            R32(0x80064ED0) = (uint32)index;
                            R32(0x80064EB0) = (uint32)index;
                        }
                        if (R32(0x8005973C) == 2 && R8(row) == 1)
                            R32(0x80064EB4) = (uint32)index;
                    }
                    else
                    {
                        if (R32(row + 48) == R8(0x800670C4))
                        {
                            R32(0x80064ED0) = (uint32)index;
                            R32(0x80064EB0) = (uint32)index;
                        }
                        if (R32(0x8005973C) == 2 && R32(row + 48) == R8(0x8006747C))
                            R32(0x80064EB4) = (uint32)index;
                    }
                }
                value = R32(0x80064EB0);
                second = R32(0x80064EB4);
                if (R32(0x8005973C) == 2 && (sint32)second < (sint32)value)
                {
                    R32(0x80064EB0) = second;
                    R32(0x80064EB4) = value;
                    value = second;
                }
                R32(0x80064EC4) = 1;
                R32(0x80064EB8) = 0;
                R32(0x80064EA8) = 116 - value * 28;
                break;
            case 4:
                if (R32(0x80059738) == 0 && !R32(0x80064EB0) && (sint32)R32(0x80064C08) < 16 && R32(0x8005973C) != 2)
                    new_record = results_record(0x8006649C, 0x800664B4, 40, 0);
                if (R32(0x80059738) == 3 && !R32(0x80064EB0) && (sint32)R32(0x80064C08) < 16 && R32(0x8005973C) != 2)
                    new_record = results_record(0x8006685C, 0x80066874, 40, 0) | new_record;
                results_continue(labels);
                results_scroll();
                if (R16(0x80065524) & 0x4000)
                {
                    R32(0x80064EC4) = 0;
                    done = 1;
                }
                break;
        }
    }
    else if (mode == 1 || mode == 2 || mode == 4)
    {
        sub_80021430(5);
        results_draw_text(24, 32, labels[1][R32(0x80064B8C)]);
        results_draw_text(60, 32, labels[2][R32(0x80064B8C)]);
        results_draw_text(222, 32, labels[4][R32(0x80064B8C)]);
        for (index = 0; index < (sint32)R32(0x80065254); ++index)
        {
            row = 0x800656BC + (uint32)index * 72;
            if (y >= 56 && y < 176 - R8(0x80065328))
            {
                sub_80021430(R32(0x80064EC4) ? 2 : 3);
                results_draw_ordinal(row + 1, (uint32)index);
                results_draw_text(24, y, row + 1);
                sub_80021430((uint32)index == R32(0x80064EB0) || (R32(0x8005973C) == 2 && (uint32)index == R32(0x80064EB4)) ? 1 : 2);
                results_draw_text(60, y, row + 9);
                if (mode != 4 && R8(row + 60) == 1)
                    results_draw_text(222, y, 0x80064FFC);
                else
                {
                    results_time(text, sizeof(text), R32(row + 56));
                    sub_80020AB8(5, 222, y, text);
                }
            }
            y = (sint32)((uint32)y + 16 + R8(0x80065328));
            if (y >= 177)
                break;
        }
        if (!R8(0x80064EC0))
            results_slide(0);
        else
        {
            if (R32(0x80059738) == 1 && !R32(0x80064EB0) && (sint32)R32(0x80064C08) < 16 && R32(0x8005973C) != 2)
                new_record = results_record(0x800665DC, 0x800665F0, 56, 1);
            if (R32(0x80059738) == 2 && !R32(0x80064EB0) && (sint32)R32(0x80064C08) < 15 && R32(0x8005973C) != 2)
                new_record = results_record(0x8006671C, 0x80066730, 56, 1) | new_record;
            if (R32(0x80059738) == 4 && !R32(0x80064EB0) && (sint32)R32(0x80064C08) < 16 && R32(0x8005973C) != 2)
                new_record = results_record(0x800668BC, 0x800668D0, 56, 1) | new_record;
            results_scroll();
            results_continue(labels);
            if (R16(0x80065524) & 0x4000)
                done = 1;
        }
    }
    if (new_record == 1 && (sint32)R32(0x80064EBC) % 16 < 8)
        results_draw_text(200, 16, labels[5][R32(0x80064B8C)]);
    return done;
}
