#include "draft_signatures.h"

extern void pres_80090730(uint32 model, uint32 mode);
extern void pres_8009CD44(void);
extern void pres_800A0240(sint32 flags, uint32 object);

static void extra_link(uint32 packet, uint32 head)
{
    w_u32(packet, (r_u32(packet) & 0xff000000u) | (r_u32(head) & 0xffffffu));
    w_u32(head, (r_u32(head) & 0xff000000u) | (packet & 0xffffffu));
}

static void extra_texture_packets(uint32 texture)
{
    uint32 i, u = r_u8(texture + 24u), v = r_u8(texture + 25u);
    uint32 width = r_u8(texture + 22u), height = r_u8(texture + 23u);
    for (i = 0; i < 2u; ++i) {
        uint32 p = 0x800789D8u + 40u * i;
        w_u8(p + 3u, 9u); w_u8(p + 7u, 0x2du);
        w_u8(p + 12u, u); w_u8(p + 13u, v);
        w_u8(p + 20u, u + width - 1u); w_u8(p + 21u, v);
        w_u8(p + 28u, u); w_u8(p + 29u, v + height - 1u);
        w_u8(p + 36u, u + width - 1u); w_u8(p + 37u, v + height - 1u);
        w_u16(p + 8u, 256u); w_u16(p + 10u, 196u - height);
        w_u16(p + 16u, width + 255u); w_u16(p + 18u, 196u - height);
        w_u16(p + 24u, 256u); w_u16(p + 26u, 195u);
        w_u16(p + 32u, width + 255u); w_u16(p + 34u, 195u);
        w_u16(p + 22u, r_u16(texture + 8u));
        w_u16(p + 14u, r_u16(texture + 10u));
    }
}

static void extra_load_model(void)
{
    uint32 model = r_u32(0x80059744u), texture;
    pres_80090730(model, 0u);
    sub_80019D80(0u, 0x800594CCu + model * 32u, 0x800594D0u + model * 32u);
    texture = sub_80039ADC(r_u32(0x8005A9D8u + model * 4u));
    w_u32(0x8006532Cu, texture);
    extra_texture_packets(texture);
}

void pres_80099268(void)
{
    static const uint32 flags[6] = {0x800596F9u, 0x800596FAu, 0x800596F5u,
        0x800596F8u, 0x800596F7u, 0x800596F6u};
    uint32 i, first = r_u32(0x800AD6B8u);
    for (i = 0; i < 6u; ++i)
        w_u16(0x800AD896u + (first + i) * 68u, r_u8(flags[i]) == 1u);
}

void pres_80099508(uint32 object)
{
    uint32 buttons = r_u16(0x80065524u), selected;
    static const uint32 modes[6] = {5u, 6u, 1u, 2u, 3u, 4u};
    (void)object;
    if (buttons & 0x1000u) {
        sub_8001D01C(3u); w_u32(0x800ABB30u, 0u); w_u32(0x800ABB20u, 3u);
        return;
    }
    if (!(buttons & 0x4008u)) return;
    selected = r_u32(0x800AD668u);
    if ((sint16)r_u16(0x800AD896u + (r_u32(0x800AD6B8u) + selected) * 68u)) return;
    w_u32(0x800ABB20u, 1u);
    if (selected < 6u) w_u32(0x80059738u, modes[selected]);
    w_u32(0x800ABB30u, 3u); sub_8001D01C(1u);
}

void pres_80099610(void)
{
    uint32 i;
    sub_8003BF0C(255u, 80u, 0u);
    sub_8003BF48(144u); sub_8003BF7C(65536u);
    extra_load_model();
    for (i = 0; i < 2u; ++i) {
        uint32 p = 0x80078A98u + i * 24u;
        w_u8(p + 3u, 5u); w_u8(p + 7u, 0x2au);
        w_u16(p + 8u, 134u); w_u16(p + 10u, 146u);
        w_u16(p + 12u, 238u); w_u16(p + 14u, 146u);
        w_u16(p + 16u, 134u); w_u16(p + 18u, 196u);
        w_u16(p + 20u, 238u); w_u16(p + 22u, 196u);
    }
    for (i = 0; i < 8u; ++i) {
        uint32 p = 0x80078B38u + i * 36u;
        w_u8(p + 3u, 8u); w_u8(p + 7u, 0x39u);
    }
}

void pres_800998E4(uint32 object)
{
    uint32 buttons, model, first, buffer, head, i;
    (void)object;
    w_u16(0x80066D58u, r_u16(0x80066D58u) + 64u);
    w_u16(0x80066D5Au, r_u16(0x80066D5Au) + 64u);
    buttons = r_u16(0x80065524u); model = r_u32(0x80059744u);
    if (buttons & 0x1000u) {
        sub_8001D01C(3u); w_u32(0x800ABB20u, 3u);
        sub_8004261C(r_u32(0x800ABB10u)); w_u32(0x800ABB30u, 3u);
    } else if ((buttons & 0x8000u) && r_u8(0x800596ECu + model) != 1u) {
        sub_8001D01C(2u); w_u32(0x800ABB20u, 2u);
        sub_8004261C(r_u32(0x800ABB10u)); pres_8009CD44();
        w_u32(0x800ABB48u, 1u); w_u32(0x800ABB30u, 9u);
    } else if ((buttons & 0x4008u) && r_u8(0x800596ECu + model) != 1u) {
        sub_8001D01C(0u); w_u32(0x800ABB20u, 1u);
        sub_8004261C(r_u32(0x800ABB10u)); pres_8009CD44();
        w_u32(0x800ABB48u, 1u); w_u32(0x800ABB30u, 9u);
    } else if (r_u32(0x800AD880u + (r_u32(0x800AD6B8u) + 2u) * 68u) == 4096u) {
        if ((buttons & 0x80u) && (sint32)model > 0) {
            sub_8001D01C(2u); w_u32(0x80059744u, r_u32(0x80059744u) - 1u);
            sub_8004261C(r_u32(0x800ABB10u)); extra_load_model();
        } else if ((buttons & 0x20u) && (sint32)model < (sint32)(r_u32(0x800A1F28u) - 1u)) {
            sub_8001D01C(2u); w_u32(0x80059744u, r_u32(0x80059744u) + 1u);
            sub_8004261C(r_u32(0x800ABB10u)); extra_load_model();
        }
    }
    model = r_u32(0x80059744u); first = r_u32(0x800AD6B8u);
    pres_800A0240(model == 0u ? 16 : model == r_u32(0x800A1F28u) - 1u ? 8 : 512,
        0x800AD860u + first * 68u);
    pres_800A0240(model == 0u ? 8 : model == r_u32(0x800A1F28u) - 1u ? 16 : 512,
        0x800AD860u + (first + 1u) * 68u);
    w_u32(0x800A40D4u, r_u32(0x800A1EB8u + model * 4u));
    pres_800A0240(r_u8(0x800596ECu + model) == 0u ? 16 : 8,
        0x800AD860u + r_u32(0x800AD7A8u) * 68u);
    buffer = r_u32(0x80065128u); head = r_u32(r_u32(0x80065118u) + 112u);
    extra_link(0x800789D8u + 40u * buffer, head + 1024u);
    for (i = 0; i < 4u; ++i) {
        uint32 p = 0x80078B38u + 36u * buffer + 72u * i;
        uint32 value = r_u8(0x8005A9F8u + 4u * model + i), y = 149u + 12u * i;
        w_u16(p + 8u, 136u); w_u16(p + 10u, y);
        w_u16(p + 16u, 146u + 10u * value); w_u16(p + 18u, y);
        w_u16(p + 24u, 136u); w_u16(p + 26u, y + 8u);
        w_u16(p + 32u, 146u + 10u * value); w_u16(p + 34u, y + 8u);
        w_u8(p + 4u, 240u); w_u8(p + 5u, 96u); w_u8(p + 6u, 16u);
        w_u8(p + 20u, 240u); w_u8(p + 21u, 96u); w_u8(p + 22u, 16u);
        w_u8(p + 12u, 240u - 16u * value); w_u8(p + 13u, 96u + 16u * value); w_u8(p + 14u, 16u);
        w_u8(p + 28u, 240u - 16u * value); w_u8(p + 29u, 96u + 16u * value); w_u8(p + 30u, 16u);
        extra_link(p, head);
    }
    extra_link(0x80078A98u + 24u * buffer, head);
}
