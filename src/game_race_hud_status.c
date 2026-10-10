#include "draft_signatures.h"

static uint32 hud_center(uint32 extent, uint32 size)
{
    uint32 difference = extent - size;
    return (uint32)((sint32)(difference + (difference >> 31)) >> 1);
}

static void hud_status_color(uint32 index, uint8 red, uint8 green, uint8 blue)
{
    uint32 packet = 0x800C3C28u + index * 40u;
    w_u8(packet + 4u, red);
    w_u8(packet + 5u, green);
    w_u8(packet + 6u, blue);
}

static void hud_status_quad(uint32 packet, uint32 x, uint32 y, uint32 width, uint32 height)
{
    w_u16(packet + 8u, (uint16)x);
    w_u16(packet + 10u, (uint16)y);
    w_u16(packet + 16u, (uint16)(x + width));
    w_u16(packet + 18u, (uint16)y);
    w_u16(packet + 24u, (uint16)x);
    w_u16(packet + 26u, (uint16)(y + height));
    w_u16(packet + 32u, (uint16)(x + width));
    w_u16(packet + 34u, (uint16)(y + height));
}

static void hud_status_link(uint32 packet)
{
    uint32 owner = r_u32(0x80065118u), head = r_u32(owner + 0x70u);
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(head + 4u) & 0xFFFFFFu));
    head = r_u32(owner + 0x70u);
    w_u32(head + 4u, (r_u32(head + 4u) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

void game_800A23CC(uint32 port)
{
    uint32 elapsed, phase = 1u, texture, width, height, x, y, buffer, index, n, multiple;
    if ((sint32)r_u32(0x80064CB0u) > 0 || r_u32(0x800BEEACu) == 1u)
        return;
    if (!r_u32(0x800BEEB0u))
        w_u32(0x800BEEB0u, r_u32(0x80064A30u));
    elapsed = r_u32(0x80064A30u) - r_u32(0x800BEEB0u);
    if ((uint32)(sint32)(sint16)r_u16(0x800BEED0u) < elapsed)
    {
        while (phase < 5u)
        {
            uint32 passed = (uint32)(sint32)(sint16)r_u16(0x800BEED0u + phase * 2u) < elapsed;
            ++phase;
            if (!passed)
                break;
        }
    }
    multiple = r_u32(0x800593A8u);
    texture = r_u32(0x800C3B80u);
    width = multiple ? (r_u32(0x80064B68u) ? 160u : 320u) : 320u;
    height = multiple ? (r_u32(0x80064B68u) ? 256u : 128u) : 256u;
    x = hud_center(width, r_u8(texture + 22u));
    y = hud_center(height, r_u8(texture + 23u));
    w_u32(0x800C3FE8u, x);
    w_u32(0x800C3FF0u, y);
    if (multiple)
    {
        w_u32(0x800C3FECu, x);
        w_u32(0x800C3FF4u, y);
    }
    if (phase != r_u8(0x800C4090u))
    {
        uint32 sound;
        w_u8(0x800C4090u, (uint8)phase);
        sound = r_u8(0x800BEE9Cu + phase);
        /* Original unsigned-byte load never equals the negative-one sentinel */
        if (sound != 0xFFFFFFFFu)
            sub_8001E858(sound, 0xFFFFFFFFu, 4096u, 0u, 4096u);
    }
    buffer = r_u32(0x80065128u);
    index = port * 12u + buffer * 6u;
    switch (phase)
    {
        case 2u:
            hud_status_color(index + 2u, 130u, 20u, 20u);
            break;
        case 3u:
            hud_status_color(index + 1u, 130u, 20u, 20u);
            break;
        case 4u:
            hud_status_color(index, 130u, 20u, 20u);
            hud_status_color(index + 3u, 50u, 240u, 50u);
            hud_status_color(index + 4u, 50u, 240u, 50u);
            hud_status_color(index + 5u, 50u, 240u, 50u);
            w_u32(0x800BEEA8u, 1u);
            break;
        case 5u:
            w_u32(0x800BEEACu, 1u);
            break;
        default:
            break;
    }
    texture = r_u32(0x800C3B80u);
    x = r_u32(0x800C3FE8u + (multiple ? port * 4u : 0u));
    y = r_u32(0x800C3FF0u + (multiple ? port * 4u : 0u));
    hud_status_quad(0x800C3B88u + (buffer + (multiple ? port * 2u : 0u)) * 40u, x, y, r_u8(texture + 22u), r_u8(texture + 23u));
    texture = r_u32(0x800C3B84u);
    index = buffer * 6u + (multiple ? port * 12u : 0u);
    for (n = 0u; n < 3u; ++n)
        hud_status_quad(0x800C3C28u + (index + n) * 40u, x + 6u + n * 19u, y + 4u, r_u8(texture + 22u), r_u8(texture + 23u));
    for (n = 3u; n < 6u; ++n)
        hud_status_quad(0x800C3C28u + (index + n) * 40u, x + 6u + (n - 3u) * 19u, y + 20u, r_u8(texture + 22u), r_u8(texture + 23u));
    for (n = 0u; n < 6u; ++n)
        hud_status_link(0x800C3C28u + (index + n) * 40u);
    hud_status_link(0x800C3B88u + (buffer + (multiple ? port * 2u : 0u)) * 40u);
}
