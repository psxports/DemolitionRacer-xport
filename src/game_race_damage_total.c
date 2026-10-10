#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>

extern void sub_8001E500(uint32 channel);
extern uint32 sub_8001ED2C(uint32 sound, uint32 voice, uint32 volume, sint32 frequency, uint32 position, uint32 velocity);
extern void sub_80030470(uint32 player, uint32 object, uint32 region, sint32 region_damage);

static void damage_total_callback(uint32 player)
{
    fprintf(stderr, "TODO GAME damage callback 800AE8FC(%08X)\n", player);
    abort();
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
    damage_total_callback(player);
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
            fprintf(stderr, "MIPS BREAK 7 in 800302F8\n");
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
