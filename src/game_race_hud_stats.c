#include "draft_signatures.h"
#include <stdio.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))

static sint32 stats_div(uint32 n, uint32 d)
{
    if (!d)
        xport_mips_break(7u);
    if (n == 0x80000000u && d == 0xFFFFFFFFu)
        xport_mips_break(6u);
    return (sint32)n / (sint32)d;
}

static void stats_link(uint32 environment, uint32 packet)
{
    uint32 ot = R32(environment + 112u);
    R32(packet) = (R32(packet) & 0xFF000000u) | (R32(ot + 4u) & 0xFFFFFFu);
    ot = R32(environment + 112u);
    R32(ot + 4u) = (R32(ot + 4u) & 0xFF000000u) | (packet & 0xFFFFFFu);
}

static void stats_color(uint32 packet, uint8 r, uint8 g, uint8 b)
{
    R8(packet + 4u) = r;
    R8(packet + 5u) = g;
    R8(packet + 6u) = b;
}

static void stats_quad(uint32 packet, uint32 texture_slot, sint32 x, sint32 y, sint32 width, sint32 height, sint32 u, sint32 v)
{
    R16(packet + 8u) = (uint16)x;
    R16(packet + 10u) = (uint16)y;
    R16(packet + 16u) = (uint16)(x + width);
    R16(packet + 18u) = (uint16)y;
    R16(packet + 24u) = (uint16)x;
    R16(packet + 26u) = (uint16)(y + height);
    R16(packet + 32u) = (uint16)(x + width);
    R16(packet + 34u) = (uint16)(y + height);
    R8(packet + 12u) = (uint8)(R8(R32(texture_slot) + 24u) + u);
    R8(packet + 13u) = (uint8)(R8(R32(texture_slot) + 25u) + v);
    R8(packet + 20u) = (uint8)(R8(R32(texture_slot) + 24u) + u + width);
    R8(packet + 21u) = (uint8)(R8(R32(texture_slot) + 25u) + v);
    R8(packet + 28u) = (uint8)(R8(R32(texture_slot) + 24u) + u);
    R8(packet + 29u) = (uint8)(R8(R32(texture_slot) + 25u) + v + height);
    R8(packet + 36u) = (uint8)(R8(R32(texture_slot) + 24u) + u + width);
    R8(packet + 37u) = (uint8)(R8(R32(texture_slot) + 25u) + v + height);
}

void game_800A37B8(uint32 object, uint32 port)
{
    sint32 flash = (sint16)R16(0x800C4080u + port * 2u);
    uint32 bank, environment, i;
    if (flash)
    {
        sint32 countdown = (sint16)R16(0x800C4088u + port * 2u) - 1;
        if (!countdown)
        {
            uint8 r, g, b;
            --flash;
            if (flash >= 0)
            {
                if (!flash)
                    r = g = b = 128u;
                else
                {
                    r = 224u;
                    g = b = (flash & 1) ? 0u : 224u;
                    countdown = 8;
                }
            }
            else
            {
                uint32 track = R32(0x800651E4u), length = R32(0x800653A4u);
                uint32 distance;
                if (R32(0x80059738u) == 2u)
                    distance = R32(track + 60u) - R32(0x80064BD4u) * length - (uint32)(sint32)(sint16)R16(object + 92u);
                else
                    distance = R32(track + 60u) + R32(0x80064BD4u) * length - (uint32)(sint32)(sint16)R16(object + 94u);
                if ((sint32)distance > (sint32)length / 2)
                    countdown = 8;
                else
                    countdown = (sint32)distance > (sint32)length / 4 ? 4 : 2;
                r = 224u;
                g = b = (flash & 1) ? 0u : 224u;
            }
            bank = port * 40u;
            for (i = port; (sint32)i < (sint32)(port + 2u - R32(0x80064C14u)); ++i)
            {
                uint32 j;
                stats_color(0x800C28ECu + bank, r, g, b);
                stats_color(0x800C293Cu + bank, r, g, b);
                stats_color(0x800C1D5Cu + bank, r, g, b);
                if (flash < 0)
                    for (j = 0; j < 9u; ++j)
                        stats_color(0x800C1A3Cu + bank + j * 80u, r, g, b);
                bank += 40u;
            }
        }
        R16(0x800C4080u + port * 2u) = (uint16)flash;
        R16(0x800C4088u + port * 2u) = (uint16)countdown;
    }
    bank = R32(0x80065128u) * 40u;
    environment = R32(0x80065118u);
    stats_link(environment, 0x800C28ECu + bank);
    stats_link(environment, 0x800C293Cu + bank);
    stats_link(environment, 0x800C1D5Cu + bank);
    for (i = 0; i < 3u; ++i)
        stats_link(environment, 0x800C194Cu + bank + i * 80u);
    bank = R32(0x80065128u);
    environment = R32(0x80065118u);
    stats_link(environment, 0x800C3AC0u + bank * 24u);
    if (flash < 0)
        for (i = 0; i < 9u; ++i)
            stats_link(environment, 0x800C1A3Cu + bank * 40u + i * 80u);
}

static void stats_atlas(uint8 c, sint32 *u, sint32 *v)
{
    if (c >= 98u)
    {
        *u = ((sint32)c - 97) * 8;
        *v = 16;
    }
    else if (c >= 65u)
    {
        *u = ((sint32)c - 64) * 8;
        *v = 8;
    }
    else
    {
        *u = c * 8;
        *v = 0;
    }
}

void game_800A3C5C(uint32 object, uint32 port)
{
    sint32 lap = (sint16)R16(object + 928u), index;
    uint32 accumulated = 0u, rate, half;
    for (index = lap - 1; index >= 0; --index)
        accumulated += R16(object + 912u + (uint32)index * 2u);
    lap = (sint16)R16(object + 928u);
    R16(object + 912u + (uint32)lap * 2u) = (uint16)(R32(0x800BEEA4u) - accumulated);
    rate = R32(0x80065124u);
    half = (uint32)((sint32)rate / 2);
    for (index = 0; index < (sint32)R32(0x80064BD4u); ++index)
    {
        char text[32];
        uint32 ticks = R16(object + 912u + (uint32)index * 2u), glyph;
        uint32 frame, data;
        sint32 x, y;
        if (ticks)
        {
            sint32 minute = stats_div(ticks, rate * half);
            uint32 remainder = ticks - (uint32)minute * rate * half;
            sint32 second = stats_div(remainder, half), hundredth;
            remainder -= (uint32)second * half;
            hundredth = stats_div(remainder * 100u, half);
            snprintf(text, sizeof(text), (const char *)psx_addr(minute < 10 ? 0x8008BA3Cu : 0x8008BA44u, 1u), minute);
            snprintf(text + 3, sizeof(text) - 3u, (const char *)psx_addr(second < 10 ? 0x8008BA3Cu : 0x8008BA44u, 1u), second);
            snprintf(text + 6, sizeof(text) - 6u, (const char *)psx_addr(hundredth < 10 ? 0x8008BA48u : 0x8008BA4Cu, 1u), hundredth);
        }
        else
            snprintf(text, sizeof(text), "%s", (const char *)psx_addr(0x8008BA50u, 1u));
        frame = R32(0x80065128u);
        data = R32(0x800C4030u) + 200u + frame * 4u;
        x = (sint16)R16(data);
        y = (sint16)R16(data + 2u) + index * 8;
        for (glyph = 0; glyph < 8u; ++glyph)
        {
            frame = R32(0x80065128u);
            uint32 packet = 0x800C2A2Cu + frame * 200u + glyph * 400u + (uint32)index * 40u;
            sint32 u, v;
            stats_atlas((uint8)text[glyph], &u, &v);
            if (text[glyph] == ':' || text[glyph] == '1')
                u -= 2;
            x += 6;
            stats_quad(packet, 0x800C0F40u, x, y, 6, 8, u, v);
            stats_link(R32(0x80065118u), packet);
        }
    }
}

static sint32 stats_lerp(sint32 first, sint32 second, sint32 percent)
{
    uint32 product = ((uint32)second - (uint32)first) * (uint32)percent;
    return (sint32)((uint32)first + (uint32)((sint32)product / 100));
}

void game_800A4230(uint32 object, uint32 port)
{
    uint32 frame = R32(0x80065128u), data = R32(0x800C4030u) + 192u + frame * 4u;
    sint32 x = (sint16)R16(data), y = (sint16)R16(data + 2u);
    sint32 current, maximum, health, previous, offset, delta = 0, width, remaining;
    uint32 texture, bottom, top, difference = 0u, glyph;
    char text[32] = {0};
    if (R8(object + 378u) & 1u)
        return;
    current = (sint16)R16(object + 320u);
    maximum = (sint16)R16(object + 322u);
    health = 100 - stats_div((uint32)current * 100u, (uint32)maximum);
    texture = R32(0x800C39CCu);
    offset = (sint32)((uint32)health * (uint32)(R8(texture + 23u) - 1)) / 100;
    previous = (sint16)R16(object + 324u);
    bottom = 0x800C3A20u + frame * 40u;
    top = 0x800C39D0u + frame * 40u;
    width = R8(texture + 22u) - 1;
    if (previous != current)
    {
        sint32 previous_offset, step;
        health = 100 - stats_div((uint32)previous * 100u, (uint32)maximum);
        previous_offset = (sint32)((uint32)health * (uint32)(R8(texture + 23u) - 1)) / 100;
        if (previous_offset != offset)
        {
            difference = 0x800C3A70u + frame * 40u;
            if (previous_offset < offset)
            {
                delta = offset - previous_offset;
                offset = previous_offset;
                stats_color(difference, 0u, 128u, 0u);
            }
            else
            {
                delta = previous_offset - offset;
                stats_color(difference, 128u, 64u, 0u);
            }
        }
        step = maximum / 128;
        if (current < previous)
        {
            previous -= step;
            if (previous < current)
                previous = current;
        }
        else
        {
            previous += step;
            if (previous > current)
                previous = current;
        }
        R16(object + 324u) = (uint16)previous;
    }
    remaining = R8(R32(0x800C39CCu) + 23u) - offset - delta;
    stats_quad(bottom, 0x800C39CCu, x, y, width, remaining, 0, 0);
    stats_quad(top, 0x800C39CCu, x, y + remaining + delta, width, offset, 0, remaining + delta);
    if (delta > 0)
        stats_quad(difference, 0x800C39CCu, x, y + remaining, width, delta, 0, remaining);
    if (R8(object + 378u) & 64u)
    {
        sint32 elapsed = (sint32)(R32(object + 316u) - R32(0x80064A30u));
        uint32 color = 0x800BEE64u + (uint32)(elapsed % 10) * 4u;
        stats_color(bottom, R8(color), R8(color + 1u), R8(color + 2u));
        stats_color(top, R8(color), R8(color + 1u), R8(color + 2u));
        if (delta > 0)
            stats_color(difference, R8(color), R8(color + 1u), R8(color + 2u));
    }
    else if (health < 15)
    {
        sint32 amplitude = stats_div(((uint32)current * 4u - (uint32)maximum * 3u) * 100u, (uint32)maximum);
        sint32 period = (sint16)stats_lerp(20, 4, amplitude);
        uint32 time = R32(0x80064A30u), brightness;
        if (!(time < R32(0x800C0B18u) + (uint32)period))
            R32(0x800C0B18u) = time;
        if (!period)
            xport_mips_break(7u);
        brightness = ((time - R32(0x800C0B18u)) << 7) / (uint32)period;
        stats_color(top, (uint8)brightness, (uint8)brightness, (uint8)brightness);
        stats_color(bottom, (uint8)brightness, 0u, 0u);
        if (delta > 0)
        {
            if (R8(difference + 4u))
            {
                R8(difference + 4u) = (uint8)brightness;
                R8(difference + 5u) = (uint8)((sint32)brightness >> 1);
            }
            else
                R8(difference + 5u) = (uint8)brightness;
        }
    }
    else
    {
        stats_color(bottom, 128u, 0u, 0u);
        stats_color(top, 128u, 128u, 128u);
    }
    {
        uint32 environment = R32(0x80065118u);
        stats_link(environment, top);
        stats_link(environment, bottom);
        if (delta > 0)
            stats_link(environment, difference);
    }
    snprintf(text, sizeof(text), (const char *)psx_addr(0x8008BA4Cu, 1u), health);
    frame = R32(0x80065128u);
    data = R32(0x800C4030u) + 184u + frame * 4u;
    x = (sint16)R16(data);
    y = (sint16)R16(data + 2u);
    if (health < 10)
        x += 6;
    else if (health < 100)
        x += 3;
    for (glyph = 0; glyph < 3u; ++glyph)
    {
        uint32 packet = 0x800C388Cu + R32(0x80065128u) * 40u + glyph * 80u;
        sint32 u, v, advance = 6;
        uint8 c = (uint8)text[glyph];
        stats_atlas(c, &u, &v);
        if (c == 'I')
        {
            advance = 5;
            --u;
        }
        else if (c == '1' || c == ':')
        {
            advance = 5;
            u -= 2;
        }
        else if (c == ' ')
            advance = 4;
        stats_quad(packet, 0x800C0F40u, x, y, advance, 8, u, v);
        stats_link(R32(0x80065118u), packet);
        x += advance;
    }
}
