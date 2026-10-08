#include "draft_signatures.h"
#include <string.h>

uint32 sub_8002BDE8(uint32 restore)
{
    uint32 source, destination, block, result;
    if (restore) {
        w_u32(0x80059744u, r_u32(0x80078CC0u));
        w_u32(0x80059738u, r_u32(0x80078CE4u));
        source = 0x80078CECu; destination = 0x8006691Cu;
    } else {
        w_u32(0x80078CC0u, r_u32(0x80059744u));
        w_u32(0x80078CE4u, r_u32(0x80059738u));
        source = 0x8006691Cu; destination = 0x80078CECu;
    }
    for (block = 0u; block < 32u; ++block) {
        uint32 a = r_u32(source), b = r_u32(source + 4u);
        uint32 c = r_u32(source + 8u), d = r_u32(source + 12u);
        w_u32(destination, a); w_u32(destination + 4u, b);
        w_u32(destination + 8u, c); w_u32(destination + 12u, d);
        source += 16u; destination += 16u;
    }
    result = r_u32(restore ? 0x80078EECu : 0x80065244u);
    w_u32(restore ? 0x80065200u : 0x80078EECu, result);
    return result;
}

static uint32 pres_record_disabled(uint32 index)
{
    return 0x800AD896u + index * 68u;
}

void pres_80098404(void)
{
    uint32 index = r_u32(0x800AD6B8u) + 1u;
    w_u16(pres_record_disabled(index), r_u32(0x80078CA8u) == 1u ? 0u : 1u);
}

void pres_8009847C(uint32 object)
{
    uint32 buttons = r_u16(0x80065524u);
    if (buttons & 0x1000u) {
        sub_8001D01C(3u);
        w_u32(0x800ABB30u, 0u);
        w_u32(0x800ABB20u, 3u);
        return;
    }
    if (!(buttons & 0x4008u) || r_u32(0x800ABB48u)) return;
    sub_8001D01C(0u);
    w_u32(0x800ABB20u, 1u);
    if (object == 0x800A65F4u) {
        memset(psx_addr(0x8006691Cu, 512u), 0, 512u);
        memset(psx_addr(0x80065FBCu, 512u), 0, 512u);
        w_u32(0x800ABB30u, 12u);
        w_u32(0x80065200u, 0u);
    } else if (object == 0x800A667Cu && r_u32(0x80078CA8u) == 1u) {
        sub_8002BDE8(1u);
        w_u32(0x800ABB48u, 1u);
        w_u32(0x800ABB20u, 7u);
        w_u32(0x80065200u, r_u32(0x80078EECu));
    }
}

void pres_800985A8(void)
{
    uint32 index;
    static const uint32 availability[5] = {
        0x80064B2Cu, 0x80064B30u, 0x80064B34u, 0x80064B38u, 0x80064B28u
    };
    w_u32(0x80078CA8u, 0u);
    for (index = 0u; index < 5u; ++index)
        if (r_u32(availability[index]) == 1u)
            w_u16(pres_record_disabled(r_u32(0x800AD6B8u) + index), 1u);
}

void pres_800986F4(uint32 object)
{
    uint32 buttons = r_u16(0x80065524u), selected;
    (void)object;
    if (buttons & 0x1000u) {
        sub_8001D01C(3u);
        w_u32(0x800ABB30u, 0u);
        w_u32(0x800ABB20u, 3u);
        return;
    }
    if (!(buttons & 0x4008u)) return;
    selected = r_u32(0x800AD668u);
    if ((sint16)r_u16(pres_record_disabled(r_u32(0x800AD6B8u) + selected)) == 1) return;
    sub_8001D01C(0u);
    w_u32(0x800ABB20u, 1u);
    w_u32(0x800ABB30u, 0u);
    sub_8001D01C(1u);
    selected = r_u32(0x800AD668u);
    if (selected < 5u) w_u32(0x80059738u, selected == 4u ? 7u : selected + 8u);
}
