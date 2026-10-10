#include "draft_signatures.h"
extern void draft_agent3_missing_80049C84(const MATRIX *matrix);
extern uint32 sub_8002C188(uint32 state, uint32 position);
extern uint32 sub_8002DC38(uint32 state, uint32 position, uint32 height_output);

static void alignment_transform(uint32 vertex, uint32 output)
{
    uint32 axis;
    xport_gte_write_data(0u, r_u32(vertex));
    xport_gte_write_data(1u, r_u32(vertex + 4u));
    xport_gte_execute(0x480012u);
    for (axis = 0u; axis < 3u; ++axis)
        w_u32(output + axis * 4u, xport_gte_read_data(25u + axis));
}

static uint32 alignment_ground(uint32 object, uint32 index, uint32 position)
{
    uint32 state = object + 398u + index * 10u, height = 0x800C08D8u + index * 4u, axis;
    for (axis = 0u; axis < 3u; ++axis)
        w_u32(position + axis * 4u, r_u32(position + axis * 4u) << 8);
    sub_8002C188(state, position);
    sub_8002DC38(state, position, height);
    return (sint32)(r_u32(position + 4u) - r_u32(height)) < 0 ? 1u << index : 0u;
}

uint32 game_80095D6C(uint32 object)
{
    uint32 model = r_u32(object), index, axis, result = 0u;
    uint16 section = r_u16(object + 88u);
    uint8 lane = r_u8(object + 96u);
    draft_agent3_missing_80049C84((const MATRIX *)psx_addr(object + 148u, 32u));
    /* Original SDK 4E5C4 loads translation from the final three matrix words */
    for (axis = 0u; axis < 3u; ++axis)
        xport_gte_write_control(5u + axis, r_u32(object + 168u + axis * 4u));
    for (index = 0u; index < 4u; ++index)
    {
        uint32 state = object + 398u + index * 10u, position = object + 480u + index * 12u;
        uint32 wheel = object + 624u + index * 8u, wheel_output = object + 760u + index * 16u;
        uint32 suspension = r_u32(object + 840u + index * 4u);
        w_u16(state, section);
        w_u8(state + 8u, lane);
        alignment_transform(model + index * 8u, position);
        if ((sint32)suspension < 0)
            suspension += 255u;
        w_u16(wheel + 2u, r_u16(r_u32(object) + index * 8u + 66u) - (uint32)((sint32)suspension >> 8));
        alignment_transform(wheel, wheel_output);
        for (axis = 0u; axis < 3u; ++axis)
            w_u32(object + 576u + index * 12u + axis * 4u, r_u32(wheel_output + axis * 4u) << 8);
        result |= alignment_ground(object, index, position);
    }
    for (index = 4u; index < 8u; ++index)
    {
        uint32 state = object + 398u + index * 10u, position = object + 480u + index * 12u;
        w_u16(state, section);
        w_u8(state + 8u, lane);
        alignment_transform(model + index * 8u, position);
        result |= alignment_ground(object, index, position);
    }
    return result;
}
