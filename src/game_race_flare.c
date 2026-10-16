#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))

uint32 dr_race_motion_sqrt(uint32 value);
uint32 sub_80043450(uint32 x, uint32 z);
sint32 draft_agent3_missing_800437EC(uint32 angle);
sint32 draft_agent3_missing_8004388C(uint32 angle);

static uint32 flare_shift(uint32 value, uint32 bits)
{
    if ((sint32)value < 0)
        value += (1u << bits) - 1u;
    return (uint32)((sint32)value >> bits);
}

static uint32 flare_half(uint32 value)
{
    return (uint32)((sint32)(value + (value >> 31)) >> 1);
}

static uint32 flare_signed_half(uint32 address)
{
    return (uint32)(sint32)(sint16)R16(address);
}

static void flare_link(uint32 packet, uint32 slot)
{
    R32(packet) = (R32(packet) & 0xFF000000u) | (R32(slot) & 0xFFFFFFu);
    R32(slot) = (R32(slot) & 0xFF000000u) | (packet & 0xFFFFFFu);
}

static void flare_quad(uint32 packet, uint32 x, uint32 y, uint32 width, uint32 height)
{
    uint32 left = x - flare_half(width), right = left + width;
    uint32 top = y - flare_half(height), bottom = top + height;
    R16(packet + 8u) = (uint16)left;
    R16(packet + 16u) = (uint16)right;
    R16(packet + 32u) = (uint16)right;
    R16(packet + 10u) = (uint16)top;
    R16(packet + 18u) = (uint16)top;
    R16(packet + 24u) = (uint16)left;
    R16(packet + 26u) = (uint16)bottom;
    R16(packet + 34u) = (uint16)bottom;
}

/* Original 4F1BC packet fields with the rectangle kept in native local storage */
static void flare_move(uint32 packet, const PSX_RECT *rectangle, uint32 x, uint32 y)
{
    uint32 source, size;
    R32(packet + 4u) = 0x01000000u;
    R32(packet + 8u) = 0x80000000u;
    R8(packet + 3u) = rectangle->w && rectangle->h ? 5u : 0u;
    memcpy(&source, rectangle, 4u);
    R32(packet + 16u) = (y << 16) | (x & 0xFFFFu);
    R32(packet + 12u) = source;
    memcpy(&size, (const uint8 *)rectangle + 4u, 4u);
    R32(packet + 20u) = size;
}

void game_8008F924(uint32 object)
{
    uint32 index, slot, owned;
    for (index = 0u; index < 10u; ++index)
    {
        slot = 0x800BFF50u + index * 4u;
        if (R32(slot) != object)
            continue;
        owned = R32(object + 92u);
        if (owned)
            sub_8004261C(owned);
        sub_8004261C(object);
        R32(slot) = 0u;
    }
}

extern void game_8008C864(uint32 object);

void game_8008F6F8(void)
{
    uint32 index, object;
    for (index = 0u; index < 10u; ++index)
    {
        object = R32(0x800BFF50u + index * 4u);
        if (!object)
            continue;
        if (R8(object + 156u) == 1u)
            game_8008C864(object);
        else
            game_8008F924(object);
    }
}

void game_800B3924(void)
{
    SVECTOR point;
    MATRIX translation = {0};
    PSX_RECT rectangle;
    sint32 screen, projected_depth, flags;
    uint32 frame, track, ordering_table, x, y, width, height, dx, dy, distance, angle;
    uint32 cosine, sine, scale, center_x, center_y, packet, bank, color, environment, slot;
    uint32 candidate, index, template_address, fw, fh, wc, ws, hc, hs;
    frame = R32(0x80065128u);
    track = R32(0x80064C08u);
    R32(0x800C7688u + frame * 4u) = 0u;
    memcpy(&point, psx_addr(0x800BF890u + track * 8u, sizeof(point)), sizeof(point));
    ordering_table = 0x800C73FCu + R32(0x80065128u) * 8u;
    ClearOTagPSX(ordering_table, 2u, 0x8005CCE8u, 0x8005CCD4u, 0x8005CC2Au, 0x8005CC24u, 0x800122D8u);
    if (R32(0x80064A44u))
        return;
    if (!point.vx && !point.vy && !point.vz)
        return;
    SetRotMatrix((MATRIX *)psx_addr(0x80081BA4u, sizeof(MATRIX)));
    translation.t[0] = (sint32)R32(0x800BF988u);
    translation.t[1] = (sint32)R32(0x800BF98Cu);
    translation.t[2] = (sint32)R32(0x800BF990u);
    SetTransMatrix(&translation);
    projected_depth = gte_project_full_depth(&point, &screen, &flags);
    if (projected_depth <= 0)
        return;
    x = (uint32)(sint32)(sint16)screen;
    y = (uint32)(sint32)(sint16)((uint32)screen >> 16);
    width = R32(0x8006511Cu);
    if ((sint32)x < -12 || (sint32)x >= (sint32)(width + 12u))
        return;
    height = R32(0x80065120u);
    if ((sint32)y < -12 || (sint32)y >= (sint32)(height + 12u))
        return;
    dx = x - flare_half(width);
    dy = y - flare_half(height);
    distance = dr_race_motion_sqrt(dx * dx + dy * dy);
    angle = sub_80043450(dy, dx) - 512u;
    cosine = (uint32)draft_agent3_missing_800437EC(angle);
    sine = (uint32)draft_agent3_missing_8004388C(angle);
    scale = flare_signed_half(0x800BF860u);
    frame = R32(0x80065128u);
    memcpy(&rectangle, psx_addr(0x8008BC50u, sizeof(rectangle)), sizeof(rectangle));
    bank = frame * 40u;
    packet = 0x800C710Cu + bank;
    center_x = flare_half(R32(0x8006511Cu)) + flare_shift(dx * scale, 8u);
    center_y = flare_half(R32(0x80065120u)) + flare_shift(dy * scale, 8u);
    flare_quad(packet, center_x, center_y, flare_signed_half(0x800C7108u), flare_signed_half(0x800C710Au));
    environment = R32(0x80065118u);
    slot = R32(environment + 112u) + (R32(0x800654F0u) << 2) - 8u;
    flare_link(packet, slot);
    R32(0x800C7688u + frame * 4u) = 1u;
    candidate = x - 8u;
    if ((sint32)candidate < 0)
        candidate = 0u;
    if ((sint32)candidate > (sint32)(R32(0x8006511Cu) - 1u))
        candidate = R32(0x8006511Cu) - 1u;
    rectangle.x = (sint16)candidate;
    environment = R32(0x80065118u);
    candidate = y - 8u;
    if ((sint32)candidate < 0)
        candidate = 0u;
    if ((sint32)candidate > (sint32)(R32(0x80065120u) - 1u))
        candidate = R32(0x80065120u) - 1u;
    rectangle.y = (sint16)(candidate + flare_signed_half(environment + 10u));
    packet = 0x800C7420u + R32(0x80065128u) * 24u;
    flare_move(packet, &rectangle, 624u, 0u);
    flare_link(packet, R32(R32(0x80065118u) + 112u) + 8u);
    frame = R32(0x80065128u);
    packet = 0x800C73ACu + frame * 40u;
    R8(packet + 4u) = (uint8)flare_half(R32(0x800C7450u));
    R8(packet + 5u) = (uint8)flare_half(R32(0x800C7450u));
    R8(packet + 6u) = (uint8)flare_half(R32(0x800C7450u));
    flare_quad(packet, center_x, center_y, flare_signed_half(0x800C73A8u), flare_signed_half(0x800C73AAu));
    flare_link(packet, R32(R32(0x80065118u) + 112u) + 8u);
    if ((sint32)x < 0 || (sint32)x >= (sint32)R32(0x8006511Cu) || (sint32)y < 0 || (sint32)y >= (sint32)R32(0x80065120u))
        return;
    if ((sint32)distance < 64)
    {
        color = 192u - distance * 3u;
        R8(0x800C745Cu + R32(0x80065128u) * 24u) = (uint8)color;
        R8(0x800C745Du + R32(0x80065128u) * 24u) = (uint8)color;
        R8(0x800C745Eu + R32(0x80065128u) * 24u) = (uint8)color;
        frame = R32(0x80065128u);
        flare_link(0x800C7458u + frame * 24u, ordering_table);
        flare_link(0x800C7410u + frame * 8u, ordering_table);
    }
    color = flare_half(R32(0x800C7450u));
    for (index = 1u; index < 8u; ++index)
    {
        scale = flare_signed_half(0x800BF860u + index * 2u);
        frame = R32(0x80065128u);
        template_address = 0x800C7108u + index * 84u;
        packet = template_address + frame * 40u + 4u;
        center_x = flare_half(R32(0x8006511Cu)) + flare_shift(dx * scale, 8u);
        center_y = flare_half(R32(0x80065120u)) + flare_shift(dy * scale, 8u);
        fw = flare_signed_half(template_address);
        fh = flare_signed_half(template_address + 2u);
        wc = flare_half(fw * cosine);
        ws = flare_half(fw * sine);
        hc = flare_half(fh * cosine);
        hs = flare_half(fh * sine);
        R16(packet + 8u) = (uint16)(center_x + flare_shift(0u - wc + hs, 12u));
        R16(packet + 10u) = (uint16)(center_y + flare_shift(0u - ws - hc, 12u));
        R16(packet + 16u) = (uint16)(center_x + flare_shift(wc + hs, 12u));
        R16(packet + 18u) = (uint16)(center_y + flare_shift(ws - hc, 12u));
        R16(packet + 24u) = (uint16)(center_x + flare_shift(0u - wc - hs, 12u));
        R16(packet + 26u) = (uint16)(center_y + flare_shift(0u - ws + hc, 12u));
        R16(packet + 32u) = (uint16)(center_x + flare_shift(wc - hs, 12u));
        R16(packet + 34u) = (uint16)(center_y + flare_shift(ws + hc, 12u));
        R8(packet + 4u) = (uint8)color;
        R8(packet + 5u) = (uint8)color;
        R8(packet + 6u) = (uint8)color;
        flare_link(packet, ordering_table);
    }
}
