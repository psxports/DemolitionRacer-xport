#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
#define GP 0x80064A10u
uint32 sub_80043450(uint32 x, uint32 z);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);

static void frame_apply(uint32 matrix, VECTOR *input, VECTOR *output)
{
    MATRIX *native = (MATRIX *)psx_addr(matrix, sizeof(MATRIX));
    SetRotMatrix(native);
    ApplyMatrixLV(native, input, output);
}

static uint32 frame_visible(uint32 object)
{
    VECTOR point, rotated;
    uint32 shift = R32(GP + 0x760u) & 31u, value;
    sint32 radius = (sint16)R16(object + 8u) >> shift;
    sint32 z, x, y, coefficient;
    point.vx = (sint32)((uint32)((sint32)R32(object + 16u) >> 8) + R32(object + 28u));
    point.vy = (sint32)((uint32)((sint32)R32(object + 20u) >> 8) + R32(object + 32u));
    point.vz = (sint32)((uint32)((sint32)R32(object + 24u) >> 8) + R32(object + 36u));
    point.pad = 0;
    frame_apply(0x80081BA4u, &point, &rotated);
    z = rotated.vz;
    x = rotated.vx;
    y = rotated.vy;
    if ((sint32)((uint32)z + (uint32)radius) <= 0)
        return 0u;
    if ((sint32)((uint32)z - (uint32)radius) >= ((sint32)R32(GP + 0x73Cu) >> shift))
        return 0u;
    coefficient = (sint16)R16(GP + 0xB40u);
    value = (uint32)x * (uint32)coefficient + (uint32)z * (uint32)(sint32)(sint16)R16(0x80065554u);
    if (((sint32)value >> 12) >= radius)
        return 0u;
    value = (0u - (uint32)x) * (uint32)coefficient + (uint32)z * (uint32)(sint32)(sint16)R16(0x80065554u);
    if (((sint32)value >> 12) >= radius)
        return 0u;
    coefficient = (sint16)R16(0x80065562u);
    value = (uint32)y * (uint32)coefficient + (uint32)z * (uint32)(sint32)(sint16)R16(0x80065564u);
    if (((sint32)value >> 12) >= radius)
        return 0u;
    value = (0u - (uint32)y) * (uint32)coefficient + (uint32)z * (uint32)(sint32)(sint16)R16(0x80065564u);
    if (((sint32)value >> 12) >= radius)
        return 0u;
    R32(GP + 0xB38u) = (uint32)z;
    return 1u;
}

static void frame_translation(uint32 object, sint32 y_offset)
{
    VECTOR input, output;
    input.vx = (sint32)R32(object + 16u) >> 8;
    input.vy = (sint32)((uint32)((sint32)R32(object + 20u) >> 8) + (uint32)y_offset);
    input.vz = (sint32)R32(object + 24u) >> 8;
    input.pad = 0;
    frame_apply(0x80081BA4u, &input, &output);
    R32(0x80081C78u) = (uint32)output.vx;
    R32(0x80081C7Cu) = (uint32)output.vy;
    R32(0x80081C80u) = (uint32)output.vz;
    xport_gte_write_control(5u, R32(0x80081C78u));
    xport_gte_write_control(6u, R32(0x80081C7Cu));
    xport_gte_write_control(7u, R32(0x80081C80u));
}

static uint32 frame_outcode(sint32 screen)
{
    sint32 x = (sint16)screen, y = (sint16)((uint32)screen >> 16);
    uint32 code = 15u;
    if (x >= 0)
    {
        code &= 14u;
        if (x < 512)
            code &= 11u;
    }
    if (y >= 0)
    {
        code &= 13u;
        if (y < 240)
            code &= 7u;
    }
    return code;
}

static void frame_render(uint32 object, uint32 visibility)
{
    uint32 group = object + R32(object + 48u), vertices = object + R32(object + 44u);
    uint32 packet = object + R32(object + 52u + R32(0x80065128u) * 4u);
    uint32 ot = R32(R32(0x80065118u) + 112u) + (R32(0x800654F0u) - 1u) * 4u;
    for (;;)
    {
        sint32 count = (sint16)R16(group);
        uint32 kind = R8(group + 2u);
        if (!count)
            return;
        if (kind > 1u)
        {
            fprintf(stderr, "Unsupported original 4ECE4 primitive kind %u\n", kind);
            abort();
        }
        group += 4u;
        do
        {
            if ((sint8)R8(visibility++))
            {
                SVECTOR input[3], fourth;
                sint32 screens[3], depths[3], last;
                uint32 i, reject = 15u, bytes = kind ? 0x09000000u : 0x07000000u;
                for (i = 0u; i < 3u; ++i)
                    memcpy(&input[i], psx_addr(vertices + (uint32)(sint32)(sint16)R16(group + i * 2u) * 8u, 8u), 8u);
                gte_project3_full_depth(input, screens, depths, NULL);
                R32(packet + 8u) = (uint32)screens[0];
                R32(packet + 16u) = (uint32)screens[1];
                R32(packet + 24u) = (uint32)screens[2];
                if (kind)
                {
                    for (i = 0u; i < 3u; ++i)
                        reject &= frame_outcode(screens[i]);
                    memcpy(&fourth, psx_addr(vertices + (uint32)(sint32)(sint16)R16(group + 6u) * 8u, 8u), 8u);
                    (void)gte_project_full_depth(&fourth, &last, NULL);
                    R32(packet + 32u) = (uint32)last;
                    reject &= frame_outcode(last);
                }
                else
                    reject = 0u;
                if (!reject)
                {
                    uint32 head = R32(ot);
                    R32(ot) = packet & 0xFFFFFFu;
                    R32(packet) = bytes | (head & 0xFFFFFFu);
                }
            }
            group += 20u;
            packet += kind ? 40u : 32u;
        } while (--count);
    }
}

void game_800A7794(uint32 argument)
{
    uint32 group, index, bank = (argument >> 31) * 16u;
    for (group = 0u; group < 3u; ++group)
        for (index = 0u; index < 6u; ++index)
        {
            uint32 object = R32(R32(0x800651E0u) + 8u + group * 24u + index * 4u);
            if (frame_visible(object))
            {
                frame_translation(object, (sint16)R16(0x800BEF6Cu + R32(0x80064C08u) * 2u));
                frame_render(object, 0x800BEEDCu + group * 48u + bank);
            }
        }
}

void game_800B3638(uint32 object)
{
    /* The original function consists of JR RA and a NOP delay slot */
}

void game_800B13A8(void)
{
    uint32 offset;
    for (offset = 0u; offset < 0x2E40u; offset += 148u)
    {
        uint32 object = R32(0x800C6BE4u) + offset, packet, i;
        VECTOR input, rotated;
        SVECTOR first[3], fourth;
        sint32 screen[3], depth[3], screen4, depth4, area, area4, average;
        if (!(R8(object + 112u) & 2u))
            continue;
        R16(object + 114u) = (uint16)(R16(object + 114u) + 1u);
        if ((sint16)R16(object + 114u) >= 601)
            R8(object + 112u) = 0u;
        object = R32(0x800C6BE4u) + offset;
        if ((sint16)R16(object + 114u) >= 301)
        {
            if (!((sint16)R16(object + 114u) & 8) && R8(object + 113u))
                --R8(object + 113u);
            for (i = 0u; i < 3u; ++i)
            {
                object = R32(0x800C6BE4u) + offset;
                R8(object + R32(0x80065128u) * 40u + (i == 0u ? 37u : (i == 1u ? 36u : 38u))) = R8(object + 113u);
            }
        }
        object = R32(0x800C6BE4u) + offset;
        input.vx = (sint32)((uint32)((sint32)R32(object + 116u) >> 8) - R32(0x80081BB8u));
        input.vy = (sint32)((uint32)((sint32)R32(object + 120u) >> 8) - R32(0x80081BBCu));
        input.vz = (sint32)((uint32)((sint32)R32(object + 124u) >> 8) - R32(0x80081BC0u));
        input.pad = 0;
        frame_apply(0x80081BA4u, &input, &rotated);
        R32(0x80081C78u) = (uint32)rotated.vx;
        R32(0x80081C7Cu) = (uint32)rotated.vy;
        R32(0x80081C80u) = (uint32)rotated.vz;
        xport_gte_write_control(5u, R32(0x80081C78u));
        xport_gte_write_control(6u, R32(0x80081C7Cu));
        xport_gte_write_control(7u, R32(0x80081C80u));
        object = R32(0x800C6BE4u) + offset;
        for (i = 0u; i < 3u; ++i)
            memcpy(&first[i], psx_addr(object + i * 8u, 8u), 8u);
        gte_project3_full_depth(first, screen, depth, NULL);
        area = NormalClip(screen[0], screen[1], screen[2]);
        packet = R32(0x800C6BE4u) + offset + 32u + R32(0x80065128u) * 40u;
        R32(packet + 8u) = (uint32)screen[0];
        R32(packet + 16u) = (uint32)screen[1];
        R32(packet + 24u) = (uint32)screen[2];
        memcpy(&fourth, psx_addr(R32(0x800C6BE4u) + offset + 24u, 8u), 8u);
        depth4 = gte_project_full_depth(&fourth, &screen4, NULL);
        area4 = NormalClip(screen[1], screen[2], screen4);
        average = AverageZ4(depth[0], depth[1], depth[2], depth4);
        if (area > 0 && area4 < 0)
            continue;
        packet = R32(0x800C6BE4u) + offset + 32u + R32(0x80065128u) * 40u;
        R32(packet + 32u) = (uint32)screen4;
        average = (sint32)((uint32)average + 64u);
        if (average > 64 && average < (sint32)R32(0x800654F0u))
        {
            uint32 environment = R32(0x80065118u), ot = R32(environment + 112u) + (uint32)average * 4u;
            R32(packet) = (R32(packet) & 0xFF000000u) | (R32(ot) & 0xFFFFFFu);
            ot = R32(environment + 112u) + (uint32)average * 4u;
            R32(ot) = (R32(ot) & 0xFF000000u) | (packet & 0xFFFFFFu);
        }
    }
}

uint32 dr_race_particle_slot(void)
{
    sint32 index = (sint32)R32(0x800C6C20u);
    uint32 object;
    if (index < 80)
    {
        object = R32(0x800C6BE4u) + (uint32)index * 148u;
        while (index < 80)
        {
            if (!(R8(object + 112u) & 1u))
            {
                R32(0x800C6C20u) = (uint32)index;
                return (uint32)index;
            }
            ++index;
            object += 148u;
        }
    }
    object = R32(0x800C6BE4u);
    for (index = 0; index < 80; ++index, object += 148u)
        if (!(R8(object + 112u) & 1u))
        {
            R32(0x800C6C20u) = (uint32)index;
            return (uint32)index;
        }
    return ~0u;
}

static sint32 frame_product12(uint32 a, uint32 b)
{
    uint32 product = a * b;
    if ((sint32)product < 0)
        product += 4095u;
    return (sint32)product >> 12;
}

static void frame_edges(uint32 object, uint32 width, sint32 cosine, sint32 sine)
{
    R16(object + 16u) = (uint16)frame_product12(0u - width, (uint32)cosine);
    R16(object + 20u) = (uint16)frame_product12(0u - (0u - width) * (uint32)sine, 1u);
    R16(object + 18u) = 0u;
    R16(object + 24u) = (uint16)frame_product12(width, (uint32)cosine);
    R16(object + 28u) = (uint16)frame_product12(0u - width * (uint32)sine, 1u);
    R16(object + 26u) = 0u;
}

void game_800B053C(void)
{
    uint32 emitter;
    for (emitter = 0u; emitter < 8u; ++emitter)
    {
        uint32 record = 0x800C6BE8u + emitter * 7u, player, lane, index, width;
        uint32 player_offset, position, object, old_index, old_offset, new_index, new_offset;
        uint32 dx, dy, dz, speed, amount, angle;
        sint32 cosine, sine;
        if (!R8(record + 2u))
            continue;
        player = R8(record);
        lane = R8(record + 1u);
        index = R8(record + 3u);
        width = R8(record + 4u);
        player_offset = player * 952u;
        speed = R32(0x8006708Cu + player_offset);
        if ((sint32)speed < 0)
            speed = 0u - speed;
        amount = (uint32)((sint32)speed >> 15);
        if ((sint32)amount <= 0)
            amount = 1u;
        else if ((sint32)amount >= 4)
            amount = 3u;
        amount = 3u - amount;
        position = 0x80066F54u + player_offset + lane * 12u;
        object = R32(0x800C6BE4u) + index * 148u;
        dx = (uint32)((sint32)(R32(position) - R32(object + 116u)) >> 8);
        dz = (uint32)((sint32)(R32(position + 8u) - R32(object + 124u)) >> 8);
        dy = (uint32)((sint32)(R32(position + 4u) - R32(object + 120u)) >> 8);
        angle = (uint32)sub_80043450(dx, dz);
        cosine = draft_agent3_missing_800437EC(angle);
        sine = draft_agent3_missing_8004388C(angle);
        object = R32(0x800C6BE4u) + index * 148u;
        if (!(R8(object + 112u) & 2u) && (dx || dz))
        {
            R8(object + 112u) |= 2u;
            R32(object + 128u) = angle;
            object = R32(0x800C6BE4u) + index * 148u;
            if ((sint32)R32(object + 136u) >= 0)
            {
                uint32 previous_index = R32(object + 136u);
                uint32 previous = R32(0x800C6BE4u) + previous_index * 148u;
                uint32 previous_angle = R32(previous + 128u), current_angle = R32(R32(0x800C6BE4u) + index * 148u + 128u);
                uint32 delta = current_angle - previous_angle;
                sint32 folded, sin_delta, cos_delta;
                if ((sint32)current_angle < (sint32)previous_angle)
                    delta += 4096u;
                delta = (sint32)delta < 2049 ? delta + 2048u : delta - 2048u;
                if (delta < 512u)
                    folded = (sint32)(0u - delta);
                else if (delta - 512u < 512u)
                    folded = -512;
                else if (delta - 1024u < 512u)
                    folded = 512;
                else if (delta - 1536u < 512u)
                    folded = (sint32)(delta - 2048u);
                else if (delta - 2048u < 512u)
                    folded = (sint32)(delta - 2048u);
                else if (delta - 2560u < 512u)
                    folded = -512;
                else if (delta - 3072u < 512u)
                    folded = 512;
                else
                    folded = (sint32)(delta - 4096u);
                if ((sint32)(previous_angle - current_angle) < 0)
                    folded = (sint32)(0u - (uint32)folded);
                sin_delta = draft_agent3_missing_8004388C((uint32)folded);
                cos_delta = draft_agent3_missing_800437EC((uint32)folded);
                object = R32(0x800C6BE4u) + index * 148u;
                R16(object + 16u) = (uint16)frame_product12(0u - width, (uint32)cos_delta);
                R16(object + 20u) = (uint16)frame_product12(0u - (0u - width) * (uint32)sin_delta, 1u);
                R16(object + 24u) = (uint16)frame_product12(width, (uint32)cos_delta);
                R16(object + 28u) = (uint16)frame_product12(0u - width * (uint32)sin_delta, 1u);
                previous = R32(0x800C6BE4u) + previous_index * 148u;
                R16(previous) = (uint16)(R16(object + 16u) + R32(previous + 140u));
                R16(previous + 4u) = (uint16)(R16(object + 20u) + R32(previous + 144u));
                R16(previous + 8u) = (uint16)(R16(object + 24u) + R32(previous + 140u));
                R16(previous + 12u) = (uint16)(R16(object + 28u) + R32(previous + 144u));
            }
            else
                frame_edges(object, width, cosine, sine);
        }
        object = R32(0x800C6BE4u) + index * 148u;
        R16(object) = (uint16)(dx + (uint32)frame_product12(0u - width, (uint32)cosine));
        R16(object + 4u) = (uint16)(dz + (uint32)frame_product12(0u - (0u - width) * (uint32)sine, 1u));
        R16(object + 2u) = (uint16)dy;
        R16(object + 8u) = (uint16)(dx + (uint32)frame_product12(width, (uint32)cosine));
        R16(object + 12u) = (uint16)(dz + (uint32)frame_product12(0u - width * (uint32)sine, 1u));
        R16(object + 10u) = (uint16)dy;
        R32(object + 140u) = dx;
        R32(object + 144u) = dz;
        if (((sint32)R32(object + 128u) >> 7) == ((sint32)angle >> 7) && (sint16)R16(object + 114u) <= (sint32)amount)
            continue;
        old_index = index;
        old_offset = old_index * 148u;
        new_index = dr_race_particle_slot();
        new_offset = new_index * 148u;
        object = R32(0x800C6BE4u) + old_offset;
        R32(object + 132u) = new_index;
        object = R32(0x800C6BE4u) + new_offset;
        R32(object + 136u) = old_index;
        R32(object + 132u) = 0xFFFFFFFDu;
        R8(record + 3u) = (uint8)new_index;
        {
            sint32 difference = (sint32)R8(record + 6u) - R8(record + 5u);
            if (difference < 0)
                ++R8(record + 6u);
            if (difference > 0)
                --R8(record + 6u);
        }
        object = R32(0x800C6BE4u) + old_offset;
        R8(object + 112u) &= 2u;
        object = R32(0x800C6BE4u) + new_offset;
        R32(object + 116u) = R32(position);
        R32(object + 120u) = R32(position + 4u);
        R32(object + 124u) = R32(position + 8u);
        R8(object + 112u) = 3u;
        object = R32(0x800C6BE4u) + new_offset;
        R32(object + 128u) = angle;
        R8(object + 113u) = R8(record + 6u);
        object = R32(0x800C6BE4u) + new_offset;
        R16(object + 114u) = 0u;
        {
            uint32 old = R32(0x800C6BE4u) + old_offset, component;
            for (component = 0u; component < 4u; ++component)
            {
                uint32 coordinate = component & 1u ? 124u : 116u;
                uint32 value = R16(old + component * 4u) + (uint32)((sint32)R32(old + coordinate) >> 8) - (uint32)((sint32)R32(object + coordinate) >> 8);
                R16(object + 16u + component * 4u) = (uint16)value;
                R16(object + component * 4u) = (uint16)value;
            }
        }
        {
            uint32 packet_bank;
            for (packet_bank = 0u; packet_bank < 2u; ++packet_bank)
            {
                uint32 color = 0x800BF7D9u + R8(0x800670B8u + player_offset) * 4u;
                object = R32(0x800C6BE4u) + new_offset + packet_bank * 40u;
                R8(object + 36u) = R8(color);
                color = 0x800BF7D9u + R8(0x800670B8u + player_offset) * 4u;
                object = R32(0x800C6BE4u) + new_offset + packet_bank * 40u;
                R8(object + 37u) = R8(color + 1u);
                color = 0x800BF7D9u + R8(0x800670B8u + player_offset) * 4u;
                object = R32(0x800C6BE4u) + new_offset + packet_bank * 40u;
                R8(object + 38u) = R8(color + 2u);
            }
        }
    }
}
