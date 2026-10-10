#include "draft_signatures.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define P32(a) (*(uint32 *)psx_addr((a), 4u))
#define P16(a) (*(uint16 *)psx_addr((a), 2u))
#define P8(a) (*(uint8 *)psx_addr((a), 1u))

static void pres_copy_name(uint32 destination, uint32 source)
{
    uint32 first, second;
    memcpy(&first, psx_addr(source, 4u), 4u);
    memcpy(&second, psx_addr(source + 4u, 4u), 4u);
    memcpy(psx_addr(destination, 4u), &first, 4u);
    memcpy(psx_addr(destination + 4u, 4u), &second, 4u);
}

extern void pres_dispatch_callback(uint32 target);
extern void pres_dispatch_callback1(uint32 target, uint32 object);
extern void pres_80093910(uint32 style);
extern void pres_80093008(uint32 font, sint32 x, sint32 y, uint32 text);
extern void pres_80094604(sint32 x, sint32 y);
extern void pres_8009D56C(void);
extern void pres_8008FC84(uint32 model, uint32 ignored);
void pres_8009F5D0(void);
void pres_8009F948(void);
void pres_8009F1F0(void);
void pres_8009EF04(uint32 mode);
void pres_8009C35C(uint32 object);
void pres_8009C23C(void);
void pres_8009B5F8(void);
uint32 pres_80092E20(uint32 identifier, uint32 text);
void pres_800A0240(sint32 flags, uint32 entry);
void pres_8009CEA8(void);

void pres_8009F5D0(void)
{
    uint32 phase = P32(0x800ABB20u), player;
    if (phase == 7u)
    {
        player = P32(0x80059744u);
        P32(0x8005973Cu) = 1u;
        P32(0x800ABBB0u) = 1u;
        P32(0x80059884u) = 0x800594CCu + (player << 5);
        P32(0x80059888u) = 0x800595CCu + (player << 5);
        return;
    }
    switch (P32(0x800ABB68u))
    {
        case 0u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
                P32(0x800ABB80u) = 17u;
                pres_8009EF04(17u);
                P32(0x800ABB68u) = 4u;
                P32(0x800ABB30u) = ~0u;
            }
            else if (phase == 1u)
            {
                P32(0x800AD670u) = (sint32)P32(0x80059738u) < 3 ? 1u : 2u;
                P32(0x800ABB80u) = 3u;
                pres_8009EF04(3u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 1u;
            }
            break;
        case 1u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
                P32(0x800ABB80u) = 7u;
                pres_8009EF04(7u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 0u;
            }
            else
            {
                P32(0x800ABB80u) = 4u;
                pres_8009EF04(4u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 2u;
            }
            break;
        case 2u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
                P32(0x800ABB80u) = 3u;
                pres_8009EF04(3u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 1u;
            }
            else if (phase == 2u)
            {
                player = P32(0x80059744u);
                P32(0x800ABB80u) = 9u;
                P32(0x800AD680u) = 0u;
                P32(0x80065330u) = 0x800594CCu + (player << 5);
                pres_8009EF04(9u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 3u;
            }
            break;
        case 3u:
            phase = P32(0x800ABB20u);
            if (phase == 3u || phase == 1u)
            {
                P32(0x800ABB80u) = 4u;
                pres_8009EF04(4u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 2u;
                phase = P32(0x800ABB20u);
            }
            goto finish_name;
        case 4u:
            phase = P32(0x800ABB20u);
        finish_name:
            if (phase == 3u)
            {
                if ((sint32)P32(P32(0x800651FCu)) > 0)
                    pres_copy_name(0x80059770u, 0x8006647Cu);
                P32(0x800ABB58u) = 0u;
                P32(0x800ABB80u) = 0u;
                pres_8009EF04(0u);
                P32(0x800ABB30u) = ~0u;
            }
            else if (phase == 1u)
            {
                pres_copy_name(0x80059770u, 0x8006647Cu);
                P32(0x800ABB80u) = 7u;
                pres_8009EF04(7u);
                P32(0x800ABB68u) = 0u;
                P32(0x800ABB30u) = ~0u;
            }
            break;
        default:
            break;
    }
}

void pres_8009F948(void)
{
    uint32 phase = P32(0x800ABB20u), player;
    if (phase == 7u)
    {
        P32(0x8005973Cu) = 1u;
        P32(0x800ABBB0u) = 1u;
        return;
    }
    switch (P32(0x800ABB68u))
    {
        case 0u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
                P32(0x800AD678u) = 1u;
                goto name_entry;
            }
            P32(0x800AD670u) = P32(0x80059738u) - 3u < 2u ? 2u : 1u;
            P32(0x800ABB80u) = 3u;
            pres_8009EF04(3u);
            P32(0x800ABB30u) = ~0u;
            P32(0x800ABB68u) = 1u;
            break;
        case 1u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
                goto opening;
            goto cars;
        case 2u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
                P32(0x800ABB80u) = 3u;
                pres_8009EF04(3u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 1u;
            }
            else
                goto first_car;
            break;
        case 3u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
            cars:
                P32(0x800ABB80u) = 16u;
                pres_8009EF04(16u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 2u;
            }
            else
            {
                player = P32(0x80059744u);
                P32(0x800ABB80u) = 9u;
                P32(0x800AD680u) = 3u;
                P32(0x80065330u) = 0x800595CCu + (player << 5);
                pres_8009EF04(9u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 4u;
            }
            break;
        case 4u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
            first_car:
                player = P32(0x80059744u);
                P32(0x800AD680u) = 2u;
                P32(0x800ABB80u) = 9u;
                P32(0x80065330u) = 0x800594CCu + (player << 5);
                pres_8009EF04(9u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 3u;
            }
            else
            {
                player = P32(0x80059744u);
                P32(0x8005973Cu) = 2u;
                P32(0x800ABBB0u) = 1u;
                P32(0x80059884u) = 0x800594CCu + (player << 5);
                P32(0x80059888u) = 0x800595CCu + (player << 5);
            }
            break;
        case 5u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
                goto opening;
            P32(0x800ABB80u) = 16u;
            pres_8009EF04(16u);
            P32(0x800ABB30u) = ~0u;
            P32(0x800ABB68u) = 6u;
            break;
        case 6u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
                if ((sint32)P32(P32(0x800651FCu)) > 0)
                    pres_copy_name(0x80059770u, 0x8006647Cu);
                P32(0x800ABB58u) = 0u;
                P32(0x800ABB80u) = 0u;
                pres_8009EF04(0u);
                P32(0x800ABB30u) = ~0u;
            }
            else if (phase == 1u)
            {
                pres_copy_name(0x80059770u, 0x8006647Cu);
                P32(0x800AD678u) = phase;
            name_entry:
                P32(0x800ABB80u) = 17u;
                pres_8009EF04(17u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 7u;
            }
            break;
        case 7u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
                if ((sint32)P32(P32(0x800651FCu)) > 0)
                    pres_copy_name(0x8005977Au, 0x8006648Cu);
                P32(0x800ABB68u) = 6u;
                P32(0x800AD678u) = 0u;
                P32(0x800ABB80u) = 17u;
                pres_8009EF04(17u);
                P32(0x800ABB30u) = ~0u;
            }
            else if (phase == 1u)
            {
                pres_copy_name(0x8005977Au, 0x8006648Cu);
            opening:
                P32(0x800ABB80u) = 15u;
                pres_8009EF04(15u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 0u;
            }
            break;
        default:
            break;
    }
}

void pres_8009F1F0(void)
{
    uint32 phase = P32(0x800ABB20u), player, type, descriptor;
    if (phase == 7u)
    {
        player = P32(0x80059744u);
        type = P32(0x80059738u);
        P32(0x8005973Cu) = 1u;
        P32(0x800ABBB0u) = 1u;
        P32(0x80059884u) = 0x800594CCu + (player << 5);
        P32(0x80059888u) = 0x800595CCu + (player << 5);
        P32(0x80059740u) = P8(0x8005A790u + (type << 4) + P32(0x80065200u));
        return;
    }
    switch (P32(0x800ABB68u))
    {
        case 0u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
                P32(0x800ABB80u) = 0u;
                pres_8009EF04(0u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB58u) = 0u;
            }
            else if (phase == 1u)
            {
                descriptor = P32(P32(0x800AD568u + P32(0x800AD668u) * 4u));
                if (P32(descriptor + 4u) == 0x800A65F4u)
                {
                    P32(0x800AD678u) = 2u;
                    P32(0x800ABB80u) = 17u;
                    pres_8009EF04(17u);
                    P32(0x800ABB30u) = ~0u;
                    P32(0x800ABB68u) = phase;
                }
            }
            break;
        case 1u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
                if ((sint32)P32(P32(0x800651FCu)) > 0)
                    pres_copy_name(0x80059770u, 0x8006647Cu);
                P32(0x800ABB80u) = 11u;
                pres_8009EF04(11u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 0u;
            }
            else if (phase == 1u)
            {
                pres_copy_name(0x80059770u, 0x8006647Cu);
                goto cross_menu;
            }
            break;
        case 2u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
                P32(0x800AD678u) = 2u;
                P32(0x800ABB80u) = 17u;
                pres_8009EF04(17u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 1u;
            }
            else if (phase == 1u)
            {
                printf("%s", (const char *)psx_addr(0x800A9B7Cu, 1u));
                goto car_menu;
            }
            break;
        case 3u:
            phase = P32(0x800ABB20u);
            if (phase == 3u)
            {
            cross_menu:
                P32(0x800ABB80u) = 12u;
                pres_8009EF04(12u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 2u;
            }
            else if (phase == 1u)
            {
                printf("%s", (const char *)psx_addr(0x800A9B8Cu, 1u));
            }
            else if (phase == 2u)
            {
                printf("%s", (const char *)psx_addr(0x800A9B9Cu, 1u));
                player = P32(0x80059744u);
                P32(0x800ABB80u) = 9u;
                P32(0x800AD680u) = 0u;
                P32(0x80065330u) = 0x800594CCu + (player << 5);
                pres_8009EF04(9u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 4u;
            }
            break;
        case 4u:
            phase = P32(0x800ABB20u);
            if (phase == 3u || phase == 1u)
            {
            car_menu:
                P32(0x800ABB80u) = 4u;
                pres_8009EF04(4u);
                P32(0x800ABB30u) = ~0u;
                P32(0x800ABB68u) = 3u;
            }
            break;
        default:
            break;
    }
}

void pres_8009880C(void)
{
    /* The complete original body is JR RA with a NOP delay slot */
}

void pres_80093CC0(void)
{
    uint32 state = P32(0x80064BDCu);
    P32(0x800A31D4u) = 300u;
    if (!state && sub_80024A30(P32(0x80064E74u)) == 0u)
        sub_80024A30(P32(0x80064E74u));
}

void pres_8009B3E0(void)
{
    uint32 index, offset, texture, primitive;
    P32(0x800AD418u) = sub_80042544(0x2800u);
    P32(0x800AD41Cu) = sub_80042544(0x3400u);
    for (index = 0u, offset = 0u; index < 256u; ++index, offset += 40u)
    {
        P8(P32(0x800AD418u) + offset + 3u) = 9u;
        P8(P32(0x800AD418u) + offset + 7u) = 0x2Cu;
        primitive = P32(0x800AD418u) + offset;
        P8(primitive + 7u) = (uint8)(P8(primitive + 7u) & 0xFDu);
        primitive = P32(0x800AD418u) + offset;
        P8(primitive + 7u) = (uint8)(P8(primitive + 7u) & 0xFEu);
    }
    texture = sub_80039ADC(0x800A9B6Cu);
    for (index = 0u, offset = 0u; index < 256u; ++index, offset += 52u)
    {
        P8(P32(0x800AD41Cu) + offset + 3u) = 12u;
        P8(P32(0x800AD41Cu) + offset + 7u) = 0x3Cu;
        primitive = P32(0x800AD41Cu) + offset;
        P8(primitive + 7u) = (uint8)(P8(primitive + 7u) & 0xFEu);
        primitive = P32(0x800AD41Cu) + offset;
        P16(primitive + 26u) = P16(texture + 8u);
        P16(primitive + 14u) = P16(texture + 10u);
        P8(primitive + 12u) = P8(texture + 24u);
        P8(P32(0x800AD41Cu) + offset + 13u) = P8(texture + 25u);
        primitive = P32(0x800AD41Cu) + offset;
        P8(primitive + 24u) = (uint8)((uint32)P8(texture + 22u) + P8(texture + 24u) + 255u);
        P8(P32(0x800AD41Cu) + offset + 25u) = P8(texture + 25u);
        P8(P32(0x800AD41Cu) + offset + 36u) = P8(texture + 24u);
        primitive = P32(0x800AD41Cu) + offset;
        P8(primitive + 37u) = (uint8)((uint32)P8(texture + 23u) + P8(texture + 25u) + 255u);
        primitive = P32(0x800AD41Cu) + offset;
        P8(primitive + 48u) = (uint8)((uint32)P8(texture + 22u) + P8(texture + 24u) + 255u);
        primitive = P32(0x800AD41Cu) + offset;
        P8(primitive + 49u) = (uint8)((uint32)P8(texture + 23u) + P8(texture + 25u) + 255u);
    }
}

void pres_8008EFE0(void)
{
    uint32 texture = sub_80039ADC(0x800A1CD0u);
    uint32 index, port, primitive;
    P32(0x800ABAB8u) = texture;
    P32(0x800ABAA8u) = sub_80042544(0x1900u);
    P32(0x800ABAACu) = sub_80042544(0x1900u);
    for (index = 0u; index < 200u; ++index)
    {
        for (port = 0u; port < 2u; ++port)
        {
            primitive = P32(0x800ABAA8u + port * 4u) + index * 32u;
            P8(primitive + 3u) = 7u;
            P8(primitive + 7u) = 0x24u;
            P16(primitive + 22u) = P16(texture + 8u);
            P8(primitive + 7u) = (uint8)((P8(primitive + 7u) | 2u) & 0xFEu);
            P8(primitive + 4u) = 0x80u;
            P8(primitive + 5u) = 0x80u;
            P8(primitive + 6u) = 0x80u;
            P16(primitive + 14u) = P16(texture + 10u);
        }
    }
    P32(0x800ABAB0u) = sub_80042544(0x1F40u);
    P32(0x800ABAB4u) = sub_80042544(0x1F40u);
    for (index = 0u; index < 200u; ++index)
    {
        for (port = 0u; port < 2u; ++port)
        {
            primitive = P32(0x800ABAB0u + port * 4u) + index * 40u;
            P8(primitive + 3u) = 9u;
            P8(primitive + 7u) = 0x2Cu;
            P16(primitive + 22u) = P16(texture + 8u);
            P8(primitive + 7u) = (uint8)((P8(primitive + 7u) | 2u) & 0xFEu);
            P8(primitive + 4u) = 0x80u;
            P8(primitive + 5u) = 0x80u;
            P8(primitive + 6u) = 0x80u;
            P16(primitive + 14u) = P16(texture + 10u);
        }
    }
}

void pres_8009EF04(uint32 mode)
{
    uint32 value = P32(0x80065334u);
    uint32 descriptor = P32(0x800A1D40u + (mode << 2));
    uint32 index, offset, count, object, pointer, candidate, left, right;
    char number[12];
    P32(0x800ABB48u) = 0u;
    P32(0x800AD830u) = 0u;
    P32(0x800AD6C0u) = 0u;
    P32(0x800AD850u) = 1u;
    P32(0x800ABB20u) = 0u;
    sprintf(number, "%d", (sint32)value);
    memcpy(psx_addr(0x800ABB88u, strlen(number) + 1u), number, strlen(number) + 1u);
    P32(0x800A2900u) = 0x800ABB88u;
    for (index = 0u; (sint32)index < (sint32)P32(descriptor + 12u); ++index)
        P32(0x800AD568u + index * 4u) = 0u;
    P32(0x800AD688u) = 0u;
    for (index = 0u, offset = 0u; (sint32)index < (sint32)P32(descriptor + 12u); ++index, offset += 48u)
        pres_8009C35C(P32(descriptor + 8u) + offset);
    count = P32(0x800AD6C0u);
    for (index = 0u; (sint32)index < (sint32)count; ++index)
    {
        object = P32(0x800AD6C8u + index * 4u);
        pointer = P32(object + 44u);
        value = P32(pointer);
        pointer = P32(object + 28u);
        offset = P32(0x80059748u) << 2;
        value = P32(pointer + (value << 2));
        P32(object + offset + 8u) = value;
    }
    pres_dispatch_callback(P32(descriptor));
    value = P32(0x800ABB80u);
    if (value != 17u)
    {
        value = P8(0x80078A78u + value);
        P32(0x800AD668u) = value;
        P32(0x800AD560u) = value;
    }
    count = P32(0x800AD830u);
    for (index = 0u, offset = 0u; (sint32)index < (sint32)count; ++index, offset += 68u)
    {
        if (P32(0x800AD864u + offset))
            continue;
        object = P32(0x800AD860u + offset);
        if (P32(object + 16u) & 0xFF000000u)
            continue;
        if (P32(object + 20u) & 0xFF000000u)
            continue;
        pointer = P32(object + 40u);
        value = P32(pointer + 16u);
        if (!value)
        {
            fputs("PRES division BREAK 7 at 8009F128\n", stderr);
            abort();
        }
        left = (uint32)(1048576 / (sint32)value);
        value = P32(pointer + 20u);
        if (!value)
        {
            fputs("PRES division BREAK 7 at 8009F15C\n", stderr);
            abort();
        }
        right = (uint32)(1048576 / (sint32)value);
        if ((sint32)left < (sint32)right)
            left = right;
        candidate = ((P32(object + 8u) + 2u) << 8) + left;
        if ((sint32)P32(0x800ABB38u) < (sint32)candidate)
            P32(0x800ABB38u) = candidate;
    }
    pres_8009C23C();
}

void pres_8009C23C(void)
{
    uint32 base, index;
    if (!P32(0x800AD688u))
        return;
    P32(0x800AD564u) = P32(0x800AD830u);
    pres_8009C35C(0x800A2464u);
    pres_8009C35C(0x800A24A0u);
    pres_8009C35C(0x800A24D8u);
    pres_8009C35C(0x800A2510u);
    pres_8009C35C(0x800A2434u);
    base = P32(0x800AD564u);
    for (index = 0u; index < 5u; ++index)
        P32(0x800AD884u + (base + index) * 68u) = 4096u;
    pres_8009B5F8();
}

static uint32 pres_object_word(const uint8 *object, uint32 offset)
{
    uint32 value;
    memcpy(&value, object + offset, 4u);
    return value;
}

static uint16 pres_object_half(const uint8 *object, uint32 offset)
{
    uint16 value;
    memcpy(&value, object + offset, 2u);
    return value;
}

static uint32 pres_copy_object(const uint8 *object)
{
    uint32 destination = 0x800ABC18u + P32(0x800AD830u) * 48u;
    uint32 offset;
    for (offset = 0u; offset < 48u; offset += 16u)
    {
        uint32 a = pres_object_word(object, offset);
        uint32 b = pres_object_word(object, offset + 4u);
        uint32 c = pres_object_word(object, offset + 8u);
        uint32 d = pres_object_word(object, offset + 12u);
        P32(destination + offset) = a;
        P32(destination + offset + 4u) = b;
        P32(destination + offset + 8u) = c;
        P32(destination + offset + 12u) = d;
    }
    return 0x800ABC18u + P32(0x800AD830u) * 48u;
}

uint32 pres_80092E20(uint32 identifier, uint32 text)
{
    uint32 font = 0x800A21D0u + (identifier & 15u) * 144u;
    uint32 length = (uint32)strlen((const char *)psx_addr(text, 1u));
    uint32 line = 0u, maximum = 0u, end = text + length;
    if ((sint32)length > 0)
    {
        do
        {
            uint32 character = P8(text);
            uint32 glyph = character - 32u;
            if (character == '$')
                glyph = 84u;
            if (character == '%')
                glyph = 85u;
            if (character == '^')
                glyph = 86u;
            if (character == '&')
                glyph = 87u;
            if (character == 10u)
            {
                if ((sint32)maximum < (sint32)line)
                    maximum = line;
                line = 0u;
            }
            else if (character == 32u)
                line += P8(font + (glyph & 255u) + 16u);
            else
                line += 1u + P8(font + (glyph & 255u) + 16u);
            ++text;
        } while ((sint32)text < (sint32)end);
    }
    if ((sint32)maximum < (sint32)line)
        maximum = line;
    return (uint32)((sint32)(maximum + (maximum >> 31)) >> 1);
}

void pres_800A0240(sint32 flags, uint32 entry)
{
    uint32 bits = (uint32)flags, count, primitive;
    if (!(bits << 16))
        return;
    if (bits & 4u)
    {
        count = P32(0x800AD688u);
        primitive = 0x800AD81Cu + P32(0x800AD830u) * 68u;
        P32(0x800AD568u + (count << 2)) = primitive;
        P32(0x800AD688u) = count + 1u;
    }
    if ((bits & 8u) && !(P16(entry + 48u) & 0x7FFFu))
        P16(entry + 48u) = 24u;
    if (bits & 16u)
        P16(entry + 48u) = 0x8000u;
    if ((bits & 32u) && !P32(0x800ABB48u))
    {
        P32(entry + 32u) = 4096u;
        P32(entry + 36u) = 4096u;
    }
    if ((bits & 64u) && P32(0x800ABB48u) == 1u)
    {
        P32(entry + 32u) = 4096u;
        P32(entry + 36u) = 4096u;
    }
    if (bits & 128u)
        P32(0x800AD6B8u) = P32(0x800AD830u) - 1u;
    if (bits & 256u)
        P32(0x800AD7A8u) = P32(0x800AD830u) - 1u;
    if (bits & 512u)
        P16(entry + 48u) = 0u;
    if (bits & 4096u)
    {
        primitive = P32(entry + 12u);
        P8(primitive + 7u) = (uint8)(P8(primitive + 7u) & 0xFDu);
        primitive = P32(entry + 12u);
        P8(primitive + 59u) = (uint8)(P8(primitive + 59u) & 0xFDu);
    }
}

static void pres_object_native(const uint8 *object)
{
    uint32 entry = 0x800AD860u + P32(0x800AD830u) * 68u;
    uint32 type, copied, data, texture, count, primitive, index;
    uint32 u0, u1, u2, u3, v0, v1, v2, v3, flags;
    memset(psx_addr(entry, 68u), 0, 68u);
    type = (uint32)(sint32)(sint16)pres_object_half(object, 0u);
    if (type < 8u && type != 4u)
    {
        copied = pres_copy_object(object);
        P32(entry) = copied;
    }
    switch (type)
    {
        case 0u:
            P32(entry + 32u) = 0u;
            P32(entry + 36u) = 0u;
            P32(entry + 16u) = pres_object_word(object, 8u);
            data = pres_object_word(object, 4u);
            index = (P32(data) & 0x7Fu) * 144u;
            P32(entry + 44u) = (uint32)P8(0x800A21D5u + index) + (uint32)(sint32)(sint8)P8(0x800A21D8u + index);
            data = pres_object_word(object, 4u);
            flags = P32(0x80059748u) << 2;
            P32(entry + 40u) = pres_80092E20(P32(data) & 0x7Fu, P32(data + flags + 8u)) << 1;
            data = pres_object_word(object, 4u);
            texture = P32(data + 28u);
            if (texture)
            {
                index = P32(P32(data + 44u));
                flags = P32(0x80059748u) << 2;
                P32(data + flags + 8u) = P32(texture + (index << 2));
                count = P32(0x800AD6C0u);
                data = pres_object_word(object, 4u);
                P32(0x800AD6C0u) = count + 1u;
                P32(0x800AD6C8u + (count << 2)) = data;
            }
            P32(entry + 24u) = 0x4000u;
            P32(entry + 20u) = 0x4000u;
            P32(0x800AD830u) = P32(0x800AD830u) + 1u;
            break;
        case 1u:
            texture = sub_80039ADC(pres_object_word(object, 4u));
            P32(entry + 8u) = texture;
            data = pres_object_word(object, 8u);
            P32(entry + 32u) = 0u;
            P32(entry + 36u) = 0u;
            P8(entry + 56u) = 128u;
            P8(entry + 57u) = 128u;
            P8(entry + 58u) = 128u;
            P32(entry + 16u) = data;
            count = P8(texture + 22u);
            P32(entry + 40u) = count;
            if (!count)
                P32(entry + 40u) = 256u;
            P32(entry + 44u) = P8(texture + 23u);
            P32(entry + 12u) = P32(0x800AD418u) + P32(0x800AD830u) * 80u;
            u0 = P8(texture + 24u);
            u1 = u0 + P8(texture + 22u) - 1u;
            u2 = u0;
            u3 = u1;
            v0 = P8(texture + 25u);
            v1 = v0;
            v2 = v0 + P8(texture + 23u) - 1u;
            v3 = v2;
            flags = P16(P32(entry) + 2u);
            if (flags & 1u)
            {
                uint32 temporary = u0;
                u0 = u1;
                u1 = temporary;
                u2 = u0;
                u3 = u1;
            }
            if (flags & 2u)
            {
                uint32 temporary = v0;
                v0 = v2;
                v2 = temporary;
                v1 = v0;
                v3 = v2;
            }
            for (index = 0u; index < 2u; ++index)
            {
                primitive = P32(entry + 12u) + index * 40u;
                P16(primitive + 22u) = P16(texture + 8u);
                P16(P32(entry + 12u) + index * 40u + 14u) = P16(texture + 10u);
                P8(P32(entry + 12u) + index * 40u + 12u) = (uint8)u0;
                P8(P32(entry + 12u) + index * 40u + 13u) = (uint8)v0;
                P8(P32(entry + 12u) + index * 40u + 20u) = (uint8)u1;
                P8(P32(entry + 12u) + index * 40u + 21u) = (uint8)v1;
                P8(P32(entry + 12u) + index * 40u + 28u) = (uint8)u2;
                P8(P32(entry + 12u) + index * 40u + 29u) = (uint8)v2;
                P8(P32(entry + 12u) + index * 40u + 36u) = (uint8)u3;
                P8(P32(entry + 12u) + index * 40u + 37u) = (uint8)v3;
            }
            P32(entry + 24u) = 0x4000u;
            P32(entry + 20u) = 0x4000u;
            P32(0x800AD830u) = P32(0x800AD830u) + 1u;
            break;
        case 2u:
            count = P32(0x800AD830u);
            data = pres_object_word(object, 8u);
            P32(entry + 32u) = 0u;
            P32(entry + 36u) = 0u;
            P32(entry + 40u) = 256u;
            P32(entry + 44u) = 256u;
            P16(entry + 64u) = 0u;
            P16(entry + 62u) = 0u;
            P16(entry + 60u) = 0u;
            P32(entry + 16u) = data;
            data = pres_object_word(object, 4u);
            data = (uint32)(sint32)(sint16)P16(data + 2u);
            P32(0x800AD830u) = count + 1u;
            P32(entry + 24u) = 0x4000u;
            P32(entry + 20u) = 0x4000u;
            P32(entry + 28u) = data;
            break;
        case 3u:
            texture = sub_80039ADC(0x800A9B74u);
            P32(entry + 8u) = texture;
            data = pres_object_word(object, 8u);
            P32(entry + 32u) = 0u;
            P32(entry + 36u) = 0u;
            P32(entry + 16u) = data;
            data = pres_object_word(object, 4u);
            P32(entry + 40u) = P32(data);
            data = pres_object_word(object, 4u);
            count = P32(0x800AD830u);
            primitive = P32(0x800AD41Cu) + count * 104u;
            P32(entry + 12u) = primitive;
            P32(entry + 44u) = P32(data + 4u);
            P8(primitive + 7u) = (uint8)(P8(primitive + 7u) | 2u);
            primitive = P32(0x800AD41Cu) + P32(0x800AD830u) * 104u;
            P8(primitive + 59u) = (uint8)(P8(primitive + 59u) | 2u);
            P32(entry + 24u) = 0x4000u;
            P32(entry + 20u) = 0x4000u;
            P32(0x800AD830u) = P32(0x800AD830u) + 1u;
            break;
        case 4u:
            data = pres_object_word(object, 4u);
            if (P32(data + 4u))
            {
                uint32 child_offset = 0u;
                do
                {
                    uint32 child[12];
                    memcpy(child, object, sizeof(child));
                    if (child_offset)
                    {
                        data = pres_object_word(object, 4u);
                        child[4] = P32(data + child_offset + 8u);
                        data = pres_object_word(object, 4u);
                        child[5] = P32(data + child_offset + 12u);
                    }
                    else
                    {
                        child[4] = pres_object_word(object, 16u);
                        child[5] = pres_object_word(object, 20u);
                    }
                    data = pres_object_word(object, 4u);
                    ((uint16 *)child)[0] = P16(data + child_offset);
                    data = pres_object_word(object, 4u);
                    child[1] = P32(data + child_offset + 4u);
                    data = pres_object_word(object, 4u);
                    ((uint16 *)child)[1] = P16(data + child_offset + 2u);
                    pres_object_native((const uint8 *)child);
                    if (child_offset)
                        P32(0x800AD864u + (P32(0x800AD830u) - 1u) * 68u) = entry;
                    data = pres_object_word(object, 4u);
                    child_offset += 16u;
                } while (P32(data + child_offset + 4u));
            }
            break;
        case 5u:
        case 6u:
            count = P32(0x800AD830u);
            P32(entry + 32u) = 0u;
            P32(entry + 36u) = 0u;
            P32(entry + 16u) = pres_object_word(object, 8u);
            data = pres_object_word(object, 4u);
            if (type == 5u)
                P32(entry + 40u) = (uint32)(sint32)(sint16)P16(data + 4u);
            else
                P32(entry + 40u) = P32(data);
            data = pres_object_word(object, 4u);
            if (type == 5u)
                data = (uint32)(sint32)(sint16)P16(data + 6u);
            else
                data = P32(data + 4u);
            P32(0x800AD830u) = count + 1u;
            P32(entry + 24u) = 0x4000u;
            P32(entry + 20u) = 0x4000u;
            P32(entry + 44u) = data;
            break;
        case 7u:
            count = P32(0x800AD830u);
            data = pres_object_word(object, 8u);
            P32(entry + 32u) = 0u;
            P32(entry + 36u) = 0u;
            P32(0x800AD830u) = count + 1u;
            P32(entry + 24u) = 0x4000u;
            P32(entry + 20u) = 0x4000u;
            P32(entry + 16u) = data;
            break;
        default:
            break;
    }
    /* Original A0240 applies the signed descriptor flags to the emitted entry */
    pres_800A0240((sint32)(sint16)pres_object_half(object, 2u), entry);
}

void pres_8009C35C(uint32 object)
{
    pres_object_native((const uint8 *)psx_addr(object, 48u));
}

static uint32 pres_signed_half(uint32 value)
{
    return (uint32)((sint32)(value + (value >> 31)) >> 1);
}

static uint32 pres_interpolate(uint32 current, uint32 previous, uint32 factor)
{
    uint32 product = factor * (previous - current);
    if ((sint32)product < 0)
        product += 4095u;
    return previous - (uint32)((sint32)product >> 12);
}

static uint32 pres_menu_buttons(void)
{
    return P16(0x80065524u + (P32(0x800A23E0u) << 1));
}

static uint32 pres_cursor_object(uint32 base, uint32 index)
{
    return P32(0x800AD860u + (base + index) * 68u);
}

void pres_8009B5F8(void)
{
    uint32 base = P32(0x800AD564u), index, object, old, current, previous;
    uint32 current_x, previous_x, current_flags, previous_flags, half, factor;
    uint32 left_current, left_previous, right_current, right_previous, position;
    if (P32(0x800AD880u + base * 68u) == 4096u && P32(0x800AD884u + base * 68u) == 4096u && !P32(0x800ABB38u) && P32(0x80064BECu) && P32(0x800A5A04u) == 2u && pres_menu_buttons())
    {
        old = P32(0x800AD668u);
        P32(0x800AD560u) = old;
        if (P32(0x800ABB80u) != 17u)
        {
            if (pres_menu_buttons() & 0x40u)
            {
                if (old == P32(0x800AD688u) - 1u)
                    P32(0x800AD668u) = 0u;
                else
                    P32(0x800AD668u) = old + 1u;
                sub_8001D01C(0u);
            }
            if (pres_menu_buttons() & 0x10u)
            {
                position = P32(0x800AD668u);
                if (!position)
                    position = P32(0x800AD688u);
                P32(0x800AD668u) = position - 1u;
                sub_8001D01C(0u);
            }
        }
        else
        {
            if (pres_menu_buttons() & 0x40u)
            {
                P32(0x800AD668u) = old + 10u;
                sub_8001D01C(0u);
            }
            if (pres_menu_buttons() & 0x10u)
            {
                P32(0x800AD668u) = P32(0x800AD668u) - 10u;
                sub_8001D01C(0u);
            }
            if (pres_menu_buttons() & 0x80u)
            {
                position = P32(0x800AD668u) - 1u;
                P32(0x800AD668u) = position;
                if (position == 0xFFFFFFFFu || position == 9u || position == 19u)
                    P32(0x800AD668u) = 29u;
                sub_8001D01C(0u);
            }
            if (pres_menu_buttons() & 0x20u)
            {
                position = P32(0x800AD668u) + 1u;
                P32(0x800AD668u) = position;
                if (position == 10u)
                    P32(0x800AD668u) = 0u;
                else if (position == 20u)
                    P32(0x800AD668u) = 10u;
                else if (position == 30u)
                    P32(0x800AD668u) = 20u;
                sub_8001D01C(0u);
            }
            position = P32(0x800AD668u);
            if ((sint32)position < 0)
                position += 30u;
            else
                position %= 30u;
            P32(0x800AD668u) = position;
        }
        if (P32(0x800AD560u) != P32(0x800AD668u))
        {
            base = P32(0x800AD564u);
            for (index = 0u; index < 5u; ++index)
            {
                object = pres_cursor_object(base, index);
                P32(0x800AD884u + (base + index) * 68u) = 0u;
                P32(object + 28u) = P32(object + 20u);
            }
        }
    }
    current = P32(0x800AD568u + (P32(0x800AD668u) << 2));
    previous = P32(0x800AD568u + (P32(0x800AD560u) << 2));
    if (P32(0x800ABB80u) == 17u)
    {
        P32(current + 40u) = 15u;
        P32(previous + 40u) = 15u;
    }
    current_x = P32(P32(current) + 16u);
    previous_x = P32(P32(previous) + 16u);
    half = pres_signed_half(P32(current + 40u));
    left_current = (current_x & 0xFFFFu) - half - 16u;
    left_previous = (previous_x & 0xFFFFu) - pres_signed_half(P32(previous + 40u)) - 16u;
    current_flags = current_x & 0xFFFF0000u;
    previous_flags = previous_x & 0xFFFF0000u;
    if (current_flags & 0x10000u)
        left_current -= half;
    else if (!current_flags)
        left_current += half;
    if (previous_flags == 0x10000u)
        left_previous -= pres_signed_half(P32(previous + 40u));
    else if (!previous_flags)
        left_previous += pres_signed_half(P32(previous + 40u));
    base = P32(0x800AD564u);
    factor = P32(0x800AD884u + base * 68u);
    position = pres_interpolate(left_current, left_previous, factor);
    object = pres_cursor_object(base, 0u);
    P32(object + 16u) = position | 0x10000u;
    object = pres_cursor_object(base, 0u);
    P32(object + 20u) = ((uint32)P16(P32(current) + 20u) - pres_signed_half(P32(current + 44u)) + 17u) | 0x20000u;
    position = P16(pres_cursor_object(base, 0u) + 16u);
    P32(pres_cursor_object(base, 2u) + 16u) = position;
    position = P32(pres_cursor_object(base, 0u) + 20u);
    P32(pres_cursor_object(base, 2u) + 20u) = position;
    if (P32(0x800ABB80u) != 17u)
        position = (uint32)P16(P32(current) + 16u) - P32(pres_cursor_object(base, 2u) + 16u) + 1u;
    else
        position = 24u;
    P32(0x800AD888u + (base + 2u) * 68u) = position;

    half = pres_signed_half(P32(current + 40u));
    right_current = (uint32)P16(P32(current) + 16u) + half + 12u;
    right_previous = (uint32)P16(P32(previous) + 16u) + pres_signed_half(P32(previous + 40u)) + 12u;
    if (current_flags & 0x10000u)
        right_current -= half;
    else if (!current_flags)
        right_current += half;
    if (previous_flags == 0x10000u)
        right_previous -= pres_signed_half(P32(previous + 40u));
    else if (!previous_flags)
        right_previous += pres_signed_half(P32(previous + 40u));
    base = P32(0x800AD564u);
    factor = P32(0x800AD884u + base * 68u);
    P32(pres_cursor_object(base, 3u) + 16u) = pres_interpolate(right_current, right_previous, factor);
    position = P32(pres_cursor_object(base, 0u) + 20u);
    P32(pres_cursor_object(base, 3u) + 20u) = position;
    position = P16(P32(current) + 16u);
    if (P32(0x800ABB80u) != 17u)
    {
        P32(pres_cursor_object(base, 1u) + 16u) = position;
        position = P32(pres_cursor_object(base, 3u) + 16u) - P32(pres_cursor_object(base, 1u) + 16u) + 1u;
    }
    else
    {
        P32(pres_cursor_object(base, 1u) + 16u) = P32(pres_cursor_object(base, 2u) + 16u) + 23u;
        position = 20u;
    }
    P32(0x800AD888u + (base + 1u) * 68u) = position;
    base = P32(0x800AD564u);
    position = P32(pres_cursor_object(base, 0u) + 20u);
    P32(pres_cursor_object(base, 1u) + 20u) = position;
    position = P16(pres_cursor_object(base, 0u) + 16u);
    P32(pres_cursor_object(base, 4u) + 16u) = position;
    position = (uint32)P16(pres_cursor_object(base, 0u) + 20u) - 12u;
    P32(pres_cursor_object(base, 4u) + 20u) = position;
    position = P32(pres_cursor_object(base, 4u) + 16u) - 12u;
    position = (uint32)P16(pres_cursor_object(base, 3u) + 16u) - position;
    P32(0x800AD888u + (base + 4u) * 68u) = position;
    for (index = 0u; index < 5u; ++index)
    {
        object = pres_cursor_object(base, index);
        P32(object + 32u) = P32(object + 16u);
        object = pres_cursor_object(base, index);
        P32(object + 36u) = P32(object + 20u);
    }
}

static uint32 pres_scale_reciprocal(uint32 value, uint32 pc)
{
    if (!value)
    {
        fprintf(stderr, "PRES division BREAK 7 at %08X\n", pc);
        abort();
    }
    return (uint32)(1048576 / (sint32)value);
}

void pres_8009CD44(void)
{
    uint32 count = P32(0x800AD830u), index, entry, object, data, left, right, candidate;
    P32(0x800ABB38u) = 0u;
    for (index = 0u; (sint32)index < (sint32)count; ++index)
    {
        entry = 0x800AD860u + index * 68u;
        P32(entry + 32u) = 0u;
        P32(entry + 36u) = 0u;
        P32(entry + 16u) = P32(P32(entry) + 12u);
        object = P32(entry);
        if (P32(object + 16u) & 0xFF000000u)
            continue;
        if (P32(object + 20u) & 0xFF000000u)
            continue;
        data = P32(object + 44u);
        left = pres_scale_reciprocal(P32(data + 16u), 0x8009CDF4u);
        right = pres_scale_reciprocal(P32(data + 20u), 0x8009CE28u);
        if ((sint32)left < (sint32)right)
            left = right;
        candidate = ((P32(object + 12u) + 2u) << 8) + left;
        if ((sint32)P32(0x800ABB38u) < (sint32)candidate)
            P32(0x800ABB38u) = candidate;
    }
}

void pres_8009C120(uint32 selected)
{
    uint32 buttons = pres_menu_buttons();
    uint32 direction = (buttons & 0x20u) ? 1u : ((buttons & 0x80u) ? 0xFFFFFFFFu : 0u);
    uint32 index, object, pointer, value, array, controller;
    for (index = 0u; (sint32)index < (sint32)P32(0x800AD6C0u); ++index)
    {
        object = P32(0x800AD6C8u + index * 4u);
        if (P32(object + 40u) != selected)
            continue;
        pointer = P32(object + 44u);
        value = P32(pointer);
        if (direction == 0xFFFFFFFFu && (sint32)P32(object + 32u) >= (sint32)value)
            continue;
        if (direction == 1u && (sint32)value >= (sint32)P32(object + 36u))
            continue;
        P32(pointer) = value + direction;
        object = P32(0x800AD6C8u + index * 4u);
        value = P32(P32(object + 44u));
        array = P32(object + 28u);
        controller = P32(0x80059748u);
        P32(object + (controller << 2) + 8u) = P32(array + (value << 2));
    }
}

void pres_8009FE34(void)
{
    uint32 state = P32(0x800ABB58u), object, next;
    if (!state)
    {
        object = P32(P32(0x800AD568u + (P32(0x800AD668u) << 2)));
        object = P32(object + 4u);
        if (object == 0x800A25D4u || object == 0x800A2658u)
        {
            if (object == 0x800A25D4u)
                P32(0x800AD678u) = 2u;
            else
                P32(0x800AD678u) = 0u;
            P32(0x800ABB80u) = 17u;
            pres_8009EF04(17u);
            P32(0x800ABB30u) = 0xFFFFFFFFu;
            P32(0x800ABB68u) = object == 0x800A25D4u ? 4u : 6u;
            P32(0x800ABB58u) = object == 0x800A25D4u ? 1u : 2u;
        }
        else if (object == 0x800A2770u || object == 0x800A26D4u)
        {
            next = object == 0x800A2770u ? 2u : 11u;
            P32(0x800ABB80u) = next;
            pres_8009EF04(next);
            P32(0x800ABB30u) = 0xFFFFFFFFu;
            P32(0x800ABB68u) = 0u;
            P32(0x800ABB58u) = object == 0x800A2770u ? 4u : 3u;
        }
    }
    else if (state == 4u)
    {
        if (P32(0x800ABB20u) == 7u)
        {
            P32(0x8005973Cu) = 1u;
            P32(0x800ABBB0u) = 1u;
        }
        else
        {
            next = P32(0x800ABB30u);
            P32(0x800ABB80u) = next;
            pres_8009EF04(next);
            next = P32(0x800ABB80u);
            P32(0x800ABB30u) = 0xFFFFFFFFu;
            if (!next)
                P32(0x800ABB58u) = 0u;
        }
    }
    else if (state == 1u)
    {
        P32(0x8005973Cu) = 1u;
        pres_8009F5D0();
    }
    else if (state == 2u)
    {
        P32(0x8005973Cu) = 2u;
        pres_8009F948();
    }
    else if (state == 3u)
    {
        P32(0x8005973Cu) = 1u;
        pres_8009F1F0();
    }
}

void pres_800A0080(uint32 menu)
{
    uint32 object, callback, descriptor;
    if (P32(0x800ABB30u) != 0xFFFFFFFFu || P32(0x800ABB20u) == 7u)
    {
        if (!P32(0x800ABB48u) && !P32(0x800ABB38u))
        {
            uint32 index, selection;
            pres_8009CD44();
            index = P32(0x800ABB80u);
            selection = P32(0x800AD668u);
            P32(0x800ABB48u) = 1u;
            P8(0x80078A78u + index) = (uint8)selection;
        }
        else if (P32(0x800ABB48u) == 1u && !P32(0x800ABB38u))
            pres_8009FE34();
    }
    else if (!P32(0x800ABB38u))
    {
        if ((sint32)P32(0x800AD6C0u) > 0)
        {
            object = P32(P32(0x800AD568u + (P32(0x800AD668u) << 2)));
            pres_8009C120(P32(object + 4u));
        }
        if (P32(0x800AD688u))
        {
            object = P32(P32(0x800AD568u + (P32(0x800AD668u) << 2)));
            descriptor = P32(0x800A1D40u + (menu << 2));
            object = P32(object + 4u);
            callback = P32(descriptor + 4u);
        }
        else
        {
            descriptor = P32(0x800A1D40u + (menu << 2));
            callback = P32(descriptor + 4u);
            object = 0u;
        }
        pres_dispatch_callback1(callback, object);
    }
    pres_8009CEA8();
}

static uint32 pres_truncate_fixed(uint32 value)
{
    if ((sint32)value < 0)
        value += 4095u;
    return (uint32)((sint32)value >> 12);
}

static void pres_color_pulse(uint32 entry, sint32 both_channels)
{
    uint32 timer = (uint32)P16(entry + 52u) - 1u;
    uint32 level, amplitude;
    P16(entry + 52u) = (uint16)timer;
    if ((sint16)timer != -1)
        return;
    level = (uint32)(sint32)(sint16)P16(entry + 50u);
    P16(entry + 52u) = 0u;
    if ((sint32)level > 0)
    {
        P16(entry + 50u) = (uint16)(level + 256u);
        if ((sint16)P16(entry + 50u) > 4096)
            P16(entry + 50u) = 4096u;
        amplitude = (uint32)(sint32)(sint16)P16(entry + 50u);
    }
    else if ((sint32)level < 0)
    {
        P16(entry + 50u) = (uint16)(level - 256u);
        if ((sint16)P16(entry + 50u) < -4096)
            P16(entry + 50u) = (uint16)-4096;
        amplitude = (uint32)(sint32)(sint16)P16(entry + 50u) + 4096u;
    }
    else
        amplitude = 0u;
    if (both_channels)
    {
        P8(entry + 56u) = (uint8)pres_truncate_fixed(amplitude * 96u);
        P8(entry + 57u) = (uint8)(pres_truncate_fixed(amplitude * 207u) + 48u);
    }
    else
        P8(entry + 56u) = (uint8)(pres_truncate_fixed(amplitude * 207u) + 48u);
}

void pres_800A03CC(uint32 entry)
{
    uint32 flags = P16(entry + 48u), countdown = flags & 0x7FFFu;
    if (countdown)
    {
        uint32 preserved = flags & 0xFFFF8000u;
        --countdown;
        if (!countdown)
            flags = (preserved ^ 0xFFFF8000u) | 24u;
        else
            flags = countdown | preserved;
        P16(entry + 48u) = (uint16)flags;
    }
    if (P16(P32(entry) + 2u) & 1024u)
        pres_color_pulse(entry, 1);
    if (P16(P32(entry) + 2u) & 2048u)
        pres_color_pulse(entry, 0);
}

static uint32 pres_position_alignment(uint32 encoded, uint32 extent)
{
    uint32 position = (uint32)(sint32)(sint16)encoded;
    if (encoded & 0x10000u)
        return position - extent + 1u;
    if (encoded & 0x20000u)
        return position - pres_signed_half(extent);
    return position;
}

static uint32 pres_spline_position(uint32 entry, uint32 curve, uint32 source, uint32 target, uint32 axis)
{
    uint32 phase = P32(entry + 32u + axis * 4u);
    uint32 square = pres_truncate_fixed(phase * phase);
    uint32 cubic = pres_truncate_fixed(square * phase);
    uint32 first = P32(curve + axis * 8u), second = P32(curve + axis * 8u + 4u);
    uint32 spline = ((square * 3u - cubic * 2u) << 12) + (cubic - square * 2u + phase) * first + (cubic - square) * second;
    uint32 extent = P32(entry + 40u + axis * 4u);
    uint32 start = pres_position_alignment(source, extent);
    uint32 end = pres_position_alignment(target, P32(entry + 40u + axis * 4u));
    return start + pres_truncate_fixed((end - start) * pres_truncate_fixed(spline));
}

void pres_8009CEA8(void)
{
    uint32 index, entry, object, mode, curve = 0u, source_x = 0u, source_y = 0u, target_x = 0u, target_y = 0u;
    uint32 encoded, linked, position, timer, phase;
    if (!P32(0x800AD830u))
        return;
    if (P32(0x800AD688u))
        pres_8009B5F8();
    timer = P32(0x800ABB38u) - 256u;
    P32(0x800ABB38u) = timer;
    if ((sint32)timer < 0)
        P32(0x800ABB38u) = 0u;
    for (index = 0u; (sint32)index < (sint32)P32(0x800AD830u); ++index)
    {
        entry = 0x800AD860u + index * 68u;
        pres_800A03CC(entry);
        mode = P32(0x800ABB48u);
        object = P32(entry);
        if (!mode)
        {
            curve = P32(object + 40u);
            source_x = P32(object + 24u);
            source_y = P32(object + 28u);
            target_x = P32(object + 16u);
            target_y = P32(object + 20u);
        }
        else if (mode == 1u)
        {
            curve = P32(object + 44u);
            source_x = P32(object + 16u);
            source_y = P32(object + 20u);
            target_x = P32(object + 32u);
            target_y = P32(object + 36u);
        }
        else
        {
            /* Original carries uninitialized incoming S2 through S6 for unsupported phases */
            fprintf(stderr, "PRES 8009CEA8 unsupported animation phase %u\n", mode);
            abort();
        }
        encoded = P32(P32(entry) + 16u);
        if (encoded & 0x40000000u)
        {
            position = P32(entry - 68u + 20u) + P32(entry - 68u + 40u) - 1u;
            P32(entry + 20u) = position;
            position += (uint32)(sint32)(sint16)P16(P32(entry) + 16u);
            P32(entry + 20u) = position;
        }
        else if (encoded & 0x20000000u)
        {
            position = P32(entry - 68u + 20u) - P32(entry + 40u) + 1u;
            P32(entry + 20u) = position;
            P32(entry + 20u) = position + (uint32)(sint32)(sint16)P16(P32(entry) + 16u);
        }
        else if ((sint32)encoded < 0)
        {
            position = P32(entry - 68u + 20u);
            P32(entry + 20u) = position;
            P32(entry + 20u) = position + (uint32)(sint32)(sint16)P16(P32(entry) + 16u);
        }
        else
        {
            linked = P32(entry + 4u);
            if (!linked)
                P32(entry + 20u) = pres_spline_position(entry, curve, source_x, target_x, 0u);
            else
            {
                position = (uint32)(sint32)(sint16)(P32(linked + 20u) + encoded);
                P32(entry + 20u) = position;
                encoded = P32(P32(entry) + 16u);
                if (encoded & 0x10000u)
                    P32(entry + 20u) = position - 1u - P32(entry + 40u);
                else if (encoded & 0x20000u)
                    P32(entry + 20u) = position - pres_signed_half(P32(entry + 40u));
            }
        }
        encoded = P32(P32(entry) + 20u);
        if (encoded & 0x20000000u)
        {
            position = P32(entry - 68u + 24u) - P32(entry + 44u) + 1u;
            P32(entry + 24u) = position;
            P32(entry + 24u) = position + (uint32)(sint32)(sint16)P16(P32(entry) + 20u);
        }
        else if (encoded & 0x40000000u)
        {
            position = P32(entry - 68u + 24u) + P32(entry - 68u + 44u) - 1u;
            P32(entry + 24u) = position;
            P32(entry + 24u) = position + (uint32)(sint32)(sint16)P16(P32(entry) + 20u);
        }
        else if ((sint32)encoded < 0)
        {
            position = P32(entry - 68u + 24u);
            P32(entry + 24u) = position;
            P32(entry + 24u) = position + (uint32)(sint32)(sint16)P16(P32(entry) + 20u);
        }
        else
        {
            linked = P32(entry + 4u);
            if (!linked)
                P32(entry + 24u) = pres_spline_position(entry, curve, source_y, target_y, 1u);
            else
            {
                position = (uint32)(sint32)(sint16)(P32(linked + 24u) + encoded);
                P32(entry + 24u) = position;
                encoded = P32(P32(entry) + 20u);
                if (encoded & 0x10000u)
                    P32(entry + 24u) = position - 1u - P32(entry + 44u);
                else if (encoded & 0x20000u)
                    P32(entry + 24u) = position - pres_signed_half(P32(entry + 44u));
            }
        }
        timer = P32(entry + 16u) - 1u;
        P32(entry + 16u) = timer;
        if ((sint32)timer < 0)
        {
            P32(entry + 16u) = 0u;
            phase = P32(entry + 32u) + P32(curve + 16u);
            P32(entry + 32u) = phase;
            if ((sint32)phase > 4096)
                P32(entry + 32u) = 4096u;
            phase = P32(entry + 36u) + P32(curve + 20u);
            P32(entry + 36u) = phase;
            if ((sint32)phase > 4096)
                P32(entry + 36u) = 4096u;
        }
    }
}

static void pres_link_primitive(uint32 primitive, uint32 slot)
{
    uint32 old_tag = P32(primitive), old_head = P32(slot);
    P32(primitive) = (old_tag & 0xFF000000u) | (old_head & 0x00FFFFFFu);
    old_head = P32(slot);
    P32(slot) = (old_head & 0xFF000000u) | (primitive & 0x00FFFFFFu);
}

static uint32 pres_render_packet(uint32 entry, uint32 stride)
{
    return P32(entry + 12u) + P32(0x80065128u) * stride;
}

static uint32 pres_ordering_slot(uint32 offset)
{
    return P32(P32(0x80065118u) + 112u) + offset;
}

static void pres_render_model(uint32 entry)
{
    MATRIX matrix;
    sint32 camera[3], target[3];
    uint32 transform, data, ambient, index, mode;
    memcpy(&matrix, psx_addr(0x8008B638u, sizeof(matrix)), sizeof(matrix));
    P32(P32(0x800ABB10u) + 16u) = P32(entry + 20u) << 8;
    P32(P32(0x800ABB10u) + 20u) = P32(entry + 24u) << 8;
    P32(P32(0x800ABB10u) + 24u) = P32(entry + 28u) << 8;
    transform = P32(0x800ABB10u);
    P32(0x80066D78u) = P32(transform + 16u);
    P32(0x80066D7Cu) = P32(transform + 20u);
    P32(0x80066D80u) = P32(transform + 24u);
    target[0] = 0;
    target[1] = (sint32)P32(transform + 20u);
    target[2] = (sint32)P32(transform + 24u);
    data = P32(P32(entry) + 4u);
    camera[0] = (sint32)P32(data + 16u);
    data = P32(P32(entry) + 4u);
    camera[1] = (sint32)P32(data + 20u);
    data = P32(P32(entry) + 4u);
    camera[2] = (sint32)P32(data + 24u);
    sub_8003C034(camera);
    sub_8003C4B0(target, 2048u);
    for (index = 0u; index < 3u; ++index)
    {
        data = P32(P32(entry) + 4u);
        P16(entry + 60u + index * 2u) = (uint16)((uint32)P16(entry + 60u + index * 2u) + P32(data + 4u + index * 4u));
    }
    sub_8003C960((sint16 *)matrix.m, (const sint16 *)psx_addr(entry + 60u, 6u));
    memcpy(psx_addr(0x80066DA8u, sizeof(matrix)), &matrix, sizeof(matrix));
    for (index = 0u; index < 3u; ++index)
    {
        data = P32(P32(entry) + 4u);
        ambient = P32(0x800ABC14u);
        P8(ambient + 12u + index) = P8(data + 44u + index);
    }
    for (index = 0u; index < 3u; ++index)
    {
        data = P32(P32(entry) + 4u);
        ambient = P32(0x800ABC14u);
        P8(ambient + 16u + index) = P8(data + 40u + index);
    }
    data = P32(P32(entry) + 4u);
    ambient = P32(0x800ABC14u);
    P16(ambient + 4u) = (uint16)P32(data + 28u);
    data = P32(P32(entry) + 4u);
    P16(ambient + 6u) = (uint16)P32(data + 32u);
    data = P32(P32(entry) + 4u);
    P16(ambient + 8u) = (uint16)P32(data + 36u);
    sub_8003C0B4(P8(ambient + 16u), P8(ambient + 17u), P8(ambient + 18u));
    ambient = P32(0x800ABC14u);
    sub_8003C0DC(0u, (const sint16 *)psx_addr(ambient + 4u, 6u), P8(ambient + 12u), P8(ambient + 13u), P8(ambient + 14u));
    ambient = P32(0x800ABC14u);
    sub_8003C0DC(1u, (const sint16 *)psx_addr(ambient + 4u, 6u), 0u, 0u, 0u);
    ambient = P32(0x800ABC14u);
    sub_8003C0DC(2u, (const sint16 *)psx_addr(ambient + 4u, 6u), 0u, 0u, 0u);
    sub_8003C164(&matrix);
    sub_8003CED8(&matrix, P32(0x800ABB10u));
    mode = P32(0x800ABB80u);
    if (mode == 4u || mode == 9u || mode == 16u)
    {
        /* The wrapper replaces the incoming lighting descriptor */
        pres_8008FC84(P32(0x800ABB10u), 0x80066D14u);
        sub_800416E0(P32(0x800ABB10u));
        sub_8001896C(0u);
    }
    else
        sub_800416E0(P32(0x800ABB10u));
}

void pres_8009DA08(void)
{
    uint32 x = P32(0x80064BF0u), y = P32(0x80064BF4u), count = P32(0x800AD830u);
    uint32 index, entry, object, type, packet, data, component, base, height, row, channel;
    uint32 value, product, source, destination, color[3], style;
    PSX_RECT rectangle;
    P16(0x80081A9Cu) = (uint16)x;
    P16(0x80081B10u) = (uint16)x;
    P16(0x80081A9Eu) = (uint16)y;
    P16(0x80081B12u) = (uint16)y;
    if ((sint32)count <= 0)
        return;
    for (index = 0u; (sint32)index < (sint32)P32(0x800AD830u); ++index)
    {
        entry = 0x800AD860u + index * 68u;
        if ((P16(entry + 48u) & 0x8000u) && index != P32(0x800AD564u) + 4u)
            continue;
        x = P32(entry + 20u);
        if ((sint32)(x + P32(entry + 40u)) < 0 || (sint32)x >= 513)
            continue;
        y = P32(entry + 24u);
        if ((sint32)(y + P32(entry + 44u)) < 0 || (sint32)y >= 241)
            continue;
        base = P32(0x800AD564u);
        if (index == base || index == base + 1u || index == base + 2u || index == base + 3u || index == base + 4u)
            if (!P32(0x800AD850u))
                continue;
        object = P32(entry);
        type = (uint32)(sint32)(sint16)P16(object);
        switch (type)
        {
            case 0u:
                data = P32(P32(P32(entry) + 4u));
                y = P32(entry + 24u) + (uint32)(sint32)(sint8)P8(0x800A21D8u + (data & 0x7Fu) * 144u);
                if (P32(0x800AD568u + (P32(0x800AD668u) << 2)) == entry && P32(0x800AD884u + P32(0x800AD564u) * 68u) == 4096u && (sint16)P16(entry + 54u) == 0)
                    style = 1u;
                else if ((sint16)P16(entry + 54u) == 1)
                    style = 7u;
                else
                    style = P32(P32(P32(entry) + 4u) + 4u);
                pres_80093910(style);
                data = P32(P32(entry) + 4u);
                pres_80093008(P32(data), (sint32)P32(entry + 20u), (sint32)y, P32(data + (P32(0x80059748u) << 2) + 8u));
                break;
            case 1u:
                y = P32(entry + 24u);
                packet = pres_render_packet(entry, 40u);
                P16(packet + 8u) = (uint16)P32(entry + 20u);
                P16(pres_render_packet(entry, 40u) + 10u) = (uint16)y;
                P16(pres_render_packet(entry, 40u) + 16u) = (uint16)(P32(entry + 20u) - 1u + P32(entry + 40u));
                P16(pres_render_packet(entry, 40u) + 18u) = (uint16)y;
                P16(pres_render_packet(entry, 40u) + 24u) = (uint16)P32(entry + 20u);
                P16(pres_render_packet(entry, 40u) + 26u) = (uint16)(y - 1u + P32(entry + 44u));
                P16(pres_render_packet(entry, 40u) + 32u) = (uint16)(P32(entry + 20u) - 1u + P32(entry + 40u));
                P16(pres_render_packet(entry, 40u) + 34u) = (uint16)(y - 1u + P32(entry + 44u));
                for (component = 0u; component < 3u; ++component)
                    P8(pres_render_packet(entry, 40u) + 4u + component) = P8(entry + 56u + component);
                pres_link_primitive(pres_render_packet(entry, 40u), pres_ordering_slot((P16(P32(entry) + 2u) & 0x2000u) ? 1024u : 0u));
                break;
            case 2u:
                pres_render_model(entry);
                break;
            case 3u:
                y = P32(entry + 24u);
                height = P32(entry + 44u);
                for (row = 0u; row < 4u; ++row)
                    for (component = 0u; component < 3u; ++component)
                    {
                        data = P32(P32(entry) + 4u);
                        P8(pres_render_packet(entry, 52u) + 4u + row * 12u + component) = P8(data + 16u + row * 4u + component);
                    }
                P16(pres_render_packet(entry, 52u) + 8u) = (uint16)P32(entry + 20u);
                P16(pres_render_packet(entry, 52u) + 10u) = (uint16)y;
                data = P32(P32(entry) + 4u);
                P16(pres_render_packet(entry, 52u) + 20u) = (uint16)(P32(entry + 20u) + P32(entry + 40u) + P32(data + 8u));
                P16(pres_render_packet(entry, 52u) + 22u) = (uint16)y;
                data = P32(P32(entry) + 4u);
                P16(pres_render_packet(entry, 52u) + 32u) = (uint16)(P32(entry + 20u) + P32(data + 12u));
                P16(pres_render_packet(entry, 52u) + 34u) = (uint16)(y + height);
                P16(pres_render_packet(entry, 52u) + 44u) = (uint16)(P32(entry + 20u) + P32(entry + 40u));
                P16(pres_render_packet(entry, 52u) + 46u) = (uint16)(y + height);
                pres_link_primitive(pres_render_packet(entry, 52u), pres_ordering_slot(0u));
                break;
            case 4u:
                /* Compound objects are expanded during construction and have no render case */
                break;
            case 5u:
                data = P32(P32(entry) + 4u);
                rectangle.w = (sint16)P16(data + 4u);
                rectangle.x = (sint16)P32(entry + 20u);
                data = P32(P32(entry) + 4u);
                rectangle.h = (sint16)P16(data + 6u);
                base = P32(0x80065128u) << 8;
                rectangle.y = (sint16)(base + P32(entry + 24u));
                data = P32(P32(entry) + 4u);
                source = P32(0x800ABBB8u) + P32(data) + 20u;
                value = (uint32)(sint32)rectangle.y + (uint32)(sint32)rectangle.h;
                if ((sint32)value < (sint32)base || (sint32)rectangle.y >= (sint32)(base + 240u))
                    break;
                if ((sint32)value >= (sint32)(base + 240u))
                    rectangle.h = (sint16)(base + 240u - (uint32)(sint32)rectangle.y);
                else if ((sint32)rectangle.y < (sint32)base)
                {
                    rectangle.h = (sint16)((uint32)(sint32)rectangle.y + (uint32)(sint32)rectangle.h - base);
                    rectangle.y = (sint16)base;
                    data = P32(P32(entry) + 4u);
                    source += ((uint32)(sint32)(sint16)P16(data + 6u) - (uint32)(sint32)rectangle.h) << 9;
                }
                LoadImagePSX(&rectangle, (uint32 *)psx_addr(source, 1u));
                break;
            case 6u:
                base = P32(0x800AD564u);
                if (P32(0x800AD880u + base * 68u) != 4096u && P32(0x800AD884u + base * 68u) != 4096u)
                    break;
                pres_8009D56C();
                height = P32(entry + 44u);
                y = P32(entry + 24u);
                for (row = 0u; (sint32)row < (sint32)height; ++row)
                {
                    for (channel = 0u; channel < 2u; ++channel)
                    {
                        for (component = 0u; component < 3u; ++component)
                        {
                            source = P8(0x80078AE0u + channel * 4u + component);
                            destination = P8(0x80078AE8u + channel * 4u + component);
                            product = (destination - source) * row;
                            color[component] = source + (uint32)((sint32)product / (sint32)height);
                        }
                        for (component = 0u; component < 3u; ++component)
                        {
                            packet = P32(0x800AD7B0u) + (row * 2u + P32(0x80065128u)) * 20u;
                            P8(packet + 4u + channel * 8u + component) = (uint8)color[component];
                        }
                    }
                    packet = P32(0x800AD7B0u) + (row * 2u + P32(0x80065128u)) * 20u;
                    P16(packet + 8u) = (uint16)P32(entry + 20u);
                    P16(packet + 10u) = (uint16)(y + row);
                    P16(packet + 18u) = (uint16)(y + row);
                    P16(packet + 16u) = (uint16)(P32(entry + 20u) + P32(entry + 40u));
                    pres_link_primitive(packet, pres_ordering_slot(0u));
                }
                break;
            case 7u:
                /* Original 94604 draws the background at 256,120 */
                pres_80094604(256, 120);
                break;
            default:
                break;
        }
    }
}
