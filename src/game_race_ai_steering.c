#include "draft_signatures.h"
#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
extern sint32 draft_agent3_missing_8004388C(uint32 angle);

void game_800B59F0(uint32 cache, sint32 strength)
{
    uint32 object = 0x80066D14u + R8(cache + 97u) * 952u;
    uint32 speed = R32(object + 900u), amount = (uint32)strength, value;
    sint32 angle = (sint16)R16(cache + 52u);
    if ((sint32)speed > 0)
    {
        uint32 scaled = (uint32)((sint32)speed >> 4);
        if ((sint32)scaled >= 751)
            scaled = 750u;
        value = scaled * R32(0x800651C4u);
        if ((sint32)value < 0)
            value += 255u;
        scaled = (uint32)((sint32)value >> 8);
        if (angle < -8)
        {
            value = R32(object + 140u);
            if ((sint32)value > 0)
                R32(object + 140u) = value - scaled;
        }
        else if (angle >= 9)
        {
            value = R32(object + 140u);
            if ((sint32)value < 0)
                R32(object + 140u) = value + scaled;
        }
    }
    if (angle < 0)
    {
        angle = -angle;
        amount = 0u - amount;
    }
    if (angle < 256)
    {
        value = amount * (uint32)draft_agent3_missing_8004388C((uint32)angle);
        if ((sint32)value < 0)
            value += 4095u;
        R32(object + 8u) += (uint32)((sint32)value >> 12);
    }
    else
        R32(object + 8u) += amount;
    value = R32(object + 8u);
    if ((sint32)value > 98304)
        R32(object + 8u) = 98304u;
    else if ((sint32)value < -98304)
        R32(object + 8u) = (uint32)-98304;
}
