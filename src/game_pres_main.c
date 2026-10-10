#include "draft_signatures.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define PRES32(address) (*(uint32 *)psx_addr((address), 4u))
#define PRES16(address) (*(uint16 *)psx_addr((address), 2u))

extern void sub_8002B2FC(void);
extern void pres_80092D7C(void);
extern void pres_8009B3E0(void);
extern void pres_8008EFE0(void);
extern void pres_8009EF04(uint32 mode);
extern void pres_80091CA8(void);
extern uint32 draft_agent3_missing_8003D9AC(uint32 mode);
extern uint32 draft_agent3_missing_8003D914(uint32 mode);
extern sint32 draft_agent1_missing_clear_image(const sint16 *rectangle, uint32 red, uint32 green, uint32 blue);

static void pres_load_allocated(uint32 path_word, uint32 buffer_word)
{
    uint32 bytes = sub_800423EC((const char *)psx_addr(PRES32(path_word), 1u), 1u);
    uint32 buffer = sub_80042544(bytes);
    const char *path = (const char *)psx_addr(PRES32(path_word), 1u);
    PRES32(buffer_word) = buffer;
    sub_8004225C(path, buffer, bytes);
}

/* Full reachable CFG from PRES.BIN 80092410 through 80092798 */
void pres_80092410(void)
{
    uint32 allocation;
    uint32 heap = PRES32(0x80065224u);
    uint32 x, y;
    uint32 random, divisor;
    char path[64];
    sint16 rectangle[4];

    sub_8004250C(heap, 0x801FF400u - heap);
    if (PRES32(0x80064C24u) == 0u)
    {
        sub_800237D8();
        PRES32(0x80064C24u) = 1u;
    }
    sub_80039690(0x1101u);
    sub_8003970C(2u);
    allocation = sub_80042544(0x80000u);
    sub_8004225C((const char *)psx_addr(0x800A20F8u, 1u), allocation, 0x80000u);
    sub_80015238(allocation);
    draft_agent3_missing_8003D9AC(0u);
    sub_8004261C(allocation);

    PRES32(0x800ABAC0u) = sub_80042544(0x800u);
    /* Both original sprintf formats contain only literal path characters */
    strcpy(path, (const char *)psx_addr(0x800A210Cu, 1u));
    sub_8004225C(path, PRES32(0x800ABAC0u), 0x800u);
    sub_80039A4C(PRES32(0x800ABAC0u));
    PRES32(0x800ABB70u) = sub_80018278();
    PRES32(0x800ABBA0u) = 0u;
    sub_8002B2FC();

    pres_load_allocated(0x800A2028u, 0x800ABAF8u);
    pres_load_allocated(0x800A1FCCu, 0x800ABB78u);
    pres_load_allocated(0x800A1F98u, 0x800ABB60u);
    pres_load_allocated(0x800A2050u, 0x800ABBB8u);
    PRES32(0x800ABB28u) = 0u;
    pres_80092D7C();
    pres_8009B3E0();
    pres_8008EFE0();
    PRES32(0x800ABB80u) = PRES32(0x80064BE0u) == 0xFFFFFFFFu ? 0u : 13u;
    allocation = PRES32(0x800ABB80u);
    PRES32(0x800ABB30u) = 0xFFFFFFFFu;
    PRES32(0x800ABBA8u) = 0xFFFFFFFFu;
    PRES32(0x800ABB68u) = 0u;
    PRES32(0x800ABB58u) = 0u;
    pres_8009EF04(allocation);

    PRES32(0x800ABB40u) = sub_80042544(0x41000u);
    strcpy(path, (const char *)psx_addr(0x800A2124u, 1u));
    sub_8004225C(path, PRES32(0x800ABB40u), 0x40014u);
    sub_8002B840();
    rectangle[2] = 512;
    rectangle[3] = 512;
    rectangle[0] = 0;
    rectangle[1] = 0;
    draft_agent1_missing_clear_image(rectangle, 0u, 0u, 0u);
    draft_agent3_missing_8003D914(1u);
    sub_80039718(2u);
    x = PRES32(0x80064BF0u);
    y = PRES32(0x80064BF4u);
    PRES16(0x80081A9Cu) = (uint16)x;
    PRES16(0x80081B10u) = (uint16)x;
    PRES16(0x80081A9Eu) = (uint16)y;
    PRES16(0x80081B12u) = (uint16)y;
    sub_800399D4();
    sub_80039934();
    sub_80020430(PRES32(0x80059758u));
    pres_80091CA8();
    random = randPSX();
    divisor = PRES32(0x80064D4Cu);
    /* Preserve the original BREAK faults rather than invoking undefined C division */
    if (divisor == 0u || (random == 0x80000000u && divisor == 0xFFFFFFFFu))
        abort();
    PRES32(0x80059758u) = (uint32)((int32_t)random % (int32_t)divisor);
}

uint32 pres_80092A50(uint32 font, uint32 character, uint32 pixels)
{
    uint32 descriptor = 0x800A21D0u + font * 144u;
    uint32 columns = r_u8(descriptor + 6u);
    uint32 texture;
    uint32 stride, height, width, pixel;
    int32_t column;
    uint32 result;
    uint32 start;
    if (columns == 0u)
        abort();
    texture = PRES32(descriptor + 12u);
    stride = ((r_u8(texture + 22u) + 3u) & 0x1FCu) >> 1;
    height = r_u8(descriptor + 5u);
    width = r_u8(descriptor + 4u);
    start = pixels + (uint32)((int32_t)character % (int32_t)columns) * (width >> 1) + (uint32)((int32_t)character / (int32_t)columns) * height * stride;
    result = width - 1u;
    column = (int32_t)result;
    while (column > 0)
    {
        uint32 row, nonzero = 0u;
        for (row = 0u; row < height; ++row)
        {
            pixel = r_u8(start + (uint32)(column / 2) + row * stride);
            pixel = (column & 1) ? pixel & 15u : pixel >> 4;
            if (pixel != 0u)
                ++nonzero;
        }
        if (nonzero == 0u)
            --result;
        --column;
        if (nonzero != 0u)
            break;
    }
    return result;
}

void pres_80092BD4(void)
{
    uint32 font;
    for (font = 0u; font < 3u; ++font)
    {
        uint32 descriptor = 0x800A21D0u + font * 144u;
        uint32 texture = sub_80039ADC(PRES32(descriptor));
        uint32 width, height, pixels, character;
        uint32 page;
        sint16 rectangle[4];
        PRES32(descriptor + 12u) = texture;
        width = (r_u8(texture + 22u) + 3u) & 0xFFFCu;
        height = r_u8(texture + 23u);
        pixels = sub_80042544((width >> 1) * height);
        texture = PRES32(descriptor + 12u);
        rectangle[0] = (sint16)((r_u8(texture + 24u) >> 2) + ((r_u16(texture + 8u) << 6) & 0x3FFu));
        texture = PRES32(descriptor + 12u);
        page = r_u16(texture + 8u);
        rectangle[1] = (sint16)(r_u8(texture + 25u) + ((page << 4) & 0x100u) + ((page >> 2) & 0x200u));
        rectangle[2] = (sint16)(width >> 2);
        rectangle[3] = (sint16)height;
        StoreImage((PSX_RECT *)rectangle, (uint32 *)psx_addr(pixels, 1u));
        draft_agent3_missing_8003D9AC(0u);
        for (character = 0u; character < 128u; ++character)
        {
            uint32 measured = pres_80092A50(font, character, pixels);
            w_u8(descriptor + 16u + character, (uint8)(measured - r_u8(descriptor + 7u)));
        }
        w_u8(descriptor + 16u, (uint8)(r_u8(descriptor + 4u) >> 1));
        sub_8004261C(pixels);
    }
}

void pres_80092D7C(void)
{
    uint32 packets, index;
    pres_80092BD4();
    packets = sub_80042544(0xCB20u);
    PRES32(0x800ABBE8u) = packets;
    for (index = 0u; index < 1000u; ++index)
    {
        w_u8(packets + index * 52u + 3u, 12u);
        w_u8(packets + index * 52u + 7u, 0x3Eu);
    }
}

void pres_8009279C(void)
{
    uint32 buttons = r_u16(0x80065524u);
    uint32 command = 0u;
    if (buttons & 0x4000u)
        command = 1u;
    else if (buttons & 0x1000u)
        command = 4u;
    else if (buttons & 0x8000u)
        command = 2u;
    else if (buttons & 0x2000u)
        command = 3u;
    if (command != 0u)
    {
        w_u8(0x800ABB00u + PRES32(0x800ABBA0u), (uint8)command);
        PRES32(0x800ABBA0u) = PRES32(0x800ABBA0u) + 1u;
    }
    PRES32(0x800ABBA0u) &= 15u;
}

extern void pres_80012D48(void);
extern void pres_80091040(void);
extern void pres_80092DD4(void);
extern void pres_800A0080(uint32 menu);
extern void pres_80092854(void);
extern void pres_8009DA08(void);
extern uint32 draft_agent3_missing_8003DC58(uint32 rectangle, const void *pixels);

static uint32 pres_signed_remainder(uint32 numerator, uint32 denominator)
{
    if (denominator == 0u || (numerator == 0x80000000u && denominator == 0xFFFFFFFFu))
        abort();
    return (uint32)((int32_t)numerator % (int32_t)denominator);
}

/* Copy each original load group before its corresponding store group */
static void pres_copy_record(uint32 destination, uint32 source)
{
    uint32 block;
    for (block = 0u; block < 32u; block += 16u)
    {
        uint32 a = r_u32(source + block);
        uint32 b = r_u32(source + block + 4u);
        uint32 c = r_u32(source + block + 8u);
        uint32 d = r_u32(source + block + 12u);
        w_u32(destination + block, a);
        w_u32(destination + block + 4u, b);
        w_u32(destination + block + 8u, c);
        w_u32(destination + block + 12u, d);
    }
    {
        uint32 a = r_u32(source + 32u);
        uint32 b = r_u32(source + 36u);
        uint32 c = r_u32(source + 40u);
        w_u32(destination + 32u, a);
        w_u32(destination + 36u, b);
        w_u32(destination + 40u, c);
    }
}

void pres_80091CA8(void)
{
    uint32 index = 0u;
    uint32 columns, rows;
    uint32 previous_tick;
    uint32 players = 1u;
    uint32 selection_base = 0u;
    uint32 selected, mode;
    pres_80012D48();
    PRES32(0x800ABBB0u) = 0u;
    PRES32(0x80064A28u) = 0u;
    previous_tick = (uint32)VSync(-1);
    pres_80091040();
    columns = PRES32(0x800A20C4u);
    rows = PRES32(0x800A20C8u);
    if ((int32_t)(columns * rows) > 0)
    {
        do
        {
            uint32 column = pres_signed_remainder(index, columns);
            if (r_u8(PRES32(0x800A20B8u) + column) != 0u)
            {
                uint32 horizontal = pres_signed_remainder(index, PRES32(0x800A20BCu));
                uint32 row = PRES32(0x800A20C0u) - 1u;
                while ((int32_t)row >= 0)
                {
                    uint32 address = PRES32(0x800A20B4u) + horizontal + row * PRES32(0x800A20BCu);
                    if (r_u8(address) == 0u)
                    {
                        w_u8(address, (uint8)(column + 1u));
                        break;
                    }
                    --row;
                }
                {
                    uint32 address = PRES32(0x800A20B8u) + column;
                    w_u8(address, (uint8)(r_u8(address) - 1u));
                }
            }
            columns = PRES32(0x800A20C4u);
            rows = PRES32(0x800A20C8u);
            ++index;
        } while ((int32_t)index < (int32_t)(columns * rows));
    }
    while (PRES32(0x800ABBB0u) == 0u)
    {
        sint16 rectangle[4];
        uint32 now;
        sub_8003A578();
        sub_800399D4();
        sub_80039908();
        sub_8003C034((const sint32 *)psx_addr(0x800A20D8u, 12u));
        sub_8003C1F8(0x800A20F0u);
        pres_80092DD4();
        {
            uint32 menu = PRES32(0x800ABB80u);
            PRES32(0x800A23E0u) = 0u;
            pres_800A0080(menu);
        }
        PRES32(0x800ABB28u) += 1u;
        pres_8009279C();
        pres_80092854();
        if (PRES32(0x800ABB80u) == 0u)
        {
            uint32 countdown = PRES32(0x80064B60u);
            if (countdown == 0u)
            {
                PRES32(0x80064B5Cu) = 1u;
                PRES32(0x800ABBB0u) = 1u;
                PRES32(0x80064B60u) = 1800u;
            }
            else
                PRES32(0x80064B60u) = countdown - 1u;
        }
        if (r_u16(0x80065524u) != 0u)
            PRES32(0x80064B60u) = 1800u;
        sub_800200E0();
        draft_agent3_missing_8003D9AC(0u);
        do
        {
            now = (uint32)VSync(-1);
        } while ((int32_t)(now - previous_tick) <= 0);
        previous_tick = (uint32)VSync(-1);
        sub_80042E98(1u);
        sub_80039934();
        {
            uint32 pixels = PRES32(0x800ABB40u);
            uint32 display = PRES32(0x80065128u);
            rectangle[3] = 256;
            rectangle[2] = 512;
            rectangle[0] = 0;
            rectangle[1] = (sint16)(display << 8);
            LoadImagePSX((PSX_RECT *)rectangle, (uint32 *)psx_addr(pixels + 20u, 1u));
        }
        pres_8009DA08();
        sub_80039A10();
    }
    {
        uint32 first = PRES32(0x80059740u);
        uint32 selected_player = PRES32(0x80059744u);
        uint32 source;
        PRES32(0x80065214u) = 1u;
        PRES32(0x80065210u) = selected_player;
        PRES32(0x80065234u) = first;
        source = 0x800594CCu + selected_player * 32u;
        memcpy(psx_addr(0x800A9BC4u, 4u), psx_addr(source, 4u), 4u);
        source = 0x800594D0u + PRES32(0x80059744u) * 32u;
        memcpy(psx_addr(0x800A9BC8u, 4u), psx_addr(source, 4u), 4u);
        pres_copy_record(0x800661BCu, 0x800A9BB0u);
        PRES32(0x800661D8u) = PRES32(0x8005978Cu);
    }
    if (PRES32(0x8005973Cu) == 2u)
    {
        uint32 selected_player = PRES32(0x80059744u);
        uint32 source = 0x800595CCu + selected_player * 32u;
        memcpy(psx_addr(0x800A9BF0u, 4u), psx_addr(source, 4u), 4u);
        source = 0x800595D0u + selected_player * 32u;
        memcpy(psx_addr(0x800A9BF4u, 4u), psx_addr(source, 4u), 4u);
        pres_copy_record(0x800661E8u, 0x800A9BDCu);
        players = 2u;
        PRES32(0x80066204u) = PRES32(0x80059790u);
    }
    mode = PRES32(0x80059738u);
    selected = PRES32(0x80059744u);
    switch (mode)
    {
        case 0u:
        case 8u:
        case 9u:
        case 10u:
        case 11u:
            switch (selected)
            {
                case 0u:
                    selection_base = 0u;
                    break;
                case 1u:
                    selection_base = 16u;
                    break;
                case 2u:
                    selection_base = 32u;
                    break;
                case 3u:
                case 4u:
                case 5u:
                    selection_base = 48u;
                    break;
                case 6u:
                case 7u:
                    selection_base = 64u;
                    break;
                default:
                    break;
            }
            break;
        case 1u:
            selection_base = 80u;
            break;
        case 2u:
            selection_base = 96u;
            break;
        case 3u:
        case 7u:
            selection_base = 112u;
            break;
        case 4u:
            selection_base = 128u;
            break;
        case 5u:
        case 6u:
            selection_base = 144u;
            break;
        default:
            selection_base = 0u;
            break;
    }
    for (index = selection_base + players; (int32_t)index < (int32_t)(selection_base + 16u); ++index)
        pres_copy_record(0x800661BCu + (index - selection_base) * 44u, 0x800A9BB0u + index * 44u);
}
