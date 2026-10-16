#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))

static void car_effect_deactivate(uint32 effect)
{
    w_u8(effect + 10, 0);
}

static void car_matrix(uint32 object)
{
    uint32 index;
    for (index = 0u; index < 8u; ++index)
        xport_gte_write_control(index, R32(object + 148u + index * 4u));
}

void dr_geometry_apply_matrix(const MATRIX *matrix, const VECTOR *input, VECTOR *output)
{
    uint32 components[3] = {(uint32)input->vx, (uint32)input->vy, (uint32)input->vz};
    uint32 low[3], high[3], scaled[3], index;
    for (index = 0u; index < 5u; ++index)
    {
        uint32 word;
        memcpy(&word, (const uint8 *)matrix + index * 4u, 4u);
        xport_gte_write_control(index, word);
    }
    for (index = 0u; index < 3u; ++index)
    {
        uint32 value = components[index];
        if ((sint32)value < 0)
        {
            value = 0u - value;
            high[index] = 0u - (uint32)((sint32)value >> 15);
            low[index] = 0u - (value & 32767u);
        }
        else
        {
            high[index] = (uint32)((sint32)value >> 15);
            low[index] = value & 32767u;
        }
        xport_gte_write_data(index + 9u, high[index]);
    }
    xport_gte_execute(0x41E012u);
    for (index = 0u; index < 3u; ++index)
        scaled[index] = xport_gte_read_data(index + 25u) << 3;
    for (index = 0u; index < 3u; ++index)
        xport_gte_write_data(index + 9u, low[index]);
    xport_gte_mvmva(0x49E012u);
    output->vx = (sint32)(xport_gte_read_data(25u) + scaled[0]);
    output->vy = (sint32)(xport_gte_read_data(26u) + scaled[1]);
    output->vz = (sint32)(xport_gte_read_data(27u) + scaled[2]);
}

uint32 sub_8003D188(uint32 model)
{
    VECTOR position, transformed;
    uint32 shift = R32(0x80065170u) & 31u;
    uint32 radius = (uint32)((sint32)(sint16)R16(model + 8u) >> shift);
    uint32 plane, coefficient, product;
    position.vx = (sint32)((uint32)((sint32)R32(model + 16u) >> 8) + R32(model + 28u) - R32(0x80081BB8u));
    position.vy = (sint32)((uint32)((sint32)R32(model + 20u) >> 8) + R32(model + 32u) - R32(0x80081BBCu));
    position.vz = (sint32)((uint32)((sint32)R32(model + 24u) >> 8) + R32(model + 36u) - R32(0x80081BC0u));
    dr_geometry_apply_matrix((const MATRIX *)psx_addr(0x80081BA4u, sizeof(MATRIX)), &position, &transformed);
    if ((sint32)((uint32)transformed.vz + radius) <= 0)
        return 0u;
    if ((sint32)((uint32)transformed.vz - radius) >= ((sint32)R32(0x8006514Cu) >> (R32(0x80065170u) & 31u)))
        return 0u;
    coefficient = (uint32)(sint32)(sint16)R16(0x80065550u);
    product = (uint32)transformed.vz * (uint32)(sint32)(sint16)R16(0x80065554u);
    plane = (uint32)transformed.vx * coefficient + product;
    if (((sint32)plane >> 12) >= (sint32)radius)
        return 0u;
    plane = (0u - (uint32)transformed.vx) * coefficient + product;
    if (((sint32)plane >> 12) >= (sint32)radius)
        return 0u;
    coefficient = (uint32)(sint32)(sint16)R16(0x80065562u);
    product = (uint32)transformed.vz * (uint32)(sint32)(sint16)R16(0x80065564u);
    plane = (uint32)transformed.vy * coefficient + product;
    if (((sint32)plane >> 12) >= (sint32)radius)
        return 0u;
    plane = (0u - (uint32)transformed.vy) * coefficient + product;
    if (((sint32)plane >> 12) >= (sint32)radius)
        return 0u;
    R32(0x80065548u) = (uint32)transformed.vz;
    return 1u;
}

uint32 sub_80017670(uint32 player)
{
    uint32 object = 0x80066D14u + player * 952u, model = R32(object + 24u);
    R32(model + 16u) = R32(object + 100u);
    R32(model + 20u) = R32(object + 104u);
    R32(model + 24u) = R32(object + 108u);
    R32(0x8006525Cu) = 0u;
    if ((R32(0x80064C14u) && R32(0x80065128u) != player) || !R32(0x800654E8u + player * 4u))
    {
        game_8009E320(object);
        if (sub_8003D188(model))
        {
            R8(object + 85u) = 1u;
            sub_8003CED8((const MATRIX *)psx_addr(object + 148u, sizeof(MATRIX)), model);
            sub_800416E0(model);
            game_800AFC88(player);
            sub_8001896C(player);
            sub_800340F0(player, object);
            if (R8(object + 940u))
                game_8009F500(object);
            else
                game_8009F030(player);
            car_matrix(object);
            game_800AF0BC(player);
            game_800AFE28(player);
            if (R8(object + 378u) & 1u)
                game_800AEC08(player);
        }
    }
    if (!R32(0x800593A8u))
    {
        game_800B24B4(object);
        return 0u;
    }
    return R32(0x800593A8u);
}

static uint32 car_trunc(uint32 value, uint32 bits)
{
    if ((sint32)value < 0)
        value += (1u << bits) - 1u;
    return (uint32)((sint32)value >> bits);
}

uint32 sub_80017820(uint32 player)
{
    uint32 object = 0x80066D14u + player * 952u, distance = 0u, model, index;
    VECTOR components;
    sint32 values[3];
    if (R8(object + 948u))
        return R8(object + 948u);
    for (index = 0u; index < 3u; ++index)
    {
        uint32 delta = car_trunc(R32(object + 100u + index * 4u), 8u) - R32(0x80081BB8u + index * 4u);
        uint32 component = car_trunc(delta, 4u);
        values[index] = (sint32)component;
    }
    components.vx = values[0];
    components.vy = values[1];
    components.vz = values[2];
    Square0(&components, &components);
    distance = (uint32)components.vx + (uint32)components.vy + (uint32)components.vz;
    if ((sint32)distance < (sint32)R32(0x80065258u))
    {
        model = R32(object + 24u);
        R32(model + 16u) = R32(object + 100u);
        R32(model + 20u) = R32(object + 104u);
        R32(model + 24u) = R32(object + 108u);
        if (!sub_8003D188(model))
            return 0u;
        R8(object + 85u) = 1u;
        game_8009E320(object);
        sub_8003CED8((const MATRIX *)psx_addr(object + 148u, sizeof(MATRIX)), model);
        sub_800416E0(model);
        sub_8001896C(player);
        sub_800340F0(player, object);
        if (R8(object + 940u))
            game_8009F500(object);
        else
            game_8009F030(player);
    }
    else
    {
        model = R32(object + 28u);
        R32(model + 16u) = R32(object + 100u);
        R32(model + 20u) = R32(object + 104u);
        R32(model + 24u) = R32(object + 108u);
        if (!sub_8003D188(model))
            return 0u;
        R8(object + 85u) = 1u;
        game_8009E320(object);
        sub_8003CED8((const MATRIX *)psx_addr(object + 148u, sizeof(MATRIX)), model);
        sub_800416E0(model);
        if (R32(0x80065110u) == 1u || R32(0x80064B5Cu))
        {
            sub_8001896C(player);
            if (R8(object + 940u))
                game_8009F500(object);
            else
                game_8009F030(player);
        }
        else if ((sint32)distance < (sint32)R32(0x80065260u))
        {
            sub_80019304(player);
            if (R8(object + 940u))
                game_8009F500(object);
            else
                game_8009F030(player);
        }
    }
    car_matrix(object);
    game_800AF0BC(player);
    if (R8(object + 378u) & 1u)
        game_800AEC08(player);
    return 0u;
}

uint32 sub_80017C9C(void)
{
    uint32 count = R32(0x80065254u), index = 0u;
    if ((sint32)count <= 0)
        return count;
    do
    {
        uint8 kind = R8(0x80059A28u + index);
        if (kind < 2u)
            sub_80017670(index);
        else if (kind == 2u)
            sub_80017820(index);
        count = R32(0x80065254u);
        ++index;
    } while ((sint32)index < (sint32)count);
    return 0u;
}

uint32 sub_80017D80(void)
{
    uint32 count = R32(0x80065254u), index = 0u, object = 0x80066D14u, part;
    if ((sint32)count <= 0)
        return count;
    do
    {
        if (!R8(object + 85u) && R8(object + 84u))
        {
            R8(object + 84u) = 0u;
            for (part = 0u; part < 5u; ++part)
            {
                uint32 effect = R32(object + 48u + part * 4u);
                if (effect)
                    car_effect_deactivate(effect);
            }
        }
        count = R32(0x80065254u);
        ++index;
        object += 952u;
    } while ((sint32)index < (sint32)count);
    return 0u;
}

uint32 sub_80017E68(void)
{
    uint32 distances[22], count = R32(0x80065254u), index, cursor, limit, slot = 0u, marker = 0x80073BECu;
    uint8 slot_byte = 0u;
    uint32 x = (uint32)((sint32)R32(0x80081B98u) >> 8);
    uint32 y = (uint32)((sint32)R32(0x80081B9Cu) >> 8);
    uint32 z = (uint32)((sint32)R32(0x80081BA0u) >> 8);
    if (count == 1u)
        return 1u;
    /* TODO: The original local array overflows when the player count exceeds 22 */
    if ((sint32)count > 22)
        abort();
    for (index = 0u; (sint32)index < (sint32)count; ++index)
    {
        uint32 player = R32(0x8006A894u + index * 4u), object = 0x80066D6Cu + player * 952u;
        uint32 dx = (uint32)(sint32)(sint16)R16(object + 14u) - x;
        uint32 dy = (uint32)(sint32)(sint16)R16(object + 18u) - y;
        uint32 dz = (uint32)(sint32)(sint16)R16(object + 22u) - z;
        distances[index] = R8(0x80059A28u + player) == 2u ? dx * dx + dy * dy + dz * dz : 0x7FFFFFFFu;
    }
    limit = count - 1u;
    for (;;)
    {
        uint32 changed = 0u;
        for (cursor = 0u; (sint32)cursor < (sint32)limit; ++cursor)
        {
            if ((sint32)distances[cursor + 1u] < (sint32)distances[cursor])
            {
                uint32 temporary = distances[cursor];
                distances[cursor] = distances[cursor + 1u];
                distances[cursor + 1u] = temporary;
                temporary = R32(0x8006A894u + (cursor + 1u) * 4u);
                R32(0x8006A894u + (cursor + 1u) * 4u) = R32(0x8006A894u + cursor * 4u);
                R32(0x8006A894u + cursor * 4u) = temporary;
                changed = 1u;
            }
        }
        --limit;
        if (!changed)
            break;
    }
    for (index = R32(0x80064CB8u); (sint32)index < (sint32)R32(0x80065254u); ++index)
    {
        uint32 object = 0x80066D2Cu + R32(0x8006A894u + index * 4u) * 952u;
        if (R8(object + 58u))
        {
            R8(0x80073BECu + R8(object + 59u)) = 255u;
            R32(object + 12u) = 0u;
            R32(object + 8u) = 0u;
            R8(object + 58u) = 0u;
            R32(object) = R32(object + 4u);
        }
    }
    count = R32(0x80064CB8u);
    if ((sint32)count <= 0)
        return count;
    for (index = 0u; (sint32)index < (sint32)R32(0x80064CB8u); ++index)
    {
        uint32 object = 0x80066D2Cu + R32(0x8006A894u + index * 4u) * 952u;
        if (!R8(object + 58u))
        {
            while (R8(marker) != 255u)
            {
                ++marker;
                ++slot;
                ++slot_byte;
            }
            R32(object) = R32(0x80073C3Cu + slot * 4u);
            R32(object + 12u) = (sint16)R16(0x80066C42u) > 0 ? R32(0x80073CBCu + slot * 4u) : 0u;
            R32(object + 8u) = (sint16)R16(0x80066C50u) > 0 ? R32(0x80073CFCu + slot * 4u) : 0u;
            R8(object + 58u) = 1u;
            R8(object + 59u) = R8(marker);
            R8(marker) = (uint8)R32(0x8006A894u + index * 4u);
            sub_8003132C(0x80066D14u + R32(0x8006A894u + index * 4u) * 952u, 23u);
            R8(object + 59u) = slot_byte;
        }
    }
    return 0u;
}
