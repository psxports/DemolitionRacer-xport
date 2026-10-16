#include "draft_signatures.h"
#include <limits.h>
extern uint32 sub_8001685C(uint32);
extern uint32 sub_8002C188(uint32, uint32);
extern void sub_8002EDE0(uint32, uint32, uint32);
extern uint32 sub_8002F5F8(uint32, uint32, uint32);
extern uint32 sub_8002F78C(uint32, uint32, uint32);
extern uint32 sub_8002F860(uint32, uint32, uint32);
extern void game_80097080(uint32);
extern void game_80099A78(uint32);
extern void dr_driving_gears(uint32, uint32);
extern sint32 draft_agent3_missing_800437EC(uint32);
extern sint32 draft_agent3_missing_8004388C(uint32);

static sint32 av_add(sint32 a, sint32 b)
{
    return (sint32)((uint32)a + (uint32)b);
}

static sint32 av_sub(sint32 a, sint32 b)
{
    return (sint32)((uint32)a - (uint32)b);
}

static sint32 av_mul(sint32 a, sint32 b)
{
    return (sint32)((uint32)a * (uint32)b);
}

static sint32 av_neg(sint32 a)
{
    return (sint32)(0u - (uint32)a);
}

static sint32 av_shift(sint32 a, uint32 n)
{
    return av_add(a, a < 0 ? (sint32)((1u << n) - 1u) : 0) >> n;
}

static sint32 av_prod(sint32 a, sint32 b, uint32 n)
{
    return av_shift(av_mul(a, b), n);
}

static sint32 av_div(sint32 a, sint32 b)
{
    if (!b)
        xport_mips_break(7u);
    if (a == INT_MIN && b == -1)
        xport_mips_break(6u);
    return a / b;
}

static sint32 av_magic(sint32 a, sint32 factor, uint32 n)
{
    return av_sub((sint32)(((int64_t)a * factor) >> 32) >> n, a >> 31);
}

static sint32 av_clamp(sint32 a, sint32 limit)
{
    if (a > limit)
        return limit;
    if (a < -limit)
        return -limit;
    return a;
}

static void av_angular(uint32 object, uint32 offset, sint16 previous, sint32 inverse)
{
    sint32 delta = (sint32)(((uint32)av_sub(r_s16(object + offset), previous) + 2048u) & 4095u) - 2048;
    w_u32(object + 136u + (offset - 40u) * 2u, (uint32)delta);
    w_u32(object + 136u + (offset - 40u) * 2u, (uint32)av_mul(delta, inverse));
    w_u32(object + 136u + (offset - 40u) * 2u, (uint32)av_clamp(r_s32(object + 136u + (offset - 40u) * 2u), 6000));
}

/* Full sixteen-entry original contact orientation switch */
void game_8009B348(uint32 object)
{
    uint32 timestep = r_u32(0x800651C4u), mask, index, registers[5], count = 0u;
    sint16 previous_x = (sint16)r_u16(object + 40u), previous_z = (sint16)r_u16(object + 44u);
    sint32 sum = 0, output[3], inverse, height;
    w_u32(object + 824u, r_u32(object + 104u));
    w_u32(object + 116u, r_u32(object + 116u) - (uint32)av_shift((sint32)(timestep * 3000u), 8u));
    w_u32(object + 124u, r_u32(object + 124u) + (uint32)av_prod(r_s32(object + 136u), (sint32)timestep, 8u));
    w_u32(object + 128u, r_u32(object + 128u) + (uint32)av_prod(r_s32(object + 140u), (sint32)timestep, 8u));
    w_u32(object + 132u, r_u32(object + 132u) + (uint32)av_prod(r_s32(object + 144u), (sint32)timestep, 8u));
    w_u32(object + 104u, r_u32(object + 104u) + (uint32)av_prod(r_s32(object + 116u), (sint32)timestep, 8u));
    w_u32(object + 100u, r_u32(object + 100u) + (uint32)av_prod(r_s32(object + 112u), (sint32)timestep, 8u));
    w_u32(object + 108u, r_u32(object + 108u) + (uint32)av_prod(r_s32(object + 120u), (sint32)timestep, 8u));
    for (index = 0u; index < 3u; ++index)
    {
        w_u16(object + 40u + index * 2u, (uint16)(r_s32(object + 124u + index * 4u) >> 8));
        w_u32(object + 168u + index * 4u, (uint32)(r_s32(object + 100u + index * 4u) >> 8));
    }
    sub_8002C188(object + 88u, object + 100u);
    sub_8002EDE0(object + 88u, object + 100u, object + 752u);
    game_80097080(object);
    mask = r_u8(object + 942u);
    switch (mask)
    {
        case 0:
        case 1:
        case 2:
        case 4:
        case 6:
        case 8:
        case 9:
            sub_8002F5F8(object + 40u, object + 576u, object + 840u);
            mask = r_u8(object + 942u);
            if (mask == r_u8(object + 943u))
            {
                inverse = av_div(65536, (sint32)timestep);
                av_angular(object, 40u, previous_x, inverse);
                av_angular(object, 44u, previous_z, inverse);
            }
            w_u32(object + 124u, (uint32)r_s16(object + 40u) << 8);
            w_u32(object + 132u, (uint32)r_s16(object + 44u) << 8);
            break;
        case 3:
        case 12:
            sub_8002F860(object + 40u, object + 576u, object + 840u);
            mask = r_u8(object + 942u);
            if (mask == r_u8(object + 943u))
                av_angular(object, 44u, previous_z, av_div(65536, (sint32)timestep));
            w_u32(object + 132u, (uint32)r_s16(object + 44u) << 8);
            break;
        case 5:
        case 10:
            sub_8002F78C(object + 40u, object + 576u, object + 840u);
            mask = r_u8(object + 942u);
            if (mask == r_u8(object + 943u))
                av_angular(object, 40u, previous_x, av_div(65536, (sint32)timestep));
            w_u32(object + 124u, (uint32)r_s16(object + 40u) << 8);
            break;
        default:
            break;
    }
    sub_8003C960((sint16 *)psx_addr(object + 148u, 18u), (const sint16 *)psx_addr(object + 40u, 6u));
    for (index = 0u; index < 5u; ++index)
        registers[index] = r_u32(object + 148u + index * 4u);
    for (index = 0u; index < 5u; ++index)
        xport_gte_write_control(index, registers[index]);
    for (index = 0u; index < 4u; ++index)
    {
        uint32 vertex = object + 624u + index * 8u, destination = object + 656u + index * 8u;
        if (mask & (1u << index))
            continue;
        height = r_s32(0x800C0920u);
        w_u16(vertex + 2u, (uint16)(r_u16(r_u32(object) + 66u + index * 8u) - av_shift(r_s32(object + 840u + index * 4u), 8u) - (uint32)height));
        xport_gte_write_data(0u, r_u32(vertex));
        xport_gte_write_data(1u, r_u32(vertex + 4u));
        xport_gte_execute(0x00486012u);
        output[0] = (sint32)xport_gte_read_data(9u);
        output[1] = (sint32)xport_gte_read_data(10u);
        output[2] = (sint32)xport_gte_read_data(11u);
        w_u16(destination, (uint16)output[0]);
        w_u16(destination + 2u, (uint16)output[1]);
        w_u16(destination + 4u, (uint16)output[2]);
        w_u16(vertex + 2u, (uint16)(r_u16(vertex + 2u) + r_u32(0x800C0920u)));
        sum = av_add(sum, av_sub(r_s32(object + 580u + index * 12u), (sint32)((uint32)(sint16)r_u16(destination + 2u) << 8)));
        ++count;
    }
    if (count)
    {
        sint32 average = av_div(sum, (sint32)count);
        w_u32(object + 104u, (uint32)average);
        if (r_u8(0x800BE9A0u + mask) && !(r_u8(object + 943u) & (mask ^ 15u)))
            w_u32(object + 116u, (uint32)av_div((sint32)((uint32)av_sub(av_sub(average, r_s32(object + 824u)), 3000) << 8), r_s32(0x800651C4u)));
    }
    game_80099A78(object);
}

/* Full original planar AI tyre forces and engine update */
void game_8009BA10(uint32 object)
{
    uint32 model = r_u32(object + 4u), timestep;
    sint32 vx, vz, yaw, steering, angle, sin_a, cos_a, sin_b, cos_b, sin_s, cos_s;
    sint32 lateral, forward, alternate, left_limit, right_limit, throttle, traction, engine = 0;
    sint32 inertia, first, second, third, denominator, force_left, force_right, loss_left = 0, loss_right = 0;
    sint32 world_x, world_z, torque, base_a, base_b, part_a, part_b, part_c, part_d, value, limit, magnitude;
    w_u8(object + 932u, (uint8)sub_8001685C(object + 88u));
    w_u32(object + 112u, r_u32(object + 112u) - (uint32)av_shift((sint32)((uint32)r_s16(object + 752u) * 1500u), 12u));
    w_u32(object + 120u, r_u32(object + 120u) - (uint32)av_shift((sint32)((uint32)r_s16(object + 756u) * 1500u), 12u));
    throttle = r_s16(object + 16u);
    if (!throttle)
    {
        sint32 drag = av_shift((sint32)(r_u32(0x800651C4u) * 6000u), 8u);
        value = r_s32(object + 112u);
        w_u32(object + 112u, (uint32)av_add(value, av_prod(av_shift(av_neg(value), 8u), drag, 12u)));
        value = r_s32(object + 120u);
        w_u32(object + 120u, (uint32)av_add(value, av_prod(av_shift(av_neg(value), 8u), drag, 12u)));
    }
    angle = r_s32(object + 128u) >> 8;
    steering = av_shift(r_s32(object + 8u), 8u);
    vx = r_s32(object + 112u);
    vz = r_s32(object + 120u);
    yaw = r_s32(object + 140u);
    traction = r_s16(model + 44u);
    w_u16(object + 904u, (uint16)dr_race_motion_sqrt((uint32)av_add(av_mul(vx >> 8, vx >> 8), av_mul(vz >> 8, vz >> 8))));
    sin_a = draft_agent3_missing_800437EC((uint32)angle);
    cos_a = draft_agent3_missing_8004388C((uint32)angle);
    angle = av_add(angle, steering);
    sin_b = draft_agent3_missing_800437EC((uint32)angle);
    cos_b = draft_agent3_missing_8004388C((uint32)angle);
    sin_s = draft_agent3_missing_800437EC((uint32)steering);
    cos_s = draft_agent3_missing_8004388C((uint32)steering);
    lateral = av_shift(av_sub(av_mul(vx, sin_a), av_mul(vz, cos_a)), 12u);
    forward = av_shift(av_add(av_mul(vx, cos_a), av_mul(vz, sin_a)), 12u);
    left_limit = av_shift(av_sub(16384, r_s32(object + 832u)), 7u);
    right_limit = av_shift(av_add(r_s32(object + 832u), 16384), 7u);
    alternate = av_sub(av_shift(av_add(av_mul(vx, cos_b), av_mul(vz, sin_b)), 12u), av_magic(av_mul(yaw, av_mul(cos_s, 320)), 0x0C907DA5, 17u));
    w_u32(object + 888u, (uint32)forward);
    w_u32(object + 892u, (uint32)alternate);
    if (throttle && forward <= (sint32)((uint32)r_s16(model + 116u) << 8))
    {
        dr_driving_gears(object, model);
        game_80016404(object, model);
        engine = game_8001675C(object, model);
    }
    inertia = av_add(av_prod(av_shift(av_mul(cos_s, 260), 12u), cos_s, 12u), av_prod(av_shift(av_mul(sin_s, 400), 12u), sin_s, 12u));
    first = av_sub(av_magic(av_mul(yaw, 320), 0x0C907DA5, 5u), lateral);
    second = av_prod(av_shift(av_mul(first, 160), 12u), cos_s, 12u);
    third = av_prod(av_shift(av_neg(av_mul(av_add(engine, forward), 60)), 12u), sin_s, 12u);
    base_a = av_mul(yaw, 251);
    base_b = av_mul(lateral, 320);
    value = av_prod(av_shift(av_mul(av_shift(av_sub(base_a, base_b), 10u), 640), 12u), sin_s, 12u);
    force_left = av_div(av_add(av_mul(av_add(second, third), cos_s), av_mul(value, sin_s)), inertia);
    value = av_mul(av_shift(av_mul(av_shift(av_neg(av_add(base_b, base_a)), 10u), 640), 12u), sin_s);
    force_right = av_div(av_add(value, av_mul(av_shift(av_mul(av_add(engine, forward), 260), 12u), cos_s)), inertia);
    limit = av_prod(traction, right_limit, 8u);
    magnitude = force_left < 0 ? av_neg(force_left) : force_left;
    if (limit < magnitude)
    {
        loss_left = av_sub(magnitude, limit);
        force_left = av_prod(force_left, av_div((sint32)((uint32)limit << 8), magnitude), 8u);
    }
    limit = av_prod(traction, left_limit, 8u);
    magnitude = force_right < 0 ? av_neg(force_right) : force_right;
    if (limit < magnitude)
    {
        loss_right = av_sub(magnitude, limit);
        force_right = av_prod(force_right, av_div((sint32)((uint32)limit << 8), magnitude), 8u);
    }
    w_u32(object + 896u, (uint32)loss_left);
    w_u32(object + 900u, (uint32)loss_right);
    part_a = av_prod(force_left, sin_a, 12u);
    part_b = av_prod(av_neg(force_left), cos_a, 12u);
    part_c = av_prod(force_right, sin_b, 12u);
    part_d = av_prod(av_neg(force_right), cos_b, 12u);
    value = av_sub(av_mul(force_right, av_shift(av_mul(sin_s, 320), 12u)), av_mul(force_left, 320));
    torque = av_clamp(av_sub(av_add((sint32)(((int64_t)value * (sint32)0x828CBFBFu) >> 32), value) >> 7, value >> 31), 1400);
    world_x = av_add(av_add(part_a, part_c), av_prod(engine, cos_a, 12u));
    world_z = av_add(av_add(part_b, part_d), av_prod(engine, sin_a, 12u));
    timestep = r_u32(0x800651C4u);
    w_u32(object + 140u, r_u32(object + 140u) + (uint32)av_prod(torque, (sint32)timestep, 8u));
    w_u32(object + 112u, r_u32(object + 112u) + (uint32)av_prod(world_x, (sint32)timestep, 8u));
    w_u32(object + 120u, r_u32(object + 120u) + (uint32)av_prod(world_z, (sint32)timestep, 8u));
    w_u16(object + 68u, (uint16)(r_u16(object + 68u) + av_prod(r_s32(object + 892u) >> 8, (sint32)timestep, 8u)));
    w_u16(object + 70u, (uint16)(r_u16(object + 70u) + av_prod(r_s32(object + 888u) >> 8, (sint32)timestep, 8u)));
    if (loss_left > 0)
    {
        value = r_s32(object + 140u);
        denominator = av_prod(av_prod(av_shift(value, 6u), (sint32)timestep, 8u), loss_left, 15u);
        w_u32(object + 140u, (uint32)av_sub(value, av_clamp(denominator, 512)));
    }
    game_8009745C(object, model, world_x, world_z);
}
