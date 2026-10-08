#include "draft_signatures.h"

uint32 sub_80017D38(void)
{
    uint32 count = r_u32(0x80065254u), index = 0u;
    if ((sint32)count <= 0) return count;
    do {
        w_u8(0x80066D69u + index * 952u, 0u);
        ++index; count = r_u32(0x80065254u);
    } while ((sint32)index < (sint32)count);
    return 0u;
}

uint32 sub_80019F54(void)
{
    uint32 previous = r_u32(0x80064CB0u);
    if (previous) w_u32(0x80064CB0u, previous - 1u);
    return previous - 1u;
}

uint32 sub_8001685C(uint32 state)
{
    uint32 record, value;
    if (r_u32(0x800593A0u) == 1u) return 1u;
    record = r_u32(0x800653D4u) + (uint32)(sint32)(sint16)r_u16(state) * 24u;
    value = r_u8(r_u32(0x800653A0u) + (uint32)r_u8(record + 1u) * 8u + r_u8(state + 8u));
    return value < 32u ? value : 1u;
}

static sint32 palette_random_remainder(sint32 divisor)
{
    return randPSX() % divisor;
}

static sint32 palette_power_four(sint32 value, sint32 divisor)
{
    uint32 result = (uint32)value * (uint32)value;
    result *= (uint32)value; result *= (uint32)value;
    return (sint32)result / divisor;
}

static void palette_pixel(sint32 x, sint32 y)
{
    PSX_RECT rectangle;
    rectangle.x = (sint16)x; rectangle.y = (sint16)y;
    rectangle.w = 1; rectangle.h = 1;
    LoadImagePSX(&rectangle, (uint32 *)psx_addr(0x800C8BA0u, 2u));
}

static void palette_high_pixels(sint32 x, sint32 y)
{
    uint32 i;
    for (i = 0; i < 4u; ++i)
        palette_pixel((sint32)r_u16(0x800C8BA4u + i * 8u) + x,
            (sint32)r_u16(0x800C8BA6u + i * 8u) + y);
}

static void palette_low_pixels(sint32 x, sint32 y)
{
    palette_pixel(x, y); palette_pixel(x + 32, y);
    palette_pixel(x, y + 128); palette_pixel(x + 32, y + 128);
}

static void palette_random_color(void)
{
    uint32 value = (uint32)(palette_random_remainder(16) + 192);
    w_u16(0x800C8BA0u, value + ((uint32)(sint32)(sint16)value << 8));
}

void game_800BCDD8(void)
{
    sint32 x, y;
    uint32 count = r_u32(0x800BFBE0u);
    if ((sint32)count > 1024) return;
    w_u32(0x800BFBE0u, count + 1u);
    palette_random_color();
    x = 47 - palette_power_four(palette_random_remainder(64), 1048576);
    y = palette_random_remainder(192); palette_high_pixels(x, y);
    x = palette_random_remainder(64);
    y = 255 - palette_power_four(palette_random_remainder(64), 262144);
    palette_high_pixels(x, y);
    x = 64 - palette_power_four(palette_random_remainder(64), 1048576);
    y = palette_random_remainder(64) + 127; palette_high_pixels(x, y);
}

void game_800BD104(void)
{
    sint32 x, y, base_x, base_y;
    uint32 count = r_u32(0x800BFBE4u);
    if ((sint32)count > 256) return;
    w_u32(0x800BFBE4u, count + 1u);
    palette_random_color();
    x = palette_random_remainder(64);
    base_x = (sint16)r_u16(0x800C8BC4u);
    x = base_x + 24 - palette_power_four(x, 2097152);
    y = palette_random_remainder(96) + (sint16)r_u16(0x800C8BC6u) - 1;
    palette_low_pixels(x, y);
    x = palette_random_remainder(32) + (sint16)r_u16(0x800C8BC4u) - 1;
    y = palette_random_remainder(64);
    base_y = (sint16)r_u16(0x800C8BC6u);
    y = base_y + 127 - palette_power_four(y, 524288); palette_low_pixels(x, y);
    x = palette_random_remainder(64);
    base_x = (sint16)r_u16(0x800C8BC4u);
    x = base_x + 32 - palette_power_four(x, 2097152);
    y = palette_random_remainder(32) + (sint16)r_u16(0x800C8BC6u) + 63;
    palette_low_pixels(x, y);
}

void game_800BD4B8(void)
{
    sint32 speed = (sint32)r_u32(0x8006708Cu) >> 8;
    uint32 type;
    if (speed < 0) speed = -speed;
    if (speed < 100) return;
    type = sub_8001685C(0x80066D6Cu);
    if (type == 4u || type == 8u) {
        game_800BCDD8(); game_800BD104();
    }
}
