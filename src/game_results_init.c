#include "draft_signatures.h"
#include <stdio.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4))
#define R16(a) (*(uint16 *)psx_addr((a), 2))
#define R8(a) (*(uint8 *)psx_addr((a), 1))

static int sub_80025348(const void *first, const void *second)
{
    sint32 a = *(const sint32 *)((const uint8 *)first + 48);
    sint32 b = *(const sint32 *)((const uint8 *)second + 48);
    return a < b ? -1 : a != b;
}

static int sub_80025370(const void *first, const void *second)
{
    sint32 a = *(const sint32 *)((const uint8 *)first + 40);
    sint32 b = *(const sint32 *)((const uint8 *)second + 40);
    return b < a ? -1 : a != b;
}

static int sub_80025398(const void *first, const void *second)
{
    sint32 a = *(const sint32 *)((const uint8 *)first + 28);
    sint32 b = *(const sint32 *)((const uint8 *)second + 28);
    return b < a ? -1 : a != b;
}

static int sub_800253C0(const void *first, const void *second)
{
    sint32 a = *(const sint32 *)((const uint8 *)first + 56);
    sint32 b = *(const sint32 *)((const uint8 *)second + 56);
    return a < b ? -1 : a != b;
}

static int sub_800253E8(const void *first, const void *second)
{
    sint32 a = *(const sint32 *)((const uint8 *)first + 56);
    sint32 b = *(const sint32 *)((const uint8 *)second + 56);
    return b < a ? -1 : a != b;
}

static void results_sort(uint32 base, uint32 size, int (*compare)(const void *, const void *))
{
    qsortPSX(psx_addr(base, 1), R32(0x80065254), size, compare);
}

static void results_copy_string(uint32 destination, uint32 source)
{
    strcpy((char *)psx_addr(destination, 1), (const char *)psx_addr(source, 1));
}

static void results_player_names(void)
{
    results_copy_string(0x800656C5, 0x80059770);
    if (R32(0x8005973C) == 2)
        results_copy_string(0x8006570D, 0x8005977A);
}

static void results_ordinal(uint32 destination, uint32 position, uint32 language)
{
    uint32 suffix = 0;
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

static void results_gradient(uint32 packet, uint16 top, uint16 bottom, uint8 first, uint8 second)
{
    uint32 index;
    /* Original SetPolyGT4 writes only the length and primitive code */
    R8(packet + 3) = 12;
    R8(packet + 7) = 0x3C;
    for (index = 0; index < 4; ++index)
    {
        uint32 vertex = packet + index * 12;
        R8(vertex + 12) = 0;
        R8(vertex + 13) = 0;
        R16(vertex + 8) = (index & 1) ? 320 : 0;
        R16(vertex + 10) = index < 2 ? top : bottom;
        R8(vertex + 4) = index < 2 ? first : second;
        R8(vertex + 5) = index < 2 ? first : second;
        R8(vertex + 6) = index < 2 ? first : second;
    }
    R8(packet + 7) = (R8(packet + 7) | 2) & 0xFE;
    R16(packet + 26) = GetTPage(2, 2, 960, 256);
}

static uint32 results_points(uint32 player)
{
    uint32 points = (uint32)(sint32)(sint16)R16(player + 0x6E98);
    sint32 bonus = (sint16)R16(player + 0x6E9A);
    if (bonus > 0)
        points += (uint32)bonus;
    return points;
}

static void results_copy_rows(uint32 identity_by_index)
{
    sint32 index;
    for (index = 0; index < (sint32)R32(0x80065254); ++index)
    {
        uint32 row = 0x800656BC + (uint32)index * 72;
        memcpy(psx_addr(0x80065B3C + (uint32)index * 72, 72), psx_addr(row, 72), 72);
        if (identity_by_index)
        {
            if (R8(row) == 0)
            {
                R32(0x80064EB0) = (uint32)index;
                R32(0x80064EC8) = R32(row + 40);
            }
            if (R32(0x8005973C) == 2 && R8(row) == 1)
            {
                R32(0x80064EB4) = (uint32)index;
                R32(0x80064ECC) = R32(row + 40);
            }
        }
        else
        {
            if (R8(0x800670C4) == R32(row + 48))
                R32(0x80064EB0) = (uint32)index;
            if (R32(0x8005973C) == 2 && R8(0x8006747C) == R32(row + 48))
                R32(0x80064EB4) = (uint32)index;
        }
    }
}

static void results_order_players(uint32 reverse)
{
    uint32 first, second;
    if (R32(0x8005973C) != 2)
        return;
    first = R32(0x80064EB0);
    second = R32(0x80064EB4);
    if (reverse ? (sint32)first < (sint32)second : (sint32)second < (sint32)first)
    {
        R32(0x80064EB0) = second;
        R32(0x80064EB4) = first;
    }
}

static void results_point_order(void)
{
    sint32 index;
    uint32 count = R32(0x80065254);
    results_sort(0x80065B3C, 72, sub_80025370);
    for (index = 0; index < (sint32)count; ++index)
        R32(0x800656F0 + R32(0x80065B6C + (uint32)index * 72) * 72) = (uint32)index;
}

uint32 sub_800294BC(void)
{
    PSX_RECT pixel_rect = {960, 256, 1, 1};
    uint32 pixel = 0xC210;
    uint32 buffer, row, player, mode, scale, addition, value, running, rank, league, result;
    sint32 index;
    R32(0x80064BD8) = 1;
    LoadImagePSX(&pixel_rect, &pixel);
    DrawSync(0);
    memset(psx_addr(0x800656BC, 1152), 0, 1152);
    memset(psx_addr(0x80065B3C, 1152), 0, 1152);
    for (buffer = 0; buffer < 2; ++buffer)
    {
        results_gradient(0x800788A0 + buffer * 52, 56, 68, 240, 176);
        results_gradient(0x80078838 + buffer * 52, 68, 80, 176, 0);
        results_gradient(0x80078970 + buffer * 52, 152, 164, 0, 176);
        results_gradient(0x80078908 + buffer * 52, 164, 176, 176, 240);
    }
    value = R32(0x800651E4) + R32(0x80065210) * 4;
    scale = R16(value + 2812);
    addition = R16(value + 2814);
    mode = R32(0x80059738);
    R32(0x80064EC4) = 0;
    switch (mode)
    {
        case 0:
        case 5:
        case 6:
        case 8:
        case 9:
        case 10:
        case 11:
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                player = 0x80060000 + (uint32)index * 952;
                R8(row) = (uint8)index;
                rank = R8(player + 0x70C4);
                R32(row + 48) = rank;
                results_ordinal(row + 1, rank, R32(0x80064B8C));
                results_copy_string(row + 9, 0x800661BC + (uint32)index * 44);
                if ((sint32)R32(0x80059738) >= 8)
                {
                    R8(0x8006691C + (uint32)index * 32) = (uint8)index;
                    results_copy_string(0x8006691D + (uint32)index * 32, 0x800661BC + (uint32)index * 44);
                }
                R32(row + 52) = 0xFFFFFFFF;
                R32(row + 40) = 0;
                R32(row + 44) = 0;
                R8(row + 62) = 0;
                memcpy(psx_addr(row + 62, 4), psx_addr(0x80065070, 4), 4);
                R8(row + 60) = 0;
                if (R8(player + 0x6E8E) & 1)
                {
                    R8(row + 60) = 1;
                    value = 0xFFFFFFFF;
                }
                else
                {
                    R32(row + 32) = R32(0x8005A740 + R32(row + 48) * 4);
                    value = results_points(player);
                    if ((sint32)R32(0x80064C14) < index && R32(0x80059738) != 5)
                    {
                        uint32 product = value * scale;
                        uint32 adjusted = (uint32)((sint32)product / 4096) + addition;
                        value = (uint32)((sint32)adjusted / 5) * 5;
                    }
                    R32(row + 36) = value;
                    value *= R32(row + 32);
                }
                R32(row + 40) = value;
            }
            results_player_names();
            if ((sint32)R32(0x80059738) >= 8)
                results_copy_string(0x8006691D, 0x800656C5);
            results_sort(0x800656BC, 72, sub_80025348);
            results_copy_rows(0);
            results_order_players(0);
            results_point_order();
            if ((sint32)R32(0x80059738) >= 8)
            {
                for (index = 0; index < (sint32)R32(0x80065254); ++index)
                {
                    value = R32(0x8005A780 + (uint32)index * 4);
                    league = (uint32)R8(0x80065B3C + (uint32)index * 72) * 32;
                    R32(0x80066934 + league) = value;
                    R32(0x80066938 + league) += value;
                    memcpy(psx_addr(0x80065FBC + league, 32), psx_addr(0x8006691C + league, 32), 32);
                }
                results_sort(0x80065FBC, 32, sub_80025398);
            }
            R32(0x800656B8) = R8(0x80065FBC) == 0;
            break;
        case 1:
        case 2:
            R32(0x80064EC4) = 1;
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                player = 0x80060000 + (uint32)index * 952;
                results_copy_string(row + 9, 0x800661BC + (uint32)index * 44);
                R32(row + 48) = R8(player + 0x70C4);
                R32(row + 56) = R16(player + 0x70A2);
                R8(row + 61) = R8(player + 0x70C5);
                if (R8(player + 0x6E8E) & 1)
                    R8(row + 60) = 1;
            }
            results_player_names();
            results_sort(0x800656BC, 72, sub_80025348);
            running = R32(0x800656F4);
            if (!running)
            {
                running = R16(0x800670A2);
                if (R32(0x8005973C) == 2 && R16(0x8006745A) < running)
                    running = R16(0x8006745A);
            }
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                if (R8(row + 60) == 1)
                {
                    running = R32(row + 56);
                    R32(row + 56) = 0x7FFFFFFF;
                }
                else if (R8(row + 61))
                    running = R32(row + 56);
                else
                {
                    running += 1 + (uint32)((sint32)randPSX() % 100);
                    R32(row + 56) = running;
                }
            }
            results_sort(0x800656BC, 72, sub_800253C0);
            results_copy_rows(0);
            results_order_players(0);
            break;
        case 3:
        case 7:
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                player = 0x80060000 + (uint32)index * 952;
                R8(row) = (uint8)index;
                results_copy_string(row + 9, 0x800661BC + (uint32)index * 44);
                R32(row + 56) = R16(player + 0x70A2);
                if (R8(player + 0x6E8E) & 1)
                    R8(row + 60) = 1;
                R32(row + 36) = results_points(player);
            }
            results_player_names();
            results_sort(0x800656BC, 72, sub_800253C0);
            running = R32(0x800656F4);
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                if (R8(row + 60) == 1)
                    running = R32(row + 56);
            }
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                if (R8(row + 60) == 1)
                    running = R32(row + 56);
                else
                {
                    running += 1 + (uint32)((sint32)randPSX() % 100);
                    R32(row + 56) = running;
                }
            }
            results_sort(0x800656BC, 72, sub_800253E8);
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                R32(row + 48) = (uint32)index;
                results_ordinal(row + 1, (uint32)index, R32(0x80064B8C));
                R32(row + 32) = R32(0x8005A740 + (uint32)index * 4);
                R32(row + 40) = R32(row + 36) * R32(0x8005A740 + (uint32)index * 4);
            }
            results_copy_rows(1);
            results_order_players(R32(0x80059738) != 3);
            results_point_order();
            break;
        case 4:
            R32(0x80064EC4) = 1;
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                player = 0x80060000 + (uint32)index * 952;
                results_copy_string(row + 9, 0x800661BC + (uint32)index * 44);
                R32(row + 56) = R16(player + 0x70A2);
                if (R8(player + 0x6E8E) & 1)
                    R8(row + 60) = 1;
            }
            results_player_names();
            results_sort(0x800656BC, 72, sub_800253C0);
            running = 0;
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                if (R8(row + 60) == 1 && (sint32)running < (sint32)R32(row + 56))
                    running = R32(row + 56);
            }
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                if (R8(row + 60) != 1)
                {
                    running += 1 + (uint32)((sint32)randPSX() % 100);
                    R32(row + 56) = running;
                }
            }
            results_sort(0x800656BC, 72, sub_800253C0);
            value = R16(0x800670A2);
            for (index = 0; index < (sint32)R32(0x80065254); ++index)
            {
                row = 0x800656BC + (uint32)index * 72;
                if (value == R32(row + 56))
                    R32(0x80064EB0) = (uint32)index;
                if (R32(0x8005973C) == 2 && R16(0x8006745A) == R32(row + 56))
                    R32(0x80064EB4) = (uint32)index;
            }
            results_order_players(0);
            break;
    }
    R8(0x80065328) = 12;
    R8(0x80064EC0) = 0;
    R32(0x80064EA4) = 0 - R32(0x80065254) * 28;
    result = 116 - R32(0x80064EB0) * 28;
    R32(0x80064EA8) = result;
    return result;
}
