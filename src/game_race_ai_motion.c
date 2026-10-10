#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
extern uint32 sub_80016340(uint32 object);
extern uint32 sub_8001ED2C(uint32, uint32, uint32, sint32, uint32, uint32);
extern uint32 sub_8002C188(uint32 state, uint32 position);

static uint32 ai_motion_step(uint32 value, uint32 timestep)
{
    sint32 product = (sint32)(value * timestep);
    if (product < 0)
        product = (sint32)((uint32)product + 255u);
    return (uint32)(product >> 8);
}

/* Full GAME original with car index distinct from object address */
void game_8009C35C(uint32 carindex)
{
    uint32 object = 0x80066D14u + carindex * 952u, index, count = 0u, action;
    sint32 threshold;
    if (r_u8(object + 378u) & 1u)
        w_u16(object + 16u, 0u);
    w_u16(object + 906u, (uint16)(r_u16(object + 906u) + 1u));
    w_u8(object + 941u, 0u);
    threshold = -(sint32)(sint16)r_u16(r_u32(object + 4u) + 100u);
    for (index = 0u; index < 4u; ++index)
        if (threshold >= (sint32)r_u32(object + 840u + index * 4u))
            ++count;
    if (count)
        sub_8001ED2C((randPSX() & 1u) + 19u, ~0u, 4096u, 4096, object + 100u, 0u);
    action = r_u8(object + 940u);
    if (action == 0u)
    {
        game_80099F74(object);
        if (r_u8(object + 942u) == 15u)
            game_80097AA0(object);
        else
            game_8009BA10(object);
        game_8009B348(object);
        game_80091FDC(carindex, 0x8009A8E8u, 0x80064C40u);
    }
    else if (action == 1u)
    {
        game_80096CA0(object);
        w_u32(object + 124u, (uint32)(sint32)(sint16)r_u16(object + 40u) << 8);
        w_u32(object + 128u, (uint32)(sint32)(sint16)r_u16(object + 42u) << 8);
        w_u32(object + 132u, (uint32)(sint32)(sint16)r_u16(object + 44u) << 8);
        sub_80016340(object);
        game_80096E10(object);
        game_80091FDC(carindex, 0x80096D84u, 0x80064C48u);
    }
}

void game_8009CE54(uint32 carindex)
{
    uint32 object = 0x80066D14u + carindex * 952u, timestep;
    if (r_u8(object + 378u) & 1u)
        w_u16(object + 16u, 0u);
    w_u8(object + 941u, 0u);
    game_8009C58C(object);
    timestep = r_u32(0x800651C4u);
    w_u32(object + 128u, r_u32(object + 128u) + ai_motion_step(r_u32(object + 140u), timestep));
    w_u32(object + 100u, r_u32(object + 100u) + ai_motion_step(r_u32(object + 112u), timestep));
    w_u32(object + 108u, r_u32(object + 108u) + ai_motion_step(r_u32(object + 120u), timestep));
    w_u16(object + 40u, 0u);
    w_u16(object + 44u, 0u);
    w_u16(object + 752u, 0u);
    w_u16(object + 756u, 0u);
    w_u16(object + 754u, (uint16)-4096);
    w_u32(object + 172u, (uint32)((sint32)r_u32(object + 104u) >> 8));
    w_u16(object + 42u, (uint16)((sint32)r_u32(object + 128u) >> 8));
    w_u32(object + 168u, (uint32)((sint32)r_u32(object + 100u) >> 8));
    w_u32(object + 176u, (uint32)((sint32)r_u32(object + 108u) >> 8));
    sub_8002C188(object + 88u, object + 100u);
    sub_8003C960((sint16 *)psx_addr(object + 148u, 18u), (const sint16 *)psx_addr(object + 40u, 6u));
    game_8009141C(carindex);
}
