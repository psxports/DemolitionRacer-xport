#include "draft_signatures.h"

/* Complete GAME overlay MIPS; no complete pseudocode export is available */
static void pause_link_packet(uint32 packet)
{
    uint32 table = r_u32(r_u32(0x80065118u) + 112u);
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(table + 8u) & 0xFFFFFFu));
    w_u32(table + 8u, (r_u32(table + 8u) & 0xFF000000u) | (packet & 0xFFFFFFu));
}

static void pause_quad_position(uint32 packet, uint32 x, uint32 y, uint32 width, uint32 height)
{
    w_u16(packet + 8u, x);
    w_u16(packet + 10u, y);
    w_u16(packet + 16u, x + width + 65535u);
    w_u16(packet + 18u, y);
    w_u16(packet + 24u, x);
    w_u16(packet + 26u, y + height + 65535u);
    w_u16(packet + 32u, x + width + 65535u);
    w_u16(packet + 34u, y + height + 65535u);
}

static void pause_volume_bar(sint32 x, sint32 y, uint32 volume, uint32 backgrounds, uint32 fills)
{
    uint32 index;
    for (index = 0u; index < 8u; ++index)
    {
        uint32 slot = index * 80u + r_u32(0x80065128u) * 40u;
        uint32 fill = fills + slot, background = backgrounds + slot;
        sint32 amount = r_s32(volume) / 512;
        uint32 texture, fill_texture, segment_x;
        w_u8(fill + 4u, (sint32)index < amount ? 128u : 16u);
        w_u8(fill + 5u, (sint32)index < amount ? 208u : 96u);
        w_u8(fill + 6u, (sint32)index < amount ? 128u : 16u);
        texture = r_u32(0x800C7EDCu);
        fill_texture = r_u32(0x800C7ED8u);
        segment_x = (uint32)x + index * r_u8(texture + 22u);
        pause_quad_position(fill, segment_x + 2u, (uint32)y + 3u, r_u8(fill_texture + 22u), r_u8(fill_texture + 23u));
        pause_quad_position(background, segment_x, (uint32)y, r_u8(texture + 22u), r_u8(texture + 23u));
        pause_link_packet(fill);
        pause_link_packet(background);
    }
}

void game_800B96E0(sint32 x, sint32 y)
{
    FUNCTION_MARKER(0x800B96E0u, "GAME.BIN");
    pause_volume_bar(x, y, 0x80064BFCu, 0x800C7F30u, 0x800C81B0u);
}

void game_800B9A8C(sint32 x, sint32 y)
{
    FUNCTION_MARKER(0x800B9A8Cu, "GAME.BIN");
    pause_volume_bar(x, y, 0x80064BF8u, 0x800C8430u, 0x800C86B0u);
}

static sint32 pause_half(uint32 value)
{
    return (sint32)(value + (value >> 31)) >> 1;
}

static void pause_center_text(uint32 messages, sint32 y, sint32 highlighted)
{
    uint32 text = r_u32(messages + r_u32(0x80064B8Cu) * 4u);
    uint32 width = sub_80020A14(5u, text);
    sub_80021430(0u);
    if (highlighted)
        sub_80021430(3u);
    sub_80020AB8(5u, pause_half(320u - width), y, (const char *)psx_addr(text, 1u));
}

void game_800B9E38(uint32 port)
{
    uint32 buttons_address = 0x80065524u + port * 2u;
    FUNCTION_MARKER(0x800B9E38u, "GAME.BIN");
    if (!r_u32(0x800BFAECu))
    {
        uint32 texture = r_u32(0x800C7ED4u);
        sint32 top = pause_half(240u - r_u8(texture + 23u));
        uint32 text, width;
        sint32 value;
        sub_80021430(2u);
        text = r_u32(0x800BFAF4u + r_u32(0x80064B8Cu) * 4u);
        width = sub_80020A14(5u, text);
        sub_80020AB8(5u, pause_half(320u - width), top + 6, (const char *)psx_addr(text, 1u));
        if (r_u8(0x800BFAE4u) || r_u8(0x800BFAE5u))
        {
            sub_80021430(3u);
            text = r_u32(0x800BFBA8u + r_u32(0x80064B8Cu) * 4u);
            width = sub_80020A14(5u, text);
            sub_80020AB8(5u, pause_half(320u - width), top + 54, (const char *)psx_addr(text, 1u));
        }
        else
        {
            pause_center_text(0x800BFB08u, top + 18, r_u32(0x800BFAE8u) == 0u);
            pause_center_text(0x800BFB1Cu, top + 30, r_u32(0x800BFAE8u) == 1u);
            texture = r_u32(0x800C7ED4u);
            game_800B96E0(pause_half(320u - r_u8(texture + 22u)) + 16, top + 44);
            pause_center_text(0x800BFB30u, top + 52, r_u32(0x800BFAE8u) == 2u);
            texture = r_u32(0x800C7ED4u);
            game_800B9A8C(pause_half(320u - r_u8(texture + 22u)) + 16, top + 66);
            pause_center_text(0x800BFB44u, top + 74, r_u32(0x800BFAE8u) == 3u);
            pause_center_text(0x800BFB58u, top + 86, r_u32(0x800BFAE8u) == 4u);
            if ((r_u16(buttons_address) & 0x10u) && r_s32(0x800BFAE8u) > 0)
                w_u32(0x800BFAE8u, r_u32(0x800BFAE8u) - 1u);
            if ((r_u16(buttons_address) & 0x40u) && r_s32(0x800BFAE8u) < 4)
                w_u32(0x800BFAE8u, r_u32(0x800BFAE8u) + 1u);
            if ((r_u16(buttons_address) & 0x80u) && r_u32(0x800BFAE8u) == 1u && r_s32(0x80064BFCu) > 0)
                w_u32(0x80064BFCu, r_u32(0x80064BFCu) - 512u);
            if ((r_u16(buttons_address) & 0x20u) && r_u32(0x800BFAE8u) == 1u && r_s32(0x80064BFCu) < 4096)
                w_u32(0x80064BFCu, r_u32(0x80064BFCu) + 512u);
            if ((r_u16(buttons_address) & 0x80u) && r_u32(0x800BFAE8u) == 2u && r_s32(0x80064BF8u) > 0)
                w_u32(0x80064BF8u, r_u32(0x80064BF8u) - 512u);
            if ((r_u16(buttons_address) & 0x20u) && r_u32(0x800BFAE8u) == 2u && r_s32(0x80064BF8u) < 4096)
                w_u32(0x80064BF8u, r_u32(0x80064BF8u) + 512u);
            value = r_s32(0x80064BF8u);
            w_u32(0x80064BF8u, value < 0 ? 0u : value > 4096 ? 4096u : (uint32)value);
            value = r_s32(0x80064BFCu);
            w_u32(0x80064BFCu, value < 0 ? 0u : value > 4096 ? 4096u : (uint32)value);
            sub_8001FF98();
        }
    }
    else
    {
        uint32 text, width;
        pause_center_text(0x800BFB6Cu, 110, 0);
        text = r_u32(0x800BFB80u + r_u32(0x80064B8Cu) * 4u);
        width = sub_80020A14(5u, text);
        sub_80021430(0u);
        if (!r_u32(0x800BFAE8u))
            sub_80021430(3u);
        sub_80020AB8(5u, (sint32)(158u - width), 122, (const char *)psx_addr(text, 1u));
        text = r_u32(0x800BFB94u + r_u32(0x80064B8Cu) * 4u);
        sub_80021430(0u);
        if (r_u32(0x800BFAE8u) == 1u)
            sub_80021430(3u);
        sub_80020AB8(5u, 162, 122, (const char *)psx_addr(text, 1u));
        if ((r_u16(buttons_address) & 0x80u) && r_u32(0x800BFAE8u) == 1u)
            w_u32(0x800BFAE8u, 0u);
        if ((r_u16(buttons_address) & 0x20u) && !r_u32(0x800BFAE8u))
            w_u32(0x800BFAE8u, 1u);
    }
    pause_link_packet(0x800C7EE0u + r_u32(0x80065128u) * 40u);
}
