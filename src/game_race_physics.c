#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>

extern void game_8009A200(uint32 state);
extern uint32 sub_8001685C(uint32 state);
extern uint32 game_800B4B58(uint32 track);
extern void game_800B4BEC(uint32 cache);
extern uint32 game_800B5D68(uint32 track, uint32 position);
extern uint32 game_800B5B7C(uint32 order);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern void dr_race_normalize(VECTOR *input, SVECTOR *output);
extern uint32 sub_8001ED2C(uint32 sound, uint32 voice, uint32 mode, sint32 frequency, uint32 position, uint32 velocity);
uint32 game_800B752C(uint32 cache);
void game_800946D4(uint32 first, uint32 second);
static sint32 physics_multiply_divide(sint32 first, sint32 second, sint32 divisor);

uint32 sub_80016340(uint32 state)
{
    sint32 maximum = (sint16)r_u16(r_u32(state + 4u) + 114u), target = 400;
    sint32 current = (sint16)r_u16(state + 12u), step;
    if (!r_u8(state + 19u))
    {
        sint32 control = (sint16)r_u16(state + 16u);
        if (control >= 0)
        {
            sint32 scaled = physics_multiply_divide(maximum - 400, control, 256);
            target = (scaled < maximum ? scaled : maximum) + 400;
        }
    }
    step = (target - current) / 16;
    if (current < target)
    {
        if (step > 400)
            step = 400;
    }
    else if (step > 200)
        step = 200;
    current += step;
    w_u16(state + 12u, (uint16)current);
    return (uint32)current;
}

void game_8009ABC4(uint32 car)
{
    uint32 state = 0x80066D14u + car * 952u, model;
    sint32 threshold;
    sub_80016340(state);
    model = r_u32(state + 4u);
    w_u8(state + 938u, 0u);
    threshold = physics_multiply_divide((sint16)r_u16(model + 114u), 3, 4);
    if (threshold < (sint16)r_u16(state + 12u))
        w_u8(state + 938u, r_u16(model + 118u));
}

uint32 game_800B5688(uint32 car)
{
    uint32 state = 0x80066D14u + car * 952u, cache = 0x800C7698u + car * 104u;
    uint32 track = (uint32)(sint32)(sint16)r_u16(state + 90u);
    uint32 angle = (uint32)(((sint32)((uint32)r_u8(r_u32(cache) + track * 3u + 1u) * 4140u)) / 256);
    sint32 sine = draft_agent3_missing_8004388C(angle), cosine = draft_agent3_missing_800437EC(angle);
    uint32 x = (uint32)((sint32)r_u32(state + 112u) >> 8), z = (uint32)((sint32)r_u32(state + 120u) >> 8);
    return (uint32)((sint32)(x * (uint32)sine + z * (uint32)cosine) / 4096);
}

void game_8009AC50(uint32 car)
{
    uint32 state = 0x80066D14u + car * 952u, i, count = 0u, action;
    sint32 threshold;
    if (r_u8(state + 378u) & 1u)
        w_u16(state + 16u, 0u);
    w_u8(state + 941u, 0u);
    w_u16(state + 906u, r_u16(state + 906u) + 1u);
    threshold = -(sint32)(sint16)r_u16(r_u32(state + 4u) + 100u);
    for (i = 0; i < 4u; ++i)
        if (threshold >= (sint32)r_u32(state + 840u + i * 4u))
            ++count;
    if (count)
    {
        sub_8001ED2C(((uint32)randPSX() & 1u) + 19u, 0xffffffffu, 4096u, 4096, state + 100u, 0u);
        game_8009082C(r_u8(state + 937u), 2048u);
    }
    action = r_u8(state + 940u);
    if (!action)
    {
        game_80099F74(state);
        if (r_u8(state + 942u) == 15u)
            game_80097AA0(state);
        else
            game_80097EC4(state);
        game_8009A200(state);
        game_80091FDC(car, 0x8009A8E8u, 0x80064C40u);
    }
    else if (action == 1u)
    {
        game_80096CA0(state);
        w_u32(state + 124u, (uint32)(sint32)(sint16)r_u16(state + 40u) << 8);
        w_u32(state + 128u, (uint32)(sint32)(sint16)r_u16(state + 42u) << 8);
        w_u32(state + 132u, (uint32)(sint32)(sint16)r_u16(state + 44u) << 8);
        sub_80016340(state);
        game_80096E10(state);
        game_80091FDC(car, 0x80096D84u, 0x80064C48u);
    }
}

static sint32 physics_multiply_divide(sint32 first, sint32 second, sint32 divisor)
{
    return (sint32)((uint32)first * (uint32)second) / divisor;
}

void game_800B4DC0(void)
{
    uint32 count = r_u32(0x80065254u), i, changed;
    sint32 limit;
    for (i = 0; (sint32)i < (sint32)count; ++i)
    {
        uint32 state = (uint32)(sint32)(sint16)r_u16(0x800C7D18u + i * 4u);
        w_u16(0x800C7D1Au + i * 4u, r_u16(0x80066D6Eu + state * 952u));
    }
    limit = (sint32)(r_u32(0x80065254u) - 1u);
    do
    {
        changed = 0u;
        for (i = 0; (sint32)i < limit; ++i)
        {
            uint32 left = 0x800C7D18u + i * 4u, right = left + 4u;
            if ((sint16)r_u16(right + 2u) < (sint16)r_u16(left + 2u))
            {
                uint32 value = r_u32(right), previous = r_u32(left);
                w_u32(right, previous);
                w_u32(left, value);
                changed = 1u;
            }
        }
        --limit;
    } while (changed);
    for (i = 0; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        uint32 state = (uint32)(sint32)(sint16)r_u16(0x800C7D18u + i * 4u);
        w_u8(0x800C76EEu + state * 104u, i);
    }
}

void game_800B7628(void)
{
    sint32 leader = (sint16)r_u16(0x80066D70u);
    uint32 i;
    for (i = 0; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        uint32 cache = 0x800C7698u + i * 104u;
        sint32 delta, factor, timer;
        if (!r_u8(0x80059A28u + i))
            continue;
        delta = (sint16)r_u16(0x80066D70u + i * 952u) - leader;
        if (delta < -32)
            delta = -32;
        else if (delta > 32)
            delta = 32;
        factor = 256 - physics_multiply_divide(delta, r_u8(cache + (delta < 0 ? 92u : 93u)), 32);
        w_u16(cache + 20u, physics_multiply_divide((sint16)r_u16(0x800BFA20u + (uint32)r_u8(cache + 89u) * 2u), factor, 256));
        w_u16(cache + 58u, physics_multiply_divide((sint16)r_u16(0x800BFA0Cu + (uint32)r_u8(cache + 89u) * 2u), factor, 256));
        timer = (sint16)r_u16(cache + 84u);
        if (timer > 0)
        {
            w_u16(cache + 20u, physics_multiply_divide(256 - timer, (sint16)r_u16(cache + 20u), 256));
            w_u16(cache + 58u, physics_multiply_divide(256 - timer, (sint16)r_u16(cache + 58u), 256));
            w_u16(cache + 84u, r_u16(cache + 84u) - 1u);
        }
    }
}

void game_8009C508(uint32 state)
{
    uint32 i;
    w_u8(state + 938u, 0u);
    w_u8(state + 940u, 0u);
    w_u16(state + 906u, 0u);
    w_u8(state + 942u, 0u);
    w_u32(state + 104u, 0xc0000000u);
    for (i = 0; i < 4u; ++i)
    {
        w_u32(state + 856u + i * 4u, 0u);
        w_u32(state + 840u + i * 4u, 0u);
    }
    w_u32(state + 116u, 0u);
    game_8009A200(state);
    w_u32(state + 116u, 0u);
    for (i = 0; i < 4u; ++i)
        w_u32(state + 884u - i * 4u, 0u);
}

static sint32 physics_track_distance(sint32 first, sint32 second)
{
    sint32 length = (sint32)r_u32(0x800653A4u);
    sint32 numerator = (sint32)((uint32)first - (uint32)second + (uint32)((sint32)((uint32)length * 3u) / 2));
    sint32 value;
    if (!length || (length == -1 && numerator == (-2147483647 - 1)))
        abort();
    value = numerator % length - length / 2;
    return value < 0 ? -value : value;
}

void game_800B47FC(void)
{
    uint32 split = r_u32(0x800593A8u), i = split ? 2u : 1u;
    sint32 first = (sint16)r_u16(0x80066D6Eu), second = (sint16)r_u16(0x80067126u);
    for (; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        uint32 state = 0x80066D14u + i * 952u;
        sint32 track = (sint16)r_u16(state + 90u), distance;
        if (!split)
            distance = physics_track_distance(first, track);
        else
        {
            sint32 left = physics_track_distance(first, track), right = physics_track_distance(second, track);
            distance = physics_track_distance(left < right ? first : second, track);
        }
        w_u16(state + 930u, (uint16)distance);
        if (distance < 30)
        {
            if (r_u8(state + 948u))
            {
                w_u8(state + 948u, 0u);
                game_8009C508(state);
            }
        }
        else if (distance >= 32 && r_u8(state + 948u) != 1u)
            w_u8(state + 948u, 1u);
    }
}

static uint32 physics_track_length(uint32 state)
{
    uint32 track = (uint32)(sint32)(sint16)r_u16(state + 88u);
    uint32 record = r_u32(0x800653D4u) + track * 24u;
    uint32 index = r_u16(record + 6u), vertices = r_u32(0x800653C4u);
    uint32 other = index + (r_u8(record + 3u) & 15u) + r_u32(0x800BF99Cu + (uint32)r_u8(record) * 8u);
    uint32 first = vertices + index * 6u, second = vertices + other * 6u;
    uint32 dx = (uint32)(sint32)(sint16)r_u16(second) - (uint32)(sint32)(sint16)r_u16(first);
    uint32 dz = (uint32)(sint32)(sint16)r_u16(second + 4u) - (uint32)(sint32)(sint16)r_u16(first + 4u);
    return sub_8003B674(dx * dx + dz * dz);
}

void game_800B6F94(uint32 car)
{
    uint32 state = 0x80066D14u + car * 952u, cache = 0x800C7698u + car * 104u;
    uint32 i, track;
    sint32 minimum, maximum;
    for (i = 0; i < 4u; ++i)
        w_u16(cache + 28u + i * 2u, game_800B5D68((uint32)(sint32)(sint16)r_u16(state + 88u), state + 480u + i * 12u));
    minimum = maximum = (sint16)r_u16(cache + 28u);
    for (i = 1; i < 4u; ++i)
    {
        sint32 value = (sint16)r_u16(cache + 28u + i * 2u);
        if (value < minimum)
            minimum = value;
        else if (value > maximum)
            maximum = value;
    }
    w_u16(cache + 22u, (uint16)minimum);
    w_u16(cache + 24u, (uint16)maximum);
    track = (uint32)(sint32)(sint16)r_u16(state + 90u);
    w_u16(cache + 40u, physics_multiply_divide((sint32)physics_track_length(state), (sint16)r_u16(cache + 22u) - r_u8(r_u32(0x800C7690u) + track * 3u), 256));
    w_u16(cache + 42u, physics_multiply_divide((sint32)physics_track_length(state), (sint16)r_u16(cache + 24u) - r_u8(r_u32(0x800C7690u) + track * 3u), 256));
    w_u16(cache + 44u, physics_multiply_divide((sint32)physics_track_length(state), (sint16)r_u16(cache + 22u) - r_u8(r_u32(0x800C7694u) + track * 3u), 256));
    w_u16(cache + 46u, physics_multiply_divide((sint32)physics_track_length(state), (sint16)r_u16(cache + 24u) - r_u8(r_u32(0x800C7694u) + track * 3u), 256));
}

void dr_physics_offset_position(uint32 track, uint32 lateral, VECTOR *output, uint32 distance)
{
    uint32 record = r_u32(0x800653D4u) + track * 24u, vertices = r_u32(0x800653C4u);
    uint32 index = r_u16(record + 4u);
    uint32 second_index = index + (r_u8(record + 3u) & 15u) + r_u32(0x800BF998u + (uint32)r_u8(record) * 8u);
    uint32 first = vertices + index * 6u, second = vertices + second_index * 6u;
    uint32 x = r_u32(record + 12u) + (uint32)(sint32)(sint16)r_u16(first);
    uint32 z = r_u32(record + 20u) + (uint32)(sint32)(sint16)r_u16(first + 4u);
    uint32 dx = r_u32(record + 12u) + (uint32)(sint32)(sint16)r_u16(second) - x;
    uint32 dz = r_u32(record + 20u) + (uint32)(sint32)(sint16)r_u16(second + 4u) - z;
    VECTOR direction = {0};
    SVECTOR normal = {0};
    output->vx = (sint32)((x << 8) + lateral * dx);
    output->vz = (sint32)((z << 8) + lateral * dz);
    direction.vx = (sint16)dx;
    direction.vz = (sint16)dz;
    dr_race_normalize(&direction, &normal);
    output->vx = (sint32)((uint32)output->vx + ((uint32)physics_multiply_divide(normal.vx, (sint32)distance, 4096) << 8));
    output->vz = (sint32)((uint32)output->vz + ((uint32)physics_multiply_divide(normal.vz, (sint32)distance, 4096) << 8));
}

static sint32 physics_native_progress(uint32 track, const VECTOR *position)
{
    /* TODO: Consolidate the native input ABI with the existing guest B5D68 wrapper */
    uint32 record, vertices, index, first, second, x, z, dx, dz, dot;
    sint32 denominator;
    if (r_u32(0x800593A0u) == 1u)
        return 0;
    record = r_u32(0x800653D4u) + track * 24u;
    vertices = r_u32(0x800653C4u);
    index = r_u16(record + 6u);
    first = vertices + index * 6u;
    second = vertices + (index + (r_u8(record + 3u) & 15u) + r_u32(0x800BF99Cu + (uint32)r_u8(record) * 8u)) * 6u;
    x = (uint32)(sint32)(sint16)r_u16(first);
    z = (uint32)(sint32)(sint16)r_u16(first + 4u);
    dx = (uint32)(sint32)(sint16)r_u16(second) - x;
    dz = (uint32)(sint32)(sint16)r_u16(second + 4u) - z;
    dot = ((uint32)(position->vx >> 8) - (r_u32(record + 12u) + x)) * dx;
    dot += ((uint32)(position->vz >> 8) - (r_u32(record + 20u) + z)) * dz;
    denominator = (sint32)(dx * dx + dz * dz) / 256;
    if (!denominator || (denominator == -1 && (sint32)dot == (-2147483647 - 1)))
        abort();
    return (sint32)dot / denominator;
}

uint32 game_800B752C(uint32 cache)
{
    VECTOR position = {0};
    uint32 car = r_u8(cache + 97u), model = r_u32(0x80066D14u + car * 952u);
    uint32 track = (uint32)(sint32)(sint16)r_u16(cache + 62u);
    uint32 lateral = r_u8(r_u32(cache) + (uint32)(sint32)(sint16)r_u16(cache + 64u) * 3u);
    sint32 speed = (sint16)r_u16(cache + 26u);
    dr_physics_offset_position(track, lateral, &position, (uint32)(speed + (sint16)r_u16(model + 8u)));
    if (physics_native_progress((uint32)(sint32)(sint16)r_u16(cache + 62u), &position) >= 251)
        return 1u;
    car = r_u8(cache + 97u);
    model = r_u32(0x80066D14u + car * 952u);
    track = (uint32)(sint32)(sint16)r_u16(cache + 62u);
    lateral = r_u8(r_u32(cache) + (uint32)(sint32)(sint16)r_u16(cache + 64u) * 3u);
    dr_physics_offset_position(track, lateral, &position, (uint32)(speed + (sint16)r_u16(model)));
    return physics_native_progress((uint32)(sint32)(sint16)r_u16(cache + 62u), &position) < 5 ? 2u : 0u;
}

void game_800B77CC(void)
{
    uint32 i, j;
    game_800B4DC0();
    game_800B7628();
    game_800B47FC();
    for (i = 0; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        uint32 state = 0x80066D14u + i * 952u, cache = 0x800C7698u + i * 104u;
        uint32 track = (uint32)(sint32)(sint16)r_u16(state + 90u), mode;
        sint32 factor;
        if ((sint32)r_u32(0x800653A4u) < (sint16)r_u16(state + 88u))
        {
            w_u8(cache + 96u, 1u);
            w_u32(cache, r_u32(0x800C7694u));
        }
        else if (game_800B4B58(track) != 0xffffffffu)
        {
            w_u8(cache + 96u, 0u);
            w_u32(cache, r_u32(0x800C7690u));
        }
        else
            w_u8(cache + 96u, 0u);
        game_800B4BEC(cache);
        for (j = 0; j < 8u; ++j)
        {
            uint32 entry = r_u32(0x800651E4u) + (uint32)((sint32)(r_u32(0x80065254u) - i) / 4) * 32u + j * 4u;
            sint32 event = (sint16)r_u16(entry + 72u);
            if (event == -1)
                break;
            if ((sint32)track == event)
            {
                w_u8(cache + 96u, r_u16(entry + 74u) ? 1u : 0u);
                w_u32(cache, r_u32(r_u16(entry + 74u) ? 0x800C7694u : 0x800C7690u));
                game_800B5B7C(i);
            }
        }
        w_u16(cache + 54u, game_800B5688(i));
        w_u16(cache + 56u, game_800B5D68((uint32)(sint32)(sint16)r_u16(state + 88u), state + 100u));
        game_800B6F94(i);
        if ((sint32)sub_8001685C(state + 88u) >= 16)
            w_u16(cache + 26u, 0u);
        if (game_800B752C(cache) || r_u8(state + 941u))
            w_u16(cache + 26u, physics_multiply_divide((sint16)r_u16(cache + 26u), 3, 4));
        mode = r_u32(cache);
        if (mode == r_u32(0x800C7690u))
        {
            w_u16(cache + 36u, r_u16(cache + 26u));
            factor = (sint16)r_u16(cache + 56u) - r_u8(r_u32(0x800C7694u) + track * 3u);
            w_u16(cache + 38u, physics_multiply_divide((sint32)physics_track_length(state), factor, 256));
            w_u16(cache + 50u, r_u16(cache + 40u));
            w_u16(cache + 48u, r_u16(cache + 42u));
        }
        else
        {
            w_u16(cache + 38u, r_u16(cache + 26u));
            factor = (sint16)r_u16(cache + 56u) - r_u8(r_u32(0x800C7690u) + track * 3u);
            w_u16(cache + 36u, physics_multiply_divide((sint32)physics_track_length(state), factor, 256));
            w_u16(cache + 50u, r_u16(cache + 44u));
            w_u16(cache + 48u, r_u16(cache + 46u));
        }
    }
    for (i = 0; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        uint32 state = 0x80066D14u + i * 952u, cache = 0x800C7698u + i * 104u;
        uint32 type;
        if (!r_u32(0x800BEEA8u))
        {
            game_8009ABC4(i);
            continue;
        }
        type = r_u8(0x80059A28u + i);
        if (!type)
        {
            if (r_u8(state + 945u))
            {
                if (r_u32(0x80059738u) == 2u)
                {
                    w_u8(state + 19u, 1u);
                    w_u8(state + 20u, 0u);
                    w_u16(state + 16u, 0xff00u);
                    w_u32(state + 8u, 0x18000u);
                }
                else
                    w_u8(0x80059A28u + i, 1u);
            }
            game_8009AC50(i);
        }
        else if (type < 3u)
        {
            uint32 action = r_u8(cache + 95u);
            if (!action)
                game_800B6498(i);
            else if (action == 1u)
                game_800B6294(i);
            else if (action == 2u)
                game_800B68B0(i);
            else if (action == 5u)
                game_800B6C50(i);
            game_800B59F0(cache, 0x20000);
            if (r_u8(state + 948u))
                game_8009CE54(i);
            else
                game_8009C35C(i);
        }
    }
}

static void physics_bounds(uint32 descriptor, uint32 position, sint32 radius)
{
    uint32 x = (uint32)((sint32)r_u32(position) >> 8), z = (uint32)((sint32)r_u32(position + 8u) >> 8);
    w_u32(descriptor, x - (uint32)radius);
    w_u32(descriptor + 4u, z - (uint32)radius);
    w_u32(descriptor + 8u, x + (uint32)radius);
    w_u32(descriptor + 12u, z + (uint32)radius);
}

void game_8009564C(void)
{
    uint32 i, j;
    for (i = 0; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        uint32 state = 0x80066D14u + i * 952u, descriptor = 0x800C03D0u + i * 20u;
        uint32 bucket = (uint32)((sint32)(sint16)r_u16(state + 90u) >> 2) + 1u;
        physics_bounds(descriptor, state + 100u, (sint16)r_u16(r_u32(state) + 128u));
        w_u8(descriptor + 16u, r_u8(0x800BFFC0u + bucket));
        w_u8(0x800BFFC0u + bucket, i);
        if (r_u8(state + 306u))
            game_80093F94(state);
        w_u8(state + 946u, 255u);
    }
    for (i = 0; i < 30u; ++i)
    {
        uint32 item = 0x800C4378u + i * 300u;
        if (r_u8(item + 295u) && !r_u8(item + 299u))
        {
            uint32 index = i + r_u32(0x800C08D0u), descriptor = 0x800C03D0u + index * 20u;
            uint32 bucket = (uint32)((sint32)(sint16)r_u16(item + 2u) >> 2) + 1u;
            physics_bounds(descriptor, r_u32(item + 120u) + 16u, (sint32)r_u32(item + 116u));
            w_u8(descriptor + 16u, r_u8(0x800BFFC0u + bucket));
            w_u8(0x800BFFC0u + bucket, index);
        }
    }
    w_u32(0x800C03C0u, 0u);
    for (i = 0; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        uint32 state = 0x80066D14u + i * 952u;
        sint32 bucket = (sint16)r_u16(state + 90u) >> 2;
        for (j = 0; j < 3u; ++j, ++bucket)
        {
            uint32 other = r_u8(0x800BFFC0u + (uint32)bucket), visited = 0u;
            while (other != 255u && visited < 17u)
            {
                if (i < other && (sint32)other < (sint32)r_u32(0x80065254u))
                {
                    uint32 other_state = 0x80066D14u + other * 952u;
                    uint32 fast = !r_u8(state + 940u) && r_u8(state + 942u) < 15u && !r_u8(other_state + 940u) && r_u8(other_state + 942u) < 15u;
                    if (fast)
                        game_800946D4(i, other);
                    else
                        game_80095054(i, other);
                }
                if (!r_u8(0x800C03E1u + i * 20u) && r_u8(0x800C03E1u + other * 20u) == 1u && i < other)
                    game_800AD59C(i, other);
                w_u32(0x800C03C0u, r_u32(0x800C03C0u) + 1u);
                ++visited;
                other = r_u8(0x800C03E0u + other * 20u);
            }
        }
    }
    for (i = 0; (sint32)i < (sint32)r_u32(0x80065254u); ++i)
    {
        uint32 bucket = (uint32)((sint32)(sint16)r_u16(0x80066D6Eu + i * 952u) >> 2);
        w_u8(0x800BFFC1u + bucket, 255u);
        w_u8(0x800C03E0u + i * 20u, 255u);
    }
    for (i = 0; i < 30u; ++i)
    {
        uint32 bucket = (uint32)((sint32)(sint16)r_u16(0x800C4378u + i * 300u) >> 2);
        w_u8(0x800BFFC1u + bucket, 255u);
        w_u8(0x800C03E0u + i * 20u, 255u);
    }
}

/* Native packed position pairs preserve the original three-word stride */
typedef struct PhysicsPosition
{
    sint32 x, y, z;
} PhysicsPosition;

typedef struct PhysicsContact
{
    sint16 x, z, offset_x, offset_z;
} PhysicsContact;

static sint32 collision_add(sint32 a, sint32 b)
{
    return (sint32)((uint32)a + (uint32)b);
}

static sint32 collision_sub(sint32 a, sint32 b)
{
    return (sint32)((uint32)a - (uint32)b);
}

static sint32 collision_mul(sint32 a, sint32 b)
{
    return (sint32)((uint32)a * (uint32)b);
}

static uint32 physics_collision_geometry(uint32 first, uint32 second, const PhysicsPosition positions[2], const sint32 angles[2], PhysicsContact contacts[8])
{
    sint32 relative = (sint16)((uint32)collision_sub(angles[0], angles[1]) >> 8);
    sint32 cosine = draft_agent3_missing_800437EC((uint32)relative);
    sint32 sine = draft_agent3_missing_8004388C((uint32)relative);
    sint32 dx = collision_sub(positions[1].x, positions[0].x) >> 8;
    sint32 dz = collision_sub(positions[1].z, positions[0].z) >> 8;
    uint32 states[2] = {first, second}, mask = 0, side, corner;
    for (side = 0; side < 2; ++side)
    {
        uint32 own = r_u32(states[side]), other = r_u32(states[1u - side]);
        sint32 width = (sint16)r_u16(other + 8u), front = (sint16)r_u16(other + 4u);
        sint32 back = -(sint16)r_u16(other + 20u);
        sint32 cw = collision_mul(cosine, width) >> 12;
        sint32 sw = collision_mul(sine, width) >> 12;
        sint32 cf = collision_mul(cosine, front) >> 12;
        sint32 sf = collision_mul(sine, front) >> 12;
        sint32 cb = collision_mul(cosine, back) >> 12;
        sint32 sb = collision_mul(sine, back) >> 12;
        sint32 own_width = (sint16)r_u16(own + 8u), own_front = (sint16)r_u16(own + 4u);
        sint32 own_rear = (sint16)r_u16(own + 20u);
        sint32 rotation = angles[side] >> 8;
        sint32 c = draft_agent3_missing_800437EC((uint32)rotation);
        sint32 sn = draft_agent3_missing_8004388C((uint32)rotation);
        sint32 center_x, center_z, offsets_x[4], offsets_z[4];
        if (side == 0)
        {
            center_x = collision_sub(collision_mul(dx, c), collision_mul(dz, sn)) >> 12;
            center_z = collision_add(collision_mul(dx, sn), collision_mul(dz, c)) >> 12;
            offsets_x[0] = collision_sub(-cw, sf);
            offsets_x[1] = collision_sub(cw, sf);
            offsets_x[2] = collision_sub(sb, cw);
            offsets_x[3] = collision_add(cw, sb);
            offsets_z[0] = collision_sub(cf, sw);
            offsets_z[1] = collision_add(sw, cf);
            offsets_z[2] = collision_sub(-sw, cb);
            offsets_z[3] = collision_sub(sw, cb);
        }
        else
        {
            center_x = collision_add(collision_mul(-dx, c), collision_mul(dz, sn)) >> 12;
            center_z = collision_sub(collision_mul(-dx, sn), collision_mul(dz, c)) >> 12;
            sint32 negative_sw = collision_mul(-sine, width) >> 12;
            sint32 negative_sf = collision_mul(-sine, front) >> 12;
            sint32 negative_sb = collision_mul(-sine, back) >> 12;
            offsets_x[0] = collision_sub(-cw, negative_sf);
            offsets_x[1] = collision_sub(cw, negative_sf);
            offsets_x[2] = collision_sub(negative_sb, cw);
            offsets_x[3] = collision_add(cw, negative_sb);
            offsets_z[0] = collision_sub(cf, negative_sw);
            offsets_z[1] = collision_add(cf, negative_sw);
            offsets_z[2] = collision_sub(-negative_sw, cb);
            offsets_z[3] = collision_sub(negative_sw, cb);
        }
        for (corner = 0; corner < 4; ++corner)
        {
            sint32 x = collision_add(center_x, offsets_x[corner]);
            sint32 z = collision_add(center_z, offsets_z[corner]);
            if (x <= own_width && x >= -own_width && z <= own_front && z >= own_rear)
            {
                uint32 index = side * 4u + corner;
                mask |= 1u << index;
                contacts[index].x = (sint16)x;
                contacts[index].z = (sint16)z;
                contacts[index].offset_x = (sint16)offsets_x[corner];
                contacts[index].offset_z = (sint16)offsets_z[corner];
            }
        }
    }
    return mask;
}

static void physics_collision_response(uint32 first, uint32 second, const PhysicsContact *contact, sint32 angle, sint32 fraction, uint32 corner)
{
    sint16 components[4] = {contact->x, contact->z, contact->offset_x, contact->offset_z};
    game_80092450(first, second, components, angle, fraction, corner);
}

void game_800946D4(uint32 first, uint32 second)
{
    uint32 bounds_a = 0x800C03D0u + first * 20u, bounds_b = 0x800C03D0u + second * 20u;
    uint32 states[2] = {0x80066D14u + first * 952u, 0x80066D14u + second * 952u}, side, iteration, mask, selected;
    PhysicsPosition positions[2];
    PhysicsContact contacts[8];
    sint32 angles[2], delta[2][3];
    sint32 fraction = 128, step = 128;
    if ((sint32)r_u32(bounds_a) >= (sint32)r_u32(bounds_b + 8u) || (sint32)r_u32(bounds_b) >= (sint32)r_u32(bounds_a + 8u) || (sint32)r_u32(bounds_a + 4u) >= (sint32)r_u32(bounds_b + 12u) || (sint32)r_u32(bounds_b + 4u) >= (sint32)r_u32(bounds_a + 12u))
        return;
    for (side = 0; side < 2; ++side)
    {
        positions[side].x = (sint32)r_u32(states[side] + 100u);
        positions[side].y = (sint32)r_u32(states[side] + 104u);
        positions[side].z = (sint32)r_u32(states[side] + 108u);
        angles[side] = (sint32)r_u32(states[side] + 128u);
    }
    mask = physics_collision_geometry(states[0], states[1], positions, angles, contacts);
    if (!mask)
        return;
    for (side = 0; side < 2; ++side)
    {
        delta[side][0] = physics_multiply_divide((sint32)r_u32(states[side] + 112u) / 2, (sint32)r_u32(0x800651C4u), 256);
        delta[side][1] = physics_multiply_divide((sint32)r_u32(states[side] + 120u) / 2, (sint32)r_u32(0x800651C4u), 256);
        delta[side][2] = physics_multiply_divide((sint32)r_u32(states[side] + 140u) / 2, (sint32)r_u32(0x800651C4u), 256);
        positions[side].x = collision_sub(positions[side].x, delta[side][0]);
        positions[side].z = collision_sub(positions[side].z, delta[side][1]);
        angles[side] = collision_sub(angles[side], delta[side][2]);
    }
    for (iteration = 0; iteration < 7; ++iteration)
    {
        uint32 current;
        step /= 2;
        for (side = 0; side < 2; ++side)
        {
            delta[side][0] /= 2;
            delta[side][1] /= 2;
            delta[side][2] /= 2;
        }
        current = physics_collision_geometry(states[0], states[1], positions, angles, contacts);
        if (current)
        {
            fraction -= step;
            mask = current;
        }
        else
            fraction += step;
        for (side = 0; side < 2; ++side)
        {
            sint32 direction = current ? -1 : 1;
            positions[side].x = collision_add(positions[side].x, collision_mul(direction, delta[side][0]));
            positions[side].z = collision_add(positions[side].z, collision_mul(direction, delta[side][1]));
            angles[side] = collision_add(angles[side], collision_mul(direction, delta[side][2]));
        }
    }
    fraction -= 16;
    if (!(mask & (mask - 1u)))
    {
        for (selected = 0; !(mask & (1u << selected)); ++selected)
        {
        }
        side = selected / 4u;
        physics_collision_response(states[side], states[1u - side], &contacts[selected], angles[side] >> 8, fraction, selected & 3u);
        return;
    }
    {
        selected = sub_8001B884(first) & 7u;
    }
    while (!(mask & (1u << selected)))
        selected = (selected + 1u) & 7u;
    side = selected / 4u;
    physics_collision_response(states[side], states[1u - side], &contacts[selected], angles[side] >> 8, fraction, selected & 3u);
    for (iteration = 0; iteration < 8; ++iteration)
        if (iteration != selected && (mask & (1u << iteration)))
        {
            side = iteration / 4u;
            physics_collision_response(states[side], states[1u - side], &contacts[iteration], angles[side] >> 8, fraction, 256u);
        }
}
