#include "draft_signatures.h"
#include <stdint.h>

/* Unverified draft translation; module is not registered */
#define D32(a) (*(uint32 *)psx_addr((uint32)(a), 4u))
#define D16(a) (*(uint16 *)psx_addr((uint32)(a), 2u))
#define D8(a) (*(uint8 *)psx_addr((uint32)(a), 1u))
#define GP 0x80064A10u

/* TODO: Resolve external SDK and game helpers without inventing implementations */
extern uint32 draft_agent3_missing_8003DC58(uint32 rectangle, const void *pixels);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern uint32 draft_agent3_missing_80049F84(uint32 enabled, uint32 voices);
extern uint32 draft_agent3_missing_8004AD94(uint32 bytes);
extern uint32 draft_agent3_missing_8004A224(uint32 mode);
extern uint32 draft_agent3_missing_8004A254(uint32 address);
extern uint32 draft_agent3_missing_8004BAC4(uint32 mode);
extern uint32 draft_agent3_missing_8001F860(void);
extern uint32 draft_agent3_missing_8001FBE4(uint32 voice, sint32 volume, uint32 mode, sint32 pitch);
extern uint32 draft_agent3_missing_8004BB74(const void *attributes);
extern uint32 draft_agent3_missing_8003D9AC(uint32 mode);
extern uint32 draft_agent3_missing_8003D914(uint32 mode);
extern uint32 draft_agent3_missing_8003DE98(uint32 ordering_table, uint32 entries);
extern void draft_agent3_missing_8003DD18(const sint16 *rectangle, uint32 x, uint32 y);
extern void draft_agent3_missing_8003DF44(uint32 packet);
extern void draft_agent3_missing_8004FB3C(uint32 red, uint32 green, uint32 blue);
extern void draft_agent3_missing_80049A64(const MATRIX *left, const MATRIX *right, MATRIX *out);
extern void draft_agent3_missing_80049B74(const MATRIX *left, MATRIX *right);
extern void draft_agent3_missing_80049C84(const MATRIX *matrix);
extern void draft_agent3_missing_8004FB0C(const MATRIX *matrix);
extern void draft_agent3_missing_8004FB5C(const sint16 *light_matrix);
extern void draft_agent3_missing_800556CC(uint32 source, uint32 bytes);
extern void draft_agent3_missing_dispatch_packet(uint32 target, uint32 packet);

static sint32 draft_product(uint32 left, uint32 right)
{
    return (sint32)(left * right);
}

static sint32 draft_fixed_round(uint32 value)
{
    return (sint32)(value + 2048u) >> 12;
}

uint32 sub_80019A3C(uint32 clut_offset, const uint8 *first_color, const uint8 *second_color)
{
    uint32 palette, i;
    const uint8 *colors[2] = {first_color, second_color};
    for (palette = 0; palette < 2; ++palette)
    {
        uint32 red = colors[palette][0], green = colors[palette][1], blue = colors[palette][2];
        uint32 red_sum = 0, green_sum = 0, blue_sum = 0;
        for (i = 0; i < 32; ++i)
        {
            uint32 r, g, b;
            if (i >= 25)
            {
                r = red * (32u - i) >> 3;
                g = green * (32u - i) >> 3;
                b = blue * (32u - i) >> 3;
            }
            else
            {
                r = red_sum / 24u;
                g = green_sum / 24u;
                b = blue_sum / 24u;
            }
            D16(0x800739ECu + palette * 64u + i * 2u) = (uint16)((b << 10) + (g << 5) + 0x8000u + r);
            red_sum += red;
            green_sum += green;
            blue_sum += blue;
        }
    }
    D16(GP + 634u) = (uint16)(D16(GP + 612u) + clut_offset);
    return draft_agent3_missing_8003DC58(0x80064C88u, psx_addr(0x800739ECu, 128u));
}

uint32 sub_80019C6C(uint32 clut_offset, uint32 color_bits)
{
    uint32 color, entry = 128u;
    for (color = 0; color < 8; ++color)
    {
        uint32 index = ((color_bits >> color) & 1u) + 2u * (color / 2u);
        uint32 start = 0x80059B2Cu + index * 4u, end = 0x80059B4Cu + index * 4u;
        sint32 red = 8 * D8(start), green = 8 * D8(start + 1u), blue = 8 * D8(start + 2u);
        sint32 dr = D8(end) - D8(start), dg = D8(end + 1u) - D8(start + 1u), db = D8(end + 2u) - D8(start + 2u);
        uint32 step;
        for (step = 0; step < 8; ++step)
        {
            D16(0x800739ECu + entry++ * 2u) = (uint16)(((uint32)(uint16)(blue >> 3) << 10) + ((uint32)(green >> 3) << 5) + (uint32)(red >> 3));
            red += dr;
            green += dg;
            blue += db;
        }
    }
    D16(GP + 642u) = (uint16)(D16(GP + 612u) + clut_offset);
    return draft_agent3_missing_8003DC58(0x80064C90u, psx_addr(0x80073AECu, 128u));
}

uint32 sub_80019DA0(uint32 palette, uint8 *base_color, uint8 *mixed_color, uint32 palette_index, uint32 blend, uint32 fade)
{
    uint16 packed = D16(palette + (palette_index << 6));
    sint32 fade_white = draft_product(0u - 255u, fade) / 40;
    uint32 white = (uint32)fade_white + 255u;
    sint32 result = 0;
    uint32 channel;
    uint8 channels[3];
    channels[0] = (uint8)((packed & 31u) << 3);
    channels[1] = (uint8)((packed & 0x3E0u) >> 2);
    channels[2] = (uint8)((packed & 0x7C00u) >> 7);
    base_color[0] = channels[0];
    base_color[1] = channels[1];
    base_color[2] = channels[2];
    for (channel = 0; channel < 3; ++channel)
    {
        uint32 c = channels[channel];
        uint32 faded = c + (uint32)(draft_product(0u - c, fade) / 40) - white;
        result = draft_product(faded, blend) / 40;
        mixed_color[channel] = (uint8)((uint32)fade_white - 1u + (uint32)result);
    }
    return (uint32)result;
}

uint32 sub_8001D01C(uint32 sound_index)
{
    sub_8001E858(sound_index + 35u, sound_index + 20u, 4096u, 0u, 4096u);
    return draft_agent3_missing_80049F84(1u, 1u << ((sound_index + 20u) & 31u));
}

uint32 sub_8001E20C(uint32 header, uint32 total_bytes, uint32 *sound_info)
{
    uint32 bytes = total_bytes - 80u, allocation = draft_agent3_missing_8004AD94(bytes), result;
    draft_agent3_missing_8004A224(1u);
    draft_agent3_missing_8004A254(allocation);
    sub_8004A914(header + 80u, bytes);
    draft_agent3_missing_8004BAC4(1u);
    sound_info[0] = allocation;
    sound_info[2] = (bytes << 5) / 7u;
    result = ((uint32)D8(header + 18u) << 8) + D8(header + 19u);
    sound_info[1] = result;
    return result;
}

uint32 sub_8001E858(uint32 sound_index, uint32 voice, uint32 pitch, uint32 mode, uint32 volume)
{
    uint32 selected = voice, offset, mask;
    sint32 scaled_pitch = draft_product(pitch, D32(0x80064BFCu)) / 4096;
    sint32 limited_volume = (sint32)volume;
    if (selected == 0xFFFFFFFFu)
    {
        selected = draft_agent3_missing_8001F860();
        if (selected == 0xFFFFFFFFu)
            return 0xFFFFFFFFu;
    }
    offset = selected << 5;
    D16(0x80073FB8u + offset) = (uint16)sound_index;
    D32(0x80073FA4u + offset) = 0;
    D32(0x80073FA0u + offset) = 0;
    D32(0x80073FB4u + offset) = volume;
    D16(0x80073FBCu + offset) = (uint16)mode;
    if (scaled_pitch == -1)
        scaled_pitch = (sint32)D32(0x80073FB0u + offset);
    else
        D32(0x80073FB0u + offset) = (uint32)scaled_pitch;
    if ((sint32)volume >= 0x4000)
        limited_volume = 0x3FFF;
    mask = 1u << (selected & 31u);
    draft_agent3_missing_80049F84(0u, mask);
    D32(0x800742A0u) = mask;
    D32(0x800742A4u) = 128u;
    D32(0x800742BCu) = D32(0x800742E0u + sound_index * 12u);
    draft_agent3_missing_8004BB74(psx_addr(0x800742A0u, 1u));
    if (D32(GP + 2204u))
        draft_agent3_missing_8001FBE4(selected, limited_volume, mode, scaled_pitch);
    else
        sub_8001F8FC(selected, limited_volume, (sint32)mode, scaled_pitch);
    draft_agent3_missing_80049F84(1u, mask);
    return voice;
}

uint32 sub_8002B4B0(void)
{
    sint16 rectangle[4] = {256, (sint16)((D32(0x80065218u) << 8) + 180u), 63, 63};
    sint32 sine, cosine;
    uint32 phase;
    draft_agent3_missing_8003DD18(rectangle, 256u, ((D32(0x80065218u) ^ 1u) << 8) | 0xB4u);
    draft_agent3_missing_8003D9AC(0u);
    D16(GP + 1696u) = (uint16)(D16(GP + 1696u) + 64u);
    phase = (uint32)(sint32)(sint16)D16(GP + 1696u);
    sine = draft_agent3_missing_800437EC(phase);
    cosine = draft_agent3_missing_8004388C(phase);
    D16(0x80078C60u) = (uint16)((sint32)((uint32)-21 * (uint32)sine + 21u * (uint32)cosine) / 4096 + 277);
    D16(0x80078C62u) = (uint16)((sint32)((uint32)-21 * (uint32)cosine - 21u * (uint32)sine) / 4096 + 201);
    D16(0x80078C68u) = (uint16)((sint32)(21u * (uint32)sine + 21u * (uint32)cosine) / 4096 + 277);
    D16(0x80078C6Au) = (uint16)((sint32)(21u * (uint32)cosine - 21u * (uint32)sine) / 4096 + 201);
    D16(0x80078C70u) = (uint16)((sint32)((uint32)-21 * (uint32)sine - 21u * (uint32)cosine) / 4096 + 277);
    D16(0x80078C72u) = (uint16)((sint32)((uint32)-21 * (uint32)cosine + 21u * (uint32)sine) / 4096 + 201);
    D16(0x80078C78u) = (uint16)((sint32)(21u * (uint32)sine - 21u * (uint32)cosine) / 4096 + 277);
    D16(0x80078C7Au) = (uint16)((sint32)(21u * (uint32)cosine + 21u * (uint32)sine) / 4096 + 201);
    draft_agent3_missing_8003DF44(0x80078C58u);
    return draft_agent3_missing_8003D9AC(0u);
}

uint32 sub_8002B6AC(void)
{
    return sub_8002B4B0();
}

uint32 sub_80039690(uint32 ordering_table_entries)
{
    draft_agent3_missing_8003D9AC(0u);
    D32(GP + 2784u) = ordering_table_entries;
    D32(0x80081AA8u) = sub_80042544(ordering_table_entries << 2);
    draft_agent3_missing_8003DE98(D32(0x80081AA8u), D32(GP + 2784u));
    D32(0x80081B1Cu) = sub_80042544(ordering_table_entries << 2);
    return draft_agent3_missing_8003DE98(D32(0x80081B1Cu), D32(GP + 2784u));
}

void sub_8003AAFC(uint32 value)
{
}

uint32 sub_8003BF7C(uint32 value)
{
    uint32 result = D32(GP + 1888u) + 2u;
    D32(GP + 1852u) = value;
    D32(GP + 2876u) = (uint32)((sint32)value >> (result & 31u));
    return result;
}

void sub_8003C0B4(uint32 red, uint32 green, uint32 blue)
{
    draft_agent3_missing_8004FB3C(red << 1, green << 1, blue << 1);
}

uint32 sub_8003C0DC(uint32 row, const sint16 *vector, uint32 x_scale, uint32 y_scale, uint32 z_scale)
{
    D16(0x80081B64u + row * 6u) = (uint16)vector[0];
    D16(0x80081B66u + row * 6u) = (uint16)vector[1];
    D16(0x80081B68u + row * 6u) = (uint16)vector[2];
    D16(0x80081C44u + row * 2u) = (uint16)(x_scale << 5);
    D16(0x80081C4Au + row * 2u) = (uint16)(y_scale << 5);
    D16(0x80081C50u + row * 2u) = (uint16)(z_scale << 5);
    return 0x80081C4Au + row * 2u;
}

void sub_8003C164(const MATRIX *matrix)
{
    MATRIX product;
    draft_agent3_missing_80049A64((const MATRIX *)psx_addr(0x80081B64u, sizeof(MATRIX)), matrix, &product);
    draft_agent3_missing_8004FB0C(&product);
    draft_agent3_missing_8004FB5C((const sint16 *)psx_addr(0x80081C44u, 18u));
}

void sub_800416E0(uint32 packet)
{
    uint32 selector = (uint32)(sint32)(sint16)D16(packet + 10u);
    draft_agent3_missing_dispatch_packet(D32(0x8005CD78u + (selector << 2)), packet);
}

uint8 *sub_80041A5C(uint32 sector, uint8 *position)
{
    uint32 frames = sector % 75u;
    uint32 seconds = sector % 4500u / 75u;
    uint32 minutes = sector / 4500u;
    position[3] = 0;
    position[2] = (uint8)((frames / 10u << 4) + frames % 10u);
    position[1] = (uint8)((seconds / 10u << 4) + seconds % 10u);
    position[0] = (uint8)((minutes / 10u << 4) + minutes % 10u);
    return position;
}

uint32 sub_800423EC(const char *path, uint32 enabled)
{
    if (enabled)
        return ((D32(0x800839F4u + (uint32)sub_80041D50(path) * 40u) + 2047u) >> 11) << 11;
    return enabled;
}

uint32 sub_80042544(uint32 bytes)
{
    uint32 heap = D32(GP + 1908u), previous = D32(heap), block = D32(previous);
    uint32 units = ((bytes + 15u) >> 4) + 1u, remaining;
    for (;;)
    {
        if ((sint32)D32(block + 4u) >= (sint32)units)
            break;
        previous = block;
        if (block == D32(D32(GP + 1908u)))
        {
            for (;;)
            {
                draft_agent3_missing_8003D914(0u);
                draft_agent3_missing_8003D914(1u);
            }
        }
        block = D32(block);
    }
    remaining = D32(block + 4u) - units;
    if (!remaining)
        D32(previous) = D32(block);
    else
    {
        D32(block + 4u) = remaining;
        block += remaining << 4;
        D32(block + 4u) = units;
    }
    heap = D32(GP + 1908u);
    D32(heap) = previous;
    D32(heap + 8u) -= units << 4;
    D32(block + 8u) = 0x12345678u;
    return block + 16u;
}

uint32 sub_8004261C(uint32 allocation)
{
    uint32 heap = D32(GP + 1908u), previous = D32(heap), block = allocation - 16u, next;
    D32(heap + 8u) += D32(allocation - 12u) << 4;
    while (!(previous < block) || block >= D32(previous))
    {
        next = D32(previous);
        if (previous >= next && (previous < block || block < next))
            break;
        previous = next;
    }
    next = D32(previous);
    if (block + (D32(block + 4u) << 4) == next)
    {
        D32(block + 4u) += D32(next + 4u);
        D32(block) = D32(next);
    }
    else
        D32(block) = next;
    if (previous + (D32(previous + 4u) << 4) == block)
    {
        D32(previous + 4u) += D32(block + 4u);
        D32(previous) = D32(block);
    }
    else
        D32(previous) = block;
    heap = D32(GP + 1908u);
    D32(heap) = previous;
    return heap;
}

uint32 sub_8004A914(uint32 source, uint32 bytes)
{
    uint32 count = bytes > 0x7EFF0u ? 0x7EFF0u : bytes;
    draft_agent3_missing_800556CC(source, count);
    if (!D32(0x8005FFA8u))
        D32(0x8005FFA4u) = 0;
    return count;
}

/* TODO: Resolve directory SDK APIs with native local buffers */
extern uint32 draft_agent3_missing_first_file(const char *pattern, uint8 *entry);
extern uint32 draft_agent3_missing_next_file(uint8 *entry);
extern void draft_agent3_missing_8004DCC4(void *destination, const void *source);
extern void draft_agent3_missing_8004DD04(void *destination, const void *source);
extern sint32 draft_agent3_missing_compare_name(const char *left, const char *right, uint32 count);

uint32 sub_80023C2C(uint32 card_slot)
{
    uint32 slot, used = 0, offset = 0, result;
    uint8 entry[40], prefix[48];
    uint32 base = card_slot * 4396u;
    for (slot = 0; slot < 15; ++slot)
    {
        uint32 item = base + slot * 292u;
        D8(0x800746E0u + item) = 0;
        D8(0x800746F6u + item) = 0;
        D16(0x80074700u + item) = 0;
        D16(0x80074702u + item) = (uint16)slot;
    }
    result = card_slot << 4;
    if (!draft_agent3_missing_first_file((const char *)psx_addr(card_slot ? 0x80064E80u : 0x80064E78u, 1u), entry))
        return result;
    do
    {
        uint32 item = base + offset, first = used, i;
        uint32 prefix_address = GP + (card_slot ? 1152u : 1144u);
        uint32 prefix_word = D32(prefix_address);
        prefix[0] = (uint8)prefix_word;
        prefix[1] = (uint8)(prefix_word >> 8);
        prefix[2] = (uint8)(prefix_word >> 16);
        prefix[3] = (uint8)(prefix_word >> 24);
        prefix[4] = (uint8)D16(prefix_address + 4u);
        prefix[5] = (uint8)(D16(prefix_address + 4u) >> 8);
        draft_agent3_missing_8004DCC4(prefix, entry);
        if (draft_agent3_missing_compare_name((const char *)entry, (const char *)psx_addr(0x80011064u, 1u), 12u))
        {
            uint32 file_bytes = (uint32)entry[24] | (uint32)entry[25] << 8 | (uint32)entry[26] << 16 | (uint32)entry[27] << 24;
            sint32 blocks = (sint32)file_bytes / 8192;
            D16(0x80074700u + item) = 2;
            D16(0x80074702u + item) = (uint16)used;
            draft_agent3_missing_8004DD04(psx_addr(0x800746E0u + item, 1u), entry);
            for (i = 0; i < 5; ++i)
                D8(0x800746F6u + item + i) = D8(0x80064E98u + i);
            offset += 292u;
            ++used;
            if (blocks >= 2)
            {
                sint32 block;
                for (block = 1; block < blocks; ++block)
                {
                    item = base + offset;
                    D16(0x80074700u + item) = 2;
                    D16(0x80074702u + item) = (uint16)first;
                    offset += 292u;
                    ++used;
                }
            }
        }
        else
        {
            for (i = 0; i < 8; ++i)
                D8(0x800746F6u + item + i) = entry[12u + i];
            D8(0x800746FEu + item) = 0;
            draft_agent3_missing_8004DD04(psx_addr(0x800746E0u + item, 1u), entry);
            offset += 292u;
            D16(0x80074702u + item) = (uint16)used;
            D16(0x80074700u + item) = 1;
            ++used;
        }
        result = draft_agent3_missing_next_file(entry);
    } while (result);
    return result;
}

uint32 sub_8003C960(sint16 *matrix, const sint16 *angles)
{
    sint32 sx = draft_agent3_missing_800437EC((uint32)(sint32)angles[0]);
    sint32 cx = draft_agent3_missing_8004388C((uint32)(sint32)angles[0]);
    sint32 sy = draft_agent3_missing_800437EC((uint32)(sint32)angles[1]);
    sint32 cy = draft_agent3_missing_8004388C((uint32)(sint32)angles[1]);
    sint32 sz = draft_agent3_missing_800437EC((uint32)(sint32)angles[2]);
    sint32 cz = draft_agent3_missing_8004388C((uint32)(sint32)angles[2]);
    sint32 cxcy = draft_fixed_round((uint32)cx * (uint32)cy);
    sint32 cxsy = draft_fixed_round((uint32)cx * (uint32)sy), result;
    matrix[5] = (sint16)(0u - (uint32)cx);
    matrix[2] = (sint16)draft_fixed_round((uint32)sx * (uint32)cy);
    matrix[3] = (sint16)draft_fixed_round((uint32)sx * (uint32)cz);
    matrix[4] = (sint16)draft_fixed_round((uint32)sx * (uint32)sz);
    matrix[8] = (sint16)draft_fixed_round((uint32)sx * (uint32)sy);
    matrix[0] = (sint16)draft_fixed_round((uint32)sy * (uint32)sz + (uint32)cxcy * (uint32)cz);
    matrix[1] = (sint16)draft_fixed_round(0u - (uint32)sy * (uint32)cz + (uint32)cxcy * (uint32)sz);
    matrix[6] = (sint16)draft_fixed_round(0u - (uint32)cy * (uint32)sz + (uint32)cxsy * (uint32)cz);
    result = draft_fixed_round((uint32)cy * (uint32)cz + (uint32)cxsy * (uint32)sz);
    matrix[7] = (sint16)result;
    return (uint32)result;
}

/* Bitstream decompressor with overlapping backward copies */
void sub_8004395C(const uint8 *source, uint8 *destination)
{
    const uint8 *input_cursor;
    uint32 bit_reservoir;
    int v4;
    int v5;
    int v6;
    int v7;
    int v8;
    int v9;
    int v10;
    int v11;
    int i;
    int v13;
    int offset_prefix;
    int v15;
    uint8 v16;
    uint8 v17;
    int v18;
    uint32 v19;
    uint32 copy_length;
    int v21;
    uint32 v22;
    int v23;
    int v24;
    int v25;
    uint32 v26;
    uint32 v27;
    int v28;
    int v29;
    int v30;
    int v31;
    int v32;
    uint8 *match_cursor;
    int v34;
    uint32 v35;
    uint8 v36;
    int v37;
    uint8 v38;
    uint8 v39;
    int v40;
    uint8 v41;
    int v42;
    uint8 v43;
    int v44;
    uint32 v45;
    int v46;
    int v47;
    int v48;
    int v49;
    int v50;
    uint32 v51;
    int v52;
    int v53;
    int v54;
    int v55;
    int v56;
    int v57;
    int v58;
    int v59;
    int v60;
    int *v61;
    sint16 v62;

    D32(0x80044020u) = (uint32)source[4] << 24 | (uint32)source[5] << 16 | (uint32)source[6] << 8 | source[7];
    D16(0x8004401Eu) = 0;
    input_cursor = source + 19;
    bit_reservoir = 2 * (2 * source[18] + 1);
    do
    {
        while (1)
        {
            while (1)
            {
                while (1)
                {
                    bit_reservoir *= 2;
                    v42 = (bit_reservoir >> 8) & 1;
                    if (v42 || (v43 = *input_cursor, ++input_cursor, *destination = v43, ++destination, bit_reservoir *= 2, (v42 = (bit_reservoir >> 8) & 1) != 0))
                    {
                        bit_reservoir = (uint8)bit_reservoir;
                        if ((uint8)bit_reservoir)
                            break;
                        v40 = (uint8)*input_cursor++;
                        bit_reservoir = 2 * v40 + v42;
                        if (((bit_reservoir >> 8) & 1) != 0)
                            break;
                    }
                    v41 = *input_cursor++;
                    *destination++ = v41;
                }
                copy_length = 2;
                offset_prefix = 0;
                v45 = 2 * bit_reservoir;
                v44 = (v45 >> 8) & 1;
                v45 = (uint8)v45;
                if (!(uint8)v45)
                {
                    v53 = (uint8)*input_cursor++;
                    v45 = 2 * v53 + v44;
                    v44 = (v45 >> 8) & 1;
                }
                if (v44)
                    break;
                v19 = 2 * v45;
                v18 = (v19 >> 8) & 1;
                v19 = (uint8)v19;
                if (!(uint8)v19)
                {
                    v5 = (uint8)*input_cursor++;
                    v19 = 2 * v5 + v18;
                    v18 = (v19 >> 8) & 1;
                }
                copy_length = v18 + 4;
                bit_reservoir = 2 * v19;
                v21 = (bit_reservoir >> 8) & 1;
                bit_reservoir = (uint8)bit_reservoir;
                if (!(uint8)bit_reservoir)
                {
                    v6 = (uint8)*input_cursor++;
                    bit_reservoir = 2 * v6 + v21;
                    v21 = (bit_reservoir >> 8) & 1;
                }
                if (!v21)
                    goto LABEL_17;
                v22 = copy_length - 1;
                bit_reservoir *= 2;
                v23 = (bit_reservoir >> 8) & 1;
                bit_reservoir = (uint8)bit_reservoir;
                if (!(uint8)bit_reservoir)
                {
                    v7 = (uint8)*input_cursor++;
                    bit_reservoir = 2 * v7 + v23;
                    v23 = (bit_reservoir >> 8) & 1;
                }
                copy_length = 2 * v22 + v23;
                if (copy_length != 9)
                    goto LABEL_17;
                for (i = 3; i >= 0; --i)
                {
                    bit_reservoir *= 2;
                    v13 = (bit_reservoir >> 8) & 1;
                    bit_reservoir = (uint8)bit_reservoir;
                    if (!(uint8)bit_reservoir)
                    {
                        v4 = (uint8)*input_cursor++;
                        bit_reservoir = 2 * v4 + v13;
                        v13 = (bit_reservoir >> 8) & 1;
                    }
                    offset_prefix = 2 * offset_prefix + v13;
                }
                v15 = offset_prefix + 2;
                do
                {
                    v16 = input_cursor[1];
                    *destination = *input_cursor;
                    destination[1] = v16;
                    v17 = input_cursor[3];
                    destination[2] = input_cursor[2];
                    destination[3] = v17;
                    input_cursor += 4;
                    --v15;
                    destination += 4;
                } while (v15 >= 0);
            }
            bit_reservoir = 2 * v45;
            v46 = (bit_reservoir >> 8) & 1;
            bit_reservoir = (uint8)bit_reservoir;
            if (!(uint8)bit_reservoir)
            {
                v54 = (uint8)*input_cursor++;
                bit_reservoir = 2 * v54 + v46;
                v46 = (bit_reservoir >> 8) & 1;
            }
            if (!v46)
                goto LABEL_31;
            copy_length = 3;
            bit_reservoir *= 2;
            v47 = (bit_reservoir >> 8) & 1;
            bit_reservoir = (uint8)bit_reservoir;
            if (!(uint8)bit_reservoir)
            {
                v55 = (uint8)*input_cursor++;
                bit_reservoir = 2 * v55 + v47;
                v47 = (bit_reservoir >> 8) & 1;
            }
            if (v47)
            {
                v48 = (uint8)*input_cursor++;
                v49 = v48 == 0;
                copy_length = v48 + 8;
                if (v49)
                    break;
            }
        LABEL_17:
            bit_reservoir *= 2;
            v24 = (bit_reservoir >> 8) & 1;
            bit_reservoir = (uint8)bit_reservoir;
            if (!(uint8)bit_reservoir)
            {
                v8 = (uint8)*input_cursor++;
                bit_reservoir = 2 * v8 + v24;
                v24 = (bit_reservoir >> 8) & 1;
            }
            if (v24)
            {
                v26 = 2 * bit_reservoir;
                v25 = (v26 >> 8) & 1;
                v26 = (uint8)v26;
                if (!(uint8)v26)
                {
                    v9 = (uint8)*input_cursor++;
                    v26 = 2 * v9 + v25;
                    v25 = (v26 >> 8) & 1;
                }
                v27 = v25;
                bit_reservoir = 2 * v26;
                v28 = (bit_reservoir >> 8) & 1;
                bit_reservoir = (uint8)bit_reservoir;
                if (!(uint8)bit_reservoir)
                {
                    v10 = (uint8)*input_cursor++;
                    bit_reservoir = 2 * v10 + v28;
                    v28 = (bit_reservoir >> 8) & 1;
                }
                if (v28)
                {
                    v51 = 2 * bit_reservoir;
                    v50 = (v51 >> 8) & 1;
                    v51 = (uint8)v51;
                    if (!(uint8)v51)
                    {
                        v56 = (uint8)*input_cursor++;
                        v51 = 2 * v56 + v50;
                        v50 = (v51 >> 8) & 1;
                    }
                    v27 = (2 * v27 + v50) | 4;
                    bit_reservoir = 2 * v51;
                    v52 = (bit_reservoir >> 8) & 1;
                    bit_reservoir = (uint8)bit_reservoir;
                    if (!(uint8)bit_reservoir)
                    {
                        v57 = (uint8)*input_cursor++;
                        bit_reservoir = 2 * v57 + v52;
                        v52 = (bit_reservoir >> 8) & 1;
                    }
                    if (v52)
                        goto LABEL_30;
                }
                else
                {
                    if (v27)
                    {
                    LABEL_30:
                        offset_prefix = (v27 >> 8) | (v27 << 8) & 0xFF00;
                        goto LABEL_31;
                    }
                    v27 = 1;
                }
                bit_reservoir *= 2;
                v29 = (bit_reservoir >> 8) & 1;
                bit_reservoir = (uint8)bit_reservoir;
                if (!(uint8)bit_reservoir)
                {
                    v11 = (uint8)*input_cursor++;
                    bit_reservoir = 2 * v11 + v29;
                    v29 = (bit_reservoir >> 8) & 1;
                }
                v27 = 2 * v27 + v29;
                goto LABEL_30;
            }
        LABEL_31:
            v30 = offset_prefix & 0xFF00;
            v31 = (uint8)*input_cursor++;
            v32 = v31 | v30;
            match_cursor = &destination[-v32 - 1];
            v34 = copy_length & 1;
            v35 = copy_length >> 1;
            if (v34)
            {
                v36 = *match_cursor;
                match_cursor = &destination[-v32];
                *destination++ = v36;
            }
            v37 = v35 - 1;
            if (v32)
            {
                do
                {
                    v39 = *match_cursor;
                    destination[1] = match_cursor[1];
                    *destination = v39;
                    match_cursor += 2;
                    --v37;
                    destination += 2;
                } while (v37 >= 0);
            }
            else
            {
                v38 = *match_cursor;
                do
                {
                    *destination = v38;
                    --v37;
                    destination[1] = v38;
                    destination += 2;
                } while (v37 >= 0);
            }
        }
        bit_reservoir *= 2;
        v58 = (bit_reservoir >> 8) & 1;
        bit_reservoir = (uint8)bit_reservoir;
        if (!(uint8)bit_reservoir)
        {
            v59 = (uint8)*input_cursor++;
            bit_reservoir = 2 * v59 + v58;
            v58 = (bit_reservoir >> 8) & 1;
        }
    } while (v58);
    v60 = (uint16)D16(0x8004401Eu);
    v61 = (int *)psx_addr(0x80044020u, 4u);
    if (D16(0x8004401Eu))
    {
        do
        {
            v62 = *(uint16 *)v61;
            v61 = (int *)((uint8 *)v61 + 2);
            *destination++ = (uint8)v62;
            --v60;
        } while (v60);
    }
}

uint32 sub_8003C4B0(const sint32 *target, uint32 angle)
{
    uint32 x = (uint32)(target[0] >> 8) - D32(0x80081B98u);
    uint32 y = (uint32)(target[1] >> 8) - D32(0x80081B9Cu);
    uint32 z = (uint32)(target[2] >> 8) - D32(0x80081BA0u);
    sint32 length, nx, ny, nz, horizontal, sine, cosine;
    sint16 rotation[16], reflection[16];
    uint32 result;
    while ((sint32)((sint32)x < 0 ? 0u - x : x) >= 16385 || (sint32)((sint32)y < 0 ? 0u - y : y) >= 16385 || (sint32)((sint32)z < 0 ? 0u - z : z) >= 16385)
    {
        x = (uint32)((sint32)x / 2);
        y = (uint32)((sint32)y / 2);
        z = (uint32)((sint32)z / 2);
    }
    length = (sint32)sub_8003B674(x * x + y * y + z * z);
    result = x << 12;
    if (length <= 0)
        return result;
    nx = (sint32)(result + (uint32)(length / 2)) / length;
    nz = (sint32)((z << 12) + (uint32)(length / 2)) / length;
    ny = (sint32)((y << 12) + (uint32)(length / 2)) / length;
    horizontal = (sint32)sub_8003B674((uint32)nx * (uint32)nx + (uint32)nz * (uint32)nz);
    if (horizontal <= 0)
    {
        D16(0x80081B84u) = 4096;
        D16(0x80081B86u) = 0;
        D16(0x80081B88u) = 0;
        D16(0x80081B8Au) = 0;
        D16(0x80081B8Cu) = 0;
        D16(0x80081B8Eu) = (uint16)(0u - (uint32)ny);
        D16(0x80081B90u) = 0;
        D16(0x80081B92u) = (uint16)ny;
        D16(0x80081B94u) = 0;
    }
    else
    {
        D16(0x80081B86u) = 0;
        D16(0x80081B8Cu) = (uint16)horizontal;
        D16(0x80081B90u) = (uint16)nx;
        D16(0x80081B92u) = (uint16)ny;
        D16(0x80081B94u) = (uint16)nz;
        D16(0x80081C26u) = 0;
        D16(0x80081C28u) = 0;
        D16(0x80081C2Au) = 0;
        D16(0x80081C2Cu) = (uint16)horizontal;
        D16(0x80081C30u) = 0;
        D16(0x80081C32u) = (uint16)ny;
        D16(0x80081C34u) = (uint16)horizontal;
        D16(0x80081C24u) = 4096;
        D16(0x80081C2Eu) = (uint16)(0u - (uint32)ny);
        D16(0x80081B84u) = (uint16)((sint32)(((uint32)nz << 12) + (uint32)(horizontal / 2)) / horizontal);
        D16(0x80081B88u) = (uint16)((sint32)(0u - (((uint32)nx << 12) + (uint32)(horizontal / 2))) / horizontal);
        D16(0x80081B8Au) = (uint16)((sint32)(0u - ((uint32)nx * (uint32)ny + (uint32)(horizontal / 2))) / horizontal);
        D16(0x80081B8Eu) = (uint16)((sint32)(0u - ((uint32)ny * (uint32)nz + (uint32)(horizontal / 2))) / horizontal);
    }
    sine = draft_agent3_missing_800437EC(angle);
    cosine = draft_agent3_missing_8004388C(angle);
    rotation[0] = (sint16)sine;
    rotation[4] = (sint16)sine;
    rotation[1] = (sint16)(0u - (uint32)cosine);
    rotation[2] = 0;
    rotation[3] = (sint16)cosine;
    rotation[5] = 0;
    rotation[6] = 0;
    rotation[7] = 0;
    rotation[8] = 4096;
    draft_agent3_missing_80049B74((const MATRIX *)rotation, (MATRIX *)psx_addr(0x80081B84u, sizeof(MATRIX)));
    reflection[0] = -4096;
    reflection[1] = 0;
    reflection[2] = 0;
    reflection[3] = 0;
    reflection[4] = 4096;
    reflection[5] = 0;
    reflection[6] = 0;
    reflection[7] = 0;
    reflection[8] = -4096;
    draft_agent3_missing_80049B74((const MATRIX *)rotation, (MATRIX *)psx_addr(0x80081C24u, sizeof(MATRIX)));
    draft_agent3_missing_80049A64((const MATRIX *)psx_addr(0x80081C24u, sizeof(MATRIX)), (const MATRIX *)reflection, (MATRIX *)psx_addr(0x80081C24u, sizeof(MATRIX)));
    draft_agent3_missing_80049A64((const MATRIX *)rotation, (const MATRIX *)reflection, (MATRIX *)psx_addr(0x80081C04u, sizeof(MATRIX)));
    /* TODO: Matrix helpers consume only initialized matrix entries in the original */
    return sub_8003B898();
}

/* TODO: GTE adapters must preserve fixed-point arithmetic, FIFOs and flags */
typedef struct DraftVertex
{
    sint16 x, y, z, padding;
} DraftVertex;

extern void draft_agent3_missing_project_three(const DraftVertex *vertices, uint32 *screen, uint32 *flags);
extern uint32 draft_agent3_missing_project_one(const DraftVertex *vertex);
extern sint32 draft_agent3_missing_normal_clip(void);
extern sint32 draft_agent3_missing_average_depth_four(void);
extern void draft_agent3_missing_rotate_vertex(const DraftVertex *vertex, sint32 *transformed);
extern void draft_agent3_missing_set_translation(const sint32 *translation);
extern uint32 draft_agent3_missing_light_color(uint32 packed_color, const DraftVertex *normal);

static void draft_link_packet(uint32 packet, uint32 ordering_entry)
{
    D32(packet) = (D32(packet) & 0xFF000000u) | (D32(ordering_entry) & 0xFFFFFFu);
    D32(ordering_entry) = (D32(ordering_entry) & 0xFF000000u) | (packet & 0xFFFFFFu);
}

uint32 sub_800187D0(uint32 object_index, uint32 face_index)
{
    uint32 vertex = 0x800708D4u + object_index * 144u;
    uint32 packet = 0x8006A8D4u + D32(0x80065128u) * 12288u + object_index * 768u + face_index * 192u;
    uint32 ordering_table = D32(D32(0x80065118u) + 0x70u) + 128u;
    sint32 depth_limit = (sint32)D32(0x800654F0u);
    uint32 face;
    for (face = 0; face < 8; ++face)
    {
        uint32 screen[3], flags, fourth;
        sint32 first_area, second_area, depth;
        draft_agent3_missing_project_three((const DraftVertex *)psx_addr(vertex, 24u), screen, &flags);
        first_area = draft_agent3_missing_normal_clip();
        D32(packet + 8u) = screen[0];
        D32(packet + 12u) = screen[1];
        D32(packet + 16u) = screen[2];
        fourth = draft_agent3_missing_project_one((const DraftVertex *)psx_addr(vertex + 24u, 8u));
        second_area = draft_agent3_missing_normal_clip();
        depth = draft_agent3_missing_average_depth_four();
        if (first_area <= 0 || second_area >= 0)
        {
            if (depth < depth_limit && depth > 0)
            {
                D32(packet + 20u) = fourth;
                draft_link_packet(packet, ordering_table + ((uint32)depth << 2));
            }
        }
        packet += 24u;
        vertex += 16u;
    }
    return 0;
}

static void draft_quad_texture(uint32 packet, uint32 texture)
{
    D8(packet + 12u) = D8(texture + 4u);
    D8(packet + 13u) = D8(texture + 5u);
    D8(packet + 20u) = (uint8)(D8(texture + 4u) + 31u);
    D8(packet + 21u) = D8(texture + 5u);
    D8(packet + 28u) = D8(texture + 4u);
    D8(packet + 29u) = (uint8)(D8(texture + 5u) + 31u);
    D8(packet + 36u) = (uint8)(D8(texture + 4u) + 31u);
    D8(packet + 37u) = (uint8)(D8(texture + 5u) + 31u);
    D16(packet + 22u) = D16(texture);
    D16(packet + 14u) = D16(texture + 2u);
}

uint32 sub_8001896C(uint32 object_index)
{
    uint32 object = 0x80066D14u + object_index * 952u, side;
    uint32 model = D32(object);
    sint32 radius = (sint16)D16(model + 130u), width = (sint16)D16(model + 132u);
    uint32 angle_a = (0u - (uint32)(sint32)(sint16)D16(object + 70u)) << 2;
    uint32 angle_b = (0u - (uint32)(sint32)(sint16)D16(object + 68u)) << 2;
    sint32 extent_x[2], extent_y[2], swing[2], translations[4][3];
    sint16 yaw[16], temporary[16];
    uint32 i;
    swing[0] = draft_agent3_missing_800437EC(angle_a);
    extent_x[0] = draft_product((uint32)swing[0], (uint32)radius) / 4096;
    extent_y[0] = draft_product((uint32)draft_agent3_missing_8004388C(angle_a), (uint32)radius) / 4096;
    swing[1] = draft_agent3_missing_800437EC(angle_b);
    extent_x[1] = draft_product((uint32)swing[1], (uint32)radius) / 4096;
    extent_y[1] = draft_product((uint32)draft_agent3_missing_8004388C(angle_b), (uint32)radius) / 4096;
    for (i = 0; i < 4; ++i)
        draft_agent3_missing_rotate_vertex((const DraftVertex *)psx_addr(object + 624u + i * 8u, 8u), translations[i]);
    for (side = 0; side < 4; ++side)
    {
        uint32 axis = side / 2u, packet, texture;
        sint32 fade = 255 - D8(object + side + 379u), sine, cosine, speed, texture_index;
        const MATRIX *matrix;
        DraftVertex vertices[4];
        uint32 screen[3], flags;
        sint32 area, depth, bucket;
        if (side == 2u)
        {
            uint32 angle = (uint32)((sint32)D32(object + 8u) / 256);
            sine = draft_agent3_missing_800437EC(angle);
            cosine = draft_agent3_missing_8004388C(angle);
            yaw[0] = (sint16)sine;
            yaw[1] = 0;
            yaw[2] = (sint16)cosine;
            yaw[3] = 0;
            yaw[4] = 4096;
            yaw[5] = 0;
            yaw[6] = (sint16)(0u - (uint32)cosine);
            yaw[7] = 0;
            yaw[8] = (sint16)sine;
        }
        if (fade <= 0)
        {
            if (side < 2u)
                matrix = (const MATRIX *)psx_addr(0x80081C64u, sizeof(MATRIX));
            else
            {
                draft_agent3_missing_80049A64((const MATRIX *)psx_addr(0x80081C64u, sizeof(MATRIX)), (const MATRIX *)yaw, (MATRIX *)temporary);
                matrix = (const MATRIX *)temporary;
            }
        }
        else
        {
            uint32 angle = (uint32)(draft_product((uint32)fade, (uint32)swing[axis]) / 2048);
            sine = draft_agent3_missing_800437EC(angle);
            cosine = draft_agent3_missing_8004388C(angle);
            temporary[0] = (sint16)sine;
            temporary[1] = (sint16)cosine;
            temporary[2] = 0;
            temporary[3] = (sint16)(0u - (uint32)cosine);
            temporary[4] = (sint16)sine;
            temporary[5] = 0;
            temporary[6] = 0;
            temporary[7] = 0;
            temporary[8] = 4096;
            draft_agent3_missing_80049B74((const MATRIX *)psx_addr(0x80081C64u, sizeof(MATRIX)), (MATRIX *)temporary);
            if (side >= 2u)
                draft_agent3_missing_80049A64((const MATRIX *)temporary, (const MATRIX *)yaw, (MATRIX *)temporary);
            matrix = (const MATRIX *)temporary;
        }
        draft_agent3_missing_80049C84(matrix);
        draft_agent3_missing_set_translation(translations[(side + 2u) & 3u]);
        sub_800187D0(object_index, side);
        vertices[0].x = vertices[1].x = vertices[2].x = vertices[3].x = (sint16)((side & 1u) ? width : -width);
        vertices[0].y = (sint16)extent_y[axis];
        vertices[0].z = (sint16)extent_x[axis];
        if (side & 1u)
        {
            vertices[1].y = (sint16)(0u - (uint32)extent_x[axis]);
            vertices[1].z = (sint16)extent_y[axis];
            vertices[2].y = (sint16)extent_x[axis];
            vertices[2].z = (sint16)(0u - (uint32)extent_y[axis]);
        }
        else
        {
            vertices[1].y = (sint16)extent_x[axis];
            vertices[1].z = (sint16)(0u - (uint32)extent_y[axis]);
            vertices[2].y = (sint16)(0u - (uint32)extent_x[axis]);
            vertices[2].z = (sint16)extent_y[axis];
        }
        vertices[3].y = (sint16)(0u - (uint32)extent_y[axis]);
        vertices[3].z = (sint16)(0u - (uint32)extent_x[axis]);
        speed = (sint32)D32(object + (axis ? 892u : 888u)) >> 8;
        if (speed < 0)
            speed = (sint32)(0u - (uint32)speed);
        texture_index = speed / 64;
        texture = texture_index < 4 ? 0x800739D4u + (uint32)texture_index * 6u : 0x800739E6u;
        packet = 0x800711D4u + D32(0x80065128u) * 5120u + object_index * 320u + side * 80u;
        draft_quad_texture(packet, texture);
        draft_agent3_missing_project_three(vertices, screen, &flags);
        D32(packet + 8u) = screen[0];
        D32(packet + 16u) = screen[1];
        D32(packet + 24u) = screen[2];
        if (!(flags & 0x1C66000u))
        {
            area = draft_agent3_missing_normal_clip();
            D32(packet + 32u) = draft_agent3_missing_project_one(&vertices[3]);
            depth = draft_agent3_missing_average_depth_four();
            if (area >= 0)
            {
                bucket = (sint32)(D32(0x800654F0u) - 1u);
                if ((sint32)((uint32)depth + 8u) < bucket)
                    bucket = (sint32)((uint32)depth + 8u);
                draft_link_packet(packet, D32(D32(0x80065118u) + 112u) + ((uint32)bucket << 2));
                D32(packet + 4u) = 0x2C000000u;
            }
            else
            {
                bucket = (sint32)((uint32)depth - 8u);
                if (bucket < 0)
                    bucket = 0;
                draft_link_packet(packet, D32(D32(0x80065118u) + 112u) + ((uint32)bucket << 2));
                D32(packet + 4u) = draft_agent3_missing_light_color(D32(0x80064C64u), (const DraftVertex *)psx_addr(0x80059A38u + side * 8u, 8u));
            }
        }
        for (i = 0; i < 4; ++i)
            vertices[i].x = (sint16)(0u - (uint32)(uint16)vertices[i].x);
        packet += 40u;
        draft_quad_texture(packet, texture);
        draft_agent3_missing_project_three(vertices, screen, &flags);
        D32(packet + 8u) = screen[0];
        D32(packet + 16u) = screen[1];
        D32(packet + 24u) = screen[2];
        if (!(flags & 0x1C66000u))
        {
            area = draft_agent3_missing_normal_clip();
            D32(packet + 32u) = draft_agent3_missing_project_one(&vertices[3]);
            depth = draft_agent3_missing_average_depth_four();
            if (area > 0)
            {
                bucket = (sint32)(D32(0x800654F0u) - 1u);
                if ((sint32)((uint32)depth + 8u) < bucket)
                    bucket = (sint32)((uint32)depth + 8u);
                draft_link_packet(packet, D32(D32(0x80065118u) + 112u) + ((uint32)bucket << 2));
            }
        }
    }
    return 0;
}
