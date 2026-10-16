#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))

void game_800A6E8C(uint32 player)
{
    uint32 texture, index, packet, ot;
    sint16 digits;
    if (!R8(0x800651C9u))
        return;
    digits = (sint16)R16(player + 88u);
    texture = sub_80039ADC(0x8008B9D8u);
    for (index = 0u; index < 3u; ++index)
    {
        sint32 quotient = digits / 10;
        sint32 offset = (sint16)(digits - quotient * 10) * 8;
        uint32 left = 160u - index * 8u, right = 168u - index * 8u;
        packet = 0x800C0A20u + R32(0x80065128u) * 40u + index * 80u;
        R8(packet + 3u) = 9u;
        R8(packet + 7u) = 44u;
        R8(packet + 12u) = (uint8)(R8(texture + 24u) + offset);
        R8(packet + 13u) = R8(texture + 25u);
        R8(packet + 20u) = (uint8)(R8(texture + 24u) + offset + 7);
        R8(packet + 21u) = R8(texture + 25u);
        R8(packet + 28u) = (uint8)(R8(texture + 24u) + offset);
        R8(packet + 29u) = (uint8)(R8(texture + 25u) + R8(texture + 23u));
        R8(packet + 36u) = (uint8)(R8(texture + 24u) + offset + 7);
        R16(packet + 8u) = (uint16)left;
        R16(packet + 10u) = 216u;
        R16(packet + 16u) = (uint16)right;
        R16(packet + 18u) = 216u;
        R16(packet + 24u) = (uint16)left;
        R8(packet + 37u) = (uint8)(R8(texture + 25u) + R8(texture + 23u));
        R16(packet + 32u) = (uint16)right;
        R16(packet + 26u) = (uint16)(R8(texture + 23u) + 216u);
        R16(packet + 34u) = (uint16)(R8(texture + 23u) + 216u);
        R16(packet + 22u) = R16(texture + 8u);
        R16(packet + 14u) = R16(texture + 10u);
        R8(packet + 7u) |= 3u;
        ot = R32(R32(0x80065118u) + 112u);
        R32(packet) = (R32(packet) & 0xFF000000u) | (R32(ot + 4u) & 0xFFFFFFu);
        ot = R32(R32(0x80065118u) + 112u);
        R32(ot + 4u) = (R32(ot + 4u) & 0xFF000000u) | (packet & 0xFFFFFFu);
        digits = (sint16)quotient;
    }
}

extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);

static uint32 hud_div(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        xport_mips_break(7u);
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        xport_mips_break(6u);
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static sint32 hud_high(uint32 left, uint32 right)
{
    int64_t product = (int64_t)(sint32)left * (sint32)right;
    return (sint32)(product >> 32);
}

static sint32 hud_trunc_shift(uint32 value, uint32 bits)
{
    if ((sint32)value < 0)
        value += (1u << bits) - 1u;
    return (sint32)value >> bits;
}

static void hud_link(uint32 packet, uint32 level)
{
    uint32 scene = R32(0x80065118u);
    uint32 ot = R32(scene + 112u);
    R32(packet) = (R32(packet) & 0xFF000000u) | (R32(ot + level * 4u) & 0xFFFFFFu);
    ot = R32(scene + 112u);
    R32(ot + level * 4u) = (R32(ot + level * 4u) & 0xFF000000u) | (packet & 0xFFFFFFu);
}

void game_800A60C8(uint32 player)
{
    uint32 value = (uint32)(sint32)(sint16)R16(player + 12u) + 200u;
    uint32 angle, pair, color, shadow, frame, texture, packet, index;
    sint32 cx, cy, sine, cosine, x, y, thin_x, thin_y, speed;
    value *= 2850u;
    angle = hud_div(value, (uint32)(sint32)(sint16)R16(R32(player + 4u) + 114u));
    pair = R32(0x800C4030u) + 40u + R32(0x80065128u) * 4u;
    cx = (sint16)R16(pair);
    cy = (sint16)R16(pair + 2u);
    for (index = 0u; index < 2u; ++index)
    {
        uint32 length = index ? 14u : 29u;
        uint32 sin_product;
        sine = draft_agent3_missing_8004388C(angle + index * 2048u);
        cosine = draft_agent3_missing_800437EC(angle + index * 2048u);
        frame = R32(0x80065128u) * 20u + index * 40u;
        color = 0x800C0B70u + frame;
        shadow = 0x800C0BC0u + frame;
        x = (sint32)((uint32)cx - (uint32)hud_trunc_shift((uint32)cosine * length, 12u));
        sin_product = (uint32)sine * length;
        y = (sint32)((uint32)cy - (uint32)((hud_high(sin_product, 0x2AAAAAABu) >> 10) - ((sint32)sin_product >> 31)));
        R16(color + 8u) = R16(shadow + 8u) = (uint16)x;
        R16(color + 10u) = R16(shadow + 10u) = (uint16)y;
        thin_x = hud_trunc_shift((uint32)sine * 3u, 12u);
        thin_y = hud_trunc_shift((uint32)cosine, 11u);
        R16(color + 12u) = R16(shadow + 12u) = (uint16)((uint32)cx + thin_x);
        R16(color + 14u) = R16(shadow + 14u) = (uint16)((uint32)cy - thin_y);
        R16(color + 16u) = R16(shadow + 16u) = (uint16)((uint32)cx - thin_x);
        R16(color + 18u) = R16(shadow + 18u) = (uint16)((uint32)cy + thin_y);
        R16(shadow + 10u) += 2u;
        R16(shadow + 14u) += 2u;
        R16(shadow + 18u) += 2u;
        hud_link(color, 1u);
        hud_link(shadow, 2u);
    }
    speed = (sint32)R32(player + 888u) >> 8;
    if (speed < 0)
        speed = 0;
    if (R32(0x80064BB8u))
    {
        value = ((uint32)speed << 8) + 389u;
        speed = (sint32)((uint32)((sint32)((uint32)hud_high(value, 0xA8791709u) + value) >> 9) - (uint32)((sint32)value >> 31));
    }
    else
    {
        value = ((uint32)speed << 8) + 626u;
        speed = (sint32)((uint32)((sint32)((uint32)hud_high(value, 0xD161543Fu) + value) >> 10) - (uint32)((sint32)value >> 31));
    }
    texture = R32(0x800C0C64u);
    frame = R32(0x80065128u) * 40u;
    value = R8(texture + 24u) + R8(player + 18u) * 13u;
    R8(0x800C0C84u + frame) = R8(0x800C0C74u + frame) = (uint8)value;
    texture = R32(0x800C0C64u);
    frame = R32(0x80065128u) * 40u;
    value = R8(texture + 24u) + R8(player + 18u) * 13u + 12u;
    R8(0x800C0C8Cu + frame) = R8(0x800C0C7Cu + frame) = (uint8)value;
    hud_link(0x800C0C68u + R32(0x80065128u) * 40u, 1u);
    for (index = 0u; index < 3u; ++index)
    {
        sint32 quotient, digit;
        if (!speed && index)
            break;
        quotient = speed / 10;
        digit = speed - quotient * 10;
        texture = R32(0x800C0F3Cu);
        packet = 0x800C0DFCu + R32(0x80065128u) * 40u + index * 80u;
        R8(packet + 12u) = (uint8)(R8(texture + 24u) + (uint32)digit * 8u);
        R8(packet + 13u) = R8(texture + 25u);
        R8(packet + 20u) = (uint8)(R8(texture + 24u) + (uint32)digit * 8u + 7u);
        R8(packet + 21u) = R8(texture + 25u);
        R8(packet + 28u) = (uint8)(R8(texture + 24u) + (uint32)digit * 8u);
        R8(packet + 29u) = (uint8)(R8(texture + 25u) + R8(texture + 23u));
        R8(packet + 36u) = (uint8)(R8(texture + 24u) + (uint32)digit * 8u + 7u);
        R8(packet + 37u) = (uint8)(R8(texture + 25u) + R8(texture + 23u));
        hud_link(packet, 1u);
        speed = quotient;
    }
    frame = R32(0x80065128u) * 40u;
    hud_link(0x800C0DACu + frame, 1u);
    hud_link(0x800C0B1Cu + frame, 1u);
    hud_link(0x800C0C14u + frame, 1u);
}

void game_800A5BC4(uint32 freeze)
{
    uint32 frequency = R32(0x80065124u);
    uint32 half = (uint32)((sint32)(frequency + (frequency >> 31)) >> 1);
    uint32 full = frequency * half, frame = R32(0x80065128u) * 4u;
    uint32 layout = R32(0x800C4030u), clock, minutes, seconds, fractions, remain, mode, index, packet, texture;
    sint32 timer_y = (sint16)R16(layout + frame + 58u);
    sint32 label_y = (sint16)R16(layout + frame + 50u);
    char text[64];
    if (!freeze)
        R32(0x800BEEA4u) += 1u;
    mode = R32(0x80059738u);
    if (mode == 3u || mode == 7u)
        return;
    if (!R32(0x80064A44u) && !R8(0x800670C5u) && mode != 4u)
        R16(0x800670A2u) = (uint16)R32(0x800BEEA4u);
    clock = R32(0x800BEEA4u);
    minutes = hud_div(clock, full);
    remain = clock - minutes * full;
    seconds = hud_div(remain, half);
    remain -= seconds * half;
    fractions = hud_div(remain * 100u, half);
    snprintf(text, sizeof(text), (const char *)psx_addr((sint32)minutes < 10 ? 0x8008BA3Cu : 0x8008BA44u, 1u), (sint32)minutes);
    snprintf(text + 3u, sizeof(text) - 3u, (const char *)psx_addr((sint32)seconds < 10 ? 0x8008BA3Cu : 0x8008BA44u, 1u), (sint32)seconds);
    snprintf(text + 6u, sizeof(text) - 6u, (const char *)psx_addr((sint32)fractions < 10 ? 0x8008BA48u : 0x8008BA4Cu, 1u), (sint32)fractions);
    for (index = 0u; index < 8u; ++index)
    {
        uint32 offset = ((uint32)(uint8)text[index] - 48u) * 8u;
        packet = 0x800C266Cu + R32(0x80065128u) * 40u + index * 80u;
        texture = R32(0x800C0F3Cu);
        R16(packet + 18u) = R16(packet + 10u) = (uint16)timer_y;
        R16(packet + 34u) = R16(packet + 26u) = (uint16)(timer_y + 8);
        R8(packet + 12u) = (uint8)(R8(texture + 24u) + offset);
        R8(packet + 13u) = R8(texture + 25u);
        R8(packet + 20u) = (uint8)(R8(texture + 24u) + offset + 7u);
        R8(packet + 21u) = R8(texture + 25u);
        R8(packet + 28u) = (uint8)(R8(texture + 24u) + offset);
        R8(packet + 29u) = (uint8)(R8(texture + 25u) + R8(texture + 23u));
        R8(packet + 36u) = (uint8)(R8(texture + 24u) + offset + 7u);
        R8(packet + 37u) = (uint8)(R8(texture + 25u) + R8(texture + 23u));
        hud_link(packet, 1u);
    }
    frame = R32(0x80065128u);
    for (index = 0u; index < 4u; ++index)
    {
        packet = 0x800C13ACu + frame * 40u + index * 80u;
        R16(packet + 18u) = R16(packet + 10u) = (uint16)label_y;
        R16(packet + 34u) = R16(packet + 26u) = (uint16)(label_y + 8);
        hud_link(packet, 1u);
    }
    packet = 0x800C3AF0u + R32(0x80065128u) * 24u;
    R16(packet + 14u) = R16(packet + 10u) = (uint16)(timer_y - 2);
    R16(packet + 22u) = R16(packet + 18u) = (uint16)(timer_y + 10);
    hud_link(packet, 1u);
}

uint32 game_800B5790(uint32 track);

uint32 game_800B5904(uint32 player)
{
    uint32 yaw = (uint32)((sint32)R32(player + 128u) >> 8);
    uint32 heading = game_800B5790((uint32)(sint32)(sint16)R16(player + 88u));
    return ((yaw - heading + 2048u) & 4095u) - 2048u;
}

void game_800A7940(uint32 player)
{
    uint32 port = R8(player + 937u), offset = port * 4u, yaw, caution = 0u;
    sint32 segment = (sint16)R16(player + 88u);
    sint32 x = 150, y = 120;
    if (R32(0x800593A0u) == 1u)
        return;
    yaw = game_800B5904(player);
    if (R32(0x80059738u) == 2u)
    {
        caution = yaw + 1023u < 2047u;
        if ((sint32)R32(0x800C3FF8u + offset) < segment)
            R32(0x800C4000u + offset) += 1u;
        if (segment < (sint32)R32(0x800C3FF8u + offset))
            R32(0x800C4000u + offset) = 0u;
    }
    else
    {
        caution = yaw + 1024u >= 2048u;
        if (segment < (sint32)R32(0x800C3FF8u + offset))
            R32(0x800C4000u + offset) += 1u;
        if ((sint32)R32(0x800C3FF8u + offset) < segment)
            R32(0x800C4000u + offset) = 0u;
    }
    R32(0x800C4008u + offset) += 1u;
    if (R32(0x8005973Cu) == 2u && !R32(0x80064B68u))
        y = 48;
    else if (R32(0x8005973Cu) == 2u)
        x = 32;
    if (caution && (sint32)R32(0x800C4000u + offset) >= 3)
    {
        sint32 phase = (sint32)R32(0x800C4008u + offset) % 32;
        if (phase >= 9)
        {
            uint32 text;
            sub_80021430(3u);
            text = R32(0x800BEE50u + R32(0x80064B8Cu) * 4u);
            sub_80020AB8(5u, x, y, (const char *)psx_addr(text, 1u));
        }
    }
    R32(0x800C3FF8u + offset) = (uint32)segment;
}

void game_800A315C(uint32 player, uint32 port, uint32 flag);
void game_800A23CC(uint32 port);

void game_800A7098(uint32 player)
{
    uint32 mode;
    if (R32(0x80064B5Cu))
    {
        R32(0x800BEEA8u) = 1u;
        return;
    }
    game_800A6E8C(player);
    game_800A23CC(0u);
    if (R32(0x80065110u) || R32(0x80064A44u))
        return;
    game_800A60C8(player);
    if (R32(0x800BEEA8u) != 1u)
        return;
    mode = R32(0x80059738u);
    switch (mode)
    {
        case 1u:
            if (!R8(player + 937u) && !R32(0x80064A44u))
                game_800A7940(player);
            game_800A315C(player, 0u, 0u);
            game_800A5BC4(0u);
            game_800A37B8(player, 0u);
            game_800A3C5C(player, 0u);
            game_800A4230(player, 0u);
            break;
        case 2u:
            if (!R8(player + 937u) && !R32(0x80064A44u))
                game_800A7940(player);
            game_800A5BC4(0u);
            game_800A315C(player, 0u, 0u);
            game_800A37B8(player, 0u);
            game_800A3C5C(player, 0u);
            game_800A4230(player, 0u);
            break;
        case 0u:
        case 8u:
        case 9u:
        case 10u:
        case 11u:
            if (!R8(player + 937u) && !R32(0x80064A44u))
                game_800A7940(player);
            game_800A315C(player, 0u, 0u);
            game_800A5864(player, 0u);
            game_800A37B8(player, 0u);
            game_800A4230(player, 0u);
            break;
        case 3u:
        case 7u:
            game_800A315C(player, 0u, 1u);
            game_800A5864(player, 0u);
            game_800A4230(player, 0u);
            game_800A5BC4(0u);
            break;
        case 4u:
            if (!R8(player + 937u) && !R32(0x80064A44u))
                game_800A7940(player);
            game_800A315C(player, 0u, 1u);
            game_800A5BC4(0u);
            game_800A4230(player, 0u);
            break;
        default:
            break;
    }
}

void game_800A69D4(uint32 player)
{
    uint32 value, texture, frame, packet, index;
    sint32 speed;
    FUNCTION_MARKER(0x800A69D4u, "GAME.BIN");
    speed = (sint32)R32(player + 888u) >> 8;
    if (speed < 0)
        speed = 0;
    if (R32(0x80064BB8u))
    {
        value = ((uint32)speed << 8) + 389u;
        speed = (sint32)((uint32)((sint32)((uint32)hud_high(value, 0xA8791709u) + value) >> 9) - (uint32)((sint32)value >> 31));
    }
    else
    {
        value = ((uint32)speed << 8) + 626u;
        speed = (sint32)((uint32)((sint32)((uint32)hud_high(value, 0xD161543Fu) + value) >> 10) - (uint32)((sint32)value >> 31));
    }
    texture = R32(0x800C0C64u);
    frame = R32(0x80065128u) * 40u;
    value = R8(texture + 24u) + R8(player + 18u) * 13u;
    R8(0x800C0C84u + frame) = R8(0x800C0C74u + frame) = (uint8)value;
    texture = R32(0x800C0C64u);
    frame = R32(0x80065128u) * 40u;
    value = R8(texture + 24u) + R8(player + 18u) * 13u + 12u;
    R8(0x800C0C8Cu + frame) = R8(0x800C0C7Cu + frame) = (uint8)value;
    hud_link(0x800C0C68u + R32(0x80065128u) * 40u, 1u);
    for (index = 0u; index < 3u; ++index)
    {
        sint32 quotient, digit;
        if (!speed && index)
            break;
        quotient = speed / 10;
        digit = speed - quotient * 10;
        texture = R32(0x800C0F3Cu);
        packet = 0x800C0DFCu + R32(0x80065128u) * 40u + index * 80u;
        R8(packet + 12u) = (uint8)(R8(texture + 24u) + (uint32)digit * 8u);
        R8(packet + 13u) = R8(texture + 25u);
        R8(packet + 20u) = (uint8)(R8(texture + 24u) + (uint32)digit * 8u + 7u);
        R8(packet + 21u) = R8(texture + 25u);
        R8(packet + 28u) = (uint8)(R8(texture + 24u) + (uint32)digit * 8u);
        R8(packet + 29u) = (uint8)(R8(texture + 25u) + R8(texture + 23u));
        R8(packet + 36u) = (uint8)(R8(texture + 24u) + (uint32)digit * 8u + 7u);
        R8(packet + 37u) = (uint8)(R8(texture + 25u) + R8(texture + 23u));
        hud_link(packet, 1u);
        speed = quotient;
    }
    frame = R32(0x80065128u) * 40u;
    hud_link(0x800C0DACu + frame, 1u);
    hud_link(0x800C0B1Cu + frame, 1u);
}

void game_800A730C(uint32 player, uint32 port)
{
    uint32 mode;
    FUNCTION_MARKER(0x800A730Cu, "GAME.BIN");
    game_800A6E8C(player);
    if (R32(0x80065110u) || R32(0x80064A44u) || R32(0x800BEEA8u) != 1u)
        return;
    game_800A69D4(player);
    mode = R32(0x80059738u);
    if (mode > 6u)
        return;
    if (mode == 3u || mode == 4u)
    {
        game_800A315C(player, port, 1u);
        if (mode == 3u)
            game_800A5864(player, port);
        game_800A4230(player, port);
        game_800A5BC4(mode == 3u ? 0u : port);
        return;
    }
    if (R8(player + 937u) < 2u && !R32(0x80064A44u))
        game_800A7940(player);
    if (mode == 1u || mode == 2u)
        game_800A5BC4(mode == 1u ? port : 0u);
    game_800A315C(player, port, 0u);
    if (mode != 1u && mode != 2u)
        game_800A5864(player, port);
    game_800A37B8(player, port);
    game_800A4230(player, port);
    if (R8(player + 945u))
    {
        const char *text = (const char *)psx_addr(R32(0x800BECB4u + R32(0x80064B8Cu) * 4u), 1u);
        uint32 count = (uint32)strlen(text), index;
        for (index = 0u; index < count; ++index)
            hud_link(0x800C0F4Cu + R32(0x80065128u) * 40u + index * 80u, 1u);
    }
}
