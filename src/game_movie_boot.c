#include "draft_signatures.h"
#include "psx_spu.h"
#include <stdio.h>
extern void sub_80047254(void);
extern void sub_800437BC(void);

uint32 sub_8002BC5C(uint32 movie_descriptor)
{
#if defined(__EMSCRIPTEN__)
    /* STR movies are excluded by the shared web packager */
    (void)movie_descriptor;
    return 0u;
#else
    uint32 header;
    FUNCTION_MARKER(0x8002BC5Cu, "SLUS_009.69");
    w_u32(r_u32(0x80064B3Cu), movie_descriptor);
    w_u32(0x80065340u, 0x800BF800u);
    sub_80041C3C(0x80011DB8u, 0x800BF800u, 327680u);
    header = r_u32(0x80065340u);
    draft_agent1_missing_diagnostic("pc = %08x\n", r_u32(header + 16u));
    header = r_u32(0x80065340u);
    draft_agent1_missing_diagnostic("t_addr = %08x\n", r_u32(header + 24u));
    header = r_u32(0x80065340u);
    draft_agent1_missing_diagnostic("t_size = %08x\n", r_u32(header + 28u));
    header = r_u32(0x80065340u);
    w_u32(header + 48u, 0x801DFFF0u);
    w_u32(header + 52u, 0u);
    DrawSync(0);
    sub_80023968();
    StopRCnt(0xF2000000u, 0x8005E8D8u, 0x8005E8E0u);
    StopRCnt(0xF2000001u, 0x8005E8D8u, 0x8005E8E0u);
    StopRCnt(0xF2000002u, 0x8005E8D8u, 0x8005E8E0u);
    StopRCnt(0xF2000003u, 0x8005E8D8u, 0x8005E8E0u);
    sub_8004E1A4();
    ResetGraph(3);
    StopCallback();
    sub_80047254();
    Exec(r_u32(0x80065340u) + 16u, 1u, 0u);
    sub_800437BC();
    ResetCallbackPSX();
    CdInit();
    ResetGraph(3);
    SetGraphDebug(0);
    return sub_800237D8();
#endif
}
