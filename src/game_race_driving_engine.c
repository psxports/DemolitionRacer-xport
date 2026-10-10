#include "draft_signatures.h"

static sint32 de_add(sint32 a, sint32 b)
{
    return (sint32)((uint32)a + (uint32)b);
}

static sint32 de_sub(sint32 a, sint32 b)
{
    return (sint32)((uint32)a - (uint32)b);
}

static sint32 de_mul(sint32 a, sint32 b)
{
    return (sint32)((uint32)a * (uint32)b);
}

static sint32 de_shift(sint32 value, uint32 shift)
{
    return de_add(value, value < 0 ? (sint32)((1u << shift) - 1u) : 0) >> shift;
}

/* Full SLUS 16404 engine speed update */
void game_80016404(uint32 object, uint32 model)
{
    uint32 gear = r_u8(object + 18u);
    sint32 target, rpm, step, value, maximum, throttle;
    if (gear == 1u)
    {
        maximum = r_s16(r_u32(object + 4u) + 114u);
        target = 400;
        if (!r_u8(object + 19u))
        {
            throttle = r_s16(object + 16u);
            if (throttle >= 0)
            {
                value = de_shift(de_mul(de_sub(maximum, 400), throttle), 8u);
                target = de_add(value < maximum ? value : maximum, 400);
            }
        }
        rpm = r_s16(object + 12u);
        step = de_shift(de_sub(target, rpm), 4u);
        if (rpm < target)
        {
            if (step >= 401)
                step = 400;
        }
        else if (step >= 201)
            step = 200;
        rpm = de_add(rpm, step);
    }
    else
    {
        value = (sint32)r_u32(object + 888u) >> 8;
        if (value < 0)
            value = (sint32)(0u - (uint32)value);
        value = de_mul(value, r_s16(model + 46u + gear * 2u));
        /* Original shift chain multiplies the low word by 45 */
        value = de_mul(value, 45);
        target = de_add(de_shift(value, 12u), 400);
        rpm = r_s16(object + 12u);
        value = de_sub(rpm, target);
        if (value >= 801)
            rpm = de_sub(rpm, 200);
        else if (value < -800)
            rpm = de_add(rpm, 200);
        else
            rpm = de_add(rpm, de_shift(de_sub(target, rpm), 2u));
        maximum = r_s16(r_u32(object + 4u) + 114u);
        if (maximum < rpm)
            rpm = maximum;
    }
    w_u16(object + 12u, (uint16)rpm);
}

/* Full SLUS 1675C torque curve interpolation */
sint32 game_8001675C(uint32 object, uint32 model)
{
    uint32 boosted = r_u8(object + 950u) & 1u;
    sint32 throttle = r_s16(object + 16u);
    uint32 gear = r_u8(object + 18u), curve;
    sint32 rpm = r_s16(object + 12u), index, scale, lower, upper, fraction, torque;
    if (boosted)
        throttle = (sint32)((uint32)throttle << 1);
    if (gear == 1u)
        return 0;
    index = de_shift(rpm, 9u);
    curve = model + ((uint32)index << 1);
    scale = r_s16(model + 104u);
    lower = de_shift(de_mul(r_s16(curve), scale), 8u);
    upper = de_shift(de_mul(r_s16(curve + 2u), scale), 8u);
    fraction = de_sub(rpm, (sint32)((uint32)index << 9));
    torque = de_add(lower, de_shift(de_mul(de_sub(upper, lower), fraction), 9u));
    torque = de_shift(de_mul(throttle, torque), 8u);
    torque = de_shift(de_mul(torque, r_s16(model + 46u + gear * 2u)), 8u);
    if (de_sub(r_s16(model + 114u), 50) < rpm)
        torque = 0;
    return torque;
}
