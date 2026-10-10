#include "draft_signatures.h"

void game_800A7FA8(uint32 packet, uint32 attributes)
{
    uint32 frame = r_u32(attributes) + (uint32)r_u8(attributes + 15u) * 32u;
    uint32 texture, u, v, width, height;
    if (r_u32(0x800C4200u) != (uint32)(sint32)(sint16)r_u16(frame + 10u))
    {
        texture = sub_80039ADC(frame);
        w_u32(frame + 12u, texture);
    }
    else
        texture = r_u32(frame + 12u);
    w_u8(packet + 3u, 9u);
    w_u8(packet + 7u, 0x2cu);
    w_u16(packet + 22u, (r_u16(texture + 8u) & 0xff9fu) | ((r_u8(frame + 31u) & 3u) << 5));
    w_u16(packet + 14u, r_u16(texture + 10u));
    u = r_u8(texture + 24u);
    v = r_u8(texture + 25u);
    width = r_u8(texture + 22u);
    height = r_u8(texture + 23u);
    w_u8(packet + 12u, u);
    w_u8(packet + 13u, v);
    w_u8(packet + 20u, u + width - 1u);
    w_u8(packet + 21u, v);
    w_u8(packet + 28u, u);
    w_u8(packet + 29u, v + height - 1u);
    w_u8(packet + 36u, u + width - 1u);
    w_u8(packet + 37u, v + height - 1u);
    w_u8(packet + 4u, r_u8(attributes + 4u));
    w_u8(packet + 5u, r_u8(attributes + 5u));
    w_u8(packet + 6u, r_u8(attributes + 6u));
    w_u8(packet + 7u, 0x2eu);
    w_u8(packet + 43u, r_u8(packet + 3u));
    w_u32(packet + 44u, r_u32(packet + 4u));
    w_u32(packet + 52u, r_u32(packet + 12u));
    w_u32(packet + 60u, r_u32(packet + 20u));
    w_u16(packet + 68u, r_u16(packet + 28u));
    w_u16(packet + 76u, r_u16(packet + 36u));
}

uint32 game_800A8848(uint32 template_address, const VECTOR *position, const SVECTOR *rotation, const SVECTOR *scale, uint32 flags)
{
    uint32 effect = r_u32(0x800C40ECu), next, active;
    if (effect == 0x800C40ECu)
        return 0u;
    next = r_u32(effect);
    w_u32(0x800C40ECu, next);
    w_u32(r_u32(effect) + 4u, 0x800C40ECu);
    active = r_u32(0x800C4174u);
    w_u32(effect + 4u, 0x800C4174u);
    w_u8(effect + 11u, 0u);
    w_u32(effect, active);
    w_u32(0x800C4208u, r_u32(0x800C4208u) + 1u);
    w_u32(0x800C4210u, r_u32(0x800C4210u) - 1u);
    w_u32(r_u32(0x800C4174u) + 4u, effect);
    w_u32(0x800C4174u, effect);
    if (!effect)
        return effect;
    w_u16(effect + 8u, flags);
    w_u8(effect + 10u, 1u);
    w_u32(effect + 92u, (uint32)position->vx);
    w_u32(effect + 96u, (uint32)position->vy);
    w_u32(effect + 100u, (uint32)position->vz);
    w_u16(effect + 104u, (uint16)rotation->vx);
    w_u16(effect + 106u, (uint16)rotation->vy);
    w_u16(effect + 108u, (uint16)rotation->vz);
    w_u16(effect + 110u, (uint16)scale->vx);
    w_u16(effect + 112u, (uint16)scale->vy);
    w_u16(effect + 114u, (uint16)scale->vz);
    w_u8(effect + 131u, 0u);
    w_u32(effect + 116u, template_address);
    w_u16(effect + 128u, 0u);
    w_u16(effect + 124u, (uint32)r_u8(template_address + 26u) << 4);
    w_u16(effect + 126u, (uint32)r_u8(template_address + 27u) << 4);
    w_u8(effect + 130u, r_u8(template_address + 24u));
    w_u32(effect + 120u, r_u32(template_address + 16u));
    game_800A7FA8(effect + 12u, effect + 116u);
    return effect;
}

void game_800AE510(void)
{
    SVECTOR rotation, scale;
    uint32 i, j, data, table;
    /* TODO: Original initial positions are undefined; native initialization uses zero */
    VECTOR positions[2] = {{0}, {0}};
    rotation.vx = (sint16)r_u16(0x8008BB80u);
    rotation.vy = (sint16)r_u16(0x8008BB82u);
    rotation.vz = (sint16)r_u16(0x8008BB84u);
    rotation.pad = (sint16)r_u16(0x8008BB86u);
    scale = rotation;
    w_u8(0x800C6AA8u, 0u);
    for (i = 0; i < 20u; ++i)
        w_u32(0x800C6AACu + i * 4u, (uint32)randPSX());
    for (i = 0; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        if (r_u8(0x80059A28u + i) != 2u)
        {
            w_u32(0x800C6B40u + i * 8u, game_800A8848(0x800BF5B8u, &positions[0], &rotation, &scale, 0u));
            w_u32(0x800C6B44u + i * 8u, game_800A8848(0x800BF5B8u, &positions[1], &rotation, &scale, 0u));
        }
    }
    for (i = 0; i < 16u; ++i)
        w_u8(0x800C6BCFu - i, 0u);
    data = r_u32(0x80066D2Cu);
    data += r_u32(data + 44u);
    table = r_u32(0x80065210u) * 8u;
    for (i = 0; i < 3u; ++i)
        w_u16(0x800C6BD0u + i * 2u, r_u16(data + r_u32(0x800BF77Cu + table) + i * 2u));
    for (i = 0; i < 3u; ++i)
        w_u16(0x800C6BD8u + i * 2u, r_u16(data + r_u32(0x800BF780u + table) + i * 2u));
    w_u32(0x800C6B3Cu, 0u);
    for (i = 0; i < 4u; ++i)
    {
        uint32 source = i & 1u ? 0x800BF178u : 0x800BF278u;
        uint32 value = i * 40u, speed = 5u - i;
        if (value < 10u)
            value = 10u;
        else if (value > 160u)
            value = 160u;
        for (j = 0; j < 8u; ++j)
        {
            uint32 target = 0x800C66A8u + i * 256u + j * 32u, k;
            for (k = 0; k < 32u; k += 4u)
                w_u32(target + k, r_u32(source + j * 32u + k));
            w_u8(target + 26u, 16u);
            w_u8(target + 27u, 16u);
            w_u8(target + 24u, speed);
            w_u8(target + 30u, value);
        }
    }
}
