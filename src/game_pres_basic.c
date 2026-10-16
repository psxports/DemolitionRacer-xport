#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern void pres_80093910(uint32 style);
extern void pres_80093008(uint32 font, sint32 x, sint32 y, uint32 text);
extern void dr_pres_text_native(uint32 font, sint32 x, sint32 y, const char *text);

/* These initializers are original JR RA and NOP leaves */
void pres_80094144(void)
{
}

void pres_80094238(void)
{
}

void pres_8009748C(void)
{
}

void pres_800983F4(void)
{
}

void pres_800991DC(void)
{
}

void pres_8009B270(void)
{
}

void pres_800983FC(uint32 object)
{
}

void pres_8009414C(uint32 object)
{
    uint32 language;
    if (!(r_u16(0x80065524u) & 0x4000u))
        return;
    w_u32(0x800ABB20u, 1u);
    if (object == 0x800A31E8u)
        language = 0u;
    else if (object == 0x800A3220u)
        language = 1u;
    else if (object == 0x800A325Cu)
        language = 2u;
    else if (object == 0x800A3294u)
        language = 3u;
    else if (object == 0x800A32CCu)
        language = 4u;
    else
        language = ~0u;
    if (language != ~0u)
    {
        w_u32(0x80064B8Cu, language);
        w_u32(0x80059748u, language);
    }
    sub_8001D01C(0u);
    w_u32(0x800ABB58u, 4u);
    w_u32(0x800ABB30u, 0u);
    w_u32(0x80064BE4u, 1u);
}

void pres_80094240(uint32 object)
{
    uint32 buttons = r_u16(0x80065524u), menu;
    if (buttons & 0x1000u)
    {
        w_u32(0x800ABB20u, 3u);
        w_u32(0x800ABB30u, 0u);
        sub_8001D01C(3u);
        return;
    }
    if (!(buttons & 0x4008u) || r_u32(0x800ABB48u))
        return;
    w_u32(0x800ABB20u, 1u);
    if (object == 0x800A35A0u)
        menu = 5u;
    else if (object == 0x800A3548u)
        menu = 6u;
    else if (object == 0x800A34C8u)
        menu = 19u;
    else if (object == 0x800A364Cu)
        menu = 8u;
    else if (object == 0x800A36E0u)
        menu = 20u;
    else if (object == 0x800A3798u)
        menu = 18u;
    else
        menu = ~0u;
    if (menu != ~0u)
        w_u32(0x800ABB30u, menu);
    sub_8001D01C(0u);
}

void pres_80097494(uint32 object)
{
    if (r_u16(0x80065524u) & 0x1000u)
    {
        sub_8001D01C(3u);
        w_u32(0x800ABB20u, 3u);
        w_u32(0x800ABB30u, 2u);
    }
    if (r_u16(0x80065524u) & 0x4000u)
    {
        sub_8001D01C(0u);
        if (object == 0x800A5674u)
        {
            w_u32(0x80064BE8u, 1u);
            w_u32(0x800ABB48u, 1u);
            w_u32(0x800ABB20u, 7u);
        }
    }
}

void pres_800991E4(uint32 object)
{
    uint32 buttons = r_u16(0x80065524u);
    if (buttons & 0x1000u)
    {
        sub_8001D01C(3u);
        w_u32(0x800ABB30u, 12u);
        w_u32(0x800ABB20u, 3u);
    }
    else if (buttons & 0x4008u)
    {
        sub_8001D01C(0u);
        w_u32(0x800ABB20u, 7u);
        w_u32(0x800ABB30u, 3u);
        sub_8001D01C(1u);
    }
}

extern uint32 pres_8009017C(void);

void pres_8009B278(uint32 object)
{
    if (pres_8009017C())
        w_u32(0x800ABB30u, 2u);
}

static uint32 pres_calibration_detect(uint32 port)
{
    uint32 waited = 0;
    for (;;)
    {
        uint32 state = PadGetStatePSX(port << 4);
        if (state == 6 || waited >= 31)
            break;
        VSync(0);
        ++waited;
    }
    if (waited >= 30)
        return 0;
    return PadInfoMode((sint32)(port << 4), 2, 0) != 0;
}

static void pres_calibration_initialize(void)
{
    uint32 index;
    w_u16(0x800AB730, 0);
    w_u16(0x8005A9AA, 0);
    w_u16(0x8005A9CE, 0);
    w_u16(0x8005A990, pres_calibration_detect(0));
    w_u16(0x8005A9B4, pres_calibration_detect(1));
    for (index = 0; index < 36; index += 16)
    {
        uint32 a = r_u32(0x8005A990 + index);
        if (index == 32)
            w_u32(0x80078AF0 + index, a);
        else
        {
            uint32 b = r_u32(0x8005A994 + index);
            uint32 c = r_u32(0x8005A998 + index);
            uint32 d = r_u32(0x8005A99C + index);
            w_u32(0x80078AF0 + index, a);
            w_u32(0x80078AF4 + index, b);
            w_u32(0x80078AF8 + index, c);
            w_u32(0x80078AFC + index, d);
        }
    }
    for (index = 0; index < 36; index += 16)
    {
        uint32 a = r_u32(0x8005A9B4 + index);
        if (index == 32)
            w_u32(0x80078B14 + index, a);
        else
        {
            uint32 b = r_u32(0x8005A9B8 + index);
            uint32 c = r_u32(0x8005A9BC + index);
            uint32 d = r_u32(0x8005A9C0 + index);
            w_u32(0x80078B14 + index, a);
            w_u32(0x80078B18 + index, b);
            w_u32(0x80078B1C + index, c);
            w_u32(0x80078B20 + index, d);
        }
    }
}

void pres_8008B9C8(uint32 port)
{
    uint32 type_address = 0x80065538 + port * 4;
    uint32 record = 0x8005A990 + port * 36;
    uint32 kind = r_u32(type_address), detected;
    if ((kind == 0x5000 || kind == 0x7000 || kind == 0x4000 || kind == 0x2000) && kind != r_u32(record + 12))
    {
        draft_agent1_missing_diagnostic("%s", (const char *)psx_addr(0x800A1C34, 0));
        kind = r_u32(type_address);
        w_u16(record + 18, 4);
        w_u16(record + 20, 4);
        w_u16(record + 22, 1);
        w_u16(record + 28, 128);
        w_u16(record + 16, 0);
        w_u16(record + 26, 0);
        w_u16(record + 30, 0);
        w_u16(record + 32, 255);
        w_u32(record + 12, kind);
    }
    kind = r_u32(type_address);
    if (kind == r_u32(record + 8))
        return;
    w_u32(record + 4, kind);
    detected = pres_calibration_detect(port);
    kind = r_u32(type_address);
    w_u16(record, detected);
    w_u32(record + 8, kind);
    kind = r_u32(record + 4);
    if (kind != 0x2000 && kind != 0x7000 && kind != 0x5000)
        w_u16(record + 26, 0);
}

void pres_8008D274(uint32 port)
{
    sint32 limit = (sint16)port == 0 ? 2 : 1;
    uint32 record = 0x8005A990 + (uint32)(sint32)(sint16)port * 36;
    uint32 buttons = 0x80065524 + (uint32)(sint32)(sint16)port * 2;
    if (!r_s16(record))
        --limit;
    if ((r_u16(buttons) & 0x10) && r_s16(record + 24) > 0)
    {
        w_u16(record + 24, r_u16(record + 24) - 1);
        sub_8001D01C(2);
    }
    if ((r_u16(buttons) & 0x40) && r_s16(record + 24) < limit)
    {
        w_u16(record + 24, r_u16(record + 24) + 1);
        sub_8001D01C(2);
    }
    if (r_s16(record + 24) < 0)
        w_u16(record + 24, 0);
    if (r_s16(record + 24) > limit)
        w_u16(record + 24, (uint32)limit);
    if (r_u16(buttons) & 0x20)
    {
        if (r_s16(record) == 1)
        {
            if (!r_s16(record + 24) && r_s16(record + 22) != 1)
            {
                w_u16(record + 22, r_u16(record + 22) + 1);
                sub_8001D01C(2);
            }
            if (r_s16(record + 24) == 1 && r_s16(record + 16) < 4)
            {
                w_u16(record + 16, r_u16(record + 16) + 1);
                sub_8001D01C(2);
            }
        }
        else if (!r_s16(record + 24) && r_s16(record + 16) < 4)
        {
            w_u16(record + 16, r_u16(record + 16) + 1);
            sub_8001D01C(2);
        }
    }
    if (r_u16(buttons) & 0x80)
    {
        if (r_s16(record) == 1)
        {
            if (!r_s16(record + 24) && r_s16(record + 22))
            {
                w_u16(record + 22, r_u16(record + 22) - 1);
                sub_8001D01C(2);
            }
            if (r_s16(record + 24) == 1 && r_s16(record + 16) > 0)
            {
                w_u16(record + 16, r_u16(record + 16) - 1);
                sub_8001D01C(2);
            }
        }
        else if (!r_s16(record + 24) && r_s16(record + 16) > 0)
        {
            w_u16(record + 16, r_u16(record + 16) - 1);
            sub_8001D01C(2);
        }
    }
    if (r_s16(record + 24) == limit && (r_u16(buttons) & 0x4000))
        w_u16(0x800AB730, 1);
}

static uint32 pres_calibration_message(uint32 table)
{
    return r_u32(table + r_u32(0x80064B8C) * 4);
}

static uint32 pres_calibration_label(sint32 index)
{
    return index < 0 ? 0x800A1C28 : pres_calibration_message(0x800A0668 + (uint32)index * 20);
}

static sint32 pres_calibration_width(const char *text)
{
    uint32 width = 0, index;
    for (index = 0; (sint32)index < (sint32)(uint32)strlen(text); ++index)
        width += r_u8(0x800A21E0 + (((uint8)text[index] - 32) & 255));
    return (sint16)width;
}

static void pres_calibration_text_columns(uint32 first, uint32 second)
{
    w_u32(0x800ABBF4, second);
    w_u32(0x800ABBF8, first);
}

static void pres_calibration_digital_labels(uint32 port)
{
    uint32 record = 0x8005A990 + port * 36;
    sint32 row = 0, x = (sint32)(port * 224 + 40);
    char text[128];
    pres_80093910(0);
    if (r_s16(record) == 1)
    {
        pres_80093910(r_s16(record + 24) == 0);
        pres_80093008(0, x, row * 10 + 48, pres_calibration_message(0x800A07BC));
        ++row;
    }
    pres_80093910(r_s16(record + 24) == row);
    pres_80093008(0, x, row * 10 + 48, pres_calibration_message(0x800A07D0));
    ++row;
    if (!port)
    {
        pres_80093910(r_s16(record + 24) == row);
        pres_80093008(0, x, row * 10 + 48, pres_calibration_message(0x800A0C44));
        ++row;
    }
    row = 0;
    if (r_s16(record) == 1)
    {
        pres_80093910(r_s16(record + 24) == 0);
        pres_80093008(0, (sint32)(port * 224 + 220), 48, pres_calibration_label(r_s16(record + 22) + 20));
        ++row;
    }
    pres_80093910(r_s16(record + 24) == row);
    snprintf(text, sizeof(text), (const char *)psx_addr(0x800A1C74, 1), r_s16(record + 16) + 1);
    dr_pres_text_native(0, (sint32)(port * 224 + 220), row * 10 + 48, text);
    pres_80093910(2);
}

static void pres_calibration_digital(uint32 port)
{
    uint32 record = 0x8005A990 + port * 36, index;
    char text[128];
    pres_8008D274(port);
    pres_calibration_digital_labels(port);
    pres_calibration_text_columns(0, port * 224 + 116);
    for (index = 0; index < 8; ++index)
    {
        sint32 length = (sint16)snprintf(text, sizeof(text), (const char *)psx_addr(0x800A1C70, 1));
        sint32 mapped;
        snprintf(text + length, sizeof(text) - (size_t)length, (const char *)psx_addr(pres_calibration_label((sint32)index + 2), 1));
        dr_pres_text_native(0, (sint32)(port * 224 + 40), (sint32)(index * 9 + 108), text);
        mapped = r_u16(0x8005A850 + index * 10 + (uint32)(sint32)r_s16(record + 16) * 2) - 140;
        pres_80093008(0, (sint32)(port * 224 + 124), (sint32)(index * 9 + 108), pres_calibration_label(mapped));
    }
}

static void pres_calibration_center(uint32 port, sint32 y, const char *text)
{
    sint32 remaining = 256 - pres_calibration_width(text);
    sint32 half = (remaining + (sint32)((uint32)remaining >> 31)) >> 1;
    dr_pres_text_native(0, (sint32)(port * 256) + half, y, text);
}

static void pres_calibration_analog_input(uint32 port)
{
    uint32 record = 0x8005A990 + port * 36, buttons = 0x80065524 + port * 2;
    sint32 limit = port == 0 ? 4 : 3;
    if ((r_u16(buttons) & 0x10) && r_s16(record + 24) > 0)
    {
        w_u16(record + 24, r_u16(record + 24) - 1);
        sub_8001D01C(2);
    }
    if ((r_u16(buttons) & 0x40) && r_s16(record + 24) < limit)
    {
        w_u16(record + 24, r_u16(record + 24) + 1);
        sub_8001D01C(2);
    }
    if (r_s16(record + 24) < 0)
        w_u16(record + 24, 0);
    if (r_s16(record + 24) > limit)
        w_u16(record + 24, (uint32)limit);
    if (r_u16(buttons) & 0x20)
    {
        switch (r_s16(record + 24))
        {
            case 0:
                if (r_s16(record + 18) < 8)
                {
                    w_u16(record + 18, r_u16(record + 18) + 1);
                    sub_8001D01C(2);
                }
                break;
            case 1:
                if (r_s16(record + 20) < 8)
                {
                    w_u16(record + 20, r_u16(record + 20) + 1);
                    sub_8001D01C(2);
                }
                break;
            case 2:
                if (r_s16(record) == 1 && r_s16(record + 22) != 1)
                {
                    w_u16(record + 22, r_u16(record + 22) + 1);
                    sub_8001D01C(2);
                }
                break;
            case 3:
                if (r_s16(record + 16) < 4)
                {
                    sub_8001D01C(2);
                    w_u16(record + 16, r_u16(record + 16) + 1);
                }
                break;
        }
    }
    if (r_u16(buttons) & 0x80)
    {
        switch (r_s16(record + 24))
        {
            case 0:
                if (r_s16(record + 18) > 0)
                {
                    w_u16(record + 18, r_u16(record + 18) - 1);
                    sub_8001D01C(2);
                }
                break;
            case 1:
                if (r_s16(record + 20) > 0)
                {
                    w_u16(record + 20, r_u16(record + 20) - 1);
                    sub_8001D01C(2);
                }
                break;
            case 2:
                if (r_s16(record) == 1 && r_s16(record + 22))
                {
                    w_u16(record + 22, r_u16(record + 22) - 1);
                    sub_8001D01C(2);
                }
                break;
            case 3:
                if (r_s16(record + 16) > 0)
                {
                    w_u16(record + 16, r_u16(record + 16) - 1);
                    sub_8001D01C(2);
                }
                break;
        }
    }
    if (!r_s16(record) && r_s16(record + 24) == 2 && (r_u16(buttons) & 0x4000))
        w_u16(record + 26, 1);
    if (r_s16(record + 24) == limit && (r_u16(buttons) & 0x4000))
        w_u16(0x800AB730, 1);
}

static void pres_calibration_analog_labels(uint32 port)
{
    uint32 record = 0x8005A990 + port * 36;
    sint32 x = (sint32)(port * 224 + 40), right = (sint32)(port * 224 + 220);
    char text[128];
    pres_80093910(r_s16(record + 24) == 0);
    pres_80093008(0, x, 48, pres_calibration_message(0x800A0780));
    pres_80093910(r_s16(record + 24) == 1);
    pres_80093008(0, x, 58, pres_calibration_message(0x800A0794));
    pres_80093910(r_s16(record + 24) == 2);
    pres_80093008(0, x, 68, pres_calibration_message(r_s16(record) == 0 ? 0x800A07A8 : 0x800A07BC));
    pres_80093910(r_s16(record + 24) == 3);
    pres_80093008(0, x, 78, pres_calibration_message(0x800A07D0));
    if (!port)
    {
        pres_80093910(r_s16(record + 24) == 4);
        pres_80093008(0, x, 88, pres_calibration_message(0x800A0C44));
    }
    pres_80093910(r_s16(record + 24) == 0);
    snprintf(text, sizeof(text), (const char *)psx_addr(0x800A1C64, 1), r_s16(record + 18) + 1);
    dr_pres_text_native(0, right, 48, text);
    pres_80093910(r_s16(record + 24) == 1);
    snprintf(text, sizeof(text), (const char *)psx_addr(0x800A1C68, 1), r_s16(record + 20) + 1);
    dr_pres_text_native(0, right, 58, text);
    if (r_s16(record) == 1)
    {
        pres_80093910(r_s16(record + 24) == 2);
        pres_80093008(0, right, 68, pres_calibration_label(r_s16(record + 22) + 20));
    }
    pres_80093910(r_s16(record + 24) == 3);
    snprintf(text, sizeof(text), (const char *)psx_addr(0x800A1C6C, 1), r_s16(record + 16) + 1);
    dr_pres_text_native(0, right, 78, text);
    pres_80093910(0);
}

static void pres_calibration_analog(uint32 port)
{
    uint32 record = 0x8005A990 + port * 36, index, axis = 0x80065508 + port * 4;
    sint32 x = (sint32)(port * 224 + 40), center;
    sint32 stage = r_s16(record + 26);
    char text[128];
    if (stage == 0)
    {
        pres_calibration_analog_input(port);
        pres_calibration_analog_labels(port);
        pres_calibration_text_columns(0, port * 224 + 116);
        for (index = 0; index < 8; ++index)
        {
            sint32 length = (sint16)snprintf(text, sizeof(text), (const char *)psx_addr(0x800A1C60, 1));
            sint32 mapped;
            snprintf(text + length, sizeof(text) - (size_t)length, (const char *)psx_addr(pres_calibration_label((sint32)index + 2), 1));
            dr_pres_text_native(0, x, (sint32)(index * 9 + 108), text);
            mapped = r_s16(0x8005A940 + index * 10 + (uint32)(sint32)r_s16(record + 16) * 2) - 140;
            pres_80093008(0, x + 84, (sint32)(index * 9 + 108), pres_calibration_label(mapped));
        }
    }
    else if (stage == 1)
    {
        pres_80093008(0, x, 48, pres_calibration_message(0x800A0820));
        pres_80093008(0, x, 58, pres_calibration_message(0x800A0834));
        if (r_u16(0x80065524 + port * 2) & 8)
        {
            w_u16(record + 26, 2);
            w_u16(record + 28, r_u32(axis));
            center = r_s16(record + 28);
            if (center < 48)
                center = 48;
            else if (center > 208)
                center = 208;
            w_u16(record + 28, (uint32)center);
            w_u16(record + 32, r_u16(record + 28) + 32);
            w_u16(record + 30, r_u16(record + 28) - 32);
        }
    }
    else if (stage == 2)
    {
        pres_80093008(0, x, 48, pres_calibration_message(0x800A0848));
        pres_80093008(0, x, 58, pres_calibration_message(0x800A085C));
        pres_80093008(0, x, 68, pres_calibration_message(0x800A0870));
        if (r_s16(record + 32) < r_s32(axis))
            w_u16(record + 32, r_u32(axis));
        if (r_s32(axis) < r_s16(record + 30))
            w_u16(record + 30, r_u32(axis));
        if (r_u16(0x80065524 + port * 2) & 8)
            w_u16(record + 26, 0);
    }
}

static void pres_calibration_steering_input(uint32 port)
{
    uint32 record = 0x8005A990 + port * 36, buttons = 0x80065524 + port * 2;
    sint32 limit = port == 0 ? 4 : 3;
    if ((r_u16(buttons) & 0x10) && r_s16(record + 24) > 0)
    {
        w_u16(record + 24, r_u16(record + 24) - 1);
        sub_8001D01C(2);
    }
    if ((r_u16(buttons) & 0x40) && r_s16(record + 24) < limit)
    {
        w_u16(record + 24, r_u16(record + 24) + 1);
        sub_8001D01C(2);
    }
    if (r_s16(record + 24) < 0)
        w_u16(record + 24, 0);
    if (r_s16(record + 24) > limit)
        w_u16(record + 24, (uint32)limit);
    if (r_u16(buttons) & 0x20)
    {
        switch (r_s16(record + 24))
        {
            case 0:
                if (r_s16(record + 18) < 8)
                {
                    w_u16(record + 18, r_u16(record + 18) + 1);
                    sub_8001D01C(2);
                }
                break;
            case 1:
                if (r_s16(record + 20) < 8)
                {
                    w_u16(record + 20, r_u16(record + 20) + 1);
                    sub_8001D01C(2);
                }
                break;
            case 3:
                if (r_s16(record + 16) < 4)
                {
                    w_u16(record + 16, r_u16(record + 16) + 1);
                    sub_8001D01C(2);
                }
                break;
        }
    }
    if (r_u16(buttons) & 0x80)
    {
        switch (r_s16(record + 24))
        {
            case 0:
                if (r_s16(record + 18) > 0)
                {
                    w_u16(record + 18, r_u16(record + 18) - 1);
                    sub_8001D01C(2);
                }
                break;
            case 1:
                if (r_s16(record + 20) > 0)
                {
                    w_u16(record + 20, r_u16(record + 20) - 1);
                    sub_8001D01C(2);
                }
                break;
            case 3:
                if (r_s16(record + 16) > 0)
                {
                    w_u16(record + 16, r_u16(record + 16) - 1);
                    sub_8001D01C(2);
                }
                break;
        }
    }
    if (r_s16(record + 24) == 2 && (r_u16(buttons) & 0x4000))
        w_u16(record + 26, 1);
    if (r_s16(record + 24) == limit && (r_u16(buttons) & 0x4000))
        w_u16(0x800AB730, 1);
}

static void pres_calibration_steering_labels(uint32 port)
{
    uint32 record = 0x8005A990 + port * 36;
    sint32 x = (sint32)(port * 224 + 40), right = (sint32)(port * 224 + 220);
    char text[128];
    pres_80093910(r_s16(record + 24) == 0);
    pres_80093008(0, x, 48, pres_calibration_message(0x800A0780));
    pres_80093910(r_s16(record + 24) == 1);
    pres_80093008(0, x, 58, pres_calibration_message(0x800A0794));
    pres_80093910(r_s16(record + 24) == 2);
    pres_80093008(0, x, 68, pres_calibration_message(0x800A07A8));
    pres_80093910(r_s16(record + 24) == 3);
    pres_80093008(0, x, 78, pres_calibration_message(0x800A07D0));
    if (!port)
    {
        pres_80093910(r_s16(record + 24) == 4);
        pres_80093008(0, x, 88, pres_calibration_message(0x800A0C44));
    }
    pres_80093910(r_s16(record + 24) == 0);
    snprintf(text, sizeof(text), (const char *)psx_addr(0x800A1C7C, 1), r_s16(record + 18) + 1);
    dr_pres_text_native(0, right, 48, text);
    pres_80093910(r_s16(record + 24) == 1);
    snprintf(text, sizeof(text), (const char *)psx_addr(0x800A1C80, 1), r_s16(record + 20) + 1);
    dr_pres_text_native(0, right, 58, text);
    pres_80093910(r_s16(record + 24) == 3);
    snprintf(text, sizeof(text), (const char *)psx_addr(0x800A1C84, 1), r_s16(record + 16) + 1);
    dr_pres_text_native(0, right, 78, text);
    pres_80093910(0);
}

static void pres_calibration_steering(uint32 port)
{
    uint32 record = 0x8005A990 + port * 36, index, axis = 0x80065508 + port * 4;
    sint32 x = (sint32)(port * 224 + 40), center;
    sint32 stage = r_s16(record + 26);
    char text[128];
    if (stage == 0)
    {
        pres_calibration_steering_input(port);
        pres_calibration_steering_labels(port);
        pres_calibration_text_columns(0, port * 224 + 116);
        for (index = 0; index < 8; ++index)
        {
            sint32 length = (sint16)snprintf(text, sizeof(text), (const char *)psx_addr(0x800A1C78, 1));
            sint32 mapped;
            snprintf(text + length, sizeof(text) - (size_t)length, (const char *)psx_addr(pres_calibration_label((sint32)index + 2), 1));
            dr_pres_text_native(0, x, (sint32)(index * 9 + 108), text);
            mapped = r_s16(0x8005A8F0 + index * 10 + (uint32)(sint32)r_s16(record + 16) * 2) - 140;
            pres_80093008(0, x + 84, (sint32)(index * 9 + 108), pres_calibration_label(mapped));
        }
    }
    else if (stage == 1)
    {
        pres_80093008(0, x, 48, pres_calibration_message(0x800A0BE0));
        pres_80093008(0, x, 58, pres_calibration_message(0x800A0BF4));
        if (r_u16(0x80065524 + port * 2) & 8)
        {
            w_u16(record + 26, 2);
            w_u16(record + 28, r_u32(axis));
            center = r_s16(record + 28);
            if (center < 48)
                center = 48;
            else if (center > 208)
                center = 208;
            w_u16(record + 28, (uint32)center);
            w_u16(record + 32, r_u16(record + 28) + 32);
            w_u16(record + 30, r_u16(record + 28) - 32);
        }
    }
    else if (stage == 2)
    {
        pres_80093008(0, x, 48, pres_calibration_message(0x800A0C08));
        pres_80093008(0, x, 58, pres_calibration_message(0x800A0C1C));
        if (r_s16(record + 32) < r_s32(axis))
            w_u16(record + 32, r_u32(axis));
        if (r_s32(axis) < r_s16(record + 30))
            w_u16(record + 30, r_u32(axis));
        if (r_u16(0x80065524 + port * 2) & 8)
            w_u16(record + 26, 0);
    }
}

static void pres_calibration_render(void)
{
    uint32 port;
    char text[128];
    for (port = 0; port < 2; ++port)
    {
        sint32 length;
        uint32 kind;
        pres_80093910(3);
        length = (sint16)snprintf(text, sizeof(text), (const char *)psx_addr(pres_calibration_message(0x800A067C), 1));
        snprintf(text + length, sizeof(text) - (size_t)length, (const char *)psx_addr(0x800A1C5C, 1), port + 1);
        pres_calibration_center(port, 16, text);
        kind = r_u32(0x80065538 + port * 4);
        if (kind == 0x5000 || kind == 0x7000)
            pres_calibration_analog(port);
        else if (kind == 0x4000)
            pres_calibration_digital(port);
        else if (kind == 0x2000)
        {
            pres_calibration_steering(port);
            snprintf(text, sizeof(text), (const char *)psx_addr(pres_calibration_message(0x800A0C58), 1));
            pres_calibration_center(port, 28, text);
        }
        else
        {
            snprintf(text, sizeof(text), (const char *)psx_addr(pres_calibration_message(0x800A0668), 1));
            pres_calibration_center(port, 28, text);
        }
    }
}

static uint32 pres_calibration_update(void)
{
    pres_8008B9C8(0);
    pres_8008B9C8(1);
    pres_calibration_render();
    w_u32(0x80065338, r_u32(0x80065538));
    w_u32(0x8006533C, r_u32(0x8006553C));
    if (r_s16(0x800AB730) == 1)
        sub_8001D01C(2);
    return (uint32)(sint32)r_s16(0x800AB730);
}

void pres_8009B2A8(void)
{
    pres_calibration_initialize();
}

void pres_8009B2C8(uint32 object)
{
    if (pres_calibration_update())
        w_u32(0x800ABB30u, 2u);
    w_u32(0x80059764u, (uint32)(sint32)(sint16)r_u16(0x8005A9A6u));
    w_u32(0x80059768u, (uint32)(sint32)(sint16)r_u16(0x8005A9CAu));
}
