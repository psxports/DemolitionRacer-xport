#include "draft_signatures.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define M32(a) (*(uint32 *)psx_addr((a), 4u))
#define M16(a) (*(uint16 *)psx_addr((a), 2u))
#define M8(a) (*(uint8 *)psx_addr((a), 1u))

static uint32 model_divide(uint32 numerator, uint32 denominator)
{
    if (!denominator || (numerator == 0x80000000u && denominator == ~0u))
    {
        draft_agent1_missing_diagnostic("PRES model renderer original DIV trap\n");
        abort();
    }
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static uint32 model_product12(uint32 left, uint32 right)
{
    uint32 product = left * right;
    if ((sint32)product < 0)
        product += 4095u;
    return (uint32)((sint32)product >> 12);
}

static void model_load_vector(uint32 index, uint32 address)
{
    xport_gte_write_data(index, M32(address));
    xport_gte_write_data(index + 1u, M32(address + 4u));
}

static void model_link(uint32 packet, uint32 slot)
{
    uint32 head = M32(slot);
    M32(packet) = (M32(packet) & 0xFF000000u) | (head & 0xFFFFFFu);
    M32(slot) = (M32(slot) & 0xFF000000u) | (packet & 0xFFFFFFu);
}

static void model_uv(uint32 packet, uint32 vertex, const SVECTOR *normal, uint32 center_x, uint32 center_y, uint32 scale_x, uint32 scale_y, uint32 normal_scale, uint32 texture_x, uint32 texture_y, uint32 bias)
{
    PsxGteSnapshot saved;
    uint32 transformed_x, transformed_y, coordinate;
    psx_gte_snapshot(&saved);
    xport_gte_write_data(0u, (uint16)normal->vx | ((uint32)(uint16)normal->vy << 16));
    xport_gte_write_data(1u, (uint16)normal->vz);
    xport_gte_mvmva(0x486012u);
    xport_gte_write_data(0u, (uint16)saved.v0[0] | ((uint32)(uint16)saved.v0[1] << 16));
    xport_gte_write_data(1u, (uint16)saved.v0[2]);
    transformed_x = (uint32)(sint32)(sint16)xport_gte_read_data(9u);
    transformed_y = (uint32)(sint32)(sint16)xport_gte_read_data(10u);
    /* IR3 is also read by the original even though its local is unused */
    (void)xport_gte_read_data(11u);
    coordinate = packet + 8u + vertex * 8u;
    M8(coordinate + 4u) = (uint8)(model_product12((uint32)(sint32)(sint16)M16(coordinate) - center_x, scale_x) + model_product12(transformed_x, normal_scale) + texture_x + bias);
    M8(coordinate + 5u) = (uint8)(model_product12((uint32)(sint32)(sint16)M16(coordinate + 2u) - center_y, scale_y) + model_product12(transformed_y, normal_scale) + texture_y + bias);
}

void pres_8008F170(uint32 model, uint32 parameters, uint32 scale)
{
    uint32 projection = M32(0x80065150u);
    uint32 horizontal = model_divide(M32(parameters + 4u) * scale, projection);
    uint32 vertical = model_divide(M32(parameters + 8u) * scale, projection);
    uint32 normalization = M32(0x8006516Cu);
    uint32 scale_x = model_divide(horizontal << 12, normalization);
    uint32 scale_y = model_divide(vertical << 12, normalization);
    uint32 bias = (uint32)(sint32)(sint16)M16(parameters + 16u);
    uint32 center_x = (uint32)(sint32)(sint16)M16(parameters);
    uint32 center_y = (uint32)(sint32)(sint16)M16(parameters + 2u);
    uint32 depth_shift = M32(0x80065134u);
    uint32 group = model + M32(model + 48u);
    uint32 normal_scale = M32(parameters + 12u);
    uint32 texture = M32(0x800ABAB8u);
    uint32 depth_limit = M32(0x8006514Cu);
    uint32 depth_reduction = M32(0x80065170u);
    uint32 texture_x = M8(texture + 24u), texture_y = M8(texture + 25u);
    uint32 vertices = model + M32(model + 44u);
    uint32 table, kind, count, packet, index, primitive_size, record_size;
    uint32 a, b, c, fourth, first_area, second_area, depth, slot;
    SVECTOR positions[3], normals[3], fourth_position, fourth_normal;
    sint32 projected[3], projected_fourth, depths[3], depth_fourth;
    depth_shift -= depth_reduction;
    depth_limit = (uint32)((sint32)depth_limit >> ((depth_reduction + 2u) & 31u));
    table = M32(M32(0x80065118u) + 112u) + ((uint32)((sint32)(sint16)M16(model + 12u) >> (depth_shift & 31u)) << 2);
    normal_scale = model_divide(normal_scale << 12, normalization);
    while (M8(group + 3u))
    {
        kind = M8(group + 2u);
        count = (uint32)(sint32)(sint16)M16(group);
        group += 4u;
        if (kind == 0u || kind == 1u)
        {
            group += count * 20u;
            continue;
        }
        if (kind != 2u && kind != 3u)
            continue;
        packet = M32((kind == 2u ? 0x800ABAA8u : 0x800ABAB0u) + M32(0x80065128u) * 4u);
        primitive_size = kind == 2u ? 32u : 40u;
        record_size = kind == 2u ? 44u : 56u;
        for (index = 0u; (sint32)index < (sint32)count; ++index, group += record_size, packet += primitive_size)
        {
            a = vertices + ((uint32)(sint32)(sint16)M16(group) << 3);
            b = vertices + ((uint32)(sint32)(sint16)M16(group + 2u) << 3);
            c = vertices + ((uint32)(sint32)(sint16)M16(group + 4u) << 3);
            memcpy(&positions[0], psx_addr(a, sizeof(SVECTOR)), sizeof(SVECTOR));
            memcpy(&positions[1], psx_addr(b, sizeof(SVECTOR)), sizeof(SVECTOR));
            memcpy(&positions[2], psx_addr(c, sizeof(SVECTOR)), sizeof(SVECTOR));
            gte_project3_full_depth(positions, projected, depths, NULL);
            first_area = (uint32)NormalClip(projected[0], projected[1], projected[2]);
            if (kind == 2u)
            {
                depth = (uint32)AverageZ3(depths[0], depths[1], depths[2]);
                if ((sint32)first_area > 0)
                    continue;
                if ((sint32)depth >= (sint32)depth_limit)
                    continue;
                M32(packet + 8u) = (uint32)projected[0];
                M32(packet + 16u) = (uint32)projected[1];
                M32(packet + 24u) = (uint32)projected[2];
            }
            else
            {
                M32(packet + 8u) = (uint32)projected[0];
                M32(packet + 16u) = (uint32)projected[1];
                M32(packet + 24u) = (uint32)projected[2];
                fourth = vertices + ((uint32)(sint32)(sint16)M16(group + 6u) << 3);
                memcpy(&fourth_position, psx_addr(fourth, sizeof(SVECTOR)), sizeof(SVECTOR));
                depth_fourth = gte_project_full_depth(&fourth_position, &projected_fourth, NULL);
                second_area = (uint32)NormalClip(projected[1], projected[2], projected_fourth);
                depth = (uint32)AverageZ4(depths[0], depths[1], depths[2], depth_fourth);
                if ((sint32)first_area > 0 && (sint32)second_area < 0)
                    continue;
                if ((sint32)depth >= (sint32)depth_limit)
                    continue;
                M32(packet + 32u) = (uint32)projected_fourth;
            }
            slot = table + ((uint32)((sint32)depth >> (depth_shift & 31u)) << 2);
            model_link(packet, slot);
            a = group + (kind == 2u ? 20u : 24u);
            memcpy(normals, psx_addr(a, sizeof(normals)), sizeof(normals));
            model_load_vector(0u, a);
            model_uv(packet, 0u, &normals[0], center_x, center_y, scale_x, scale_y, normal_scale, texture_x, texture_y, bias);
            model_uv(packet, 1u, &normals[1], center_x, center_y, scale_x, scale_y, normal_scale, texture_x, texture_y, bias);
            model_uv(packet, 2u, &normals[2], center_x, center_y, scale_x, scale_y, normal_scale, texture_x, texture_y, bias);
            if (kind == 3u)
            {
                memcpy(&fourth_normal, psx_addr(group + 48u, sizeof(SVECTOR)), sizeof(SVECTOR));
                model_load_vector(0u, group + 48u);
                model_uv(packet, 3u, &fourth_normal, center_x, center_y, scale_x, scale_y, normal_scale, texture_x, texture_y, bias);
            }
            M32(packet + 4u) = M32(0x800A1CB8u) | (kind == 2u ? 0x26000000u : 0x2E000000u);
        }
    }
}

void pres_8008FC84(uint32 model, uint32 ignored)
{
    pres_8008F170(model, 0x800A1CBCu, 4000u);
}

/* Complete original renderer entry 50D50 and its four table-selected loops */
void sdk_model_80050D50(uint32 model)
{
    uint32 group = model + M32(model + 48u);
    uint32 packet = model + M32(model + 52u + M32(0x80065128u) * 4u);
    uint32 vertices = model + M32(model + 44u);
    uint32 shift = M32(0x80065134u) - M32(0x80065170u);
    uint32 table, limit = M32(0x8006554Cu), kind, count, record_size, packet_size;
    sint64 difference = (sint64)(sint32)M32(0x80065134u) - (sint32)M32(0x80065170u);
    if (difference > 2147483647LL || difference < -2147483648LL)
        xport_mips_break(6u);
    table = M32(M32(0x80065118u) + 112u) + ((uint32)((sint32)(sint16)M16(model + 12u) >> (shift & 31u)) << 2);
    for (;;)
    {
        count = (uint32)(sint32)(sint16)M16(group);
        kind = M8(group + 2u);
        if (!count)
            return;
        if (kind > 3u)
        {
            draft_agent1_missing_diagnostic("Unsupported original 50D50 primitive group %u\n", kind);
            abort();
        }
        record_size = kind < 2u ? 20u : (kind == 2u ? 44u : 56u);
        packet_size = kind == 0u ? 32u : (kind == 3u ? 52u : 40u);
        group += 4u;
        do
        {
            SVECTOR positions[3], fourth, normals[3], normal;
            sint32 screen[3], depths[3], screen_fourth, depth_fourth, area, depth;
            uint32 i, slot, head, colors[3], code, fourth_index, source_color;
            for (i = 0u; i < 3u; ++i)
                memcpy(&positions[i], psx_addr(vertices + ((uint32)(sint32)(sint16)M16(group + i * 2u) << 3), sizeof(SVECTOR)), sizeof(SVECTOR));
            gte_project3_full_depth(positions, screen, depths, NULL);
            area = NormalClip(screen[0], screen[1], screen[2]);
            if (kind == 0u || kind == 2u)
            {
                depth = AverageZ3(depths[0], depths[1], depths[2]);
                if (area > 0 || depth <= 0)
                    goto next_primitive;
                /* The first coordinate write is also the original failed-limit delay slot */
                M32(packet + 8u) = (uint32)screen[0];
                if ((sint32)(uint32)depth >= (sint32)limit)
                    goto next_primitive;
                M32(packet + (kind == 0u ? 16u : 20u)) = (uint32)screen[1];
                M32(packet + (kind == 0u ? 24u : 32u)) = (uint32)screen[2];
            }
            else
            {
                fourth_index = (uint32)(sint32)(sint16)M16(group + 6u);
                M32(packet + 8u) = (uint32)screen[0];
                M32(packet + (kind == 1u ? 16u : 20u)) = (uint32)screen[1];
                M32(packet + (kind == 1u ? 24u : 32u)) = (uint32)screen[2];
                memcpy(&fourth, psx_addr(vertices + (fourth_index << 3), sizeof(SVECTOR)), sizeof(SVECTOR));
                depth_fourth = gte_project_full_depth(&fourth, &screen_fourth, NULL);
                if (area > 0 && NormalClip(screen[1], screen[2], screen_fourth) < 0)
                    goto next_primitive;
                depth = AverageZ4(depths[0], depths[1], depths[2], depth_fourth);
                M32(packet + (kind == 1u ? 32u : 44u)) = (uint32)screen_fourth;
                if (depth <= 0 || (sint32)(uint32)depth >= (sint32)limit)
                    goto next_primitive;
            }
            slot = table + ((uint32)(depth >> (shift & 31u)) << 2);
            code = kind == 0u ? 0x07000000u : (kind == 3u ? 0x0C000000u : 0x09000000u);
            if (kind < 2u)
            {
                head = M32(slot);
                source_color = M32(group + 8u);
                memcpy(&normal, psx_addr(group + 12u, sizeof(SVECTOR)), sizeof(SVECTOR));
                M32(slot) = packet & 0xFFFFFFu;
                colors[0] = gte_normal_color_col(&normal, source_color);
                M32(packet) = code | (head & 0xFFFFFFu);
                M32(packet + 4u) = colors[0];
            }
            else
            {
                source_color = M32(group + 8u);
                memcpy(normals, psx_addr(group + (kind == 2u ? 20u : 24u), sizeof(normals)), sizeof(normals));
                if (kind == 2u)
                {
                    head = M32(slot);
                    M32(slot) = packet & 0xFFFFFFu;
                    gte_normal_color_col3(normals, source_color, colors);
                    M32(packet) = code | (head & 0xFFFFFFu);
                    M32(packet + 4u) = colors[0];
                    M32(packet + 16u) = colors[1];
                    M32(packet + 28u) = colors[2];
                }
                else
                {
                    gte_normal_color_col3(normals, source_color, colors);
                    head = M32(slot);
                    M32(slot) = packet & 0xFFFFFFu;
                    M32(packet + 4u) = colors[0];
                    M32(packet + 16u) = colors[1];
                    M32(packet + 28u) = colors[2];
                    memcpy(&normal, psx_addr(group + 48u, sizeof(SVECTOR)), sizeof(SVECTOR));
                    M32(packet) = code | (head & 0xFFFFFFu);
                    M32(packet + 40u) = gte_normal_color_col(&normal, source_color);
                }
            }
        next_primitive:
            group += record_size;
            packet += packet_size;
            --count;
        } while (count);
    }
}
