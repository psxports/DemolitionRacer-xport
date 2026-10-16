#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))

uint32 sub_800172A4(uint32 model);
uint32 sub_8002F934(uint32 table);
void sub_8002FCE0(void);
uint32 sub_8002FDA4(uint32 mask);
void game_80097080(uint32 object);
void game_8009D5F0(uint32 angles, uint32 output);
void game_80099A78(uint32 object);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
void sub_80035458(const sint16 *first, const sint16 *second, sint16 *output);
uint32 sub_8002C188(uint32 state, uint32 position);
void sub_8002EDE0(uint32 state, uint32 position, uint32 normal);
uint32 sub_8002DC38(uint32 state, uint32 position, uint32 height_output);
uint32 sub_8002E408(uint32 state, uint32 position, uint32 normal_output, uint32 mode);
uint32 sub_8002F5F8(uint32 angles, uint32 positions, uint32 heights);
uint32 sub_8002F78C(uint32 angles, uint32 positions, uint32 heights);
uint32 sub_8002F860(uint32 angles, uint32 positions, uint32 heights);

static uint32 motion_pending(uint32 target, uint32 first, uint32 second, uint32 third, uint32 count)
{
    /* TODO: Translate the full GAME or SLUS callee before replacing this boundary */
    draft_agent1_missing_diagnostic("Untranslated GAME motion %08X (%u arguments): %08X %08X %08X\n", target, count, first, second, third);
    abort();
}

static uint32 motion_product8(uint32 left, uint32 right)
{
    uint32 product = left * right;
    if ((sint32)product < 0)
        product += 255u;
    return (uint32)((sint32)product >> 8);
}

static uint32 motion_divide(uint32 numerator, uint32 denominator)
{
    if (denominator == 0u)
        xport_mips_break(7u);
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        xport_mips_break(6u);
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static void motion_velocity(uint32 object, uint32 angle, uint32 old_angle, uint32 velocity, uint32 scale)
{
    uint32 delta = (uint32)(sint32)(sint16)R16(object + angle) - old_angle;
    delta = ((delta + 2048u) & 4095u) - 2048u;
    R32(object + velocity) = delta;
    R32(object + velocity) = delta * scale;
    if ((sint32)R32(object + velocity) > 6000)
        R32(object + velocity) = 6000u;
    if ((sint32)R32(object + velocity) < -6000)
        R32(object + velocity) = (uint32)-6000;
}

void game_8009A200(uint32 object)
{
    uint32 time, gravity, kind, scale, index, sum = 0u, contacts = 0u, average;
    uint32 saved_angles[2];
    R32(object + 824u) = R32(object + 104u);
    time = R32(0x800651C4u);
    memcpy(saved_angles, psx_addr(object + 40u, 8u), 8u);
    gravity = time * 3000u;
    if ((sint32)gravity < 0)
        gravity += 255u;
    R32(object + 116u) -= (uint32)((sint32)gravity >> 8);
    R32(object + 124u) += motion_product8(R32(object + 136u), time);
    R32(object + 128u) += motion_product8(R32(object + 140u), time);
    R32(object + 132u) += motion_product8(R32(object + 144u), time);
    R32(object + 104u) += motion_product8(R32(object + 116u), time);
    R32(object + 100u) += motion_product8(R32(object + 112u), time);
    R32(object + 108u) += motion_product8(R32(object + 120u), time);
    R16(object + 44u) = (uint16)((sint32)R32(object + 132u) >> 8);
    R32(object + 168u) = (uint32)((sint32)R32(object + 100u) >> 8);
    R32(object + 172u) = (uint32)((sint32)R32(object + 104u) >> 8);
    R16(object + 40u) = (uint16)((sint32)R32(object + 124u) >> 8);
    R16(object + 42u) = (uint16)((sint32)R32(object + 128u) >> 8);
    R32(object + 176u) = (uint32)((sint32)R32(object + 108u) >> 8);
    sub_8003C960((sint16 *)psx_addr(object + 148u, 32u), (const sint16 *)psx_addr(object + 40u, 8u));
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
            if (R8(object + 942u) == R8(object + 943u))
            {
                scale = motion_divide(65536u, R32(0x800651C4u));
                motion_velocity(object, 40u, (uint32)(sint32)(sint16)saved_angles[0], 136u, scale);
                motion_velocity(object, 44u, (uint32)(sint32)(sint16)saved_angles[1], 144u, scale);
            }
            R32(object + 124u) = (uint32)(sint32)(sint16)R16(object + 40u) << 8;
            R32(object + 132u) = (uint32)(sint32)(sint16)R16(object + 44u) << 8;
            break;
        case 3u:
        case 12u:
            sub_8002F860(object + 40u, object + 576u, object + 840u);
            if (R8(object + 942u) == R8(object + 943u))
            {
                scale = motion_divide(65536u, R32(0x800651C4u));
                motion_velocity(object, 44u, (uint32)(sint32)(sint16)saved_angles[1], 144u, scale);
            }
            R32(object + 132u) = (uint32)(sint32)(sint16)R16(object + 44u) << 8;
            break;
        case 5u:
        case 10u:
            sub_8002F78C(object + 40u, object + 576u, object + 840u);
            if (R8(object + 942u) == R8(object + 943u))
            {
                scale = motion_divide(65536u, R32(0x800651C4u));
                motion_velocity(object, 40u, (uint32)(sint32)(sint16)saved_angles[0], 136u, scale);
            }
            R32(object + 124u) = (uint32)(sint32)(sint16)R16(object + 40u) << 8;
            break;
        default:
            break;
    }
    sub_8003C960((sint16 *)psx_addr(object + 148u, 32u), (const sint16 *)psx_addr(object + 40u, 8u));
    game_8009D5F0(object + 40u, object + 252u);
    SetRotMatrix((MATRIX *)psx_addr(object + 148u, 32u));
    for (index = 0u; index < 4u; ++index)
    {
        if (((R8(object + 942u) >> index) & 1u) == 0u)
        {
            uint32 suspension = R32(object + 840u + index * 4u);
            uint32 vector = object + 624u + index * 8u;
            uint32 output = object + 656u + index * 8u;
            uint32 height = R16(R32(object) + 66u + index * 8u);
            if ((sint32)suspension < 0)
                suspension += 255u;
            height -= (uint32)((sint32)suspension >> 8);
            height -= R32(0x800C0920u);
            R16(vector + 2u) = (uint16)height;
            xport_gte_write_data(0u, R32(vector));
            xport_gte_write_data(1u, R32(vector + 4u));
            xport_gte_mvmva(0x486012u);
            R16(output) = (uint16)xport_gte_read_data(9u);
            R16(output + 2u) = (uint16)xport_gte_read_data(10u);
            R16(output + 4u) = (uint16)xport_gte_read_data(11u);
            R16(vector + 2u) = (uint16)(R16(vector + 2u) + R32(0x800C0920u));
            ++contacts;
            sum += R32(object + 580u + index * 12u) - ((uint32)(sint32)(sint16)R16(output + 2u) << 8);
        }
    }
    if ((sint32)contacts > 0)
    {
        average = motion_divide(sum, contacts);
        kind = R8(object + 942u);
        R32(object + 104u) = average;
        if (R8(0x800BE990u + kind) != 0u && (R8(object + 943u) & (kind ^ 15u)) == 0u)
        {
            uint32 numerator = (average - R32(object + 824u) - 3000u) << 8;
            R32(object + 116u) = motion_divide(numerator, R32(0x800651C4u));
        }
    }
    game_80099A78(object);
}

static uint32 motion_shift(uint32 value, uint32 bits)
{
    if ((sint32)value < 0)
        value += (1u << bits) - 1u;
    return (uint32)((sint32)value >> bits);
}

static void motion_transform(uint32 source, uint32 destination, uint32 command, uint32 shift)
{
    uint32 component;
    xport_gte_write_data(0u, R32(source));
    xport_gte_write_data(1u, R32(source + 4u));
    xport_gte_mvmva(command);
    for (component = 0u; component < 3u; ++component)
        R32(destination + component * 4u) = xport_gte_read_data(25u + component) << shift;
}

void game_80097080(uint32 object)
{
    uint32 geometry = R32(object), index, mask = 0u;
    uint16 track = R16(object + 88u);
    uint8 sector = R8(object + 96u);
    uint32 offset = motion_shift((uint32)(sint32)(sint16)R16(geometry + 130u) * 181u, 8u);
    SetTransMatrix((MATRIX *)psx_addr(object + 148u, 32u));
    SetRotMatrix((MATRIX *)psx_addr(object + 148u, 32u));
    R8(object + 943u) = R8(object + 942u);
    for (index = 0u; index < 4u; ++index)
    {
        uint32 position = object + 576u + index * 12u;
        uint32 vector = object + 624u + index * 8u;
        uint32 context = object + 438u + index * 10u;
        uint32 previous[3], component, height, ground, suspension, magnitude;
        memcpy(previous, psx_addr(position, 12u), 12u);
        R16(object + 438u + index * 10u) = track;
        R8(object + 446u + index * 10u) = sector;
        height = R16(R32(object) + 66u + index * 8u);
        height -= motion_shift(R32(object + 840u + index * 4u), 8u);
        R16(vector + 2u) = (uint16)(height - offset);
        motion_transform(vector, position, 0x480012u, 0u);
        R16(vector + 2u) = (uint16)(R16(vector + 2u) + offset);
        motion_transform(vector, object + 760u + index * 16u, 0x480012u, 0u);
        for (component = 0u; component < 3u; ++component)
            R32(position + component * 4u) <<= 8;
        sub_8002C188(context, position);
        ground = sub_8002E408(context, position, object + 688u + index * 8u, 0u);
        suspension = R32(position + 4u) - ground + 3000u;
        R32(object + 872u + index * 4u) = suspension;
        for (component = 0u; component < 3u; ++component)
            R16(object + 720u + index * 8u + component * 2u) = (uint16)motion_shift(R32(position + component * 4u) - previous[component], 8u);
        magnitude = (sint32)suspension < 0 ? 0u - suspension : suspension;
        if ((sint32)magnitude < 512)
            R32(object + 872u + index * 4u) = 0u;
        if ((sint32)R32(object + 872u + index * 4u) >= 2049)
        {
            mask |= 1u << index;
            R32(object + 872u + index * 4u) = 12000u;
        }
        else
            R32(position + 4u) = ground;
    }
    R8(object + 942u) = (uint8)mask;
    for (index = 0u; index < 4u; ++index)
    {
        uint32 context = object + 398u + index * 10u;
        uint32 position = object + 480u + index * 12u;
        R16(context) = track;
        R8(context + 8u) = sector;
        motion_transform(geometry + index * 8u, position, 0x480012u, 8u);
        sub_8002C188(context, position);
        sub_8002DC38(context, position, position + 4u);
    }
}

void dr_race_quaternion_angles(const sint16 *angles, sint16 *output)
{
    sint32 x = angles[0] >> 1;
    sint32 y = angles[1] >> 1;
    sint32 z = angles[2] >> 1;
    uint32 cx = (uint32)draft_agent3_missing_800437EC((uint32)x), sx = 0u - (uint32)draft_agent3_missing_8004388C((uint32)x);
    uint32 cy = (uint32)draft_agent3_missing_800437EC((uint32)y), sy = 0u - (uint32)draft_agent3_missing_8004388C((uint32)y);
    uint32 cz = (uint32)draft_agent3_missing_800437EC((uint32)z), sz = 0u - (uint32)draft_agent3_missing_8004388C((uint32)z);
    sint16 quaternion[4];
    quaternion[3] = (sint16)((sint32)(cx * cz) >> 12);
    quaternion[0] = (sint16)((sint32)(sx * cz) >> 12);
    quaternion[1] = (sint16)((sint32)(sx * sz) >> 12);
    quaternion[2] = (sint16)((sint32)(cx * sz) >> 12);
    output[3] = (sint16)cy;
    output[0] = 0;
    output[1] = (sint16)sy;
    output[2] = 0;
    sub_80035458(output, quaternion, output);
    output[3] = (sint16)(0u - (uint32)(uint16)output[3]);
}

void game_8009D5F0(uint32 angles, uint32 output)
{
    dr_race_quaternion_angles((const sint16 *)psx_addr(angles, 6u), (sint16 *)psx_addr(output, 8u));
}

static uint32 quaternion_subtract(uint32 first, uint32 second)
{
    sint64 result = (sint64)(sint32)first - (sint64)(sint32)second;
    if (result < (-2147483647LL - 1LL) || result > 2147483647LL)
    {
        draft_agent1_missing_diagnostic("Original quaternion signed SUB overflow\n");
        abort();
    }
    return (uint32)result;
}

void sub_80035458(const sint16 *first, const sint16 *second, sint16 *output)
{
    sint16 aw = first[3], bw = second[3];
    sint16 bx = second[0], by = second[1], bz = second[2];
    sint16 ax = first[0], ay = first[1], az = first[2];
    uint32 scalar = (uint32)((sint32)aw * bw);
    uint32 vector[3];
    SVECTOR gpl_input = {0};
    VECTOR gpl_output;
    xport_gte_write_data(9u, (uint32)(sint32)ax);
    xport_gte_write_data(10u, (uint32)(sint32)ay);
    xport_gte_write_data(11u, (uint32)(sint32)az);
    xport_gte_write_control(0u, (uint32)(sint32)bx);
    xport_gte_write_control(2u, (uint32)(sint32)by);
    xport_gte_write_control(4u, (uint32)(sint32)bz);
    xport_gte_execute(0x178000Cu);
    xport_gte_write_data(8u, (uint32)(sint32)bw);
    xport_gte_write_data(9u, (uint32)(sint32)ax);
    xport_gte_write_data(10u, (uint32)(sint32)ay);
    xport_gte_write_data(11u, (uint32)(sint32)az);
    scalar = quaternion_subtract(scalar, (uint32)((sint32)ax * bx));
    gpl_input.vx = ax;
    gpl_input.vy = ay;
    gpl_input.vz = az;
    gte_gpl12(&gpl_input, bw, &gpl_output);
    xport_gte_write_data(8u, (uint32)(sint32)aw);
    xport_gte_write_data(9u, (uint32)(sint32)bx);
    xport_gte_write_data(10u, (uint32)(sint32)by);
    xport_gte_write_data(11u, (uint32)(sint32)bz);
    scalar = quaternion_subtract(scalar, (uint32)((sint32)ay * by));
    gpl_input.vx = bx;
    gpl_input.vy = by;
    gpl_input.vz = bz;
    gte_gpl12(&gpl_input, aw, &gpl_output);
    vector[0] = xport_gte_read_data(25u);
    vector[1] = xport_gte_read_data(26u);
    vector[2] = xport_gte_read_data(27u);
    scalar = quaternion_subtract(scalar, (uint32)((sint32)az * bz));
    output[0] = (sint16)vector[0];
    output[1] = (sint16)vector[1];
    output[2] = (sint16)vector[2];
    output[3] = (sint16)((sint32)scalar >> 12);
}

void game_80099A78(uint32 object)
{
    uint32 kind = R8(object + 942u), previous = R8(object + 943u);
    uint32 index, torque_x = 0u, torque_z = 0u, impact_x = 0u, impact_z = 0u;
    uint32 impulse = 0u, count = 0u, impacts = 0u, time, scale, component;
    MATRIX transpose;
    if (kind == 15u)
        return;
    TransposeMatrix((MATRIX *)psx_addr(object + 148u, 32u), &transpose);
    SetRotMatrix(&transpose);
    for (index = 0u; index < 4u; ++index)
    {
        if (((kind >> index) & 1u) == 0u)
        {
            uint32 contact = object + 688u + index * 8u;
            uint32 vector = object + 624u + index * 8u;
            uint32 movement = object + 720u + index * 8u;
            uint32 x = (uint32)(sint32)(sint16)R16(vector);
            uint32 z = (uint32)(sint32)(sint16)R16(vector + 4u);
            uint32 transformed[3], force, dot, rate;
            xport_gte_write_data(0u, R32(contact));
            xport_gte_write_data(1u, R32(contact + 4u));
            xport_gte_mvmva(0x486012u);
            for (component = 0u; component < 3u; ++component)
                transformed[component] = (uint32)(sint32)(sint16)xport_gte_read_data(9u + component);
            force = motion_shift(transformed[1] * 3000u, 12u);
            torque_x += force * z;
            torque_z -= force * x;
            ++count;
            if (((previous >> index) & 1u) != 0u)
            {
                dot = (uint32)(sint32)(sint16)R16(movement) * (uint32)(sint32)(sint16)R16(contact);
                dot += (uint32)(sint32)(sint16)R16(movement + 2u) * (uint32)(sint32)(sint16)R16(contact + 2u);
                dot += (uint32)(sint32)(sint16)R16(movement + 4u) * (uint32)(sint32)(sint16)R16(contact + 4u);
                if ((sint32)dot < 0)
                    dot += 4095u;
                rate = (uint32)((sint32)dot >> 12);
                ++impacts;
                impulse += (uint32)((sint32)(rate + (dot >> 31)) >> 1);
                impact_x -= (rate * z) << 8;
                impact_z += (rate * x) << 8;
            }
        }
    }
    torque_x = motion_divide(torque_x, count);
    torque_z = motion_divide(torque_z, count);
    if ((sint32)impacts > 0)
    {
        impact_x = motion_divide(impact_x, impacts);
        impulse = motion_divide(impulse, impacts);
        impact_z = motion_divide(impact_z, impacts);
    }
    time = R32(0x800651C4u);
    scale = motion_divide(307200u, time);
    impact_x = motion_divide(torque_x + impact_x, scale);
    scale = motion_divide(140800u, time);
    impact_z = motion_divide(torque_z + impact_z, scale);
    R32(object + 136u) += impact_x;
    R32(object + 116u) += impulse + 3000u;
    R32(object + 144u) += impact_z;
    if ((sint32)impacts > 0 && previous < 16u)
    {
        switch (previous)
        {
            case 0u:
            case 1u:
            case 2u:
            case 4u:
            case 6u:
            case 8u:
            case 9u:
                if ((sint32)R32(object + 136u) > 4000)
                    R32(object + 136u) = 4000u;
                if ((sint32)R32(object + 136u) < -4000)
                    R32(object + 136u) = (uint32)-4000;
                if ((sint32)R32(object + 144u) > 4000)
                    R32(object + 144u) = 4000u;
                if ((sint32)R32(object + 144u) < -4000)
                    R32(object + 144u) = (uint32)-4000;
                break;
            case 3u:
            case 12u:
                if ((sint32)R32(object + 136u) > 4000)
                    R32(object + 136u) = 4000u;
                if ((sint32)R32(object + 136u) < -4000)
                    R32(object + 136u) = (uint32)-4000;
                break;
            case 5u:
            case 10u:
                if ((sint32)R32(object + 144u) > 4000)
                    R32(object + 144u) = 4000u;
                if ((sint32)R32(object + 144u) < -4000)
                    R32(object + 144u) = (uint32)-4000;
                break;
            default:
                break;
        }
    }
}

static void init_copy_chunks(uint32 destination, uint32 source, uint32 chunks)
{
    uint32 index;
    for (index = 0u; index < chunks; ++index)
    {
        uint32 words[4];
        memcpy(words, psx_addr(source, 16u), 16u);
        memcpy(psx_addr(destination, 16u), words, 16u);
        source += 16u;
        destination += 16u;
    }
}

static void init_copy_model(uint32 destination, uint32 source, uint32 bytes)
{
    /* Original SDK boundary is BIOS A0:2A memcpy */
    memcpy(psx_addr(destination, bytes), psx_addr(source, bytes), bytes);
}

uint32 sub_80019F70(void)
{
    uint32 base = R32(R32(0x80065184u));
    uint32 table = base + 16u;
    uint32 index, slot, palette, textures[5], chassis, block, bytes, allocation;
    uint32 player_count, pool_count, human_index, ai_index, extra_offset;
    sint32 x, y, palette_x, palette_y;
    int extra_a, extra_b;
    const char *path = (const char *)psx_addr(R32(0x80059B6Cu + R32(0x80065210u) * 4u), 1u);
    sub_800423EC(path, 1u);
    path = (const char *)psx_addr(R32(0x80059B6Cu + R32(0x80065210u) * 4u), 1u);
    sub_8004225C(path, table + 0x40000u, 0x20000u);
    sub_80015074(table + 0x40000u, table, 0x40000);
    index = 1u;
    if (R32(table) != 0u)
    {
        slot = table + 4u;
        do
        {
            R32(slot) += table;
            ++index;
            slot += 4u;
        } while (R32(table) >= index);
    }
    palette = sub_80039ADC(0x80064CC0u);
    textures[0] = sub_80039ADC(0x80064CC8u);
    palette_x = (sint16)R16(palette + 12u);
    x = (sint16)R16(textures[0] + 12u);
    y = (sint16)R16(textures[0] + 14u);
    R16(textures[0] + 16u) = (uint16)palette_x;
    palette_y = (sint16)R16(palette + 14u);
    R16(textures[0] + 18u) = (uint16)palette_y;
    R16(textures[0] + 10u) = GetClut(palette_x, palette_y);
    sub_8001B7B0(R32(table + 24u), (uint32)x, (uint32)y, (uint32)palette_x, (uint32)palette_y, 1u);
    textures[1] = sub_80039ADC(0x80064CD0u);
    sub_8001B7B0(R32(table + 28u), (uint32)(sint32)(sint16)R16(textures[1] + 12u), (uint32)(sint32)(sint16)R16(textures[1] + 14u), 0u, 0u, 0u);
    textures[2] = sub_80039ADC(0x80064CD8u);
    sub_8001B7B0(R32(table + 32u), (uint32)(sint32)(sint16)R16(textures[2] + 12u), (uint32)(sint32)(sint16)R16(textures[2] + 14u), 0u, 0u, 0u);
    textures[3] = sub_80039ADC(0x80064CE0u);
    sub_8001B7B0(R32(table + 36u), (uint32)(sint32)(sint16)R16(textures[3] + 12u), (uint32)(sint32)(sint16)R16(textures[3] + 14u), 0u, 0u, 0u);
    textures[4] = sub_80039ADC(0x80064CE8u);
    palette_x = (sint16)R16(palette + 12u);
    x = (sint16)R16(textures[4] + 12u);
    y = (sint16)R16(textures[4] + 14u);
    R16(textures[4] + 16u) = (uint16)palette_x;
    palette_y = (sint16)R16(palette + 14u);
    R16(textures[4] + 18u) = (uint16)palette_y;
    R16(textures[4] + 10u) = GetClut(palette_x, palette_y);
    sub_8001B7B0(R32(table + 40u), (uint32)x, (uint32)y, 0u, 0u, 0u);
    slot = sub_80039ADC(0x80064CF0u);
    sub_8001B7B0(R32(table + 48u), (uint32)(sint32)(sint16)R16(slot + 12u), (uint32)(sint32)(sint16)R16(slot + 14u), (uint32)(sint32)(sint16)R16(slot + 16u), (uint32)(sint32)(sint16)R16(slot + 18u), 1u);
    DrawSync(0);
    for (index = 0u; index < 5u; ++index)
    {
        R8(textures[index] + 22u) = 128u;
        R8(textures[index] + 23u) = 0u;
    }
    chassis = sub_80039ADC(0x80064CF8u);
    block = R32(table + 56u);
    init_copy_chunks(0x80066BB0u, block, 13u);
    {
        uint32 words[3];
        uint16 tail;
        memcpy(words, psx_addr(block + 208u, 12u), 12u);
        tail = R16(block + 220u);
        memcpy(psx_addr(0x80066C80u, 12u), words, 12u);
        R16(0x80066C8Cu) = tail;
    }
    block = R32(table + 56u);
    init_copy_chunks(0x80066C90u, block + 224u, 8u);
    R32(0x80066D10u) = R32(block + 352u);
    block = R32(table + 56u);
    init_copy_chunks(0x80066B2Cu, block + 356u, 8u);
    R32(0x80066BACu) = R32(block + 484u);
    if (R32(0x800593A0u) == 1u)
        slot = 568u;
    else if (R32(0x80064C14u) == 0u || R32(0x80064B68u) != 0u)
        slot = 488u;
    else
        slot = 648u;
    init_copy_chunks(0x8007A9ECu, R32(table + 56u) + slot, 5u);
    extra_b = (sint16)R16(0x80066C42u) > 0;
    extra_a = (sint16)R16(0x80066C50u) > 0;
    sub_80017484(R32(table + 4u));
    sub_8001AB18(R32(table + 4u), chassis, textures[0]);
    sub_80017484(R32(table + 8u));
    sub_8001AB18(R32(table + 8u), chassis, textures[0]);
    sub_80017484(R32(table + 12u));
    sub_8001AB18(R32(table + 12u), chassis, textures[4]);
    if (extra_a)
    {
        sub_80017484(R32(table + 16u));
        sub_8001AB18(R32(table + 16u), chassis, textures[1]);
    }
    if (extra_b)
    {
        sub_80017484(R32(table + 20u));
        sub_8001AB18(R32(table + 20u), chassis, textures[1]);
    }
    player_count = R32(0x80065254u);
    R32(0x80064CB4u) = 0u;
    R32(0x80064CBCu) = 0u;
    for (index = 0u; (sint32)index < (sint32)player_count; ++index)
    {
        uint32 kind = R8(0x80059A28u + index);
        if (kind < 2u)
            ++R32(0x80064CB4u);
        else if (kind == 2u)
            ++R32(0x80064CBCu);
    }
    pool_count = R32(0x80065254u) - R32(0x80064CB4u);
    if ((sint32)pool_count >= 6)
        pool_count = 5u;
    else if ((sint32)pool_count <= 0)
        pool_count = 1u;
    bytes = R32(table + 8u) - R32(table + 4u);
    R32(0x80064CB8u) = pool_count;
    for (index = 0u; (sint32)index < (sint32)R32(0x80064CB4u); ++index)
    {
        allocation = sub_80042544(bytes);
        R32(0x80073BFCu + index * 4u) = allocation;
        init_copy_model(allocation, R32(table + 4u), bytes);
    }
    bytes = R32(table + 12u) - R32(table + 8u);
    for (index = 0u; (sint32)index < (sint32)R32(0x80064CB8u); ++index)
    {
        allocation = sub_80042544(bytes);
        R32(0x80073C3Cu + index * 4u) = allocation;
        init_copy_model(allocation, R32(table + 8u), bytes);
        R8(0x80073BECu + index) = 255u;
    }
    bytes = R32(table + 16u) - R32(table + 12u);
    for (index = 0u; (sint32)index < (sint32)R32(0x80064CBCu); ++index)
    {
        allocation = sub_80042544(bytes);
        R32(0x80073C7Cu + index * 4u) = allocation;
        init_copy_model(allocation, R32(table + 12u), bytes);
    }
    if (extra_a)
        R32(0x80065268u) = R32(table + 20u) - R32(table + 16u);
    if (extra_b)
        R32(0x80065264u) = R32(table + 24u) - R32(table + 20u);
    for (index = 0u; (sint32)index < (sint32)(R32(0x80064CB8u) + R32(0x80064CB4u)); ++index)
    {
        if (extra_a)
        {
            allocation = sub_80042544(R32(0x80065268u));
            R32(0x80073CFCu + index * 4u) = allocation;
            init_copy_model(allocation, R32(table + 16u), R32(0x80065268u));
        }
        if (extra_b)
        {
            allocation = sub_80042544(R32(0x80065264u));
            R32(0x80073CBCu + index * 4u) = allocation;
            init_copy_model(allocation, R32(table + 20u), R32(0x80065264u));
        }
    }
    ai_index = 0u;
    human_index = 0u;
    extra_offset = R32(0x80064CB8u) * 4u;
    for (index = 0u; (sint32)index < (sint32)R32(0x80065254u); ++index)
    {
        uint32 object = 0x80066D14u + index * 952u;
        uint32 kind;
        sub_800172A4(R32(table + 4u));
        kind = R8(0x80059A28u + index);
        if (kind < 2u)
        {
            allocation = R32(0x80073BFCu + human_index * 4u);
            R32(object + 24u) = allocation;
            R32(object + 28u) = allocation;
            ++human_index;
            R32(object + 32u) = extra_a ? R32(0x80073CFCu + extra_offset) : 0u;
            if (extra_b)
            {
                R32(object + 36u) = R32(0x80073CBCu + extra_offset);
                extra_offset += 4u;
            }
            else
                R32(object + 36u) = 0u;
        }
        else if (kind == 2u)
        {
            allocation = R32(0x80073C7Cu + ai_index * 4u);
            ++ai_index;
            R32(object + 24u) = allocation;
            R32(object + 28u) = allocation;
        }
    }
    return sub_8002F934(table + 60u);
}

uint32 sub_800172A4(uint32 model)
{
    sint32 maximum_x = 0, maximum_y = 0, maximum_z = 0;
    sint32 count = (sint16)R16(model + 14u), index, value, difference, vertical;
    uint32 vertex = model + R32(model + 44u), width, depth, radius;
    const uint32 geometry = 0x80066BB0u;
    for (index = 0; index < count; ++index, vertex += 8u)
    {
        value = (sint16)R16(vertex);
        if (maximum_x < value)
            maximum_x = value;
        if (maximum_x < -value)
            maximum_x = -value;
        value = (sint16)R16(vertex + 2u);
        if (maximum_y < value)
            maximum_y = value;
        if (maximum_y < -value)
            maximum_y = -value;
        value = (sint16)R16(vertex + 4u);
        if (maximum_z < value)
            maximum_z = value;
        if (maximum_z < -value)
            maximum_z = -value;
    }
    difference = (sint16)R16(geometry + 132u) - (sint16)R16(geometry + 64u);
    if (maximum_x < difference)
        maximum_x = difference;
    width = (uint32)maximum_x + 20u;
    depth = (uint32)maximum_z - 20u;
    R16(geometry + 32u) = (uint16)(0u - width);
    R16(geometry + 34u) = (uint16)maximum_y;
    R16(geometry + 36u) = (uint16)depth;
    R16(geometry + 40u) = (uint16)width;
    R16(geometry + 42u) = (uint16)maximum_y;
    R16(geometry + 44u) = (uint16)depth;
    R16(geometry + 48u) = (uint16)(0u - width);
    R16(geometry + 50u) = (uint16)maximum_y;
    R16(geometry + 52u) = (uint16)(0u - depth);
    R16(geometry + 56u) = (uint16)width;
    R16(geometry + 58u) = (uint16)maximum_y;
    R16(geometry + 60u) = (uint16)(0u - depth);
    vertical = (sint16)R16(geometry + 130u) * 181 / 256;
    vertical -= (sint16)R16(geometry + 66u);
    R16(geometry) = (uint16)(0u - width);
    R16(geometry + 4u) = (uint16)depth;
    R16(geometry + 8u) = (uint16)width;
    R16(geometry + 12u) = (uint16)depth;
    R16(geometry + 16u) = (uint16)(0u - width);
    R16(geometry + 20u) = (uint16)(0u - depth);
    R16(geometry + 24u) = (uint16)width;
    R16(geometry + 28u) = (uint16)(0u - depth);
    R16(geometry + 2u) = (uint16)(0u - (uint32)vertical);
    R16(geometry + 10u) = (uint16)(0u - (uint32)vertical);
    R16(geometry + 18u) = (uint16)(0u - (uint32)vertical);
    R16(geometry + 26u) = (uint16)(0u - (uint32)vertical);
    R16(geometry + 134u) = (uint16)vertical;
    radius = sub_8003B674(width * width + (uint32)vertical * (uint32)vertical + depth * depth);
    R16(geometry + 128u) = (uint16)radius;
    return radius;
}

uint32 sub_8002F934(uint32 table)
{
    uint32 side, index, cursor, total, allocation, model, heads, end, vertices;
    uint32 group, ordinal, input, entries, node, linkage;
    const uint32 gp = 0x80064A10u;
    R8(gp + 2521u) = 0u;
    R8(gp + 2520u) = 0u;
    R8(gp + 2525u) = 0u;
    R8(gp + 2524u) = 0u;
    R8(gp + 2529u) = 0u;
    R8(gp + 2528u) = 0u;
    R8(gp + 2533u) = 0u;
    R8(gp + 2532u) = 0u;
    R8(gp + 2536u) = 0u;
    R8(gp + 2540u) = 255u;
    R32(gp + 2544u) = 0u;
    sub_8002FCE0();
    {
        uint32 count = R32(0x80065254u), pool = R32(0x80064CB8u);
        for (index = 0u; (sint32)index < (sint32)count; ++index)
        {
            R16(0x80066E58u + index * 952u) = 0u;
            if ((sint32)index < (sint32)pool)
                R32(0x8007A3F8u + index * 4u) = 0u;
        }
    }
    cursor = table;
    for (side = 0u; side < 2u; ++side)
    {
        total = 0u;
        for (group = 0u; group < 13u; ++group)
        {
            input = R32(cursor);
            cursor += 4u;
            total += 1u + ((R32(cursor) - input) >> 3);
        }
        allocation = sub_80042544(total * 16u);
        R32(0x800650B4u + side * 4u) = allocation;
        if (allocation == 0u)
            xport_mips_break(0x000101CDu);
    }
    cursor = table;
    for (side = 0u; side < 2u; ++side)
    {
        model = side != 0u ? R32(0x80073C3Cu) : R32(0x80066D2Cu);
        heads = 0x8007A438u + side * 600u;
        end = heads + (uint32)((sint32)(sint16)R16(model + 14u) * 2);
        vertices = model + R32(model + 44u);
        linkage = heads;
        while (linkage < end)
        {
            R16(linkage) = 0u;
            linkage += 2u;
        }
        R16(linkage) = 65535u;
        ordinal = 0u;
        for (group = 0u; group < 13u; ++group)
        {
            input = R32(cursor);
            cursor += 4u;
            entries = (R32(cursor) - input) >> 3;
            node = R32(0x800650B4u + side * 4u) + ordinal * 16u;
            R32(0x8007A330u + side * 52u + group * 4u) = node;
            linkage = entries != 0u ? node + 12u : linkage;
            for (index = 0u; (sint32)index < (sint32)entries; ++index)
            {
                sint32 vertex_index = (sint16)R16(input + 6u);
                uint32 vertex = vertices + (uint32)vertex_index * 8u;
                uint32 head = heads + (uint32)vertex_index * 2u;
                sint32 previous;
                R32(linkage) = (R32(linkage) & 0x000FFFFFu) | ((uint32)vertex_index << 20);
                R8(linkage) = (uint8)group;
                R16(linkage - 10u) = R16(vertex);
                R16(linkage - 6u) = R16(vertex + 2u);
                R16(linkage - 2u) = R16(vertex + 4u);
                R16(node) = R16(input);
                R16(linkage - 8u) = R16(input + 2u);
                R16(linkage - 4u) = R16(input + 4u);
                previous = (sint16)R16(head);
                if (previous == 0)
                {
                    R32(linkage) &= 0xFFF000FFu;
                    R16(head) = (uint16)(ordinal + index + 1u);
                }
                else
                {
                    uint32 previous_node = R32(0x800650B4u + side * 4u) + (uint32)previous * 16u - 16u;
                    uint32 previous_link = R32(previous_node + 12u) & 0x000FFF00u;
                    if (previous_link == 0u)
                        previous_link = ((uint32)previous & 4095u) << 8;
                    R32(linkage) = (R32(linkage) & 0xFFF000FFu) | previous_link;
                    R32(previous_node + 12u) = (R32(previous_node + 12u) & 0xFFF000FFu) | (((uint32)((sint32)(sint16)(ordinal + index) + 1) & 4095u) << 8);
                }
                linkage += 16u;
                node += 16u;
                input += 8u;
            }
            R8(node + 12u) = 255u;
            ordinal += 1u + entries;
        }
    }
    /* The callee overwrites A1 before reading it */
    return sub_8002FDA4(65535u);
}

void sub_8002FCE0(void)
{
    uint32 index, texture;
    /* These immutable strings equal the original formatter results Hicar through Hicar4 */
    for (index = 0u; index < 4u; ++index)
    {
        texture = sub_80039ADC(0x80064CC8u + index * 8u);
        R16(0x8007A324u + index * 2u) = R16(texture + 8u);
    }
    texture = sub_80039ADC(0x800650C8u);
    R16(0x8007A32Cu) = R16(texture + 8u);
    texture = sub_80039ADC(0x800650D0u);
    R16(0x8007A32Eu) = R16(texture + 8u);
}

uint32 sub_8002FDA4(uint32 mask)
{
    uint32 index;
    for (index = 0u; (sint32)index < (sint32)R32(0x80065254u); ++index)
    {
        if (((1u << (index & 31u)) & mask) != 0u)
        {
            uint32 object = 0x80066E4Cu + index * 952u;
            uint32 value, entry, offset;
            R8(object + 66u) = 0u;
            R8(object + 65u) = 255u;
            R16(object + 8u) = 0u;
            entry = R32(0x800651E4u) + R32(0x80065210u) * 4u;
            if ((sint32)R32(0x80064C14u) < (sint32)index)
            {
                value = R16(entry + 2782u) * R8(0x800661E4u + index * 44u);
                if ((sint32)value < 0)
                    value += 127u;
                value >>= 7;
            }
            else
                value = R16(entry + 2780u);
            R16(object + 10u) = (uint16)value;
            for (offset = 0u; offset < 13u; ++offset)
                R16(object + 50u - offset * 2u) = 0u;
            for (offset = 0u; offset < 13u; ++offset)
                R8(object + 64u - offset) = 0u;
            R16(object + 20u) = R16(0x80066C54u);
            R16(object + 18u) = R16(0x80066C46u);
        }
    }
    R8(0x800653F8u) = 0u;
    R8(0x800653FCu) = 255u;
    return 0xFFFFFFFFu;
}
