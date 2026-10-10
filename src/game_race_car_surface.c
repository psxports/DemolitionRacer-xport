#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
extern void game_800AF8C4(uint32 object, uint32 effect, uint32 mask);
extern uint32 dr_race_particle_slot(void);

uint32 game_800B0F50(uint32 wheel, uint32 owner, uint32 attachment, uint32 texture, uint32 intensity)
{
    uint32 combined = wheel + R32(0x8006525Cu) * 4u, record = 0x800C6BE8u + combined * 7u;
    uint32 pool, offset = owner * 952u, position = offset + attachment * 12u, i, component, current;
    R8(record + 1u) = (uint8)attachment;
    R8(record) = (uint8)owner;
    R8(record + 2u) = 1u;
    /* Original truncates an exhausted allocator result to 255 and still uses that index */
    R8(record + 3u) = (uint8)dr_race_particle_slot();
    i = R8(record + 3u);
    R8(record + 4u) = (uint8)texture;
    R8(record + 6u) = (uint8)intensity;
    R8(record + 5u) = (uint8)intensity;
    pool = R32(0x800C6BE4u);
    R32(pool + i * 148u + 116u) = R32(0x80066F54u + position);
    R32(pool + R8(record + 3u) * 148u + 120u) = R32(0x80066F58u + position);
    R32(pool + R8(record + 3u) * 148u + 124u) = R32(0x80066F5Cu + position);
    R8(pool + R8(record + 3u) * 148u + 112u) = 1u;
    pool = R32(0x800C6BE4u);
    R16(pool + R8(record + 3u) * 148u + 114u) = 0u;
    R8(pool + R8(record + 3u) * 148u + 113u) = (uint8)intensity;
    for (i = 0u; i < 2u; ++i)
        for (component = 0u; component < 3u; ++component)
        {
            uint32 selected = R8(record + 3u), base = R32(0x800C6BE4u), kind = R8(0x800670B8u + offset);
            uint8 color = R8(0x800BF7D9u + kind * 4u + component);
            R8(base + selected * 148u + i * 40u + 36u + component) = color;
        }
    i = R8(record + 3u);
    pool = R32(0x800C6BE4u);
    current = R32(0x8006525Cu);
    R32(pool + i * 148u + 136u) = 0xFFFFFFFEu;
    i = R8(record + 3u);
    wheel = (combined - current * 4u) & 255u;
    R32(pool + i * 148u + 132u) = 0xFFFFFFFEu;
    return wheel;
}

static uint32 surface_allocate(uint32 wheel, uint32 owner, uint32 texture)
{
    return game_800B0F50(wheel, owner, wheel, texture, 26u);
}

static int surface_half(uint32 kind)
{
    return kind == 2u || kind == 5u || kind == 6u || kind == 4u || kind == 9u;
}

static void surface_release(uint32 object, uint32 wheel)
{
    uint32 id = R8(object + 933u + wheel), record, linked, base;
    if (id == 255u)
        return;
    record = 0x800C6BEAu + ((id + R32(0x8006525Cu) * 4u) & 255u) * 7u;
    id = R8(record + 1u);
    R8(record) = 0u;
    base = R32(0x800C6BE4u);
    R8(base + id * 148u + 112u) = 0u;
    id = R8(record + 1u);
    base = R32(0x800C6BE4u);
    linked = R32(base + id * 148u + 136u);
    R32(base + linked * 148u + 132u) = 0xFFFFFFFFu;
    R8(object + 933u + wheel) = 255u;
}

static void surface_strength(uint32 object, uint32 strength, uint32 rear)
{
    uint32 wheel, begin = rear ? 2u : 0u, end = rear ? 4u : 2u, threshold = rear ? 10000u : 15000u;
    uint32 mask = rear ? 1u : 2u;
    for (wheel = begin; wheel < end; ++wheel)
    {
        uint32 blocked;
        if ((sint32)strength > (sint32)threshold)
        {
            if (R8(object + 940u) != 0u)
                goto release;
            if ((R8(object + 938u) & mask) != 0u)
                goto inactive;
            if ((R8(object + 942u) >> wheel) & 1u)
                goto inactive;
            if (rear && R8(0x800BF7D8u + R8(object + 932u) * 4u) == 2u)
                goto inactive;
            strength -= threshold;
            if (R8(object + 933u + wheel) == 255u)
                R8(object + 933u + wheel) = (uint8)surface_allocate(wheel, R8(object + 937u), rear ? 55u : 40u);
            R8(0x800C6BEDu + R8(object + 933u + wheel) * 7u) = (uint8)((sint32)strength / 2048);
            if (surface_half(R8(object + 932u)))
            {
                uint32 address = 0x800C6BEDu + R8(object + 933u + wheel) * 7u;
                R8(address) = (uint8)(R8(address) >> 1);
            }
            continue;
        }
    inactive:
        if (R8(object + 940u) != 0u)
            goto release;
        if ((R8(object + 938u) & mask) == 0u)
            goto release;
        blocked = (R8(object + 942u) >> wheel) & 1u;
        if (blocked)
            goto release;
        if (!rear)
            continue;
        if (R8(0x800BF7D8u + R8(object + 932u) * 4u) != 2u)
            continue;
    release:
        surface_release(object, wheel);
    }
}

void game_800B1804(uint32 object, uint32 strength)
{
    surface_strength(object, strength, 0u);
}

void game_800B1A70(uint32 object, uint32 strength)
{
    surface_strength(object, strength, 1u);
}

static void surface_flag(uint32 object, uint32 flags, uint32 rear)
{
    uint32 start = rear ? 2u : 0u, wheel, kind;
    sint32 random;
    if ((flags & (rear ? 1u : 2u)) == 0u)
        return;
    for (wheel = start; wheel < start + 2u; ++wheel)
        if (R8(object + 933u + wheel) == 255u)
            R8(object + 933u + wheel) = (uint8)surface_allocate(wheel, R8(object + 937u), 55u);
    random = (sint32)randPSX();
    if (random % 50 < (sint32)(sint16)R16(object + 908u) / 2)
    {
        uint32 selected = (randPSX() & 1u) + start, position = object + selected * 12u;
        /* Original computes a local position that is not passed to the effect callee */
        uint32 y = R32(position + 580u) + 30720u, x = R32(position + 576u), z = R32(position + 584u);
        kind = R8(object + 932u);
        if ((kind == 1u || kind == 7u || kind == 18u || kind == 22u || kind == 23u || kind == 20u) && R8(object + 937u) == 0u)
            game_800AF8C4(object, 2u, 3u);
    }
    for (wheel = start; wheel < start + 2u; ++wheel)
    {
        uint32 address = 0x800C6BEDu + R8(object + 933u + wheel) * 7u;
        R8(address) = (uint8)((sint32)(sint16)R16(object + 908u) / 4);
    }
    if (surface_half(R8(object + 932u)))
        for (wheel = start; wheel < start + 2u; ++wheel)
        {
            uint32 address = 0x800C6BEDu + R8(object + 933u + wheel) * 7u;
            R8(address) = (uint8)(R8(address) >> 1);
        }
}

void game_800B1F98(uint32 object, uint32 flags)
{
    surface_flag(object, flags, 0u);
}

void game_800B1D30(uint32 object, uint32 flags)
{
    surface_flag(object, flags, 1u);
}
