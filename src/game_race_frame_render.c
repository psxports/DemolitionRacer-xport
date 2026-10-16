#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void sub_80042B3C(uint32 buffer);
extern uint32 draft_agent3_missing_8003D9AC(uint32 mode);
#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))

static void render_pending(uint32 target, uint32 object, uint32 second)
{
    /* TODO: Translate this complete original callee when reached */
    draft_agent1_missing_diagnostic("Untranslated frame render %08X(%08X,%08X)\n", target, object, second);
    abort();
}

static sint32 render_rem(uint32 value, uint32 divisor)
{
    if (!divisor)
        xport_mips_break(7u);
    if (value == 0x80000000u && divisor == 0xFFFFFFFFu)
        xport_mips_break(6u);
    return (sint32)value % (sint32)divisor;
}

static sint32 render_distance(uint32 object, int nearest)
{
    uint32 length = R32(0x800653A4u), triple = length * 3u;
    uint32 bias = (uint32)((sint32)(triple + (triple >> 31)) >> 1);
    sint32 half = (sint32)length / 2;
    sint32 distance = (sint32)((uint32)render_rem((uint32)(sint32)(sint16)R16(0x80066D6Eu) - (uint32)(sint32)(sint16)R16(object + 2u) + bias, length) - (uint32)half);
    if (distance < 0)
        distance = (sint32)(0u - (uint32)distance);
    if (R32(0x80064C14u))
    {
        sint32 other = (sint32)((uint32)render_rem((uint32)(sint32)(sint16)R16(0x80067126u) - (uint32)(sint32)(sint16)R16(object + 2u) + bias, length) - (uint32)half);
        if (other < 0)
            other = (sint32)(0u - (uint32)other);
        if (!nearest || other < distance)
            distance = other;
    }
    return distance;
}

void game_800AA78C(void)
{
    uint32 index;
    for (index = 0u; index < 30u; ++index)
    {
        uint32 object = 0x800C4378u + index * 300u;
        sint32 distance;
        if (!R8(object + 295u))
            continue;
        if (R8(object + 299u))
        {
            R8(object + 299u) = (uint8)(R8(object + 299u) - 1u);
            if (R8(object + 299u))
                return;
            sub_80042B3C(R32(object + 120u));
            R8(object + 295u) = 0u;
            --R32(0x800C4268u);
            return;
        }
        distance = render_distance(object, 0);
        if (distance >= 61)
        {
            if (!R8(object + 299u))
                R8(object + 299u) = 4u;
            return;
        }
        R16(object + 284u) = (uint16)distance;
        if (distance < (sint16)R16(object + 290u) + 8 && R8(object + 297u) == 1u)
        {
            static const uint32 fields[5] = {24u, 32u, 104u, 108u, 112u};
            uint32 i;
            game_800AADF4(object);
            game_800AE020(object);
            game_800AB0D8(object);
            for (i = 0; i < 5u; ++i)
            {
                uint32 value = R32(object + fields[i]), damped = value * 250u;
                if ((sint32)damped < 0)
                    damped += 4095u;
                R32(object + fields[i]) = value - (uint32)((sint32)damped >> 12);
            }
            R32(object + 28u) -= 3000u;
        }
    }
}

void game_800AA544(void)
{
    uint32 index;
    sub_8003C0B4(128u, 128u, 128u);
    for (index = 0u; index < 30u; ++index)
    {
        uint32 object = 0x800C4378u + index * 300u, model;
        if (!R8(object + 295u) || R8(object + 299u))
            continue;
        {
            sint32 distance = render_distance(object, 1);
            R16(object + 290u) = 20u;
            if (distance >= 20)
                continue;
        }
        model = R32(object + 120u);
        R32(model + 16u) = R32(object + 12u);
        R32(model + 20u) = R32(object + 16u);
        R32(model + 24u) = R32(object + 20u);
        R8(object + 295u) |= 128u;
        if (sub_8003D188(model))
        {
            sub_8003CED8((const MATRIX *)psx_addr(object + 52u, sizeof(MATRIX)), model);
            sub_800416E0(model);
        }
    }
}

void dr_race_quaternion_matrix(const sint16 *quaternion, MATRIX *matrix)
{
    SVECTOR q, tail;
    VECTOR scaled, squared, tail_scaled;
    sint32 w = quaternion[3], yz;
    q.vx = quaternion[0];
    q.vy = quaternion[1];
    q.vz = quaternion[2];
    q.pad = 0;
    gte_gpf12(&q, w * 2, &scaled);
    scaled.vx = (sint32)xport_gte_read_data(25u);
    scaled.vy = (sint32)xport_gte_read_data(26u);
    scaled.vz = (sint32)xport_gte_read_data(27u);
    squared.vx = q.vx;
    squared.vy = q.vy;
    squared.vz = q.vz;
    squared.pad = 0;
    Square12(&squared, &squared);
    squared.vx >>= 12;
    squared.vy >>= 12;
    squared.vz >>= 12;
    tail.vx = 0;
    tail.vy = q.vy;
    tail.vz = q.vz;
    tail.pad = 0;
    gte_gpf12(&tail, q.vx * 2, &tail_scaled);
    tail_scaled.vy = (sint32)xport_gte_read_data(26u);
    tail_scaled.vz = (sint32)xport_gte_read_data(27u);
    yz = (sint32)((uint32)((sint32)q.vy * q.vz)) >> 11;
    matrix->m[2][0] = (sint16)((uint32)tail_scaled.vz - (uint32)scaled.vy);
    matrix->m[2][1] = (sint16)((uint32)yz + (uint32)scaled.vx);
    matrix->m[0][2] = (sint16)((uint32)tail_scaled.vz + (uint32)scaled.vy);
    matrix->m[1][0] = (sint16)((uint32)tail_scaled.vy + (uint32)scaled.vz);
    matrix->m[0][0] = (sint16)(4096u - (uint32)squared.vz * 2u - (uint32)squared.vy * 2u);
    matrix->m[0][1] = (sint16)((uint32)tail_scaled.vy - (uint32)scaled.vz);
    matrix->m[1][1] = (sint16)(4096u - (uint32)squared.vz * 2u - (uint32)squared.vx * 2u);
    matrix->m[1][2] = (sint16)((uint32)yz - (uint32)scaled.vx);
    matrix->m[2][2] = (sint16)(4096u - (uint32)squared.vx * 2u - (uint32)squared.vy * 2u);
    memset((uint8 *)matrix + 18u, 0, 2u);
}

static void render_quaternion(uint32 quaternion, MATRIX *matrix)
{
    dr_race_quaternion_matrix((const sint16 *)psx_addr(quaternion, 8u), matrix);
}

void game_8008F784(uint32 port)
{
    uint32 index, started = 0u;
    for (index = 0u; index < 10u; ++index)
    {
        uint32 object = R32(0x800BFF50u + index * 4u), model;
        MATRIX matrix;
        if (!object || R8(object + 156u) != 1u)
            continue;
        if (!started)
        {
            game_8009E320(0x80066D14u + port * 952u);
            started = 1u;
        }
        model = R32(object + 92u);
        if (!model)
            continue;
        R32(model + 16u) = R32(object + 40u);
        R32(model + 20u) = R32(object + 44u);
        R32(model + 24u) = R32(object + 48u);
        if (!sub_8003D188(model))
            continue;
        render_quaternion(object + 64u, &matrix);
        sub_8003CED8(&matrix, model);
        sub_800416E0(model);
    }
}

void game_800BB558(void)
{
    /* Native C uses the host stack; the original function has no other effect */
}

void game_800B3810(void)
{
    uint32 frame = R32(0x80065128u), row, column, count = 0u;
    PSX_RECT rectangle;
    memcpy(&rectangle, psx_addr(0x8008BC48u, 8u), 8u);
    if (!R32(0x800C7688u + (frame ^ 1u) * 4u))
    {
        R32(0x800C7450u) = 0u;
        return;
    }
    StoreImage(&rectangle, (uint32 *)psx_addr(0x800C7488u, 512u));
    draft_agent3_missing_8003D9AC(0u);
    for (row = 0u; row < 16u; row += 2u)
        for (column = 0u; column < 16u; column += 2u)
            if (R16(0x800C7488u + (row + column * 16u) * 2u) == 65535u)
                ++count;
    R32(0x800C7450u) = count * 4u;
    if (count)
        DrawOTag((uint32 *)psx_addr(0x800C73FCu + R32(0x80065128u) * 8u, 4u));
}

void game_800A9A68(void)
{
    uint32 object = R32(0x800C4174u);
    SetRotMatrix((MATRIX *)psx_addr(0x80081BA4u, sizeof(MATRIX)));
    xport_gte_write_control(5u, R32(0x800BEFA8u));
    xport_gte_write_control(6u, R32(0x800BEFACu));
    xport_gte_write_control(7u, R32(0x800BEFB0u));
    while (object != 0x800C4174u)
    {
        uint32 kind, i;
        if (R8(object + 11u) == 128u)
        {
            object = R32(object);
            continue;
        }
        kind = R8(object + 10u);
        if (!kind)
        {
            uint32 removed = object, next = R32(object), previous = R32(object + 4u);
            object = next;
            R32(next + 4u) = previous;
            R32(R32(removed + 4u)) = R32(removed);
            R32(R32(0x800C40ECu) + 4u) = removed;
            next = R32(0x800C40ECu);
            ++R32(0x800C4210u);
            --R32(0x800C4208u);
            R32(removed + 4u) = 0x800C40ECu;
            R32(removed) = next;
            R32(0x800C40ECu) = removed;
            continue;
        }
        R16(object + 8u) = (uint16)(R16(object + 8u) - 1u);
        if (!R16(object + 8u))
        {
            R8(object + 10u) = 0u;
            continue;
        }
        switch (kind)
        {
            case 1u:
            case 6u:
                game_800A9078(object);
                game_800A9504(object);
                break;
            case 2u:
            {
                uint32 frame;
                game_800A96C4(object);
                frame = R32(object + 124u) + R8(object + 133u) * 28u;
                --R8(object + 132u);
                if (!R8(object + 132u))
                {
                    R8(object + 133u) = R8(frame + 25u);
                    frame = R32(object + 124u) + R8(object + 133u) * 28u;
                    R8(object + 132u) = R8(frame + 24u);
                    R8(object + 132u) = R8(R32(object + 124u) + R8(object + 133u) * 28u + 24u);
                    memcpy(psx_addr(object + 128u, 4u), psx_addr(frame + 16u, 4u), 4u);
                    game_800A81F0(object + 12u, object + 124u);
                }
                for (i = 0; i < 3u; ++i)
                    R8(object + 128u + i) = (uint8)(R8(object + 128u + i) + R8(frame + 20u + i));
                break;
            }
            case 3u:
                break;
            case 4u:
                R16(object + 40u) = (uint16)(R16(object + 40u) - 1u);
                if ((sint16)R16(object + 40u) <= 0)
                {
                    render_pending(R32(object + 12u), object + 12u, 0u);
                    R16(object + 40u) = R16(object + 42u);
                }
                for (i = 0; i < 3u; ++i)
                {
                    uint32 value = R16(object + 28u + i * 2u) + R16(object + 34u + i * 2u);
                    R16(object + 28u + i * 2u) = (uint16)value;
                    R32(object + 16u + i * 4u) += (uint32)((sint32)(value << 16) >> 12);
                }
                break;
            case 5u:
                game_800A8DD8(object);
                for (i = 0; i < 3u; ++i)
                    R32(object + 64u + i * 4u) = R32(object + 52u + i * 4u);
                for (i = 0; i < 3u; ++i)
                {
                    uint32 value = R16(object + 76u + i * 2u) + R16(object + 82u + i * 2u);
                    R16(object + 76u + i * 2u) = (uint16)value;
                    R32(object + 52u + i * 4u) += (uint32)((sint32)(value << 16) >> 12);
                }
                break;
            default:
                break;
        }
        object = R32(object);
    }
}

static void render_link(uint32 packet, sint32 depth)
{
    uint32 environment = R32(0x80065118u), ot = R32(environment + 112u) + (uint32)depth * 4u;
    R32(packet) = (R32(packet) & 0xFF000000u) | (R32(ot) & 0xFFFFFFu);
    ot = R32(environment + 112u) + (uint32)depth * 4u;
    R32(ot) = (R32(ot) & 0xFF000000u) | (packet & 0xFFFFFFu);
}

void game_800BC0B4(void)
{
    uint32 object = R32(0x800C8AB4u);
    while (object != 0x800C8AB4u)
    {
        uint32 kind, count, i, packet, age;
        MATRIX matrix;
        SVECTOR positions[4];
        VECTOR translation, rotated;
        sint32 screens[3], depths[3], screen4, depth4, flags, depth;
        --R8(object + 8u);
        if (!R8(object + 8u))
        {
            uint32 removed = object, next = R32(object), previous = R32(object + 4u);
            object = next;
            R32(next + 4u) = previous;
            R32(R32(removed + 4u)) = R32(removed);
            R32(R32(0x800C89E8u) + 4u) = removed;
            next = R32(0x800C89E8u);
            R32(removed + 4u) = 0x800C89E8u;
            R32(removed) = next;
            R32(0x800C89E8u) = removed;
            continue;
        }
        for (i = 0; i < 3u; ++i)
            R16(object + 24u + i * 2u) = (uint16)(R16(object + 24u + i * 2u) + R16(object + 32u + i * 2u));
        memset(&matrix, 0, sizeof(matrix));
        matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 4096;
        sub_8003C960(&matrix.m[0][0], (const sint16 *)psx_addr(object + 24u, 6u));
        kind = R8(object + 9u);
        count = (kind == 1u || kind == 3u) ? 4u : 3u;
        for (i = 0; i < count; ++i)
        {
            positions[i].vx = (sint16)R16(object + 60u + i * 8u);
            positions[i].vy = (sint16)R16(object + 62u + i * 8u);
            positions[i].vz = (sint16)R16(object + 64u + i * 8u);
            {
                uint32 registers[5], xy = (uint16)positions[i].vx | ((uint32)(uint16)positions[i].vy << 16);
                sint32 output[3];
                memcpy(registers, &matrix, 20u);
                gte_rotate_packed(registers, xy, (uint16)positions[i].vz, output);
                positions[i].vx = (sint16)output[0];
                positions[i].vy = (sint16)output[1];
                positions[i].vz = (sint16)output[2];
            }
            positions[i].vx = (sint16)((uint16)positions[i].vx + R32(object + 12u));
            positions[i].vy = (sint16)((uint16)positions[i].vy + R32(object + 16u));
            positions[i].vz = (sint16)((uint16)positions[i].vz + R32(object + 20u));
        }
        SetRotMatrix((MATRIX *)psx_addr(0x80081BA4u, sizeof(MATRIX)));
        translation.vx = (sint32)((uint32)((sint32)R32(object + 40u) >> 8) - R32(0x80081BB8u));
        translation.vy = (sint32)((uint32)((sint32)R32(object + 44u) >> 8) - R32(0x80081BBCu));
        translation.vz = (sint32)((uint32)((sint32)R32(object + 48u) >> 8) - R32(0x80081BC0u));
        translation.pad = 0;
        ApplyMatrixLV((MATRIX *)psx_addr(0x80081BA4u, sizeof(MATRIX)), &translation, &rotated);
        xport_gte_write_control(5u, (uint32)rotated.vx);
        xport_gte_write_control(6u, (uint32)rotated.vy);
        xport_gte_write_control(7u, (uint32)rotated.vz);
        gte_project3_full_depth(positions, screens, depths, &flags);
        if (kind < 4u)
        {
            uint32 stride = kind == 0u ? 32u : (kind == 3u ? 52u : 40u);
            packet = object + 100u + R32(0x80065128u) * stride;
            R32(packet + 8u) = (uint32)screens[0];
            R32(packet + (kind < 2u ? 16u : 20u)) = (uint32)screens[1];
            R32(packet + (kind < 2u ? 24u : 32u)) = (uint32)screens[2];
            if (!((uint32)flags & 0x01C66000u))
            {
                if (kind == 1u || kind == 3u)
                {
                    depth4 = gte_project_full_depth(&positions[3], &screen4, NULL);
                    packet = object + 100u + R32(0x80065128u) * stride;
                    R32(packet + (kind == 1u ? 32u : 44u)) = (uint32)screen4;
                    depth = AverageZ4(depths[0], depths[1], depths[2], depth4);
                }
                else
                    depth = AverageZ3(depths[0], depths[1], depths[2]);
                if (depth > 0 && depth < (sint32)R32(0x800654F0u))
                {
                    packet = object + 100u + R32(0x80065128u) * stride;
                    render_link(packet, depth);
                }
            }
        }
        age = R32(object + 92u) + 1u;
        R32(object + 92u) = age;
        if (positions[0].vy < (sint32)R32(object + 96u))
        {
            if ((sint32)age >= 3)
            {
                sint32 velocity = (sint16)R16(object + 54u);
                R32(object + 92u) = 0u;
                R16(object + 54u) = (uint16)(-velocity * 90 / 100);
            }
            else
                R8(object + 8u) = 2u;
        }
        for (i = 0; i < 3u; ++i)
            R32(object + 12u + i * 4u) += (uint32)(sint32)(sint16)R16(object + 52u + i * 2u);
        R16(object + 54u) = (uint16)(R16(object + 54u) - 5u);
        object = R32(object);
    }
}

void game_800A9E24(void)
{
    uint32 object = R32(0x800C4174u);
    FUNCTION_MARKER(0x800A9E24u, "GAME.BIN");
    SetRotMatrix((MATRIX *)psx_addr(0x80081BA4u, sizeof(MATRIX)));
    xport_gte_write_control(5u, R32(0x800BEFB8u));
    xport_gte_write_control(6u, R32(0x800BEFBCu));
    xport_gte_write_control(7u, R32(0x800BEFC0u));
    while (object != 0x800C4174u)
    {
        if (R8(object + 11u) != 128u)
        {
            uint32 kind = R8(object + 10u);
            if (kind == 1u || kind == 6u)
                game_800A9078(object);
            else if (kind == 2u)
                game_800A96C4(object);
            else if (kind == 5u)
                game_800A8DD8(object);
        }
        object = R32(object);
    }
}
