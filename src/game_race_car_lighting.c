#include "draft_signatures.h"
#include <stdlib.h>
#include <stdio.h>
#define L32(a) r_u32(a)
#define L16(a) r_u16(a)
#define LS16(a) ((sint16)r_u16(a))
#define L8(a) r_u8(a)

static uint32 light_div(uint32 a, uint32 b)
{
    if (!b || (b == 0xFFFFFFFFu && a == 0x80000000u))
    {
        draft_agent1_missing_diagnostic("Original lighting division exception\n");
        abort();
    }
    return (uint32)((sint32)a / (sint32)b);
}

static uint32 light_normalize(sint16 vector[3])
{
    SVECTOR input = {vector[0], vector[1], vector[2], 0};
    VECTOR output;
    uint32 sum = (uint32)((sint32)input.vx * input.vx) + (uint32)((sint32)input.vy * input.vy) + (uint32)((sint32)input.vz * input.vz);
    uint32 leading = 0u, probe = (sint32)sum < 0 ? ~sum : sum, index, shift;
    if (!probe)
        leading = 32u;
    else
        while (!(probe & 0x80000000u))
        {
            ++leading;
            probe <<= 1;
        }
    leading &= ~1u;
    index = leading >= 24u ? sum << ((leading - 24u) & 31u) : (uint32)((sint32)sum >> ((24u - leading) & 31u));
    shift = (uint32)((31 - (sint32)leading) >> 1) & 31u;
    gte_gpf0(&input, LS16(0x8005E91Cu + (index - 64u) * 2u), &output);
    vector[0] = (sint16)(output.vx >> shift);
    vector[1] = (sint16)(output.vy >> shift);
    vector[2] = (sint16)(output.vz >> shift);
    return sum;
}

static void light_transition(uint32 object, uint32 descriptor, uint32 extent, uint32 end)
{
    uint32 section = (uint32)(sint32)LS16(descriptor + (end ? 2u : 0u)), record = L32(0x800653D4u) + section * 24u + (end ? 24u : 0u);
    uint32 a = L32(0x800653C4u) + L16(record + 4u) * 6u, b = L32(0x800653C4u) + (L16(record + 6u) - 1u) * 6u;
    sint16 normal[3] = {(sint16)(L16(a + 4u) - L16(b + 4u)), 0, (sint16)(L16(b) - L16(a))};
    sint16 dx, dz;
    uint32 dot, weight, remainder, rgb[3], scales[3], other = end ? descriptor + 32u : descriptor - 32u, i;
    light_normalize(normal);
    dx = (sint16)((uint32)((sint32)L32(object + 100u) / 256) - L16(b) - L32(record + 12u));
    dz = (sint16)((uint32)((sint32)L32(object + 108u) / 256) - L16(b + 4u) - L32(record + 20u));
    dot = (uint32)((sint32)normal[0] * dx) + (uint32)((sint32)normal[2] * dz);
    if (!end)
        dot = 0u - dot;
    weight = (uint32)((sint32)dot / 4096);
    if ((sint32)weight < 0)
        weight = 0u;
    if ((sint32)extent < (sint32)weight)
        weight = extent;
    remainder = extent - weight;
    for (i = 0u; i < 3u; ++i)
    {
        rgb[i] = light_div(L8(descriptor + 16u + i) * weight + L8(other + 16u + i) * remainder, extent);
        scales[i] = light_div(L8(other + 12u + i) * remainder, extent);
    }
    sub_8003C0B4(rgb[0], rgb[1], rgb[2]);
    sub_8003C0DC(0u, (const sint16 *)psx_addr(other + 4u, 6u), scales[0], scales[1], scales[2]);
    sub_8003C0DC(1u, (const sint16 *)psx_addr(descriptor + 4u, 6u), 0u, 0u, 0u);
    sub_8003C0DC(2u, (const sint16 *)psx_addr(descriptor + 4u, 6u), 0u, 0u, 0u);
    sub_8003C164((const MATRIX *)psx_addr(object + 148u, sizeof(MATRIX)));
}

void game_8009DBD0(uint32 object, uint32 descriptor, uint32 extent)
{
    light_transition(object, descriptor, extent, 0u);
}

void game_8009DF74(uint32 object, uint32 descriptor, uint32 extent)
{
    light_transition(object, descriptor, extent, 1u);
}

static void light_point(uint32 object, uint32 descriptor, uint32 slot, const uint32 point[3])
{
    sint16 vector[3];
    uint32 i, sum, strength, scales[3];
    for (i = 0u; i < 3u; ++i)
        vector[i] = (sint16)(point[i] - (uint32)((sint32)L32(object + 100u + i * 4u) / 256) + (i == 1u ? L16(descriptor + 22u) : 0u));
    sum = light_normalize(vector);
    strength = (uint32)((sint32)(sum * L8(descriptor + 27u)) / 16384);
    strength = (sint32)strength > 4096 ? 0u : 4096u - strength;
    for (i = 0u; i < 3u; ++i)
        scales[i] = (uint32)((sint32)(L8(descriptor + 12u + i) * strength) / 4096);
    sub_8003C0DC(slot, vector, scales[0], scales[1], scales[2]);
}

void game_8009E320(uint32 object)
{
    uint32 descriptor = L32(0x800C0A10u) + L8(object + 939u) * 32u, kind, i;
    sint32 section = LS16(object + 88u);
    while (section < LS16(descriptor))
    {
        descriptor -= 32u;
        w_u8(object + 939u, (uint8)(L8(object + 939u) - 1u));
    }
    while (LS16(descriptor + 2u) < section)
    {
        descriptor += 32u;
        w_u8(object + 939u, (uint8)(L8(object + 939u) + 1u));
    }
    kind = L8(descriptor + 24u);
    if (kind == 0u)
    {
        sub_8003C0B4(L8(descriptor + 16u), L8(descriptor + 17u), L8(descriptor + 18u));
        sub_8003C0DC(0u, (const sint16 *)psx_addr(descriptor + 4u, 6u), L8(descriptor + 12u), L8(descriptor + 13u), L8(descriptor + 14u));
        sub_8003C0DC(1u, (const sint16 *)psx_addr(descriptor + 4u, 6u), 0u, 0u, 0u);
        sub_8003C0DC(2u, (const sint16 *)psx_addr(descriptor + 4u, 6u), 0u, 0u, 0u);
    }
    else if (kind == 1u || kind == 2u)
    {
        uint32 step = L8(descriptor + 25u), selected, record, vertices = L32(0x800653C4u), lane, point[3];
        if (section - LS16(descriptor) < 3)
        {
            game_8009DBD0(object, descriptor, 2048u);
            return;
        }
        if (LS16(descriptor + 2u) - section < 3)
        {
            game_8009DF74(object, descriptor, 2048u);
            return;
        }
        selected = light_div((uint32)(section - LS16(descriptor + 20u)) + (step >> 1), step) * step + (uint32)(sint32)LS16(descriptor + 20u);
        record = L32(0x800653D4u) + selected * 24u;
        if (kind == 1u)
        {
            uint32 index = L16(record + 4u), other = index + (L8(record + 3u) & 15u);
            /* Original kind-one vertex loads are unsigned */
            for (i = 0u; i < 3u; ++i)
                point[i] = L16(vertices + index * 6u + i * 2u) + L32(record + 12u + i * 4u);
            light_point(object, descriptor, 0u, point);
            for (i = 0u; i < 3u; ++i)
                point[i] = L16(vertices + other * 6u + i * 2u) + L32(record + 12u + i * 4u);
            light_point(object, descriptor, 1u, point);
            sub_8003C0B4(L8(descriptor + 16u), L8(descriptor + 17u), L8(descriptor + 18u));
            sub_8003C0DC(2u, (const sint16 *)psx_addr(descriptor + 4u, 6u), 0u, 0u, 0u);
        }
        else
        {
            uint32 mode = L8(descriptor + 26u), points[3][3];
            if (mode > 2u)
            {
                draft_agent1_missing_diagnostic("Original lighting selects undefined local points: %u\n", mode);
                abort();
            }
            for (lane = 0u; lane < 3u; ++lane)
            {
                uint32 r = L32(0x800653D4u) + (selected + (lane - 1u) * step) * 24u;
                uint32 a = vertices + L16(r + 4u) * 6u, b = vertices + (L16(r + 6u) - 1u) * 6u;
                for (i = 0u; i < 3u; ++i)
                {
                    uint32 component = mode == 0u ? (uint32)(sint32)LS16(a + i * 2u) : mode == 1u ? (uint32)(sint32)LS16(b + i * 2u) : (uint32)((LS16(a + i * 2u) + LS16(b + i * 2u)) / 2);
                    points[lane][i] = component + L32(r + 12u + i * 4u);
                }
            }
            for (lane = 0u; lane < 3u; ++lane)
                light_point(object, descriptor, lane, points[lane]);
            sub_8003C0B4(L8(descriptor + 16u), L8(descriptor + 17u), L8(descriptor + 18u));
        }
    }
    else if (kind == 3u)
    {
        uint32 extent = (uint32)(LS16(descriptor + 2u) - LS16(descriptor) + 1);
        if (section - LS16(descriptor) < (sint32)extent / 2)
            game_8009DBD0(object, descriptor, extent << 9);
        else
            game_8009DF74(object, descriptor, extent << 9);
        return;
    }
    else
        return;
    sub_8003C164((const MATRIX *)psx_addr(object + 148u, sizeof(MATRIX)));
}

extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);

static void light_camera_translation(const sint32 position[3])
{
    uint32 i, high[3], low[3], product[3];
    for (i = 0u; i < 5u; ++i)
        xport_gte_write_control(i, L32(0x80081BA4u + i * 4u));
    for (i = 0u; i < 3u; ++i)
    {
        uint32 value = (uint32)position[i], magnitude = (sint32)value < 0 ? 0u - value : value;
        high[i] = (uint32)((sint32)magnitude >> 15);
        low[i] = magnitude & 32767u;
        if ((sint32)value < 0)
        {
            high[i] = 0u - high[i];
            low[i] = 0u - low[i];
        }
        xport_gte_write_data(9u + i, high[i]);
    }
    xport_gte_execute(0x41E012u);
    for (i = 0u; i < 3u; ++i)
        product[i] = xport_gte_read_data(25u + i);
    for (i = 0u; i < 3u; ++i)
        xport_gte_write_data(9u + i, low[i]);
    xport_gte_execute(0x49E012u);
    for (i = 0u; i < 3u; ++i)
        w_u32(0x80081C78u + i * 4u, xport_gte_read_data(25u + i) + (product[i] << 3));
    for (i = 0u; i < 3u; ++i)
        xport_gte_write_control(5u + i, L32(0x80081C78u + i * 4u));
}

static void light_vertex(uint32 index, const sint16 vertex[3])
{
    xport_gte_write_data(index * 2u, (uint32)(uint16)vertex[0] | ((uint32)(uint16)vertex[1] << 16));
    xport_gte_write_data(index * 2u + 1u, (uint16)vertex[2]);
}

static void light_project_quads(uint32 packet, const sint16 points[6][3])
{
    uint32 pass, i, depth, ot;
    for (pass = 0u; pass < 2u; ++pass, packet += 40u)
    {
        for (i = 0u; i < 3u; ++i)
            light_vertex(i, points[pass * 2u + i]);
        xport_gte_execute(0x280030u);
        for (i = 0u; i < 3u; ++i)
            w_u32(packet + 8u + i * 8u, xport_gte_read_data(12u + i));
        light_vertex(0u, points[pass * 2u + 3u]);
        xport_gte_execute(0x180001u);
        w_u32(packet + 32u, xport_gte_read_data(14u));
        xport_gte_execute(0x168002Eu);
        depth = xport_gte_read_data(7u);
        if ((sint32)depth > 0 && (sint32)depth < (sint32)L32(0x800654F0u))
        {
            ot = L32(L32(0x80065118u) + 112u) + depth * 4u;
            w_u32(packet, (L32(packet) & 0xFF000000u) | (L32(ot) & 0xFFFFFFu));
            w_u32(ot, (L32(ot) & 0xFF000000u) | (packet & 0xFFFFFFu));
        }
    }
}

void game_8009F030(uint32 player)
{
    uint32 object = 0x80066D14u + player * 952u, packet = L32(0x800C0A18u + L32(0x80065128u) * 4u) + player * 80u, i, j;
    uint32 values[4][3], center[3];
    sint16 points[6][3];
    sint32 relative[3];
    for (i = 0u; i < 4u; ++i)
        for (j = 0u; j < 3u; ++j)
            values[i][j] = (uint32)((sint32)L32(object + 480u + i * 12u + j * 4u) >> 8);
    for (j = 0u; j < 3u; ++j)
    {
        center[j] = (uint32)((sint32)(values[0][j] + values[1][j] + values[2][j] + values[3][j]) >> 2);
        points[0][j] = (sint16)(values[0][j] - center[j]);
        points[1][j] = (sint16)(values[1][j] - center[j]);
        points[4][j] = (sint16)(values[2][j] - center[j]);
        points[5][j] = (sint16)(values[3][j] - center[j]);
        points[2][j] = (sint16)((points[0][j] + points[4][j]) / 2);
        points[3][j] = (sint16)((points[1][j] + points[5][j]) / 2);
        relative[j] = (sint32)(center[j] - L32(0x80081BB8u + j * 4u));
    }
    light_camera_translation(relative);
    light_project_quads(packet, points);
}

void game_8009F500(uint32 object)
{
    uint32 sine = (uint32)draft_agent3_missing_800437EC((uint32)(sint32)LS16(object + 42u));
    uint32 cosine = (uint32)draft_agent3_missing_8004388C((uint32)(sint32)LS16(object + 42u));
    uint32 max_x = 0u, max_z = 0u, i, j, packet = L32(0x800C0A18u + L32(0x80065128u) * 4u) + L8(object + 937u) * 80u;
    uint32 ax, az, bx, bz;
    sint16 points[6][3];
    sint32 relative[3];
    for (i = 0u; i < 8u; ++i)
    {
        uint32 x = (uint32)((sint32)(L32(object + 480u + i * 12u) - L32(object + 100u)) >> 8);
        uint32 z = (uint32)((sint32)(L32(object + 488u + i * 12u) - L32(object + 108u)) >> 8);
        uint32 a = (uint32)((sint32)(x * sine - z * cosine) >> 12), b = (uint32)((sint32)(x * cosine + z * sine) >> 12);
        if ((sint32)a < 0)
            a = 0u - a;
        if ((sint32)b < 0)
            b = 0u - b;
        if ((sint32)max_x < (sint32)a)
            max_x = a;
        if ((sint32)max_z < (sint32)b)
            max_z = b;
    }
    if (!LS16(object + 754u))
        return;
    ax = max_x * sine;
    az = max_x * cosine;
    bx = max_z * cosine;
    bz = max_z * sine;
    points[0][0] = (sint16)((sint32)(ax + bx) >> 12);
    points[0][2] = (sint16)((sint32)((0u - az) + bz) >> 12);
    points[1][0] = (sint16)((sint32)((0u - ax) + bx) >> 12);
    points[1][2] = (sint16)((sint32)(az + bz) >> 12);
    points[4][0] = (sint16)((sint32)(ax - bx) >> 12);
    points[4][2] = (sint16)((sint32)((0u - az) - bz) >> 12);
    points[5][0] = (sint16)((sint32)((0u - ax) - bx) >> 12);
    points[5][2] = (sint16)((sint32)(az - bz) >> 12);
    for (i = 0u; i < 6u; ++i)
        if (i != 2u && i != 3u)
        {
            uint32 numerator = 0u - ((uint32)(sint32)points[i][0] * (uint32)(sint32)LS16(object + 752u) + (uint32)(sint32)points[i][2] * (uint32)(sint32)LS16(object + 756u));
            points[i][1] = (sint16)light_div(numerator, (uint32)(sint32)LS16(object + 754u));
        }
    for (j = 0u; j < 3u; ++j)
    {
        points[2][j] = (sint16)((points[0][j] + points[4][j]) / 2);
        points[3][j] = (sint16)((points[1][j] + points[5][j]) / 2);
    }
    relative[0] = (sint32)((uint32)((sint32)L32(object + 100u) >> 8) - L32(0x80081BB8u));
    relative[1] = (sint32)((uint32)((sint32)L32(object + 824u) >> 8) - L32(0x80081BBCu));
    relative[2] = (sint32)((uint32)((sint32)L32(object + 108u) >> 8) - L32(0x80081BC0u));
    light_camera_translation(relative);
    light_project_quads(packet, points);
}
