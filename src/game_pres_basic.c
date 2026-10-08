#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>

/* These initializers are original JR RA and NOP leaves */
void pres_80094144(void) {}
void pres_80094238(void) {}
void pres_8009748C(void) {}
void pres_800983F4(void) {}
void pres_800991DC(void) {}
void pres_8009B270(void) {}
void pres_800983FC(uint32 object) { (void)object; }

void pres_8009414C(uint32 object)
{
    uint32 language;
    if (!(r_u16(0x80065524u) & 0x4000u)) return;
    w_u32(0x800ABB20u, 1u);
    if (object == 0x800A31E8u) language = 0u;
    else if (object == 0x800A3220u) language = 1u;
    else if (object == 0x800A325Cu) language = 2u;
    else if (object == 0x800A3294u) language = 3u;
    else if (object == 0x800A32CCu) language = 4u;
    else language = ~0u;
    if (language != ~0u) {
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
    if (buttons & 0x1000u) {
        w_u32(0x800ABB20u, 3u);
        w_u32(0x800ABB30u, 0u);
        sub_8001D01C(3u);
        return;
    }
    if (!(buttons & 0x4008u) || r_u32(0x800ABB48u)) return;
    w_u32(0x800ABB20u, 1u);
    if (object == 0x800A35A0u) menu = 5u;
    else if (object == 0x800A3548u) menu = 6u;
    else if (object == 0x800A34C8u) menu = 19u;
    else if (object == 0x800A364Cu) menu = 8u;
    else if (object == 0x800A36E0u) menu = 20u;
    else if (object == 0x800A3798u) menu = 18u;
    else menu = ~0u;
    if (menu != ~0u) w_u32(0x800ABB30u, menu);
    sub_8001D01C(0u);
}

void pres_80097494(uint32 object)
{
    if (r_u16(0x80065524u) & 0x1000u) {
        sub_8001D01C(3u);
        w_u32(0x800ABB20u, 3u);
        w_u32(0x800ABB30u, 2u);
    }
    if (r_u16(0x80065524u) & 0x4000u) {
        sub_8001D01C(0u);
        if (object == 0x800A5674u) {
            w_u32(0x80064BE8u, 1u);
            w_u32(0x800ABB48u, 1u);
            w_u32(0x800ABB20u, 7u);
        }
    }
}

void pres_800991E4(uint32 object)
{
    uint32 buttons = r_u16(0x80065524u);
    (void)object;
    if (buttons & 0x1000u) {
        sub_8001D01C(3u);
        w_u32(0x800ABB30u, 12u);
        w_u32(0x800ABB20u, 3u);
    } else if (buttons & 0x4008u) {
        sub_8001D01C(0u);
        w_u32(0x800ABB20u, 7u);
        w_u32(0x800ABB30u, 3u);
        sub_8001D01C(1u);
    }
}

static uint32 pres_records_action(void)
{
    /* TODO: Translate the original records viewer 8009017C */
    fprintf(stderr, "Untranslated PRES dependency 8009017C\n");
    abort();
}

void pres_8009B278(uint32 object)
{
    (void)object;
    if (pres_records_action()) w_u32(0x800ABB30u, 2u);
}

static uint32 pres_calibration_pending(uint32 target)
{
    /* TODO: Translate the original calibration helpers */
    fprintf(stderr, "Untranslated PRES dependency %08X\n", target);
    abort();
}

void pres_8009B2A8(void)
{
    pres_calibration_pending(0x8008B7DCu);
}

void pres_8009B2C8(uint32 object)
{
    (void)object;
    if (pres_calibration_pending(0x8008B8CCu)) w_u32(0x800ABB30u, 2u);
    w_u32(0x80059764u, (uint32)(sint32)(sint16)r_u16(0x8005A9A6u));
    w_u32(0x80059768u, (uint32)(sint32)(sint16)r_u16(0x8005A9CAu));
}
