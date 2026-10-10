#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
extern uint32 game_800A8848(uint32 template_address, const VECTOR *position, const SVECTOR *rotation, const SVECTOR *scale, uint32 flags);

void game_8009082C(uint32 owner, uint32 value)
{
    if ((sint32)owner < 2)
    {
        w_u32(0x800BFFA8u + owner * 4u, value);
        w_u32(0x800BFFB8u + owner * 4u, value);
    }
}

static sint16 drive_effect_velocity(uint32 word)
{
    uint32 value = (uint32)((sint32)word >> 4) * 3u;
    if ((sint32)value < 0)
        value += 3u;
    return (sint16)((sint32)value >> 2);
}

void game_800AEDDC(uint32 player)
{
    uint32 object = 0x80066D14u + player * 952u, index;
    if (!r_u8(object + 84u))
    {
        /* TODO Original A86AC reads an undefined initial position before the first projection */
        fprintf(stderr, "TODO AEDDC undefined original initial effect position player=%u\n", player);
        abort();
    }
    for (index = 0u; index < 5u; ++index)
    {
        uint32 effect = r_u32(object + 48u + index * 4u), vertex, i;
        VECTOR projected;
        SVECTOR velocity = {0}, scale = {0};
        if (!effect)
            continue;
        vertex = 0x800BF618u + r_u32(0x80065210u) * 40u + index * 8u;
        xport_gte_write_data(0u, r_u32(vertex));
        xport_gte_write_data(1u, r_u32(vertex + 4u));
        xport_gte_execute(0x480012u);
        projected.vx = (sint32)xport_gte_read_data(25u);
        projected.vy = (sint32)xport_gte_read_data(26u);
        projected.vz = (sint32)xport_gte_read_data(27u);
        projected.pad = 0;
        w_u32(effect + 92u, (uint32)projected.vx << 8);
        w_u32(effect + 96u, (uint32)projected.vy << 8);
        w_u32(effect + 100u, (uint32)projected.vz << 8);
        if ((uint32)randPSX() & 7u)
            continue;
        /* Original jitter local is computed but is not passed to the allocator */
        (void)((uint32)randPSX() & 127u);
        (void)((uint32)randPSX() & 127u);
        velocity.vx = drive_effect_velocity(r_u32(object + 112u));
        velocity.vy = (sint16)((uint16)drive_effect_velocity(r_u32(object + 116u)) + 800u);
        velocity.vz = drive_effect_velocity(r_u32(object + 120u));
        scale.vx = (sint16)(-(sint32)velocity.vx / 32);
        scale.vz = (sint16)(-(sint32)velocity.vz / 32);
        for (i = 0u; i < 3u; ++i)
        {
            uint32 word = r_u32(effect + 92u + i * 4u);
            if (i == 0u)
                projected.vx = (sint32)word;
            else if (i == 1u)
                projected.vy = (sint32)word;
            else
                projected.vz = (sint32)word;
        }
        game_800A8848(0x800BF0D8u, &projected, &velocity, &scale, 32u);
    }
}
