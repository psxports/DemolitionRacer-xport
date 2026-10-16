#include "draft_signatures.h"

extern void pres_800A0240(sint32 flags, uint32 object);

static void controls_availability(void)
{
    uint32 first = r_u32(0x800AD6B8u), disabled = r_u32(0x80064E5Cu) != 1u;
    w_u16(0x800AD896u + first * 68u, disabled);
    w_u16(0x800AD896u + (first + 1u) * 68u, disabled);
}

static void controls_indicators(sint32 flags)
{
    uint32 first = r_u32(0x800AD7A8u);
    pres_800A0240(flags, 0x800AD8A4u + first * 68u);
    pres_800A0240(flags, 0x800AD8E8u + r_u32(0x800AD7A8u) * 68u);
}

void pres_8009AE08(void)
{
    uint32 texture, i;
    w_u32(0x80064E60u, 0u);
    w_u32(0x80064E58u, 512u);
    texture = sub_80039ADC(0x800A97ECu);
    w_u32(0x800652F4u, texture);
    for (i = 0; i < 2u; ++i)
    {
        uint32 p = 0x80074590u + i * 40u;
        uint32 u = r_u8(texture + 24u), v = r_u8(texture + 25u);
        uint32 width = r_u8(texture + 22u), height = r_u8(texture + 23u);
        w_u8(p + 3u, 9u);
        w_u8(p + 7u, 0x2eu);
        w_u8(p + 12u, u);
        w_u8(p + 13u, v);
        w_u8(p + 20u, u + width - 1u);
        w_u8(p + 21u, v);
        w_u8(p + 28u, u);
        w_u8(p + 29u, v + height - 1u);
        w_u8(p + 36u, u + width - 1u);
        w_u8(p + 37u, v + height - 1u);
        w_u8(p + 4u, 128u);
        w_u8(p + 5u, 128u);
        w_u8(p + 6u, 128u);
        w_u16(p + 22u, r_u16(texture + 8u));
        w_u16(p + 14u, r_u16(texture + 10u));
    }
    w_u32(0x80064E68u, 0u);
    controls_availability();
    controls_indicators(16);
}

void pres_8009B094(uint32 object)
{
    uint32 buttons;
    sub_80021DB8();
    controls_availability();
    if (r_u32(0x80064E68u) == 5u)
    {
        w_u32(0x80064BECu, 1u);
        controls_indicators(512);
    }
    else
        w_u32(0x80064BECu, 0u);
    buttons = r_u16(0x80065524u);
    if ((buttons & 0x1000u) || !(buttons & 0x4008u) || r_u32(0x800ABB48u))
        return;
    sub_8001D01C(0u);
    if (r_u32(0x80064E5Cu) == 1u)
    {
        if (object == 0x800A958Cu)
            w_u32(0x80064E68u, 12u);
        if (object == 0x800A95ECu)
            w_u32(0x80064E68u, 6u);
    }
    if (object == 0x800A9644u)
        w_u32(0x800ABB30u, 2u);
}
