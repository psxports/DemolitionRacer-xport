#include "draft_signatures.h"
#include <stdio.h>

/* Unverified draft translations */
/* TODO: Supply unavailable SDK and native adapter declarations at integration */
uint32 draft_agent2_missing_800182D8(uint32 index, uint32 object, uint32 data, uint32 offset);
uint32 draft_agent2_missing_draw_otag_result(uint32 ordering_table);
uint32 draft_agent2_missing_spu_voice_attr_result(SpuVoiceAttr *attributes);
uint32 draft_agent2_missing_8004D9D4(uint32 port);
uint32 draft_agent2_missing_8004DCF4(uint32 event);
uint32 draft_agent2_missing_8004DC84(uint32 port);
uint32 draft_agent2_missing_8004DFD4(uint32 port);
sint32 CdControlBPSX(uint8 command, uint8 *parameter, uint8 *result);
sint32 CdRead(sint32 count, uint32 *destination, sint32 mode);
sint32 CdReadSync(sint32 mode, uint8 *result);
sint32 CdReset(sint32 mode);

static uint32 draft2_mul(uint32 left, uint32 right)
{
    return left * right;
}

static sint32 draft2_div4096(uint32 value)
{
    return (sint32)value / 4096;
}

void sub_80015074(uint32 source, uint32 destination, sint32 capacity)
{
    uint32 unpacked;
    if (r_u8(source) != 'R' || r_u8(source + 1u) != 'N' || r_u8(source + 2u) != 'C')
    {
        draft_agent1_missing_diagnostic((const char *)psx_addr(0x80010078u, 1u));
        return;
    }
    unpacked = ((uint32)r_u8(source + 4u) << 24) | ((uint32)r_u8(source + 5u) << 16) | ((uint32)r_u8(source + 6u) << 8) | r_u8(source + 7u);
    if (capacity < (sint32)unpacked)
    {
        draft_agent1_missing_diagnostic((const char *)psx_addr(0x80010084u, 1u));
        return;
    }
    sub_80047254();
    sub_8004395C((const uint8 *)psx_addr(source, 1u), (uint8 *)psx_addr(destination, 1u));
    sub_800437BC();
    /* TODO: Void projection discards the original incidental return */
}

uint32 sub_80015150(uint32 source)
{
    if (r_u8(source) == 'R' && r_u8(source + 1u) == 'N' && r_u8(source + 2u) == 'C')
        return (((uint32)r_u8(source + 4u) << 24) | ((uint32)r_u8(source + 5u) << 16) | ((uint32)r_u8(source + 6u) << 8) | r_u8(source + 7u)) + 1024u;
    draft_agent1_missing_diagnostic((const char *)psx_addr(0x80010078u, 1u));
    return 0u;
}

uint32 sub_80017484(uint32 model)
{
    uint32 stream = model + r_u32(model + 48u);
    uint32 result = r_u32(model + 52u);
    uint32 first = model + result;
    uint32 second = model + r_u32(model + 56u);
    uint32 type, item, color, vertex, stride;
    sint32 count;
    while (r_u8(stream + 3u) != 0u)
    {
        type = r_u8(stream + 2u);
        count = (sint16)r_u16(stream);
        stream += 4u;
        if (type <= 1u)
        {
            stride = type == 0u ? 32u : 40u;
            for (item = 0u; (sint32)item < count; ++item)
            {
                w_u8(stream + 8u, 128u);
                w_u8(stream + 9u, 128u);
                w_u8(stream + 10u, 128u);
                stream += 20u;
                w_u8(first + 4u, 128u);
                w_u8(first + 5u, 128u);
                w_u8(first + 6u, 128u);
                first += stride;
                w_u8(second + 4u, 128u);
                w_u8(second + 5u, 128u);
                w_u8(second + 6u, 128u);
                second += stride;
            }
        }
        else if (type == 2u || type == 3u)
        {
            stride = type == 2u ? 40u : 52u;
            for (item = 0u; (sint32)item < count; ++item)
            {
                color = stream;
                for (vertex = 0u; vertex < (type == 2u ? 3u : 4u); ++vertex)
                {
                    w_u8(color + 8u, 128u);
                    w_u8(color + 9u, 128u);
                    w_u8(color + 10u, 128u);
                    color += 4u;
                }
                stream += type == 2u ? 44u : 56u;
                first += stride;
                second += stride;
            }
        }
        result = r_u8(stream + 3u);
    }
    return result;
}

uint32 sub_80018278(void)
{
    uint32 path = r_u32(0x80064C60u);
    uint32 size = sub_80042488((const char *)psx_addr(path, 1u), 1u);
    uint32 buffer;
    size = (uint32)((sint32)(size + 2047u) >> 9) << 9;
    buffer = sub_80042544(size);
    path = r_u32(0x80064C60u);
    sub_8004225C((const char *)psx_addr(path, 1u), buffer, size);
    return buffer;
}

uint32 sub_800197B8(uint32 flags)
{
    uint32 texture = sub_80039ADC(0x80064C98u);
    sint32 x, y;
    uint32 allocation = 0u, index, object, table_offset;
    uint16 clut;
    w_u16(0x80064C78u, r_u16(texture + 12u));
    x = (sint16)r_u16(texture + 12u);
    y = (sint16)r_u16(texture + 14u);
    w_u16(0x80064C7Au, (uint16)(r_u16(texture + 14u) + 16u));
    w_u16(0x80064C88u, r_u16(texture + 12u));
    w_u16(0x80064C8Au, (uint16)(r_u16(texture + 14u) + 16u));
    w_u16(0x80064C90u, (uint16)(r_u16(texture + 12u) + 128u));
    clut = r_u16(texture + 14u);
    w_u32(0x80064C70u, (uint32)x);
    w_u32(0x80064C74u, (uint32)y);
    w_u16(0x80064C92u, (uint16)(clut + 16u));
    texture = sub_80039ADC(0x80064CA0u);
    clut = r_u16(texture + 10u);
    sub_80039ADC(0x80064CA8u);
    if ((flags & 32u) != 0u)
        allocation = sub_80018278();
    object = 0x80066D14u;
    for (index = 0u, table_offset = 0u; (sint32)index < (sint32)r_u32(0x80065254u); ++index)
    {
        sub_8001993C(index, (uint32)(sint32)(sint16)clut);
        if ((flags & 32u) != 0u)
            draft_agent2_missing_800182D8(index, object, allocation, r_u32(0x800661D8u + table_offset));
        sub_8003132C(object, flags);
        table_offset += 44u;
        object += 952u;
    }
    if ((flags & 32u) != 0u)
        return sub_8004261C(allocation);
    return flags & 32u;
}

uint32 sub_80019D80(uint32 index, uint32 destination, uint32 source)
{
    return sub_80019A3C(index, (const uint8 *)psx_addr(destination, 3u), (const uint8 *)psx_addr(source, 3u));
}

uint32 sub_8001B7B0(uint32 tim, uint32 x, uint32 y, uint32 clut_x, uint32 clut_y, uint32 load_clut)
{
    uint32 block, bytes;
    PSX_RECT rectangle;
    if (r_u32(tim) != 16u)
        return 16u;
    block = tim + 8u;
    if (load_clut != 0u)
    {
        rectangle.x = (sint16)clut_x;
        rectangle.y = (sint16)clut_y;
        rectangle.w = (sint16)r_u16(block + 8u);
        rectangle.h = (sint16)r_u16(block + 10u);
        LoadImagePSX(&rectangle, (uint32 *)psx_addr(block + 12u, 1u));
    }
    bytes = r_u32(block);
    block += (bytes >> 2) << 2;
    if (r_u32(block) != 4u)
    {
        rectangle.x = (sint16)x;
        rectangle.y = (sint16)y;
        rectangle.w = (sint16)r_u16(block + 8u);
        rectangle.h = (sint16)r_u16(block + 10u);
        LoadImagePSX(&rectangle, (uint32 *)psx_addr(block + 12u, 1u));
    }
    return (uint32)DrawSync(0);
}

uint32 sub_8001C958(void)
{
    uint32 path = r_u32(0x80064D10u);
    uint32 buffer = r_u32(0x80065238u);
    uint32 size = sub_80042488((const char *)psx_addr(path, 1u), 1u);
    uint32 result, data, info, length;
    path = r_u32(0x80064D10u);
    data = buffer + 2048u;
    result = sub_8004225C((const char *)psx_addr(path, 1u), buffer, size);
    if ((sint32)size > 0)
    {
        info = 0x800742E0u;
        for (;;)
        {
            length = r_u32(buffer);
            buffer += 8u;
            if ((sint32)length <= 0)
                break;
            result = sub_8001E20C(data, length, (uint32 *)psx_addr(info, 12u));
            info += 12u;
            data += length;
        }
    }
    return result;
}

void sub_8001E320(uint32 value)
{
}

uint32 sub_8001F8FC(uint32 voice, sint32 frequency, sint32 pan, sint32 volume)
{
    uint32 sound = (uint32)(sint32)(sint16)r_u16(0x80073FB8u + voice * 32u);
    uint32 product = draft2_mul(r_u32(0x800742E4u + sound * 12u), (uint32)frequency);
    sint32 pitch = (sint32)product / 44100;
    uint32 left, right, amount;
    sint32 clamped;
    if (pitch >= 16384)
        pitch = 16383;
    left = (uint32)volume;
    if (pan >= 0)
    {
        amount = (uint32)draft2_div4096(draft2_mul((uint32)volume, (uint32)pan));
        right = (uint32)volume - amount;
        if ((sint32)right < 0)
            right = amount - (uint32)volume;
        right <<= 2;
    }
    else
    {
        left = (uint32)volume + (uint32)draft2_div4096(draft2_mul((uint32)volume, (uint32)pan));
        if ((sint32)left < 0)
            left = 0u - left;
        right = (uint32)volume << 2;
    }
    w_u32(0x800742A4u, 19u);
    w_u32(0x800742A0u, 1u << (voice & 31u));
    w_u16(0x800742B4u, (uint16)pitch);
    clamped = (sint32)right;
    if (clamped < -16384)
        clamped = -16384;
    else if (clamped >= 16384)
        clamped = 16383;
    w_u16(0x800742A8u, (uint16)clamped);
    left <<= 2;
    clamped = (sint32)left;
    if (clamped < -16384)
        clamped = -16384;
    else if (clamped >= 16384)
        clamped = 16383;
    w_u16(0x800742AAu, (uint16)clamped);
    /* TODO: Original SpuSetVoiceAttr result needs a reviewed return adapter */
    return draft_agent2_missing_spu_voice_attr_result((SpuVoiceAttr *)psx_addr(0x800742A0u, sizeof(SpuVoiceAttr)));
}

uint32 sub_80020430(uint32 index)
{
    uint32 offset = index * 20u;
    uint32 entry = (uint32)(sint32)(sint16)r_u16(0x80059C14u + offset);
    uint32 channel, path;
    w_u32(0x800652CCu, index);
    channel = r_u8(0x80059C12u + offset);
    path = r_u32(0x80059C00u + entry * 4u);
    return (uint32)sub_800201EC(path, channel);
}

uint32 sub_80023F78(uint32 index)
{
    uint32 entry = 0x80074700u + index * 4396u;
    uint32 count = 0u, i;
    for (i = 0u; i < 15u; ++i, entry += 292u)
        if ((sint16)r_u16(entry) == 1)
            ++count;
    return count;
}

uint32 sub_80039934(void)
{
    uint32 result, environment;
    DrawSync(0);
    environment = r_u32(0x80065118u);
    PutDrawEnv((DRAWENV *)psx_addr(environment, sizeof(DRAWENV)));
    environment = r_u32(0x80065118u);
    PutDispEnv((DISPENV *)psx_addr(environment + 92u, sizeof(DISPENV)));
    result = r_u32(0x8006512Cu) + 1u;
    w_u32(0x8006512Cu, result);
    return result;
}

uint32 sub_80039A10(void)
{
    uint32 environment, table, entries;
    DrawSync(0);
    environment = r_u32(0x80065118u);
    table = r_u32(environment + 112u);
    entries = r_u32(0x800654F0u);
    /* TODO: Original DrawOTag return requires a native adapter */
    return draft_agent2_missing_draw_otag_result(table + entries * 4u - 4u);
}

void sub_80039A4C(uint32 value)
{
    w_u32(0x800654F4u, value);
}

uint32 sub_8003BF48(uint32 projection)
{
    sub_8004FABC(projection);
    w_u32(0x80065150u, projection);
    return sub_8003BC1C();
}

uint32 sub_8003C034(const sint32 *vector)
{
    uint32 result;
    result = (uint32)(vector[0] >> 8);
    w_u32(0x80081B98u, result);
    result = (uint32)(vector[1] >> 8);
    w_u32(0x80081B9Cu, result);
    result = (uint32)(vector[2] >> 8);
    w_u32(0x80081BA0u, result);
    result = (uint32)(vector[0] >> 8);
    w_u32(0x80081BB8u, result);
    result = (uint32)(vector[1] >> 8);
    w_u32(0x80081BBCu, result);
    result = (uint32)(vector[2] >> 8);
    w_u32(0x80081BC0u, result);
    return result;
}

uint32 sub_8004225C(const char *path, uint32 buffer, uint32 capacity)
{
    draft_agent1_missing_diagnostic("LOADING %s\n", path);
    return dr_data_read(path, buffer, capacity);
}

void sub_8004FABC(uint32 projection)
{
    xport_gte_write_control(26u, projection);
}

uint32 sub_80018430(uint32 index, uint32 object)
{
    uint32 bank, row, item, packet, mesh, angle;
    sint32 radius, height, value;
    for (bank = 0u; bank < 2u; ++bank)
    {
        packet = 0x8006A8D4u + index * 768u + bank * 12288u;
        for (row = 0u; row < 4u; ++row)
            for (item = 0u; item < 8u; ++item)
            {
                uint32 address = packet + row * 192u + item * 24u;
                w_u8(address + 3u, 5u);
                w_u8(address + 7u, 40u);
                w_u8(address + 4u, 0u);
                w_u8(address + 5u, 0u);
                w_u8(address + 6u, 0u);
            }
    }
    radius = (sint16)r_u16(object + 130u) * 195 / 256;
    height = (sint16)r_u16(object + 132u) + 4;
    mesh = 0x800708D4u + index * 144u;
    for (item = 0u; item < 18u; item += 2u)
    {
        w_u16(mesh, (uint16)(0u - (uint32)height));
        angle = item << 8;
        value = draft2_div4096(draft2_mul((uint32)rcos((sint32)angle), (uint32)radius));
        w_u16(mesh + 2u, (uint16)value);
        value = draft2_div4096(draft2_mul((uint32)rsin((sint32)angle), (uint32)radius));
        w_u16(mesh + 4u, (uint16)value);
        w_u16(mesh + 8u, (uint16)height);
        value = draft2_div4096(draft2_mul((uint32)rcos((sint32)angle), (uint32)radius));
        w_u16(mesh + 10u, (uint16)value);
        value = draft2_div4096(draft2_mul((uint32)rsin((sint32)angle), (uint32)radius));
        w_u16(mesh + 12u, (uint16)value);
        mesh += 16u;
    }
    return 0u;
}

uint32 sub_8001860C(void)
{
    uint32 texture = sub_80039ADC(0x80064C68u);
    uint32 tile, destination, item, bank, packet, count, result;
    sint32 color;
    for (tile = 0u; tile < 4u; ++tile)
    {
        destination = 0x800739D4u + tile * 6u;
        w_u16(destination, r_u16(texture + 8u));
        w_u16(destination + 2u, r_u16(texture + 10u));
        w_u8(destination + 4u, (uint8)(r_u8(texture + 24u) + ((tile & 1u) ? 32u : 0u)));
        w_u8(destination + 5u, (uint8)(r_u8(texture + 25u) + ((tile & 2u) ? 32u : 0u)));
    }
    for (item = 0u; (sint32)item < (sint32)(r_u32(0x80065254u) << 3); ++item)
        for (bank = 0u; bank < 2u; ++bank)
        {
            packet = 0x800711D4u + item * 40u + bank * 5120u;
            w_u8(packet + 3u, 9u);
            w_u8(packet + 7u, 44u);
            w_u8(packet + 4u, 32u);
            w_u8(packet + 5u, 32u);
            w_u8(packet + 6u, 32u);
        }
    count = r_u32(0x80065254u);
    result = count;
    for (item = 0u; (sint32)item < (sint32)r_u32(0x80065254u); ++item)
    {
        for (color = 3; color >= 0; --color)
            w_u8(0x80066E8Fu + item * 952u + (uint32)color, 255u);
        result = (sint32)(item + 1u) < (sint32)r_u32(0x80065254u);
    }
    return result;
}

uint32 sub_8001AB18(uint32 model, uint32 texture_a, uint32 texture_b)
{
    uint32 stream = model + r_u32(model + 48u);
    uint32 result = r_u32(model + 52u);
    uint32 first = model + result;
    uint32 second = model + r_u32(model + 56u);
    uint32 type, item, vertex, texture, stride, vertices, uv_step, tpage_offset;
    uint32 width_a, height_a, width_b, height_b, width, height;
    uint32 origin_u_a, origin_v_a, origin_u_b, origin_v_b, origin_u, origin_v;
    uint32 coordinate, offset;
    uint16 palette;
    sint32 count;
    while (r_u8(stream + 3u) != 0u)
    {
        count = (sint16)r_u16(stream);
        type = r_u8(stream + 2u);
        stream += 4u;
        origin_u_a = r_u8(texture_a + 24u);
        origin_v_a = r_u8(texture_a + 25u);
        origin_u_b = r_u8(texture_b + 24u);
        origin_v_b = r_u8(texture_b + 25u);
        height_a = (uint8)(r_u8(texture_a + 23u) - 1u) + 1u;
        width_a = (uint8)(r_u8(texture_a + 22u) - 1u) + 1u;
        width_b = (uint8)(r_u8(texture_b + 22u) - 1u) + 1u;
        height_b = (uint8)(r_u8(texture_b + 23u) - 1u) + 1u;
        if (type <= 3u)
        {
            vertices = (type & 1u) ? 4u : 3u;
            stride = type == 0u ? 32u : type == 3u ? 52u : 40u;
            uv_step = type < 2u ? 8u : 12u;
            tpage_offset = type < 2u ? 22u : 26u;
            for (item = 0u; (sint32)item < count; ++item)
            {
                if (r_u16(second + 14u) != 0u)
                {
                    texture = texture_a;
                    width = width_a;
                    height = height_a;
                    origin_u = origin_u_a;
                    origin_v = origin_v_a;
                }
                else
                {
                    texture = texture_b;
                    width = width_b;
                    height = height_b;
                    origin_u = origin_u_b;
                    origin_v = origin_v_b;
                }
                palette = r_u16(texture + 8u);
                w_u16(first + tpage_offset, palette);
                w_u16(second + tpage_offset, palette);
                palette = r_u16(texture + 10u);
                w_u16(first + 14u, palette);
                w_u16(second + 14u, palette);
                for (vertex = 0u; vertex < vertices; ++vertex)
                {
                    offset = 12u + vertex * uv_step;
                    coordinate = origin_u + ((uint32)r_u8(first + offset) * width >> 8);
                    w_u8(first + offset, (uint8)coordinate);
                    w_u8(second + offset, (uint8)coordinate);
                }
                for (vertex = 0u; vertex < vertices; ++vertex)
                {
                    offset = 13u + vertex * uv_step;
                    coordinate = origin_v + ((uint32)r_u8(first + offset) * height >> 8);
                    w_u8(first + offset, (uint8)coordinate);
                    w_u8(second + offset, (uint8)coordinate);
                }
                first += stride;
                second += stride;
                stream += type < 2u ? 20u : type == 2u ? 44u : 56u;
            }
        }
        result = r_u8(stream + 3u);
    }
    return result;
}

uint32 sub_80024A30(uint32 port)
{
    uint32 state = 3u, event, selection;
    draft_agent2_missing_8004D9D4(port << 4);
    for (;;)
    {
        if (draft_agent2_missing_8004DCF4(r_u32(0x800652FCu)) == 1u)
        {
            state = 0u;
            break;
        }
        if (draft_agent2_missing_8004DCF4(r_u32(0x80065300u)) == 1u)
        {
            state = 1u;
            break;
        }
        if (draft_agent2_missing_8004DCF4(r_u32(0x80065304u)) == 1u)
        {
            state = 2u;
            break;
        }
        if (draft_agent2_missing_8004DCF4(r_u32(0x80065308u)) == 1u)
            break;
    }
    if (state - 1u < 2u)
    {
        /* TODO: Bind the original CARD ERROR format string */
        draft_agent1_missing_diagnostic("CARD ERROR\n");
        return 0u;
    }
    if (state == 3u)
    {
        for (event = 0u; event < 4u; ++event)
            draft_agent2_missing_8004DCF4(r_u32(0x8006530Cu + event * 4u));
        draft_agent2_missing_8004DC84(port << 4);
        while (draft_agent2_missing_8004DCF4(r_u32(0x8006530Cu)) != 1u && draft_agent2_missing_8004DCF4(r_u32(0x80065310u)) != 1u && draft_agent2_missing_8004DCF4(r_u32(0x80065314u)) != 1u && draft_agent2_missing_8004DCF4(r_u32(0x80065318u)) != 1u)
            ;
    }
    for (event = 0u; event < 4u; ++event)
        draft_agent2_missing_8004DCF4(r_u32(0x800652FCu + event * 4u));
    draft_agent2_missing_8004DFD4(port << 4);
    for (;;)
    {
        selection = 0u;
        if (draft_agent2_missing_8004DCF4(r_u32(0x800652FCu)) == 1u)
            break;
        selection = 1u;
        if (draft_agent2_missing_8004DCF4(r_u32(0x80065300u)) == 1u)
            break;
        selection = 2u;
        if (draft_agent2_missing_8004DCF4(r_u32(0x80065304u)) == 1u)
            break;
        selection = 3u;
        if (draft_agent2_missing_8004DCF4(r_u32(0x80065308u)) == 1u)
            break;
    }
    if (selection == 0u)
    {
        w_u32(0x80075704u + port * 4396u, 1u);
        w_u32(0x80075708u + port * 4396u, 1u);
    }
    if (selection == 3u)
    {
        w_u32(0x80075704u + port * 4396u, 1u);
        w_u32(0x80075708u + port * 4396u, 0u);
    }
    sub_80023C2C(port);
    return 1u;
}

uint32 sub_8003A578(void)
{
    uint32 player, packet, state, port, actuator, buttons, previous, value, mode;
    for (player = 0u; player < 2u; ++player)
    {
        packet = 0x80081B20u + player * 34u;
        port = player * 16u;
        state = r_u16(packet) & 0xF0FFu;
        w_u32(0x80065538u + player * 4u, state);
        if (state == 0x4000u || state == 0x5000u || state == 0x7000u)
        {
            actuator = 0x800654F8u + player * 4u;
            mode = PadGetStatePSX(port);
            mode = mode <= 1u ? 0u : mode == 4u || mode == 5u ? 3u : mode == 6u ? 1u : 2u;
            if (mode == 1u)
            {
                if (r_u8(actuator + 2u) != 0u)
                {
                    PadSetActAlignPSX(port, 0x8006513Cu);
                    PadSetActPSX(port, actuator, 2u);
                }
                else if (PadSetActAlignPSX(port, 0x8006513Cu) != 0u)
                    w_u8(actuator + 2u, 1u);
            }
            else
                w_u8(actuator + 2u, 0u);
            if (state != 0x4000u)
                w_u32(0x80065508u + player * 4u, r_u8(packet + 6u));
            buttons = ~(uint32)r_u16(packet + 2u);
            previous = r_u32(0x80065144u + player * 4u);
            w_u16(0x80065524u + player * 2u, (uint16)(~previous & buttons));
            w_u16(0x80065520u + player * 2u, (uint16)buttons);
            w_u32(0x80065144u + player * 4u, buttons);
            if (state != 0x4000u)
            {
                mode = r_u32(0x80065530u + player * 4u);
                if (mode != 0u)
                {
                    buttons = r_u16(0x80065520u + player * 2u);
                    value = (buttons & r_u32(0x80065528u + player * 4u)) != 0u ? 256u : 0u;
                    w_u32(0x80065510u + player * 4u, value);
                    value = (buttons & r_u32(0x80065530u + player * 4u)) != 0u ? 256u : 0u;
                    w_u32(0x80065518u + player * 4u, value);
                }
                else
                {
                    value = r_u8(packet + 5u);
                    if (value >= 128u)
                    {
                        w_u32(0x80065510u + player * 4u, 0u);
                        w_u32(0x80065518u + player * 4u, (value - 127u) * 2u);
                    }
                    else
                    {
                        w_u32(0x80065510u + player * 4u, (128u - value) * 2u);
                        w_u32(0x80065518u + player * 4u, 0u);
                    }
                }
            }
        }
        else if (state == 0x2000u)
        {
            w_u32(0x80065508u + player * 4u, r_u8(packet + 4u));
            w_u32(0x80065510u + player * 4u, r_u8(packet + 5u));
            w_u32(0x80065518u + player * 4u, r_u8(packet + 6u));
            buttons = ~(uint32)r_u16(packet + 2u);
            if (r_u32(0x80065510u + player * 4u) >= 65u)
                buttons |= 0x4000u;
            if (r_u8(packet + 7u) >= 65u)
                buttons |= 0x400u;
            previous = r_u32(0x80065144u + player * 4u);
            w_u16(0x80065524u + player * 2u, (uint16)(~previous & buttons));
            w_u16(0x80065520u + player * 2u, (uint16)buttons);
            w_u32(0x80065144u + player * 4u, buttons);
        }
        else
        {
            w_u16(0x80065520u + player * 2u, 0u);
            w_u16(0x80065524u + player * 2u, 0u);
            w_u32(0x80065508u + player * 4u, 0u);
            w_u32(0x80065510u + player * 4u, 0u);
            w_u32(0x80065518u + player * 4u, 0u);
            w_u32(0x80065500u + player * 4u, 0u);
        }
    }
    return 0u;
}

uint32 sub_8003B898(void)
{
    uint32 factor_y = r_u32(0x80065168u);
    uint32 scale = r_u32(0x8006516Cu);
    uint32 factor_x, shift, horizontal, vertical, value;
    uint32 source, destination, row, column, result = 0u;
    horizontal = (uint32)draft2_div4096(draft2_mul(factor_y, scale));
    factor_x = r_u32(0x80065164u);
    shift = r_u32(0x80065170u) & 31u;
    horizontal = (uint32)((sint32)horizontal >> shift);
    w_u32(0x80065558u, horizontal);
    vertical = (uint32)((sint32)draft2_div4096(draft2_mul(factor_x, scale)) >> shift);
    w_u32(0x8006555Cu, vertical);
    /* TODO: Preserve interleaved source loads and destination stores for aliases */
    for (row = 0u; row < 3u; ++row)
    {
        source = row == 0u ? 0x80081B84u : row == 1u ? 0x80081C24u : 0x80081C04u;
        destination = row == 0u ? 0x80081BA4u : row == 1u ? 0x80081BE4u : 0x80081BC4u;
        for (column = 0u; column < 3u; ++column)
        {
            value = (uint32)(sint32)(sint16)r_u16(source + column * 2u);
            value = (uint32)draft2_div4096(draft2_mul(value, horizontal));
            w_u16(destination + column * 2u, (uint16)value);
        }
    }
    vertical = r_u32(0x8006555Cu);
    for (row = 0u; row < 3u; ++row)
    {
        source = row == 0u ? 0x80081B84u : row == 1u ? 0x80081C24u : 0x80081C04u;
        destination = row == 0u ? 0x80081BA4u : row == 1u ? 0x80081BE4u : 0x80081BC4u;
        for (column = 3u; column < 6u; ++column)
        {
            value = (uint32)(sint32)(sint16)r_u16(source + column * 2u);
            result = (uint32)draft2_div4096(draft2_mul(value, vertical));
            w_u16(destination + column * 2u, (uint16)result);
        }
    }
    shift = r_u32(0x80065170u) & 31u;
    for (row = 0u; row < 3u; ++row)
    {
        source = row == 0u ? 0x80081B84u : row == 1u ? 0x80081C24u : 0x80081C04u;
        destination = row == 0u ? 0x80081BA4u : row == 1u ? 0x80081BE4u : 0x80081BC4u;
        for (column = 6u; column < 9u; ++column)
        {
            value = (uint32)(sint32)(sint16)r_u16(source + column * 2u);
            w_u16(destination + column * 2u, (uint16)((sint32)value >> shift));
        }
    }
    return result;
}

static uint32 draft2_round12(uint32 value)
{
    return (uint32)((sint32)(value + 2048u) >> 12);
}

extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);

uint32 dr_camera_angles(const sint16 *angles)
{
    uint32 sin_x = (uint32)draft_agent3_missing_800437EC(angles[0]);
    uint32 cos_x = (uint32)draft_agent3_missing_8004388C(angles[0]);
    uint32 sin_y = (uint32)draft_agent3_missing_800437EC(angles[1]);
    uint32 cos_y = (uint32)draft_agent3_missing_8004388C(angles[1]);
    uint32 sin_z = (uint32)draft_agent3_missing_800437EC(angles[2]);
    uint32 cos_z = (uint32)draft_agent3_missing_8004388C(angles[2]);
    uint32 xy = draft2_round12(draft2_mul(cos_x, cos_y));
    uint32 xs = draft2_round12(draft2_mul(cos_x, sin_y));
    w_u16(0x80081B92u, (uint16)cos_x);
    w_u16(0x80081C24u, (uint16)sin_z);
    w_u16(0x80081C2Au, (uint16)cos_z);
    w_u16(0x80081C30u, 0u);
    w_u16(0x80081C32u, (uint16)cos_x);
    w_u16(0x80081C34u, (uint16)sin_x);
    w_u16(0x80081C04u, (uint16)sin_z);
    w_u16(0x80081C08u, 0u);
    w_u16(0x80081C0Au, (uint16)cos_z);
    w_u16(0x80081C0Cu, (uint16)sin_z);
    w_u16(0x80081C0Eu, 0u);
    w_u16(0x80081C10u, 0u);
    w_u16(0x80081C12u, 0u);
    w_u16(0x80081C06u, (uint16)(0u - cos_z));
    w_u16(0x80081C14u, 4096u);
    w_u16(0x80081B86u, (uint16)draft2_round12(draft2_mul(0u - sin_x, cos_z)));
    w_u16(0x80081B8Cu, (uint16)draft2_round12(draft2_mul(sin_x, sin_z)));
    w_u16(0x80081C26u, r_u16(0x80081B86u));
    w_u16(0x80081C2Cu, r_u16(0x80081B8Cu));
    w_u16(0x80081B90u, (uint16)draft2_round12(draft2_mul(0u - sin_x, cos_y)));
    w_u16(0x80081B94u, (uint16)draft2_round12(draft2_mul(sin_x, sin_y)));
    w_u16(0x80081C28u, (uint16)draft2_round12(draft2_mul(cos_x, sin_z)));
    w_u16(0x80081C2Eu, (uint16)draft2_round12(draft2_mul(0u - cos_x, sin_z)));
    w_u16(0x80081B84u, (uint16)draft2_round12(draft2_mul(sin_y, sin_z) - draft2_mul(xy, cos_z)));
    w_u16(0x80081B88u, (uint16)draft2_round12(draft2_mul(cos_y, sin_z) + draft2_mul(xs, cos_z)));
    w_u16(0x80081B8Au, (uint16)draft2_round12(draft2_mul(sin_y, cos_z) + draft2_mul(xy, sin_z)));
    w_u16(0x80081B8Eu, (uint16)draft2_round12(draft2_mul(cos_y, cos_z) - draft2_mul(xs, sin_z)));
    return sub_8003B898();
}

uint32 sub_8003C1F8(uint32 angles)
{
    return dr_camera_angles((const sint16 *)psx_addr(angles, 6u));
}

const uint16 *sub_8003CED8(const MATRIX *matrix, uint32 transform)
{
    VECTOR translation;
    uint32 column, row, output, word;
    translation.vx = (sint32)((uint32)((sint32)r_u32(transform + 16u) >> 8) - r_u32(0x80081BB8u));
    translation.vy = (sint32)((uint32)((sint32)r_u32(transform + 20u) >> 8) - r_u32(0x80081BBCu));
    translation.vz = (sint32)((uint32)((sint32)r_u32(transform + 24u) >> 8) - r_u32(0x80081BC0u));
    dr_geometry_apply_matrix((const MATRIX *)psx_addr(0x80081BA4u, sizeof(MATRIX)), &translation, (VECTOR *)psx_addr(0x80081C78u, sizeof(VECTOR)));
    for (column = 0u; column < 3u; ++column)
    {
        for (row = 0u; row < 3u; ++row)
            xport_gte_write_data(9u + row, (uint16)matrix->m[row][column]);
        xport_gte_mvmva(0x49E012u);
        output = 0x80081C64u + column * 2u;
        for (row = 0u; row < 3u; ++row)
            w_u16(output + row * 6u, (uint16)xport_gte_read_data(9u + row));
    }
    for (row = 0u; row < 5u; ++row)
    {
        word = r_u32(0x80081C64u + row * 4u);
        xport_gte_write_control(row, word);
    }
    for (row = 0u; row < 3u; ++row)
    {
        word = r_u32(0x80081C78u + row * 4u);
        xport_gte_write_control(5u + row, word);
    }
    return (const uint16 *)matrix->m + 1u;
}
