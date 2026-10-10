#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
uint32 sub_80036B9C(uint32 object, uint32 angle, uint32 port);
#define VIEW_GP 0x80064A10u
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
uint32 sub_80037B30(uint32 port);
uint32 sub_80037580(uint32 object, uint32 update, uint32 port);
uint32 sub_800379D4(uint32 object, uint32 port);
extern uint32 dr_camera_angles(const sint16 *angles);

static uint32 view_pending(uint32 target, uint32 object, uint32 port)
{
    fprintf(stderr, "Untranslated camera %08X: %08X %08X\n", target, object, port);
    abort();
}

uint32 sub_80037DC8(uint32 object, uint32 port)
{
    sint32 state = (sint32)r_u32(0x80064CB0u);
    uint32 offset = port * 4u;
    if (state > 0)
    {
        if (state == 1)
            return sub_80036914(0x80066D3Cu + port * 952u, 1u, port);
        return sub_80037B30(port);
    }
    if (r_u32(0x80065110u) || r_u32(0x80064B5Cu))
    {
        sub_80038ABC(object, port);
        return sub_80038E8C(object, port);
    }
    if (r_u8(0x800651C8u))
    {
        if (r_u16(0x80065520u + port * 2u) & 2048u)
            w_u32(0x800654D8u + offset, r_u32(0x800654D8u + offset) + 32u);
        if (r_u16(0x80065520u + port * 2u) & 1024u)
            w_u32(0x800654D8u + offset, r_u32(0x800654D8u + offset) - 32u);
    }
    else
        w_u32(0x800654D8u + offset, (r_u16(0x80065520u + port * 2u) & r_u16(0x800BE824u + port * 2u)) ? 2048u : 0u);
    if (r_u16(0x80065524u + port * 2u) & r_u16(0x800BE820u + port * 2u))
    {
        uint32 next = r_u32(0x800654C4u + offset) + 1u;
        w_u32(0x800654C4u + offset, (uint32)((sint32)next % 5));
        sub_80036914(object + 40u, 0u, port);
    }
    if (r_u32(0x800654E8u + offset))
        return sub_800379D4(object, port);
    return sub_80037580(object, 1u, port);
}

static uint32 view_product256(uint32 left, uint32 right)
{
    return (uint32)((sint32)(left * right) / 256);
}

uint32 sub_80037580(uint32 object, uint32 update, uint32 port)
{
    uint32 offset = port * 4u, rate_address = 0x8006546Cu + offset, rate = r_u32(rate_address), angle, sine, cosine;
    uint32 distance, delta = 0x8007A960u + port * 16u, camera = 0x8007A9D4u + port * 12u, result;
    sint32 target[3];
    if (r_u8(object + 940u) && r_u8(object + 942u) == 15u)
    {
        if ((sint32)rate >= 2)
            w_u32(rate_address, rate - 1u);
    }
    else
    {
        if ((sint32)rate < (sint32)r_u32(VIEW_GP + 0x6DCu))
        {
            ++rate;
            w_u32(rate_address, rate);
        }
        if ((sint32)r_u32(VIEW_GP + 0x6DCu) < (sint32)rate)
            w_u32(rate_address, rate - 1u);
    }
    w_u32(0x800654B4u + offset, 0u);
    angle = r_u32(0x800654D8u + offset) + (uint32)((sint32)(0u - r_u32(0x80065464u + offset)) >> 8) + r_u32(0x800654BCu + offset);
    w_u16(0x800654CCu + port * 2u, (uint16)angle);
    sine = (uint32)draft_agent3_missing_800437EC(angle);
    cosine = (uint32)draft_agent3_missing_8004388C(angle);
    distance = r_u32(0x8006547Cu + offset);
    w_u32(delta, view_product256((uint32)((sint32)distance / 16), cosine));
    w_u32(delta + 4u, r_u32(0x800654D0u + offset));
    w_u32(delta + 8u, view_product256((uint32)((sint32)(0u - distance) / 16), sine));
    w_u32(camera, r_u32(object + 100u) + r_u32(delta) + view_product256((uint32)((sint32)r_u32(0x80065484u + offset) >> 8), sine));
    w_u32(camera + 4u, r_u32(object + 104u) + r_u32(delta + 4u));
    w_u32(camera + 8u, r_u32(object + 108u) + r_u32(delta + 8u) + view_product256((uint32)((sint32)r_u32(0x80065484u + offset) >> 8), cosine));
    /* TODO: Full terrain-camera 36B9C native-vector dependency */
    sub_80036B9C(object, (r_u32(0x800654B4u + offset) << 11) + (uint32)((sint32)r_u32(0x80065464u + offset) >> 8), port);
    if (update)
    {
        uint32 error = (((r_u32(0x800654B4u + offset) << 11) + (uint32)(sint32)(sint16)r_u16(object + 42u)) << 8) - r_u32(0x80065464u + offset);
        uint32 magnitude, factor, product;
        error &= 0xFFFFFu;
        if (error > 0x7FFFFu)
            error -= 0x100000u;
        magnitude = (uint32)((sint32)error >> 11);
        if ((sint32)magnitude < 0)
            magnitude = 0u - magnitude;
        factor = magnitude + 5u;
        if ((sint32)factor < 15)
            factor = 15u;
        rate = r_u32(rate_address);
        if ((sint32)rate < (sint32)factor)
            factor = rate;
        product = error * factor;
        w_u32(0x80065464u + offset, r_u32(0x80065464u + offset) + view_product256((uint32)((sint32)product >> 8), r_u32(0x800651C4u)));
    }
    target[0] = (sint32)r_u32(object + 100u);
    target[1] = (sint32)(r_u32(object + 104u) + r_u32(0x800654ACu + offset));
    target[2] = (sint32)r_u32(object + 108u);
    sub_8003C034((const sint32 *)psx_addr(camera, 12u));
    sub_8003C4B0(target, r_u32(0x80065104u + offset));
    result = (uint32)(sint32)(sint16)r_u16(object + 88u);
    w_u32(0x800654E0u + offset, result);
    return result;
}

uint32 sub_800379D4(uint32 object, uint32 port)
{
    sint16 angles[3];
    uint32 i, camera = 0x8007A9D4u + port * 12u, result;
    angles[0] = (sint16)(r_u16(object + 40u) + 2048u);
    angles[1] = (sint16)(r_u32(0x800654D8u + port * 4u) + 2048u - r_u16(object + 42u));
    angles[2] = (sint16)(0u - r_u16(object + 44u));
    w_u16(0x800654CCu + port * 2u, (uint16)angles[1]);
    for (i = 0u; i < 5u; ++i)
        xport_gte_write_control(i, r_u32(0x80081B84u + i * 4u));
    xport_gte_write_data(0u, r_u32(0x8007A980u + port * 8u));
    xport_gte_write_data(1u, r_u32(0x8007A984u + port * 8u));
    xport_gte_execute(0x486012u);
    for (i = 0u; i < 3u; ++i)
        w_u32(camera + i * 4u, xport_gte_read_data(25u + i));
    for (i = 0u; i < 3u; ++i)
        w_u32(camera + i * 4u, (r_u32(camera + i * 4u) << 8) + r_u32(object + 100u + i * 4u));
    sub_8003C034((const sint32 *)psx_addr(camera, 12u));
    dr_camera_angles(angles);
    result = (uint32)(sint32)(sint16)r_u16(object + 88u);
    w_u32(0x800654E0u + port * 4u, result);
    return result;
}

uint32 sub_80037B30(uint32 port)
{
    uint32 i, j, k, origin = 0x80066D78u + port * 952u, curve = 0x8007A8E8u + port * 60u;
    uint32 camera = 0x8007A9D4u + port * 12u, count = r_u32(0x80065254u), index, time, square, cube;
    sint32 target[3], relative[12];
    for (i = 0u; i < 3u; ++i)
        target[i] = (sint32)r_u32(origin + i * 4u);
    for (i = 0u; i < 3u; ++i)
    {
        uint32 delta = i == 0u ? 180000u : 250000u;
        w_u32(0x8007A9C8u + i * 4u, r_u32(origin + i * 4u) + delta);
        w_u32(0x8007A9BCu + i * 4u, r_u32(origin + i * 4u) + delta);
    }
    index = (uint32)((sint32)count / 4) + (uint32)((sint32)count / 2);
    for (i = 0u; i < 3u; ++i)
        w_u32(0x8007A9B0u + i * 4u, r_u32(0x80066D78u + index * 952u + i * 4u) + (i == 1u ? 250000u : 0u));
    for (i = 0u; i < 3u; ++i)
        w_u32(0x8007A9A4u + i * 4u, r_u32(0x80066D78u + (count - 1u) * 952u + i * 4u) + (i == 1u ? 250000u : 0u));
    target[1] = (sint32)((uint32)target[1] + 50000u);
    /* Original SDK 4E8B0 derives cubic coefficients from four points */
    for (i = 0u; i < 3u; ++i)
        w_u32(curve + i * 4u, r_u32(0x8007A9B0u + i * 4u));
    for (j = 0u; j < 4u; ++j)
        for (i = 0u; i < 3u; ++i)
            relative[j * 3u + i] = (sint32)(r_u32(0x8007A9A4u + j * 12u + i * 4u) - r_u32(0x8007A9B0u + i * 4u)) >> 8;
    for (j = 0u; j < 4u; ++j)
        for (i = 0u; i < 3u; ++i)
        {
            uint32 sum = 0u;
            for (k = 0u; k < 4u; ++k)
                sum += r_u32(0x8005F2B8u + (j * 4u + k) * 4u) * (uint32)relative[k * 3u + i];
            w_u32(curve + 12u + (j * 3u + i) * 4u, (uint32)((sint32)sum / 2));
        }
    time = r_u32(VIEW_GP + 0x6D0u);
    w_u32(VIEW_GP + 0x6D8u, time);
    if ((sint32)time < 4096)
        w_u32(VIEW_GP + 0x6D0u, time + r_u32(VIEW_GP + 0x6D4u));
    /* Original SDK 4EA48 evaluates cubic with wrapped products */
    square = (uint32)((sint32)(time * time) >> 12);
    cube = (uint32)((sint32)(square * time) >> 12);
    for (i = 0u; i < 3u; ++i)
    {
        uint32 sum = r_u32(curve + 12u + i * 4u) * cube + r_u32(curve + 24u + i * 4u) * square + r_u32(curve + 36u + i * 4u) * time;
        uint32 value = ((uint32)((sint32)sum >> 12) + r_u32(curve + 48u + i * 4u)) << 8;
        w_u32(camera + i * 4u, value + r_u32(curve + i * 4u));
    }
    sub_8003C034((const sint32 *)psx_addr(camera, 12u));
    return sub_8003C4B0(target, r_u32(0x80065104u + port * 4u));
}

extern uint32 dr_track_find(uint8 *state, const sint32 *position);
extern uint32 dr_track_height(const uint8 *state, const sint32 *position, sint32 *height);
extern uint32 game_800AE420(uint32 track);
extern uint32 game_800B4B58(uint32 track);
extern uint32 sub_80043450(uint32 x, uint32 z);
extern uint32 dr_race_motion_sqrt(uint32 value);
uint32 sub_80036B9C(uint32 object, uint32 angle, uint32 port);

static uint32 view_div(uint32 numerator, uint32 denominator)
{
    if (!denominator || (denominator == 0xFFFFFFFFu && numerator == 0x80000000u))
    {
        fprintf(stderr, "Original camera division exception\n");
        abort();
    }
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static void view_normalize(VECTOR *normal)
{
    SVECTOR input;
    VECTOR output;
    uint32 sum, leading = 0u, shift, index, probe;
    input.vx = (sint16)normal->vx;
    input.vy = (sint16)normal->vy;
    input.vz = (sint16)normal->vz;
    input.pad = 0;
    sum = (uint32)((sint32)input.vx * input.vx) + (uint32)((sint32)input.vy * input.vy) + (uint32)((sint32)input.vz * input.vz);
    probe = (sint32)sum < 0 ? ~sum : sum;
    if (!probe)
        leading = 32u;
    else
        while (!(probe & 0x80000000u))
        {
            ++leading;
            probe <<= 1;
        }
    leading &= ~1u;
    shift = (uint32)((31 - (sint32)leading) >> 1) & 31u;
    index = leading >= 24u ? sum << ((leading - 24u) & 31u) : (uint32)((sint32)sum >> ((24u - leading) & 31u));
    /* Original SDK 4E324 uses the boot reciprocal square-root table */
    gte_gpf0(&input, (sint16)r_u16(0x8005E91Cu + (index - 64u) * 2u), &output);
    normal->vx = output.vx >> shift;
    normal->vy = output.vy >> shift;
    normal->vz = output.vz >> shift;
}

uint32 sub_80036B9C(uint32 object, uint32 angle, uint32 port)
{
    /* Original local state fields are undefined; native initialization uses zero */
    uint8 states[3][10] = {{0}}, *state;
    sint32 positions[3][3] = {{0}}, height;
    sint16 angles[3], matrix[16];
    SVECTOR vertex = {0};
    VECTOR normal = {0};
    uint32 offset = port * 4u, i, sine = (uint32)draft_agent3_missing_800437EC(angle), cosine = (uint32)draft_agent3_missing_8004388C(angle);
    uint32 vertical = r_u32(0x800654A4u + offset), relative, length, record, first = 0u, second = 0u, boundary = 0u, correction = 0u, result;
    positions[0][0] = (sint32)(r_u32(object + 100u) + (cosine << 5));
    positions[0][2] = (sint32)(r_u32(object + 108u) + (sine << 5));
    positions[1][0] = (sint32)(r_u32(object + 100u) - (sine << 5));
    positions[1][2] = (sint32)(r_u32(object + 108u) + (cosine << 5));
    positions[2][0] = (sint32)(r_u32(object + 100u) + (sine << 5));
    positions[2][2] = (sint32)(r_u32(object + 108u) - (cosine << 5));
    for (i = 0u; i < 3u; ++i)
    {
        uint16 section = r_u16(object + 88u);
        memcpy(states[i], &section, 2u);
        states[i][8] = r_u8(object + 96u);
        dr_track_find(states[i], positions[i]);
        dr_track_height(states[i], positions[i], &positions[i][1]);
    }
    angles[0] = (sint16)sub_80043450(0u - (uint32)((sint32)((uint32)positions[0][1] - (uint32)((sint32)((uint32)positions[1][1] + (uint32)positions[2][1]) / 2)) >> 8), 512u);
    angles[1] = (sint16)angle;
    angles[2] = (sint16)sub_80043450(0u - (uint32)((sint32)((uint32)positions[1][1] - (uint32)positions[2][1]) >> 8), 1024u);
    sub_8003C960(matrix, angles);
    relative = r_u32(0x800654D8u + offset) + (uint32)((sint32)r_u32(0x80065464u + offset) >> 8) - (uint32)(sint32)(sint16)r_u16(object + 42u);
    cosine = (uint32)draft_agent3_missing_8004388C(relative);
    vertex.vx = (sint16)((sint32)((uint32)((sint32)r_u32(0x8006549Cu + offset) / 256) * cosine) / 4096);
    vertex.vy = (sint16)((sint32)r_u32(0x80065494u + offset) / 256);
    sine = (uint32)draft_agent3_missing_800437EC(relative);
    vertex.vz = (sint16)((sint32)((uint32)((sint32)(0u - r_u32(0x8006549Cu + offset)) / 256) * sine) / 4096);
    for (i = 0u; i < 5u; ++i)
    {
        uint32 word;
        memcpy(&word, (uint8 *)matrix + i * 4u, 4u);
        xport_gte_write_control(i, word);
    }
    xport_gte_write_data(0u, (uint32)(uint16)vertex.vx | ((uint32)(uint16)vertex.vy << 16));
    xport_gte_write_data(1u, (uint16)vertex.vz);
    xport_gte_execute(0x486012u);
    vertex.vx = (sint16)xport_gte_read_data(9u);
    vertex.vy = (sint16)xport_gte_read_data(10u);
    vertex.vz = (sint16)xport_gte_read_data(11u);
    positions[0][0] = (sint32)(r_u32(object + 100u) + ((uint32)(sint32)vertex.vx << 8));
    positions[0][2] = (sint32)(r_u32(object + 108u) + ((uint32)(sint32)vertex.vz << 8));
    state = (uint8 *)psx_addr(0x8007A990u + port * 10u, 10u);
    dr_track_find(state, positions[0]);
    {
        sint16 section;
        uint16 next;
        memcpy(&section, state, 2u);
        next = (uint16)game_800AE420((uint32)(sint32)section);
        memcpy(state + 2u, &next, 2u);
    }
    length = (uint32)((sint32)vertex.vx * vertex.vx) + (uint32)((sint32)vertex.vz * vertex.vz);
    if (!r_u32(0x800593A0u))
    {
        sint16 section, next;
        uint32 lanes;
        memcpy(&section, state, 2u);
        memcpy(&next, state + 2u, 2u);
        if (game_800B4B58((uint32)(sint32)(sint16)r_u16(object + 90u)) != 0xFFFFFFFFu && game_800B4B58((uint32)(sint32)next) != 0xFFFFFFFFu)
        {
            sint32 a = (sint16)r_u16(object + 90u) - (sint16)r_u16(object + 88u), b = next - section;
            if (a < 0)
                a = -a;
            if (b < 0)
                b = -b;
            if ((a < 2) != (b < 2))
            {
                uint16 updated = (uint16)((uint16)next + r_u16(object + 88u) - r_u16(object + 90u));
                memcpy(state, &updated, 2u);
            }
        }
        dr_track_height(state, positions[0], &height);
        result = (uint32)((sint32)((uint32)height - r_u32(object + 104u)) / 256) + r_u32(VIEW_GP + 0x6FCu);
        if ((sint32)vertex.vy < (sint32)result)
            vertex.vy = (sint16)result;
        memcpy(&section, state, 2u);
        record = r_u32(0x800653D4u) + (uint32)(sint32)section * 24u;
        lanes = r_u8(record + 3u) & 15u;
        if (!state[8])
        {
            first = r_u32(0x800653C4u) + r_u16(record + 6u) * 6u;
            second = r_u32(0x800653C4u) + r_u16(record + 4u) * 6u;
        }
        else if ((sint32)state[8] >= (sint32)(lanes - 1u))
        {
            uint32 type = r_u8(record) * 2u;
            first = r_u32(0x800653C4u) + (r_u16(record + 4u) + lanes + (uint32)(sint32)(sint8)r_u8(0x8005AAB4u + type)) * 6u;
            second = r_u32(0x800653C4u) + (r_u16(record + 6u) + lanes + (uint32)(sint32)(sint8)r_u8(0x8005AAB5u + type)) * 6u;
        }
        if (first)
        {
            uint32 base_x, base_z;
            positions[0][0] >>= 8;
            positions[0][2] >>= 8;
            normal.vx = (sint16)r_u16(first + 4u) - (sint16)r_u16(second + 4u);
            normal.vz = (sint16)r_u16(second) - (sint16)r_u16(first);
            view_normalize(&normal);
            base_x = (uint32)(sint32)(sint16)r_u16(first) + r_u32(record + 12u);
            base_z = (uint32)(sint32)(sint16)r_u16(first + 4u) + r_u32(record + 20u);
            boundary = (uint32)((sint32)(((uint32)positions[0][0] - base_x) * (uint32)normal.vx + ((uint32)positions[0][2] - base_z) * (uint32)normal.vz) >> 12) + r_u32(VIEW_GP + 0x6E0u);
        }
    }
    else
    {
        sint16 section;
        memcpy(&section, state, 2u);
        record = r_u32(0x800653ACu) + ((uint32)(sint32)section << 4);
        normal.vx = (sint32)(r_u32(record + 24u) - r_u32(record + 8u));
        normal.vz = (sint32)(r_u32(record) - r_u32(record + 16u));
        view_normalize(&normal);
        positions[0][0] >>= 8;
        positions[0][2] >>= 8;
        boundary = (uint32)((sint32)(((uint32)positions[0][0] - r_u32(record)) * (uint32)normal.vx + ((uint32)positions[0][2] - r_u32(record + 8u)) * (uint32)normal.vz) >> 12) + r_u32(VIEW_GP + 0x6E0u);
    }
    if ((sint32)boundary > 0)
    {
        uint32 dx = (uint32)positions[0][0] - (uint32)((sint32)r_u32(object + 100u) >> 8), dz = (uint32)positions[0][2] - (uint32)((sint32)r_u32(object + 108u) >> 8);
        uint32 projection = (uint32)((sint32)(dx * (uint32)normal.vx + dz * (uint32)normal.vz) >> 12), cross = dx * (uint32)normal.vz - dz * (uint32)normal.vx;
        uint32 distance = dr_race_motion_sqrt(dx * dx + dz * dz), a = view_div(projection * 255u, distance), b = view_div(boundary * 255u, distance);
        if ((sint32)a >= 257)
            a = 256u;
        correction = b << 2;
        if ((sint32)b >= 129)
        {
            correction = view_div(((b << 8) - b * a) << 9, (b << 8) - (a << 7));
            if ((sint32)a >= 129)
            {
                uint32 product, delta;
                a -= 128u;
                length -= (uint32)((sint32)length / 256) * a;
                product = r_u32(0x80065494u + offset) * a;
                delta = (uint32)((sint32)product / 65536);
                vertex.vy = (sint16)((uint16)vertex.vy + delta);
                vertical = r_u32(0x800654A4u + offset) + (delta << 8);
            }
        }
        if ((sint32)cross > 0)
            correction = 0u - correction;
    }
    result = r_u32(0x800654BCu + offset);
    w_u32(0x800654BCu + offset, result + (uint32)((sint32)(correction - result) / 8));
    result = r_u32(0x800654D0u + offset);
    w_u32(0x800654D0u + offset, result + view_product256((uint32)((sint32)(((uint32)(sint32)vertex.vy << 8) - result) / 8), r_u32(0x800651C4u)));
    result = sub_8003B674(length) << 8;
    relative = r_u32(0x8006547Cu + offset);
    w_u32(0x8006547Cu + offset, relative + view_product256((uint32)((sint32)(result - relative) / 8), r_u32(0x800651C4u)));
    relative = r_u32(0x800654ACu + offset);
    result = relative + view_product256((uint32)((sint32)(vertical - relative) / 8), r_u32(0x800651C4u));
    w_u32(0x800654ACu + offset, result);
    return result;
}
