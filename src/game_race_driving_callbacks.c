#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
extern uint32 sub_8002C188(uint32 state, uint32 position);
extern void sub_8002EDE0(uint32 state, uint32 position, uint32 normal);
extern uint32 sub_8002E408(uint32 state, uint32 position, uint32 normal, uint32 mode);
extern void game_80097080(uint32 object);
extern void game_8009D5F0(uint32 angles, uint32 output);
extern uint32 sub_8002F5F8(uint32 angles, uint32 positions, uint32 heights);
extern uint32 sub_8002F78C(uint32 angles, uint32 positions, uint32 heights);
extern uint32 sub_8002F860(uint32 angles, uint32 positions, uint32 heights);

void game_8009A8E8(uint32 object)
{
    uint32 axis, kind, index, sum = 0u, contacts = 0u;
    uint32 rotation[5];
    uint32 angles[3], position[3];
    for (axis = 0u; axis < 3u; ++axis)
        angles[axis] = R32(object + 124u + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis)
        position[axis] = R32(object + 100u + axis * 4u);
    for (axis = 0u; axis < 3u; ++axis)
        R16(object + 40u + axis * 2u) = (uint16)((sint32)angles[axis] >> 8);
    for (axis = 0u; axis < 3u; ++axis)
        R32(object + 168u + axis * 4u) = (uint32)((sint32)position[axis] >> 8);
    sub_8003C960((sint16 *)psx_addr(object + 148u, 18u), (const sint16 *)psx_addr(object + 40u, 6u));
    game_8009D5F0(object + 40u, object + 252u);
    sub_8002C188(object + 88u, object + 100u);
    sub_8002EDE0(object + 88u, object + 100u, object + 752u);
    game_80097080(object);
    kind = R8(object + 942u);
    switch (kind)
    {
        case 0u:
        case 1u:
        case 2u:
        case 4u:
        case 6u:
        case 8u:
        case 9u:
            sub_8002F5F8(object + 40u, object + 576u, object + 840u);
            R32(object + 124u) = (uint32)(sint32)(sint16)R16(object + 40u) << 8;
            R32(object + 132u) = (uint32)(sint32)(sint16)R16(object + 44u) << 8;
            break;
        case 3u:
        case 12u:
            sub_8002F860(object + 40u, object + 576u, object + 840u);
            R32(object + 132u) = (uint32)(sint32)(sint16)R16(object + 44u) << 8;
            break;
        case 5u:
        case 10u:
            sub_8002F78C(object + 40u, object + 576u, object + 840u);
            R32(object + 124u) = (uint32)(sint32)(sint16)R16(object + 40u) << 8;
            break;
        default:
            break;
    }
    sub_8003C960((sint16 *)psx_addr(object + 148u, 18u), (const sint16 *)psx_addr(object + 40u, 6u));
    game_8009D5F0(object + 40u, object + 252u);
    for (index = 0u; index < 5u; ++index)
        rotation[index] = R32(object + 148u + index * 4u);
    for (index = 0u; index < 5u; ++index)
        xport_gte_write_control(index, rotation[index]);
    for (index = 0u; index < 4u; ++index)
    {
        if (((R8(object + 942u) >> index) & 1u) == 0u)
        {
            uint32 suspension = R32(object + 840u + index * 4u), vector = object + 624u + index * 8u, output = object + 656u + index * 8u;
            uint32 height = R16(R32(object) + 66u + index * 8u);
            sint32 result[3];
            if ((sint32)suspension < 0)
                suspension += 255u;
            height -= (uint32)((sint32)suspension >> 8);
            height -= R32(0x800C0920u);
            R16(vector + 2u) = (uint16)height;
            gte_rotate_packed(rotation, R32(vector), R32(vector + 4u), result);
            R16(output) = (uint16)result[0];
            R16(output + 2u) = (uint16)result[1];
            R16(output + 4u) = (uint16)result[2];
            R16(vector + 2u) = (uint16)(R16(vector + 2u) + R32(0x800C0920u));
            ++contacts;
            sum += R32(object + 580u + index * 12u) - ((uint32)(sint32)(sint16)R16(output + 2u) << 8);
        }
    }
    if (contacts)
        R32(object + 104u) = (uint32)((sint32)sum / (sint32)contacts);
}

extern uint32 game_80095D6C(uint32 object);

void game_80096D84(uint32 object)
{
    uint32 value, position[3], axis;
    sub_8002C188(object + 88u, object + 100u);
    value = sub_8002E408(object + 88u, object + 100u, object + 752u, 0u);
    for (axis = 0u; axis < 3u; ++axis)
        position[axis] = R32(object + 100u + axis * 4u);
    R32(object + 824u) = value;
    for (axis = 0u; axis < 3u; ++axis)
        R32(object + 168u + axis * 4u) = (uint32)((sint32)position[axis] >> 8);
    game_80095D6C(object);
}
