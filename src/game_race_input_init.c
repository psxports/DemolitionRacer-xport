#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>

void sub_8001E500(uint32 channel)
{
    w_u16(0x80073FBAu + channel * 32u, 1u);
}

uint32 sub_8001CBAC(void)
{
    uint32 i;
    for (i = 0; i < 13u; ++i) sub_8001E500(i);
    return 0u;
}

uint32 sub_8002B41C(uint32 key)
{
    static const uint16 masks[37] = {
        0x1000u, 0x4000u, 0x8000u, 0x2000u, 0x400u, 0x100u, 0u, 0x200u,
        0x10u, 0x40u, 0x80u, 0x20u, 0x10u, 0x40u, 0x80u, 0x20u,
        0x2000u, 0x1000u, 0x4000u, 0u, 0x400u, 0u, 0u, 0u,
        0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u, 0u};
    uint32 index = key - 170u;
    return index < 37u ? masks[index] : 0u;
}

void game_8008F9A8(uint32 port)
{
    static const uint32 source_offsets[8] = {0u, 10u, 20u, 30u, 40u, 60u, 50u, 70u};
    static const uint32 destinations[8] = {0x800BE808u, 0x800BE80Cu, 0x800BE814u, 0x800BE818u,
        0x800BE810u, 0x800BE81Cu, 0x800BE824u, 0x800BE820u};
    uint32 type = r_u32(0x80065538u + port * 4u), table, choice = 0u, i;
    if (type == 0x4000u) table = 0x8005A850u;
    else if (type == 0x2000u) table = 0x8005A8F0u;
    else if (type == 0x5000u || type == 0x7000u) table = 0x8005A940u;
    else return;
    if (type == r_u32(0x80065338u + port * 4u)) {
        uint32 configuration = 0x8005A990u + port * 36u;
        sint32 middle = (sint16)r_u16(configuration + 28u);
        sint32 lower = (sint16)r_u16(configuration + 30u);
        sint32 upper = (sint16)r_u16(configuration + 32u);
        sint32 extent = middle - lower, other = upper - middle;
        sint32 divisor = (sint16)r_u16(configuration + 20u) + 1;
        choice = (uint32)(sint32)(sint16)r_u16(configuration + 16u);
        w_u16(0x800BE828u + port * 2u, (uint16)middle);
        if (extent < other) extent = other;
        if (!divisor) {
            fprintf(stderr, "Original GAME 8008F9A8 division by zero\n");
            abort();
        }
        w_u16(0x800BE82Cu + port * 2u, extent / divisor > 0 ? (uint16)(extent / divisor) : 1u);
        w_u16(0x800BE830u + port * 2u, (uint32)(sint32)(sint16)r_u16(configuration + 18u) << 3);
    } else {
        w_u16(0x800BE828u + port * 2u, 128u);
        w_u16(0x800BE82Cu + port * 2u, 25u);
        w_u16(0x800BE830u + port * 2u, 32u);
    }
    table += choice * 2u;
    for (i = 0; i < 8u; ++i) {
        uint32 mapped = sub_8002B41C(r_u16(table + source_offsets[i]));
        w_u16(destinations[i] + port * 2u, mapped);
        if (i == 0u) w_u32(0x80065528u + port * 4u, (uint32)(sint32)(sint16)mapped);
        if (i == 1u) w_u32(0x80065530u + port * 4u, (uint32)(sint32)(sint16)mapped);
    }
}
