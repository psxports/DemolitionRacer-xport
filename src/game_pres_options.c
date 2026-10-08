#include "draft_signatures.h"

extern void pres_80093008(uint32 font, sint32 x, sint32 y, uint32 guest_text);

static uint32 pres_option_pointer(uint32 index)
{
    return r_u32(0x800AD860u + index * 68u);
}

static void pres_option_screen_positions(void)
{
    uint32 index = r_u32(0x800AD6B8u);
    uint32 x = 0u - r_u32(0x80064BF0u), y = 0u - r_u32(0x80064BF4u);
    uint32 horizontal = ((x + 255u) & 0xffffu) | 0x20000u;
    uint32 vertical = ((y + 120u) & 0xffffu) | 0x20000u;
    w_u32(pres_option_pointer(index) + 16u, (x + 304u) & 0xffffu);
    w_u32(pres_option_pointer(index) + 20u, vertical);
    w_u32(pres_option_pointer(index + 1u) + 16u, ((x + 207u) & 0xffffu) | 0x10000u);
    w_u32(pres_option_pointer(index + 1u) + 20u, vertical);
    w_u32(pres_option_pointer(index + 2u) + 16u, horizontal);
    w_u32(pres_option_pointer(index + 2u) + 20u, ((y + 88u) & 0xffffu) | 0x10000u);
    w_u32(pres_option_pointer(index + 3u) + 16u, horizontal);
    w_u32(pres_option_pointer(index + 3u) + 20u, (y + 152u) & 0xffffu);
}

void pres_800964DC(void)
{
    pres_option_screen_positions();
}

void pres_80096608(uint32 object)
{
    (void)object;
    if (r_u16(0x80065524u) & 0x1000u) {
        sub_8001D01C(3u);
        w_u32(0x800ABB20u, 3u);
        w_u32(0x800ABB30u, 2u);
        return;
    }
    if (r_u32(0x800ABB38u)) return;
    if ((r_u16(0x80065520u) & 0x80u) && (sint32)r_u32(0x80064BF0u) > 0)
        w_u32(0x80064BF0u, r_u32(0x80064BF0u) - 1u);
    if ((r_u16(0x80065520u) & 0x20u) && (sint32)r_u32(0x80064BF0u) < 24)
        w_u32(0x80064BF0u, r_u32(0x80064BF0u) + 1u);
    if ((r_u16(0x80065520u) & 0x10u) && (sint32)r_u32(0x80064BF4u) > 0)
        w_u32(0x80064BF4u, r_u32(0x80064BF4u) - 1u);
    if ((r_u16(0x80065520u) & 0x40u) && (sint32)r_u32(0x80064BF4u) < 24)
        w_u32(0x80064BF4u, r_u32(0x80064BF4u) + 1u);
    pres_option_screen_positions();
}

static void pres_option_volume_bar(uint32 base, uint32 index, sint32 volume)
{
    uint32 offset = (base + index) * 68u;
    if ((sint32)index < volume) {
        w_u16(0x800AD894u + offset, (uint16)index);
        w_u16(0x800AD892u + offset, 1u);
    } else {
        w_u16(0x800AD894u + offset, 0u);
        w_u16(0x800AD892u + offset, 0u);
    }
    w_u8(0x800AD898u + offset, 0u);
    w_u8(0x800AD899u + offset, 16u);
    w_u8(0x800AD89Au + offset, 0u);
}

void pres_8009686C(void)
{
    uint32 index;
    w_u32(0x8005974Cu, (uint32)((sint32)r_u32(0x80064BFCu) / 256));
    w_u32(0x80059754u, (uint32)((sint32)r_u32(0x80064BF8u) / 256));
    for (index = 0u; index < 16u; ++index) {
        pres_option_volume_bar(r_u32(0x800AD6B8u), index, (sint32)r_u32(0x8005974Cu));
        pres_option_volume_bar(r_u32(0x800AD7A8u), index, (sint32)r_u32(0x80059754u));
    }
}

static void pres_option_volume_raise(uint32 count_address, uint32 group_address)
{
    sint32 count = (sint32)r_u32(count_address);
    if (count < 16) {
        uint32 index = r_u32(group_address) + (uint32)count;
        w_u16(0x800AD892u + index * 68u, 1u);
        w_u32(count_address, r_u32(count_address) + 1u);
    }
}

static void pres_option_volume_lower(uint32 count_address, uint32 group_address)
{
    sint32 count = (sint32)r_u32(count_address);
    if (count > 0) {
        uint32 index = r_u32(group_address) + (uint32)count - 1u;
        w_u16(0x800AD892u + index * 68u, 0xffffu);
        w_u32(count_address, r_u32(count_address) - 1u);
    }
}

void pres_80096AD4(uint32 object)
{
    uint32 track;
    if (r_u16(0x80065524u) & 0x1000u) {
        sub_8001D01C(3u);
        w_u32(0x800ABB20u, 3u);
        w_u32(0x800ABB30u, 2u);
    }
    if (r_u16(0x80065524u) & 0x20u) {
        sub_8001D01C(2u);
        if (object == 0x800A4A2Cu && (sint32)r_u32(0x8005974Cu) < 16)
            pres_option_volume_raise(0x8005974Cu, 0x800AD6B8u);
        else if (object == 0x800A4AB0u && (sint32)r_u32(0x80059754u) < 16)
            pres_option_volume_raise(0x80059754u, 0x800AD7A8u);
        else if (object == 0x800A4B34u && (sint32)r_u32(0x80059758u) < (sint32)(r_u32(0x80064D4Cu) - 1u))
            w_u32(0x80059758u, r_u32(0x80059758u) + 1u);
    }
    if (r_u16(0x80065524u) & 0x80u) {
        sub_8001D01C(2u);
        if (object == 0x800A4A2Cu && (sint32)r_u32(0x8005974Cu) > 0)
            pres_option_volume_lower(0x8005974Cu, 0x800AD6B8u);
        else if (object == 0x800A4AB0u && (sint32)r_u32(0x80059754u) > 0)
            pres_option_volume_lower(0x80059754u, 0x800AD7A8u);
        else if (object == 0x800A4B34u && (sint32)r_u32(0x80059758u) > 0)
            w_u32(0x80059758u, r_u32(0x80059758u) - 1u);
    }
    if (r_u16(0x80065524u) & 0x4000u) {
        sub_8001D01C(0u);
        if (object == 0x800A4B34u && r_u32(0x80059758u) != r_u32(0x800652CCu))
            sub_80020430(r_u32(0x80059758u));
    }
    track = r_u32(0x80059758u);
    pres_80093008(2u, 264, 133, r_u32(0x80059C08u + track * 20u));
    track = r_u32(0x80059758u);
    pres_80093008(2u, 264, 157, r_u32(0x80059C0Cu + track * 20u));
    w_u32(0x80064BF8u, r_u32(0x80059754u) << 8);
    sub_8001FF98();
    w_u32(0x80064BFCu, r_u32(0x8005974Cu) << 8);
}
