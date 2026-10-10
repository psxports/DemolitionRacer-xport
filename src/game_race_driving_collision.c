#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern uint32 sub_8001ED2C(uint32 sound, uint32 voice, uint32 volume, sint32 frequency, uint32 position, uint32 velocity);
extern void game_8009082C(uint32 owner, uint32 value);

static uint32 collision_shift(uint32 value, uint32 shift)
{
    if ((sint32)value < 0)
        value += (1u << shift) - 1u;
    return (uint32)((sint32)value >> shift);
}

static uint32 collision_scale(uint32 value)
{
    sint64 product = (sint64)(sint32)value * 0x0C907DA5LL;
    sint32 high = (sint32)((uint64)product >> 32);
    return (uint32)(high >> 5) - (uint32)((sint32)value >> 31);
}

static uint32 collision_divide(uint32 a, uint32 b)
{
    if (!b)
    {
        fprintf(stderr, "GAME91A6C original DIV break7\n");
        abort();
    }
    if (a == 0x80000000u && b == 0xFFFFFFFFu)
    {
        fprintf(stderr, "GAME91A6C original DIV break6\n");
        abort();
    }
    return (uint32)((sint32)a / (sint32)b);
}

static void collision_pending(uint32 target, uint32 a, uint32 b, uint32 c)
{
    fprintf(stderr, "TODO GAME %08X(%08X,%08X,%08X)\n", target, a, b, c);
    abort();
}

void game_80091A6C(uint32 object, uint32 position, uint32 angle, sint32 penetration, uint32 wheel)
{
    uint32 cosine = (uint32)draft_agent3_missing_800437EC(angle), sine = (uint32)draft_agent3_missing_8004388C(angle);
    uint32 depth = (uint32)penetration - 4u, dx = collision_shift((0u - depth) * sine, 4u), dz = collision_shift(depth * cosine, 4u);
    uint32 vx = R32(object + 112u), vz = R32(object + 120u), angular;
    uint32 longitudinal_raw, along, across, lever, approach, mass, owner, impulse, spin, normal_velocity, tangent, value;
    uint32 px = R32(object + 100u), pz = R32(object + 108u);
    angular = R32(object + 140u);
    R32(object + 100u) = px + dx;
    pz += dz;
    R32(object + 108u) = pz;
    longitudinal_raw = vx * cosine + vz * sine;
    if ((sint32)longitudinal_raw < 0)
        longitudinal_raw += 4095u;
    along = (uint32)((sint32)longitudinal_raw >> 12);
    across = collision_shift(vz * cosine - vx * sine, 12u);
    value = (uint32)((sint32)(R32(object + 100u) - R32(position)) >> 8) * cosine;
    value += (uint32)((sint32)(pz - R32(position + 8u)) >> 8) * sine;
    lever = collision_shift(value, 12u);
    approach = across + lever * collision_scale(angular);
    if ((sint32)approach < 0)
        return;
    mass = R32(0x800BE968u);
    owner = R8(object + 937u);
    value = collision_shift(mass, 8u);
    value = (value * 17u) << 8;
    value = 0u - collision_shift(value, 12u);
    impulse = collision_divide(value * approach, collision_shift(mass + lever * lever, 8u));
    spin = collision_divide(impulse * lever, collision_scale(mass));
    normal_velocity = across + impulse;
    spin += angular;
    if ((sint32)along > 0)
    {
        value = approach * 2u + (uint32)((sint32)longitudinal_raw >> 18) + 2048u;
        value = collision_shift(value * 3u * 128u, 11u);
        tangent = along - value;
        if ((sint32)tangent < 0)
            tangent = 0u;
    }
    else
    {
        value = approach * 2u - (collision_shift(along, 6u) - 2048u);
        tangent = along + collision_shift(value * 3u * 128u, 11u);
        if ((sint32)tangent > 0)
            tangent = 0u;
    }
    value = tangent * cosine - normal_velocity * sine;
    R32(object + 140u) = spin;
    R32(object + 112u) = collision_shift(value, 12u);
    R32(object + 120u) = collision_shift(normal_velocity * cosine + tangent * sine, 12u);
    if ((sint32)across > 1280)
    {
        if ((sint32)R32(0x80064C14u) >= (sint32)owner)
        {
            VECTOR local;
            local.vx = (wheel & 1u) ? -300 : 300;
            local.vy = 0;
            local.vz = (wheel & 2u) ? 300 : -300;
            local.pad = 0;
            if ((sint32)across > 51200)
            {
                uint32 volume = collision_shift(across, 2u), sound;
                if ((sint32)volume > 4096)
                    volume = 4096u;
                sound = (uint32)((sint32)randPSX() % 3 + 11);
                (void)sub_8001EA08(sound, -1, (sint32)volume, 4096, &local);
            }
            else
            {
                uint32 strength = across;
                if ((sint32)strength > 4096)
                    strength = 4096u;
                (void)sub_8001CF18(owner, (sint32)strength, &local);
            }
        }
        else if ((sint32)across > 12800)
        {
            uint32 volume = (uint32)(((sint32)across + (sint32)(across >> 31)) >> 1), sound;
            if ((sint32)volume > 4096)
                volume = 4096u;
            sound = (uint32)((sint32)randPSX() % 3 + 11);
            sub_8001ED2C(sound, 0xFFFFFFFFu, volume, 4096, object + 100u, 0u);
        }
    }
    value = along;
    if ((sint32)value < 0)
        value = 0u - value;
    if ((sint32)value >= 12801 || (sint32)across >= 12801)
    {
        uint32 current_owner = R8(object + 937u);
        if ((sint32)R32(0x80064C14u) >= (sint32)current_owner)
            game_8009082C(current_owner, 2048u);
    }
    if (R8(object + 306u) != 0u && R8(object + 309u) == 0u)
    {
        uint32 owner_index = R8(object + 307u);
        R8(object + 309u) = 1u;
        sub_8001532C(owner_index, 4u);
    }
    if ((sint32)impulse < 0)
        impulse = 0u - impulse;
    if ((sint32)impulse > 0)
        sub_80030110(owner, R8(0x800BE988u + wheel), (sint32)impulse >> 10);
}
