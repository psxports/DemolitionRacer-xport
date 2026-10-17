#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
uint32 sub_80037580(uint32 object, uint32 update, uint32 port);
uint32 sub_800379D4(uint32 object, uint32 port);
uint32 sub_80038020(uint32 object, uint32 port);
uint32 sub_80038774(uint32 object, uint32 port);
uint32 sub_80038638(uint32 object, uint32 port);
uint32 sub_80038258(uint32 object, uint32 port, uint32 pattern);

uint32 sub_80038ABC(uint32 object, uint32 port)
{
    uint32 offset = port * 4u, old = r_u32(0x800650FCu + offset), index = 0u;
    uint32 count = r_u32(0x80065408u), table = r_u32(0x80065404u), record, section, vertex, mode, result;
    sint32 track = (sint16)r_u16(object + 88u);
    while ((sint32)index < (sint32)count)
    {
        record = table + index * 16u;
        if (track >= (sint32)r_u16(record + 2u) && track <= (sint32)r_u16(record + 4u))
        {
            w_u32(0x800650FCu + offset, index);
            break;
        }
        ++index;
    }
    index = r_u32(0x800650FCu + offset);
    result = index * 16u;
    if (old == index)
        return result;
    record = table + result;
    vertex = r_u16(record + 8u);
    section = r_u16(record + 6u);
    w_u32(0x80065424u + offset, vertex);
    w_u32(0x800654E0u + offset, section);
    section = r_u32(0x800653D4u) + section * 24u;
    vertex = r_u32(0x800653C4u) + (r_u16(section + 4u) + vertex) * 6u;
    w_u32(0x8006540Cu + offset, ((uint32)(sint32)(sint16)r_u16(vertex) + r_u32(section + 12u)) << 8);
    w_u32(0x80065414u + offset, ((uint32)(sint32)(sint16)r_u16(vertex + 4u) + r_u32(section + 20u)) << 8);
    w_u32(0x800654E8u + offset, 0u);
    w_u32(0x8006541Cu + offset, r_u16(record + 10u));
    mode = r_u16(record);
    w_u32(0x8006545Cu + offset, mode);
    if (mode >= 11u)
        return mode * 4u;
    result = mode * 4u;
    switch (mode)
    {
        case 0u:
            w_u32(0x800654D8u + offset, 0u);
            w_u32(0x800650F4u + offset, 20000u);
            result = 20000u;
            break;
        case 1u:
        case 2u:
            w_u32(0x800654D8u + offset, 0u);
            w_u16(0x8007A980u + port * 8u, (uint16)(mode == 1u ? 512 : -512));
            w_u16(0x8007A982u + port * 8u, 196u);
            w_u16(0x8007A984u + port * 8u, (uint16)-1024);
            result = (uint32)-1024;
            break;
        case 3u:
            w_u32(0x800654D8u + offset, 0u);
            result = offset;
            break;
        case 4u:
        case 5u:
        case 9u:
        case 10u:
            result = r_u16(record + 10u);
            w_u32(0x8006541Cu + offset, result);
            break;
        case 6u:
            w_u32(0x800654D8u + offset, 0u);
            w_u32(0x800650F4u + offset, 20000u);
            w_u32(0x80065434u + offset, 0u);
            w_u32(0x8006543Cu + offset, r_u16(record + 14u));
            result = r_u16(record + 12u);
            w_u32(0x80065444u + offset, result);
            break;
        case 7u:
        case 8u:
            if (mode == 7u)
                w_u32(0x800654D8u + offset, 0u);
            w_u16(0x8007A980u + port * 8u, 0u);
            w_u16(0x8007A982u + port * 8u, (uint16)-200);
            w_u16(0x8007A984u + port * 8u, 0u);
            result = 1u;
            w_u32(0x800654E8u + offset, result);
            break;
    }
    return result;
}

uint32 sub_80038E8C(uint32 object, uint32 port)
{
    uint32 offset = port * 4u, mode = r_u32(0x8006545Cu + offset), result;
    switch (mode)
    {
        case 0u:
            sub_80038020(object, port);
            break;
        case 1u:
        case 2u:
            w_u32(0x8006516Cu, 4096u);
            sub_80038774(object, port);
            break;
        case 3u:
            w_u32(0x8006516Cu, 4096u);
            sub_80038638(object, port);
            break;
        case 4u:
        case 5u:
            w_u32(0x8006516Cu, 4096u);
            w_u32(0x800654D8u + offset, r_u32(0x800654D8u + offset) + (mode == 4u ? 8u : (uint32)-8));
            sub_80037580(object, 1u, port);
            break;
        case 6u:
            sub_80038258(object, port, r_u32(0x80065444u + offset));
            break;
        case 7u:
        case 8u:
            w_u32(0x8006516Cu, 4096u);
            if (mode == 8u)
                w_u32(0x800654D8u + offset, 2048u);
            sub_800379D4(object, port);
            break;
        case 9u:
        case 10u:
            w_u32(0x8006516Cu, 4096u);
            w_u32(0x800654D8u + offset, mode == 9u ? 2048u : 0u);
            sub_80037580(object, 1u, port);
            break;
    }
    result = r_u32(0x8006516Cu);
    if (r_u32(0x8006544Cu + offset) != result)
    {
        sub_8003BC1C();
        result = r_u32(0x8006516Cu);
    }
    w_u32(0x8006544Cu + offset, result);
    return result;
}

static uint32 mode_shift(uint32 word)
{
    if ((sint32)word < 0)
        word += 4095u;
    return (uint32)((sint32)word >> 12);
}

static void mode_static_position(uint32 port)
{
    uint32 offset = port * 4u, camera = 0x8007A9D4u + port * 12u;
    uint32 section = r_u32(0x800653D4u) + r_u32(0x800654E0u + offset) * 24u;
    uint32 vertex = r_u32(0x800653C4u) + (r_u16(section + 4u) + r_u32(0x80065474u + offset)) * 6u;
    w_u32(camera, r_u32(0x8006540Cu + offset));
    w_u32(camera + 4u, (r_u32(section + 16u) + (uint32)(sint32)(sint16)r_u16(vertex + 2u) + r_u32(0x8006541Cu + offset)) << 8);
    w_u32(camera + 8u, r_u32(0x80065414u + offset));
}

static uint32 mode_look_zoom(uint32 object, uint32 port)
{
    sint32 target[3];
    uint32 i, sum = 0u, distance, product, result;
    uint32 camera = 0x8007A9D4u + port * 12u, offset = port * 4u;
    for (i = 0u; i < 3u; ++i)
        target[i] = (sint32)r_u32(object + 100u + i * 4u);
    sub_8003C034((const sint32 *)psx_addr(camera, 12u));
    sub_8003C4B0(target, r_u32(0x80065104u + offset));
    for (i = 0u; i < 3u; ++i)
    {
        uint32 delta = (uint32)((sint32)(r_u32(camera + i * 4u) - (uint32)target[i]) >> 10);
        sum += delta * delta;
    }
    distance = dr_race_motion_sqrt(sum) * 4u;
    product = distance * r_u32(0x800650F4u + offset);
    w_u32(0x80065454u + offset, distance);
    result = mode_shift(product);
    w_u32(0x8006542Cu + offset, result);
    if ((sint32)result < 4096)
        result = 4096u;
    else if ((sint32)result > 20000)
        result = 20000u;
    w_u32(0x8006542Cu + offset, result);
    w_u32(0x8006516Cu, result);
    return result;
}

uint32 sub_80038020(uint32 object, uint32 port)
{
    mode_static_position(port);
    return mode_look_zoom(object, port);
}

uint32 sub_80038638(uint32 object, uint32 port)
{
    sint32 target[3];
    uint32 i;
    mode_static_position(port);
    for (i = 0u; i < 3u; ++i)
        target[i] = (sint32)r_u32(object + 100u + i * 4u);
    target[1] = (sint32)((uint32)target[1] + r_u32(0x800654ACu + port * 4u));
    sub_8003C034((const sint32 *)psx_addr(0x8007A9D4u + port * 12u, 12u));
    return sub_8003C4B0(target, r_u32(0x80065104u + port * 4u));
}

uint32 sub_80038774(uint32 object, uint32 port)
{
    sint16 angles[3];
    uint32 i, camera = 0x8007A9D4u + port * 12u, result;
    angles[0] = (sint16)(r_u16(object + 40u) + 2048u);
    angles[1] = (sint16)(2048u - r_u16(object + 42u));
    angles[2] = (sint16)(0u - r_u16(object + 44u));
    dr_camera_angles(angles);
    for (i = 0u; i < 5u; ++i)
        xport_gte_write_control(i, r_u32(object + 148u + i * 4u));
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

uint32 sub_80038258(uint32 object, uint32 port, uint32 pattern)
{
    uint8 patterns[80];
    sint32 points[4][3], relative[4][3];
    uint32 offset = port * 4u, time = r_u32(0x80065434u + offset), step = r_u32(0x8006543Cu + offset);
    uint32 i, j, k, section, vertex, curve = 0x8007A8E8u + port * 60u, camera = 0x8007A9D4u + port * 12u, square, cube;
    for (i = 0u; i < 80u; ++i)
        patterns[i] = r_u8(0x80011F48u + i);
    if (pattern >= 5u)
    {
        draft_agent1_missing_diagnostic("Original camera pattern outside local table: %u\n", pattern);
        abort();
    }
    time = (sint32)time < (sint32)(4095u - step) ? time + step : 4095u;
    w_u32(0x80065434u + offset, time);
    for (i = 0u; i < 4u; ++i)
    {
        uint32 position = pattern * 16u + i * 4u;
        uint32 section_delta = (uint32)(sint32)(sint16)((uint16)patterns[position] | ((uint16)patterns[position + 1u] << 8));
        uint32 vertex_delta = (uint32)(sint32)(sint16)((uint16)patterns[position + 2u] | ((uint16)patterns[position + 3u] << 8));
        section = r_u32(0x800653D4u) + (r_u32(0x800654E0u + offset) + section_delta) * 24u;
        vertex = r_u32(0x800653C4u) + (r_u16(section + 4u) + r_u32(0x80065424u + offset) + vertex_delta) * 6u;
        points[i][0] = (sint32)(((uint32)(sint32)(sint16)r_u16(vertex) + r_u32(section + 12u)) << 8);
        points[i][2] = (sint32)(((uint32)(sint32)(sint16)r_u16(vertex + 4u) + r_u32(section + 20u)) << 8);
        points[i][1] = (sint32)(((uint32)(sint32)(sint16)r_u16(vertex + 2u) + r_u32(section + 16u) + r_u32(0x8006541Cu + offset)) << 8);
    }
    for (i = 0u; i < 3u; ++i)
        w_u32(curve + i * 4u, (uint32)points[1][i]);
    for (j = 0u; j < 4u; ++j)
        for (i = 0u; i < 3u; ++i)
            relative[j][i] = (sint32)((uint32)points[j][i] - (uint32)points[1][i]) >> 8;
    for (j = 0u; j < 4u; ++j)
        for (i = 0u; i < 3u; ++i)
        {
            uint32 sum = 0u;
            for (k = 0u; k < 4u; ++k)
                sum += r_u32(0x8005F2B8u + (j * 4u + k) * 4u) * (uint32)relative[k][i];
            w_u32(curve + 12u + (j * 3u + i) * 4u, (uint32)((sint32)sum / 2));
        }
    time = r_u32(0x80065434u + offset);
    square = (uint32)((sint32)(time * time) >> 12);
    cube = (uint32)((sint32)(square * time) >> 12);
    for (i = 0u; i < 3u; ++i)
    {
        uint32 sum = r_u32(curve + 12u + i * 4u) * cube + r_u32(curve + 24u + i * 4u) * square + r_u32(curve + 36u + i * 4u) * time;
        w_u32(camera + i * 4u, (((uint32)((sint32)sum >> 12) + r_u32(curve + 48u + i * 4u)) << 8) + r_u32(curve + i * 4u));
    }
    return mode_look_zoom(object, port);
}
