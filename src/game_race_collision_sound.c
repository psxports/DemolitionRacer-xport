#include "draft_signatures.h"
extern uint32 sub_80043450(uint32, uint32);
extern uint32 draft_agent3_missing_8001F860(void);
extern uint32 draft_agent3_missing_80049F84(uint32, uint32);
extern uint32 draft_agent3_missing_8004BB74(const void *);
extern uint32 draft_agent3_missing_8001FBE4(uint32, sint32, uint32, sint32);

uint32 sub_8001EA08(uint32 sound, sint32 voice, sint32 volume, sint32 frequency, const VECTOR *position)
{
    sint32 scaled, angle, pan, limited_frequency;
    uint32 product, selected = (uint32)voice, offset, mask;
    if (volume >= 4097) volume = 4096;
    product = (uint32)volume * r_u32(0x80064BFCu);
    if ((sint32)product < 0) product += 4095u;
    scaled = (sint32)product >> 12;
    if (scaled <= 0) return 0xFFFFFFFFu;
    angle = (sint32)sub_80043450((uint32)position->vz, (uint32)position->vx);
    if (angle < 2049 || r_u32(0x800652ACu))
        pan = (sint32)((1024u - (uint32)angle) << 2u);
    else pan = (sint32)(((uint32)angle - 3072u) << 2u);
    if (voice == -1) {
        selected = draft_agent3_missing_8001F860();
        if (selected == 0xFFFFFFFFu) return 0xFFFFFFFFu;
    }
    offset = selected << 5u;
    w_u16(0x80073FB8u + offset, (uint16)sound);
    w_u32(0x80073FA4u + offset, 0u);
    w_u32(0x80073FA0u + offset, 0u);
    w_u32(0x80073FB4u + offset, (uint32)frequency);
    w_u16(0x80073FBCu + offset, (uint16)pan);
    // Preserve the original retained-volume branch omitted by pseudocode
    if (scaled == -1) scaled = (sint32)r_u32(0x80073FB0u + offset);
    else w_u32(0x80073FB0u + offset, (uint32)scaled);
    mask = 1u << (selected & 31u);
    limited_frequency = frequency;
    if (limited_frequency >= 16384) limited_frequency = 16383;
    draft_agent3_missing_80049F84(0u, mask);
    w_u32(0x800742A0u, mask);
    w_u32(0x800742A4u, 128u);
    w_u32(0x800742BCu, r_u32(0x800742E0u + sound * 12u));
    draft_agent3_missing_8004BB74(psx_addr(0x800742A0u, 1u));
    if (r_u32(0x800652ACu))
        draft_agent3_missing_8001FBE4(selected, limited_frequency, (uint32)pan, scaled);
    else sub_8001F8FC(selected, limited_frequency, pan, scaled);
    draft_agent3_missing_80049F84(1u, mask);
    return selected;
}
