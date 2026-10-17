#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern uint32 sub_80043450(uint32 x, uint32 z);
extern uint32 dr_track_normal(const uint8 *state, const sint32 *position, sint16 *output);

static uint32 effect_abs(uint32 value)
{
    return (sint32)value < 0 ? 0u - value : value;
}

static uint32 effect_shift(uint32 value)
{
    if ((sint32)value < 0)
        value += 4095u;
    return (uint32)((sint32)value >> 12);
}

void game_800ADCE8(uint32 object, uint32 mask)
{
    uint32 sine, cosine, x, y, z;
    if ((sint32)effect_abs(r_u32(object + 24u)) < 5000 && (sint32)effect_abs(r_u32(object + 28u)) < 5000 && (sint32)effect_abs(r_u32(object + 32u)) < 5000)
    {
        sint16 normal[3], rotated[3];
        uint32 pitch, roll, first, second;
        w_u32(object + 28u, 0u);
        sine = (uint32)draft_agent3_missing_800437EC((uint32)(sint32)(sint16)r_u16(object + 98u));
        cosine = (uint32)draft_agent3_missing_8004388C((uint32)(sint32)(sint16)r_u16(object + 98u));
        if (!dr_track_normal((const uint8 *)psx_addr(object, 10u), (const sint32 *)psx_addr(object + 12u, 12u), normal))
        {
            /* TODO Original normal output is undefined when the track helper rejects the cell */
            draft_agent1_missing_diagnostic("TODO ADCE8 unresolved track normal object=%08X\n", object);
            abort();
        }
        x = (uint32)(sint32)normal[0];
        z = (uint32)(sint32)normal[2];
        rotated[0] = (sint16)effect_shift(x * sine - z * cosine);
        rotated[1] = normal[1];
        rotated[2] = (sint16)effect_shift(x * cosine + z * sine);
        pitch = sub_80043450(0u - (uint32)(sint32)rotated[2], 0u - (uint32)(sint32)rotated[1]);
        y = (uint32)(sint32)rotated[1];
        z = (uint32)(sint32)rotated[2];
        roll = sub_80043450((uint32)(sint32)rotated[0], dr_race_motion_sqrt(y * y + z * z));
        pitch = ((pitch - (uint32)(sint32)(sint16)r_u16(object + 96u) + 512u) & 1023u) - 512u;
        roll = ((roll - (uint32)(sint32)(sint16)r_u16(object + 100u) + 512u) & 1023u) - 512u;
        if (effect_abs(pitch) < 64u && effect_abs(roll) < 64u)
        {
            first = ((r_u32(object + 104u) + 2048u) & 4095u) - 1921u;
            second = ((r_u32(object + 112u) + 2048u) & 4095u) - 1921u;
            if (first < 255u && second < 255u)
            {
                w_u8(object + 297u, 0u);
                w_u32(object + 104u, 0u);
                w_u32(object + 108u, 0u);
                w_u32(object + 112u, 0u);
                w_u32(object + 24u, 0u);
                w_u32(object + 28u, 0u);
                w_u32(object + 32u, 0u);
                game_800AB0D8(object);
            }
        }
        else
        {
            w_u32(object + 104u, r_u32(object + 104u) + (pitch << 3));
            w_u32(object + 112u, r_u32(object + 112u) + (roll << 3));
        }
    }
    else if (!(r_u8(object + 296u) & 1u))
    {
        sine = (uint32)draft_agent3_missing_800437EC((uint32)(sint32)(sint16)r_u16(object + 98u));
        cosine = (uint32)draft_agent3_missing_8004388C((uint32)(sint32)(sint16)r_u16(object + 98u));
        x = r_u32(object + 24u);
        z = r_u32(object + 32u);
        w_u32(object + 112u, effect_shift(x * sine + z * cosine));
        w_u32(object + 104u, effect_shift((0u - x) * cosine + z * sine));
        y = r_u32(object + 28u) * 2u;
        w_u32(object + 28u, 0u - effect_shift(y * (uint32)(sint32)(sint16)r_u16(object + 288u)));
    }
    else
        w_u32(object + 28u, 0u);
}
