#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
extern uint32 game_800A8848(uint32 template_address, const VECTOR *position, const SVECTOR *rotation, const SVECTOR *scale, uint32 flags);
extern void game_800A7FA8(uint32 packet, uint32 attributes);

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

/* Original A86AC allocation used only by the AEDDC following effects */
static uint32 drive_effect_spawn_follow(uint32 template_address, const SVECTOR *velocity, const SVECTOR *scale, uint32 lifetime)
{
    uint32 effect = r_u32(0x800C40ECu), next, active;
    if (effect == 0x800C40ECu)
        return 0;
    next = r_u32(effect);
    w_u32(0x800C40ECu, next);
    w_u32(r_u32(effect) + 4u, 0x800C40ECu);
    active = r_u32(0x800C4174u);
    w_u32(effect + 4u, 0x800C4174u);
    w_u8(effect + 11u, 0);
    w_u32(effect, active);
    w_u32(0x800C4208u, r_u32(0x800C4208u) + 1u);
    w_u32(0x800C4210u, r_u32(0x800C4210u) - 1u);
    w_u32(r_u32(0x800C4174u) + 4u, effect);
    w_u32(0x800C4174u, effect);
    if (!effect)
        return 0;
    w_u16(effect + 8u, lifetime);
    w_u8(effect + 10u, 6);
    /* AEDDC overwrites all three undefined position fields before their first use */
    w_u16(effect + 104u, (uint16)velocity->vx);
    w_u16(effect + 106u, (uint16)velocity->vy);
    w_u16(effect + 108u, (uint16)velocity->vz);
    w_u16(effect + 110u, (uint16)scale->vx);
    w_u16(effect + 112u, (uint16)scale->vy);
    w_u16(effect + 114u, (uint16)scale->vz);
    w_u8(effect + 131u, 0);
    w_u32(effect + 116u, template_address);
    w_u16(effect + 128u, 0);
    w_u16(effect + 124u, (uint32)r_u8(template_address + 26u) << 4);
    w_u16(effect + 126u, (uint32)r_u8(template_address + 27u) << 4);
    w_u8(effect + 130u, r_u8(template_address + 24u));
    w_u32(effect + 120u, r_u32(template_address + 16u));
    game_800A7FA8(effect + 12u, effect + 116u);
    return effect;
}

void game_800AEDDC(uint32 player)
{
    uint32 object = 0x80066D14u + player * 952u, index;
    if (!r_u8(object + 84u))
    {
        for (index = 0; index < 5; ++index)
        {
            uint32 effect = drive_effect_spawn_follow(0x800BF058u, (const SVECTOR *)psx_addr(0x800BF7C4u, 6u), (const SVECTOR *)psx_addr(0x800BF7BCu, 6u), 0x7FFFu);
            w_u32(object + 48u + index * 4u, effect);
            if (effect)
                w_u8(effect + 131u, index & 3u);
        }
        w_u8(object + 84u, 1);
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
