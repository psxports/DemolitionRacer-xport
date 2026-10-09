#include "draft_signatures.h"
#include <limits.h>

void game_800B52F0(uint32 ai_record)
{
    uint32 self = r_u8(ai_record + 97u), selected = self;
    uint32 object = 0x80066D14u + self * 952u;
    uint32 candidate = 0u, other_ai = 0x800C7698u, other_object = 0x80066D14u;
    sint32 nearest = 8, period, period_half, three_halves;
    if ((sint32)r_u32(0x80065254u) <= 0) {
        w_u8(ai_record + 94u, selected);
        return;
    }
    period = (sint32)r_u32(0x800653A4u);
    {
        uint32 triple = (uint32)period * 3u;
        three_halves = (sint32)(triple + (triple >> 31u)) >> 1;
        period_half = (sint32)((uint32)period + ((uint32)period >> 31u)) >> 1;
    }
    do {
        if (candidate != self && r_u8(ai_record + 96u) == r_u8(other_ai + 96u)) {
            sint32 numerator = (sint32)((uint32)(sint32)(sint16)r_u16(other_object + 92u)
                - (uint32)(sint32)(sint16)r_u16(object + 92u) + (uint32)three_halves);
            sint32 distance;
            if (!period) xport_mips_break(7u);
            if (numerator == INT_MIN && period == -1) xport_mips_break(6u);
            distance = (sint32)((uint32)(numerator % period) - (uint32)period_half);
            if (distance > 0 && distance < nearest) {
                selected = candidate;
                nearest = distance;
            }
        }
        other_object += 952u;
        ++candidate;
        other_ai += 104u;
    } while ((sint32)candidate < (sint32)r_u32(0x80065254u));
    w_u8(ai_record + 94u, selected);
}
