#include "draft_signatures.h"
#include <stdio.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4))
#define R16(a) (*(uint16 *)psx_addr((a), 2))
#define R8(a) (*(uint8 *)psx_addr((a), 1))

static sint32 results_two_half(uint32 value)
{
    return (sint32)(value + (value >> 31)) >> 1;
}

static const char *results_two_label(uint32 labels[10][5], uint32 index)
{
    return (const char *)psx_addr(labels[index][R32(0x80064B8C)], 1);
}

static void results_two_center(const char *text, sint32 y)
{
    uint32 index = 0, width = 0;
    /* Native text adapter for the complete 80020A14 glyph lookup */
    while (text[index])
    {
        width += R8(0x80059DD8 + 5 * 144 + (uint8)((uint8)text[index] - 32) + 16);
        ++index;
    }
    width += index;
    sub_80020AB8(5, results_two_half(320 - width), y, text);
}

static uint32 results_two_score(uint32 player)
{
    uint32 points = (uint32)(sint32)(sint16)R16(player + 0x6E98);
    sint32 bonus = (sint16)R16(player + 0x6E9A);
    if (bonus > 0)
        points += (uint32)bonus;
    return points * R32(0x8005A740 + (uint32)R8(player + 0x70C4) * 4);
}

static void results_two_time(char *text, size_t capacity, const char *player, const char *label, uint32 ticks)
{
    uint32 numerator = ticks * 100;
    sint32 divisor = results_two_half(R32(0x80065124));
    sint32 time;
    if (!divisor)
        xport_mips_break(7);
    if (divisor == -1 && numerator == 0x80000000)
        xport_mips_break(6);
    time = (sint32)numerator / divisor;
    /* Preserve all three original stack-carried time fields */
    snprintf(text, capacity, "%s %s %02d : %02d : %02d", player, label, time / 6000, time % 6000 / 100, time % 6000 % 100);
}

uint32 sub_800272F0(void)
{
    static const uint32 tables[10] = {0x800113C8, 0x80011418, 0x80011498, 0x80011518, 0x8001152C, 0x800111C8, 0x800115C4, 0x8001166C, 0x800116F0, 0x80011774};
    uint32 labels[10][5], prompt[5], index, language, first = 0, second = 0, value, width, text_address;
    sint32 count;
    char text[64];
    for (index = 0; index < 10; ++index)
        for (language = 0; language < 5; ++language)
            labels[index][language] = R32(tables[index] + language * 4);
    switch (R32(0x80059738))
    {
        case 1:
        case 2:
            sub_80021430(2);
            if (R8(0x80066E8E) & 1)
            {
                first = 0x07FFFFFF;
                snprintf(text, sizeof(text), "%s : %s", results_two_label(labels, 0), (const char *)psx_addr(0x8006500C, 1));
            }
            else
            {
                first = R16(0x800670A2);
                results_two_time(text, sizeof(text), results_two_label(labels, 0), results_two_label(labels, 5), first);
            }
            results_two_center(text, 64);
            if (R8(0x80067246) & 1)
            {
                second = 0x07FFFFFF;
                snprintf(text, sizeof(text), "%s : %s", results_two_label(labels, 1), (const char *)psx_addr(0x8006500C, 1));
            }
            else
            {
                second = R16(0x8006745A);
                results_two_time(text, sizeof(text), results_two_label(labels, 1), results_two_label(labels, 5), second);
            }
            results_two_center(text, 96);
            strcpy(text, results_two_label(labels, (sint32)first < (sint32)second ? 2 : 3));
            results_two_center(text, 128);
            break;
        case 3:
            sub_80021430(2);
            snprintf(text, sizeof(text), "%s %s : %d", results_two_label(labels, 0), results_two_label(labels, 4), (sint32)R32(0x80064EC8));
            results_two_center(text, 64);
            snprintf(text, sizeof(text), "%s %s : %d", results_two_label(labels, 1), results_two_label(labels, 4), (sint32)R32(0x80064ECC));
            results_two_center(text, 96);
            strcpy(text, results_two_label(labels, (sint32)R32(0x80064ECC) < (sint32)R32(0x80064EC8) ? 2 : 3));
            results_two_center(text, 128);
            break;
        case 4:
            sub_80021430(2);
            first = R16(0x800670A2);
            second = R16(0x8006745A);
            results_two_time(text, sizeof(text), results_two_label(labels, 0), results_two_label(labels, 5), first);
            results_two_center(text, 64);
            results_two_time(text, sizeof(text), results_two_label(labels, 1), results_two_label(labels, 5), second);
            results_two_center(text, 96);
            strcpy(text, results_two_label(labels, (sint32)first < (sint32)second ? 2 : 3));
            results_two_center(text, 128);
            break;
        case 5:
            for (index = 0; index < 2; ++index)
            {
                value = 0x80060000 + index * 952;
                if (!(R8(value + 0x6E8E) & 1))
                    first += results_two_score(value);
            }
            count = (sint32)R32(0x80065254);
            for (index = 2; (sint32)index < count; ++index)
            {
                value = 0x80060000 + index * 952;
                if (!(R8(value + 0x6E8E) & 1))
                    second += results_two_score(value);
            }
            sub_80021430(2);
            snprintf(text, sizeof(text), "%s: %d", results_two_label(labels, 6), (sint32)first);
            results_two_center(text, 64);
            snprintf(text, sizeof(text), "%s: %d", results_two_label(labels, 7), (sint32)second);
            results_two_center(text, 96);
            strcpy(text, results_two_label(labels, (sint32)second < (sint32)first ? 8 : 9));
            results_two_center(text, 128);
            break;
        case 6:
            sub_80021430(2);
            if (R8(0x80066E8E) & 1)
                snprintf(text, sizeof(text), "%s %s : %s", results_two_label(labels, 0), results_two_label(labels, 4), (const char *)psx_addr(0x8006500C, 1));
            else
            {
                first = results_two_score(0x80060000);
                snprintf(text, sizeof(text), "%s %s : %d", results_two_label(labels, 0), results_two_label(labels, 4), (sint32)first);
            }
            results_two_center(text, 64);
            if (R8(0x80067246) & 1)
                snprintf(text, sizeof(text), "%s %s : %s", results_two_label(labels, 1), results_two_label(labels, 4), (const char *)psx_addr(0x8006500C, 1));
            else
            {
                second = results_two_score(0x800603B8);
                snprintf(text, sizeof(text), "%s %s : %d", results_two_label(labels, 1), results_two_label(labels, 4), (sint32)second);
            }
            results_two_center(text, 96);
            strcpy(text, results_two_label(labels, (sint32)second < (sint32)first ? 2 : 3));
            results_two_center(text, 128);
            break;
    }
    for (language = 0; language < 5; ++language)
        prompt[language] = R32(0x80011120 + language * 4);
    sub_80021430(0);
    width = sub_80020A14(5, prompt[R32(0x80064B8C)]);
    text_address = prompt[R32(0x80064B8C)];
    sub_80020AB8(5, results_two_half(320 - width), 200, (const char *)psx_addr(text_address, 1));
    sub_80021430(8);
    sub_80020AB8(5, (sint32)((uint32)results_two_half(320 - width) + width + 4), 200, (const char *)psx_addr(0x80064F00, 1));
    return (R16(0x80065524) & 0x4000) != 0;
}
