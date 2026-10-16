#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);

static uint32 contact_shift(uint32 value, uint32 shift)
{
    if ((sint32)value < 0)
        value += (1u << shift) - 1u;
    return (uint32)((sint32)value >> shift);
}

static uint32 contact_divide(uint32 numerator, uint32 denominator)
{
    if (!denominator)
    {
        draft_agent1_missing_diagnostic("GAME97C44 original DIV break7\n");
        abort();
    }
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
    {
        draft_agent1_missing_diagnostic("GAME97C44 original DIV break6\n");
        abort();
    }
    return (uint32)((sint32)numerator / (sint32)denominator);
}

void game_80097C44(uint32 object)
{
    uint32 mask = R8(object + 942u), index, point, count, sum, value, cosine, sine, velocity, pointer;
    sint32 bounded;
    switch (mask)
    {
        case 0u:
        case 1u:
        case 2u:
        case 4u:
        case 6u:
        case 8u:
        case 9u:
        case 15u:
            return;
        default:
            break;
    }
    R32(0x800C08F8u) = object + 656u;
    R32(0x800C08FCu) = object + 752u;
    R32(0x800C0904u) = 0u;
    R32(0x800C0918u) = 0u;
    R32(0x800C0908u) = 0u;
    do
    {
        index = R32(0x800C0904u);
        if (((mask >> (index & 31u)) & 1u) == 0u)
        {
            point = R32(0x800C08F8u);
            value = (uint32)(sint32)(sint16)R16(point + 2u);
            count = R32(0x800C0918u);
            sum = R32(0x800C0908u);
            R32(0x800C0918u) = count + 1u;
            R32(0x800C0908u) = sum + value;
        }
        point = R32(0x800C08F8u);
        R32(0x800C0904u) = index + 1u;
        R32(0x800C08F8u) = point + 8u;
    } while ((sint32)(index + 1u) < 4);
    sum = R32(0x800C0908u);
    count = R32(0x800C0918u);
    value = contact_divide(sum, count);
    value *= (uint32)(sint32)(sint16)R16(R32(0x800C08FCu) + 2u);
    R32(0x800C0908u) = contact_shift(value, 12u);
    R32(0x800C0900u) = object + 112u;
    cosine = (uint32)draft_agent3_missing_800437EC((uint32)(sint32)(sint16)R16(object + 42u));
    R32(0x800C0910u) = cosine;
    sine = (uint32)draft_agent3_missing_8004388C((uint32)(sint32)(sint16)R16(object + 42u));
    pointer = R32(0x800C0900u);
    velocity = R32(pointer);
    R32(0x800C0914u) = sine;
    cosine = R32(0x800C0910u);
    value = contact_shift(velocity, 8u) * cosine;
    velocity = R32(pointer + 8u);
    value -= contact_shift(velocity, 8u) * sine;
    value = contact_shift(value, 12u);
    R32(0x800C090Cu) = value;
    bounded = (sint32)value;
    if (bounded < -512)
        bounded = -512;
    else if (bounded > 512)
        bounded = 512;
    value = (uint32)bounded * R32(0x800C0908u);
    R32(0x800C090Cu) = (uint32)bounded;
    value = contact_shift(value, 6u);
    velocity = R32(object + 144u);
    R32(0x800C091Cu) = value;
    R32(object + 144u) = velocity - value;
}
