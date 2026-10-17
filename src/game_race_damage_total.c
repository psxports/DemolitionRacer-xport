#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void sub_8001E500(uint32 channel);
extern uint32 sub_8001ED2C(uint32 sound, uint32 voice, uint32 volume, sint32 frequency, uint32 position, uint32 velocity);
extern void sub_80030470(uint32 player, uint32 object, uint32 region, sint32 region_damage);
extern void game_800AF468(uint32 object, uint32 force);
extern uint32 game_800A8848(uint32 template_address, const VECTOR *position, const SVECTOR *rotation, const SVECTOR *scale, uint32 flags);

static void game_800AE8FC(uint32 player)
{
    uint32 object = 0x80066D14u + player * 952u;
    uint32 model = 0x800BF618u + r_u32(0x80065210u) * 40u;
    VECTOR position = {0, 0, 0, 0};
    SVECTOR rotation = {0, 210, 0, 0}, scale;
    rotation.vx = (sint16)((sint32)r_u32(object + 112u) >> 4);
    rotation.vz = (sint16)((sint32)r_u32(object + 120u) >> 4);
    memcpy(&scale, psx_addr(0x8008BB80u, 8u), 8u);
    position.vx = (sint32)((r_u32(object + 168u) + (uint32)(sint32)(sint16)r_u16(model)) << 8);
    position.vy = (sint32)((r_u32(object + 172u) + (uint32)(sint32)(sint16)r_u16(model + 2u)) << 8);
    position.vz = (sint32)((r_u32(object + 176u) + (uint32)(sint32)(sint16)r_u16(model + 4u)) << 8);
    w_u32(object + 116u, r_u32(object + 116u) + 36000u);
    w_u32(object + 136u, r_u32(object + 136u) - 25000u);
    game_800AF468(object, 1u);
    game_800A8848(0x800BF498u, &position, &rotation, &scale, 24u);
    rotation.vy = (sint16)((uint16)rotation.vy + 500u);
    game_800A8848(0x800BF598u, &position, &rotation, &scale, 24u);
    w_u8(0x800C6BC0u + player, 1u);
    w_u32(0x800C6AFCu + player * 4u, r_u32(0x80064A30u) + (r_u32(0x80064C14u) ? 3u : 7u));
}

uint32 sub_800302F8(uint32 player, uint32 object)
{
    uint32 previous = r_u8(object + 949u), count = r_u8(0x800653F8u), limit, threshold, flags, i, result;
    threshold = r_u16(object + 322u);
    w_u8(object + 949u, (uint8)(previous * 2u));
    limit = r_u32(0x800BEEA4u);
    flags = r_u8(object + 378u);
    w_u16(object + 320u, (uint16)threshold);
    w_u8(object + 378u, (uint8)(flags | 1u));
    w_u16(object + 910u, (uint16)limit);
    sub_8001ED2C(18u, 0xFFFFFFFFu, 6144u, (sint32)((uint32)randPSX() & 255u) + 3996, object + 100u, 0u);
    game_800AE8FC(player);
    for (i = 0; i < 13u; ++i)
    {
        w_u16(object + 338u + i * 2u, 1024u);
        sub_80030470(player, object, i, 1024);
    }
    result = 0xFFFFFFFFu;
    if ((sint32)r_u32(0x80064C14u) < (sint32)player && (sint8)r_u8(0x800653FCu) == -1)
    {
        uint32 divisor;
        result = sub_8001B884(player);
        divisor = (uint32)((sint32)(r_u32(0x80065254u) - count) >> 1);
        if (!divisor)
        {
            draft_agent1_missing_diagnostic("MIPS BREAK 7 in 800302F8\n");
            abort();
        }
        if (result % divisor == 0u)
        {
            uint32 channel;
            w_u8(0x800653FCu, (uint8)player);
            channel = sub_8001ED2C(33u, 0xFFFFFFFFu, 4096u, 4096, object + 100u, 0u);
            sub_8001E500(channel);
            /* The seven-word original 1E500 sets its result to one */
            result = 1u;
        }
    }
    w_u8(0x800653F8u, (uint8)(count + 1u));
    return result;
}
