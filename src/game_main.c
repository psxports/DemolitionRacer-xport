#include "psx.h"
#include "xport.h"
#include "draft_signatures.h"
#include "psx_spu.h"
#include <string.h>

/* Original ResetGraph and SetGraphDebug share this graph type byte */
const uint32 xport_gpu_graph_type_address = 0x8005CC28u;

void dr_startup(void);

void xport_main(void)
{
    dr_startup();
}

#include <stdlib.h>
uint32 xport_missing_cd_sync_guest_callback(uint32 guest_callback);
sint32 CD_getsector(uint32 guest_destination, uint32 words);
uint32 ChangeClearPAD(uint32 mode);
uint32 _copy_memcard_patch(void);
uint32 _patch_card(void);
uint32 _patch_card2(void);
uint32 _SpuDataCallback(uint32 guest_callback);
uint32 _bu_init(void);
sint32 CdReset(sint32 mode);
sint32 CdControlBPSX(uint8 command, uint8 *parameter, uint8 *result);
sint32 CdRead(sint32 count, uint32 *destination, sint32 mode);
sint32 CdReadSync(sint32 mode, uint8 *result);
sint32 StopCallback(void);
sint32 Exec(uint32 guest_header, uint32 argc, uint32 argv);
uint32 ReadInitPadFlag(void);
sint32 sub_80047C94(uint32 first, uint32 second, uint32 third);
void dr_native_scanout(void);
sint32 CdInit(void);
sint32 randPSX(void);
sint32 sub_8004BA44(uint32 position);
uint32 sub_800511F0(void);
sint32 sub_80023968(void);
void sub_8004E1A4(void);
void sub_8004DB74(uint32 mode);
void sub_8004DBE0(void);
sint32 sub_800535C8(void);
void sub_80015E38(void);
uint32 sub_80041C3C(uint32 path, uint32 destination, uint32 capacity);
uint32 sub_8002BC5C(uint32 movie_path);
uint32 sub_800237D8(void);
sint32 sub_8004B8BC(uint32 destination, uint32 words);
void sub_800437BC(void);
void sub_80047254(void);
sint32 sub_800582C8(sint32 mode);
sint32 sub_800584A8(void);
uint32 sub_8001E13C(void);
uint32 sub_80041E24(uint32 path);
void sub_8002B88C(uint32 buffer, uint32 path);
void sub_8003BC14(void);
void sub_8003AAFC(uint32 value);
void sub_8001E320(uint32 value);
uint32 sub_8004225C(const char *path, uint32 buffer, uint32 capacity);
void dr_overlay_call(uint32 entry);
uint32 sub_8001DE6C();
uint32 sub_800161CC();
uint32 sub_80014BC4();
uint32 sub_800151DC();
uint32 sub_8003A448(void);

sint32 sub_8004BA44(uint32 position)
{
    FUNCTION_MARKER(0x8004BA44u, "SLUS_009.69");
    uint32 minute = r_u8(position);
    uint32 second = r_u8(position + 1u);
    uint32 frame = r_u8(position + 2u);
    minute = 10u * (minute >> 4) + (minute & 15u);
    second = 10u * (second >> 4) + (second & 15u);
    frame = 10u * (frame >> 4) + (frame & 15u);
    return (sint32)(75u * (60u * minute + second) + frame - 150u);
}

uint32 sub_800511F0(void)
{
    FUNCTION_MARKER(0x800511F0u, "SLUS_009.69");
    uint32 previous = xport_missing_cd_sync_guest_callback(r_u32(0x8005F340u));
    w_u32(0x8005F33Cu, 0u);
    return previous;
}

sint32 sub_80023968(void)
{
    FUNCTION_MARKER(0x80023968u, "SLUS_009.69");
    CloseEventPSX(r_u32(0x800652FCu));
    CloseEventPSX(r_u32(0x80065300u));
    CloseEventPSX(r_u32(0x80065304u));
    CloseEventPSX(r_u32(0x80065308u));
    CloseEventPSX(r_u32(0x8006530Cu));
    CloseEventPSX(r_u32(0x80065310u));
    CloseEventPSX(r_u32(0x80065314u));
    return CloseEventPSX(r_u32(0x80065318u));
}

void sub_8004E1A4(void)
{
    FUNCTION_MARKER(0x8004E1A4u, "SLUS_009.69");
    if (r_u32(0x8005FF60u) == 1u)
    {
        w_u32(0x8005FF60u, 0u);
        sub_80047254();
        w_u32(0x8005FFA8u, 0u);
        w_u32(0x8005FFACu, 0u);
        _SpuDataCallback(0u);
        CloseEventPSX(r_u32(0x8005FEF8u));
        DisableEventPSX(r_u32(0x8005FEF8u));
        sub_800437BC();
    }
}

void sub_8004DB74(uint32 mode)
{
    FUNCTION_MARKER(0x8004DB74u, "SLUS_009.69");
    ChangeClearPAD(0u);
    sub_80047254();
    if (ReadInitPadFlag() == 0u)
        mode = 0u;
    sub_800582C8(mode);
    _copy_memcard_patch();
    _patch_card();
    _patch_card2();
    sub_800437BC();
}

void sub_8004DBE0(void)
{
    FUNCTION_MARKER(0x8004DBE0u, "SLUS_009.69");
    sub_80047254();
    sub_800584A8();
    ChangeClearPAD(0u);
    sub_800437BC();
}

sint32 sub_800535C8(void)
{
    FUNCTION_MARKER(0x800535C8u, "SLUS_009.69");
    uint8 result_bytes[8];
    uint32 interrupt_code, count, error = 0u, i;
    w_u8(r_u32(0x8005FE94u), 1u);
    uint32 interrupt_register = r_u32(0x8005FEA0u);
    interrupt_code = r_u8(interrupt_register) & 7u;
    if (interrupt_code == 0u)
        return 0;
    while (interrupt_code != (r_u8(interrupt_register) & 7u))
        interrupt_code = r_u8(interrupt_register) & 7u;
    count = 0u;
    do
    {
        if ((r_u8(r_u32(0x8005FE94u)) & 0x20u) == 0u)
            break;
        result_bytes[count++] = r_u8(r_u32(0x8005FE98u));
    } while (count < 8u);
    for (i = count; i < 8u; ++i)
        result_bytes[i] = 0u;
    w_u8(r_u32(0x8005FE94u), 1u);
    w_u8(r_u32(0x8005FEA0u), 7u);
    w_u8(r_u32(0x8005FE9Cu), 7u);
    if (interrupt_code != 3u || r_u32(0x8005FD94u + 4u * r_u8(0x8005FBEDu)) != 0u)
    {
        if ((r_u32(0x8005FBDCu) & 0x10u) == 0u && (result_bytes[0] & 0x10u) != 0u)
            w_u32(0x8005FBE4u, r_u32(0x8005FBE4u) + 1u);
        error = result_bytes[0] & 0x1Du;
        w_u32(0x8005FBDCu, result_bytes[0]);
        w_u32(0x8005FBE0u, result_bytes[1]);
    }
    if (interrupt_code == 5u && (sint32)r_u32(0x8005FBD8u) > 0)
    {
        printf((const char *)psx_addr(0x80012AC4u, 1u));
        if ((sint32)r_u32(0x8005FBD8u) > 0)
        {
            uint32 command_index = r_u8(0x8005FBEDu);
            uint32 status_word = r_u32(0x8005FBDCu);
            uint32 detail_word = r_u32(0x8005FBE0u);
            uint32 command_name = r_u32(0x8005FBF4u + 4u * command_index);
            printf((const char *)psx_addr(0x80012AD0u, 1u), (const char *)psx_addr(command_name, 1u), status_word, detail_word);
        }
    }
    switch (interrupt_code)
    {
        case 1:
            if (error != 0u && count == 1u)
                error = 0u;
            w_u8(0x8005FEADu, error != 0u ? 5u : 1u);
            for (i = 0u; i < 8u; ++i)
                w_u8(0x8008B36Cu + i, result_bytes[i]);
            w_u8(r_u32(0x8005FE94u), 0u);
            w_u8(r_u32(0x8005FEA0u), 0u);
            return 4;
        case 2:
            w_u8(0x8005FEACu, error != 0u ? 5u : 2u);
            for (i = 0u; i < 8u; ++i)
                w_u8(0x8008B364u + i, result_bytes[i]);
            return 2;
        case 3:
            if (error != 0u)
            {
                w_u8(0x8005FEACu, 5u);
                for (i = 0u; i < 8u; ++i)
                    w_u8(0x8008B364u + i, result_bytes[i]);
                return 2;
            }
            if (r_u32(0x8005FC94u + 4u * r_u8(0x8005FBEDu)) != 0u)
            {
                w_u8(0x8005FEACu, 3u);
                for (i = 0u; i < 8u; ++i)
                    w_u8(0x8008B364u + i, result_bytes[i]);
                return 1;
            }
            w_u8(0x8005FEACu, 2u);
            for (i = 0u; i < 8u; ++i)
                w_u8(0x8008B364u + i, result_bytes[i]);
            return 2;
        case 4:
            w_u8(0x8005FEAEu, 4u);
            w_u8(0x8005FEADu, r_u8(0x8005FEAEu));
            for (i = 0u; i < 8u; ++i)
                w_u8(0x8008B374u + i, result_bytes[i]);
            for (i = 0u; i < 8u; ++i)
                w_u8(0x8008B36Cu + i, result_bytes[i]);
            return 4;
        case 5:
            w_u8(0x8005FEADu, 5u);
            w_u8(0x8005FEACu, r_u8(0x8005FEADu));
            for (i = 0u; i < 8u; ++i)
                w_u8(0x8008B364u + i, result_bytes[i]);
            for (i = 0u; i < 8u; ++i)
                w_u8(0x8008B36Cu + i, result_bytes[i]);
            return 6;
        default:
            puts((const char *)psx_addr(0x80012AECu, 1u));
            printf((const char *)psx_addr(0x80012B00u, 1u), interrupt_code);
            return 0;
    }
}

void sub_80015E38(void)
{
    FUNCTION_MARKER(0x80015E38u, "SLUS_009.69");
    if (r_u32(0x8005E7F4u) == 0u)
        w_u32(0x8005E7F4u, 1u);
    sub_80047C94(0x801FFFF0u, 16u, 16u);
    ResetCallback();
    VSyncCallback(dr_native_scanout);
    CdInit();
    sub_8002BC5C(0x800597C4u);
    ResetGraph(0);
    SetGraphDebug(0);
    DrawSync(0);
    InitGeom();
    sub_8001E13C();
    SetDispMask(0);
    ClearImage((PSX_RECT *)psx_addr(0x80064C28u, sizeof(PSX_RECT)), 0u, 0u, 0u);
    w_u32(0x80065220u, 0x800C8D28u);
    w_u32(0x80065224u, 0x800AFA60u);
    w_u32(0x80065238u, 0x800C8D28u);
    sub_80041E24(0x800102A0u);
    sub_8002B88C(r_u32(0x80065238u), 0x800102ACu);
    sub_8002B6CC();
    sub_8001C958();
    sub_8003BC14();
    sub_8003AAFC(1u);
    sub_8001E320(0x1000u);
    w_u32(0x80065214u, 0u);
    for (;;)
    {
        uint32 level, inverted, setting, demo_mode;
        if (r_u32(0x80064C20u) == 0u)
        {
            sub_8004225C((const char *)psx_addr(0x800102BCu, 1u), r_u32(0x80010004u), 0x64000u);
            if (r_u32(0x80064B64u) == 0u)
            {
                dr_overlay_call(0x8008FD38u);
                w_u32(0x80064B64u, 1u);
            }
            dr_overlay_call(0x80092410u);
        }
        if (r_u32(0x80064BE8u) != 0u)
        {
            w_u32(0x80064BE8u, 0u);
            sub_8002BC5C(0x800598E0u);
        }
        else
        {
            sub_8001DE6C();
            sub_8004225C((const char *)psx_addr(0x800102C8u, 1u), r_u32(0x80010000u), 0x7D000u);
            level = r_u32(0x80065234u) + 1u;
            inverted = (r_u32(0x8005973Cu) ^ 1u) != 0u;
            setting = r_u32(0x8005976Cu);
            demo_mode = r_u32(0x80064B5Cu);
            w_u32(0x80064C04u, level);
            w_u32(0x80064C08u, level);
            w_u32(0x800593A8u, inverted);
            w_u32(0x80064C14u, inverted);
            w_u32(0x80064B68u, setting);
            if (demo_mode != 0u)
            {
                uint32 random, quotient, index;
                w_u16(0x8007AA3Eu, 1u);
                random = randPSX();
                quotient = (uint32)(((uint64)random * 0xCCCCCCCDu) >> 32) >> 3;
                index = r_u8(0x80059A1Cu + random - quotient * 10u);
                w_u32(0x80064C04u, index);
                w_u32(0x80064C08u, index);
                random = randPSX();
                w_u32(0x80065210u, (uint32)((sint32)random % 8));
                w_u32(0x800593A8u, 0u);
                w_u32(0x8005973Cu, 1u);
                w_u32(0x80064C14u, 0u);
            }
            w_u32(0x800593A4u, r_u8(0x800599FBu + r_u32(0x80064C08u)));
            do
            {
                uint32 selection = (uint32)((sint32)(r_u32(0x80064C08u) - 1u) % 15);
                uint32 loading_buffer = r_u32(0x80065238u);
                uint32 loading_path = r_u32(0x800596FCu + (selection << 2));
                sub_8002B88C(loading_buffer, loading_path);
                sub_8002B6CC();
                w_u32(0x80064C18u, 0u);
                sub_800161CC();
                w_u32(0x80065248u, 0u);
                w_u32(0x80065110u, 0u);
                sub_80014BC4();
                w_u32(0x80064B58u, 0u);
                sub_8002B840();
                sub_800151DC();
            } while (r_u8(0x80064EA0u) == 1u || r_u8(0x80064EA1u) == 1u);
            w_u32(0x80064B5Cu, 0u);
        }
        sub_8002B88C(r_u32(0x80065238u), 0x800102D4u);
        sub_8002B6CC();
    }
}

uint32 sub_80041C3C(uint32 path, uint32 destination, uint32 capacity)
{
    FUNCTION_MARKER(0x80041C3Cu, "SLUS_009.69");
    uint32 file[6];
    for (;;)
    {
        if (CdSearchFile((CdlFILE *)file, (const char *)psx_addr(path, 1u)) == NULL)
        {
            CdReset(0);
            continue;
        }
        if (CdControlBPSX(0x15u, (uint8 *)file, NULL) == 0)
        {
            CdReset(0);
            continue;
        }
        if (CdRead((file[1] + 2047u) >> 11, (uint32 *)psx_addr(destination, 1u), 0x80) != 0)
            break;
        CdReset(0);
    }
    while (CdReadSync(1, NULL) > 0)
    {
    }
    return file[1];
}

uint32 sub_800237D8(void)
{
    FUNCTION_MARKER(0x800237D8u, "SLUS_009.69");
    sub_8004DB74(0u);
    sub_8004DBE0();
    ChangeClearPAD(0u);
    _bu_init();
    sub_80047254();
    w_u32(0x800652FCu, OpenEventPSX(0xF4000001u, 4u, 0x2000u, 0u));
    w_u32(0x80065300u, OpenEventPSX(0xF4000001u, 0x8000u, 0x2000u, 0u));
    w_u32(0x80065304u, OpenEventPSX(0xF4000001u, 0x100u, 0x2000u, 0u));
    w_u32(0x80065308u, OpenEventPSX(0xF4000001u, 0x2000u, 0x2000u, 0u));
    w_u32(0x8006530Cu, OpenEventPSX(0xF0000011u, 4u, 0x2000u, 0u));
    w_u32(0x80065310u, OpenEventPSX(0xF0000011u, 0x8000u, 0x2000u, 0u));
    w_u32(0x80065314u, OpenEventPSX(0xF0000011u, 0x100u, 0x2000u, 0u));
    uint32 last_event = OpenEventPSX(0xF0000011u, 0x2000u, 0x2000u, 0u);
    uint32 first_event = r_u32(0x800652FCu);
    w_u32(0x80065318u, last_event);
    EnableEventPSX(first_event);
    EnableEventPSX(r_u32(0x80065300u));
    EnableEventPSX(r_u32(0x80065304u));
    EnableEventPSX(r_u32(0x80065308u));
    EnableEventPSX(r_u32(0x8006530Cu));
    EnableEventPSX(r_u32(0x80065310u));
    EnableEventPSX(r_u32(0x80065314u));
    EnableEventPSX(r_u32(0x80065318u));
    sub_800437BC();
    return sub_8003A448();
}

sint32 sub_8004B8BC(uint32 destination, uint32 words)
{
    FUNCTION_MARKER(0x8004B8BCu, "SLUS_009.69");
    return CD_getsector(destination, words) == 0;
}

void sub_800437BC(void)
{
    FUNCTION_MARKER(0x800437BCu, "SLUS_009.69");
    xport_bios_exit_critical();
}

void sub_80047254(void)
{
    FUNCTION_MARKER(0x80047254u, "SLUS_009.69");
    xport_bios_enter_critical();
}

sint32 sub_800582C8(sint32 mode)
{
    FUNCTION_MARKER(0x800582C8u, "SLUS_009.69");
    return (sint32)xport_bios_init_card((uint32)mode);
}

sint32 sub_800584A8(void)
{
    FUNCTION_MARKER(0x800584A8u, "SLUS_009.69");
    return (sint32)xport_bios_start_card();
}

uint32 sub_8003A448(void);
uint32 sub_80056C54(void);
uint32 sub_8001E13C(void);
uint32 sub_8004B294(void);
sint32 sub_8001FE94(void);
uint32 sub_80058C38(uint32 owner);
uint32 sub_8004B89C(const uint8 *volume);
uint32 sub_80041E24(uint32 path);
void sub_8002B88C(uint32 buffer, uint32 path);
uint32 sub_80039718(uint32 mode);
uint32 sub_8004F3B0(void);
uint32 sub_8003BF0C(uint32 x, uint32 y, uint32 incoming_v0);
void sub_8004FACC(uint32 x, uint32 y);
uint32 sub_8003BC1C(void);
uint32 sub_8003B674(uint32 remainder);
uint32 sub_800399D4(void);
void xport_missing_pad_cleanup_callback(uint32 target, uint32 owner);
void xport_missing_guest_sprintf_format(char *destination, uint32 format_address);
uint32 xport_missing_pad_owner_callback(uint32 target, uint32 owner);
void xport_mips_break(uint32 code);

uint32 SysEnqIntRP(uint32 priority, uint32 node);
uint32 ChangeClearRCnt(uint32 counter, uint32 mode);
uint32 sub_80057160(uint32 owner, sint32 value);
uint32 sub_8004F39C(uint32 value);
uint32 sub_80039934(void);
uint32 sub_8004225C(const char *path, uint32 buffer, uint32 capacity);
uint32 SpuInitMalloc(uint32 count, uint32 table);
uint32 _SpuInit(uint32 mode);
sint32 CD_vol(CdlATV *volume);
void SpuSetReverbDepth(SpuReverbAttr *attr);
sint32 SpuReserveReverbWorkArea(sint32 mode);
sint32 CdReset(sint32 mode);
sint32 CdControlBPSX(uint8 command, uint8 *parameter, uint8 *result);
sint32 CdRead(sint32 count, uint32 *destination, sint32 mode);
sint32 CdReadSync(sint32 mode, uint8 *result);

uint32 sub_8003A448(void)
{
    FUNCTION_MARKER(0x8003A448u, "SLUS_009.69");
    PadInitDirect((uint8 *)psx_addr(0x80081B20u, 0x22u), (uint8 *)psx_addr(0x80081B42u, 0x22u));
    PadStartCom();
    return bzeroPSX(0x800654F8u, 8u);
}

uint32 sub_80056C54(void)
{
    FUNCTION_MARKER(0x80056C54u, "SLUS_009.69");
    uint32 interrupt_control;
    uint32 record;
    uint32 callback;
    w_u32(0x800605A0u, 0u);
    sub_80047254();
    SysDeqIntRP(2u, 0x8008B3ACu);
    SysEnqIntRP(2u, 0x8008B3ACu);
    interrupt_control = r_u32(0x800605C8u);
    w_u32(interrupt_control, 0xFFFFFFFEu);
    w_u32(interrupt_control + 4u, r_u32(interrupt_control + 4u) | 1u);
    ChangeClearRCnt(3u, 0u);
    sub_800437BC();
    record = r_u32(0x8006059Cu);
    callback = r_u32(0x8006056Cu);
    xport_missing_pad_cleanup_callback(callback, record);
    record = r_u32(0x8006059Cu);
    callback = r_u32(0x8006056Cu);
    xport_missing_pad_cleanup_callback(callback, record + 0xF0u);
    w_u32(0x8008B3C0u, 0u);
    w_u32(0x8008B3BCu, 0u);
    w_u32(0x800605A0u, 1u);
    return 1u;
}

uint32 sub_8001E13C(void)
{
    FUNCTION_MARKER(0x8001E13Cu, "SLUS_009.69");
    uint32 initialized = r_u8(0x80064D28u);
    uint32 volume_word = r_u32(0x80064D2Cu);
    uint8 volume[4];
    uint32 voice;
    volume[0] = (uint8)volume_word;
    volume[1] = (uint8)(volume_word >> 8);
    volume[2] = (uint8)(volume_word >> 16);
    volume[3] = (uint8)(volume_word >> 24);
    if (initialized != 0u)
        return initialized;
    w_u8(0x80064D28u, 1u);
    sub_8004B294();
    SpuInitMalloc(0x30u, 0x80073DF0u);
    w_u16(0x80073F7Cu, 0x3FFCu);
    w_u16(0x80073F7Eu, 0x3FFCu);
    w_u32(0x80073F78u, 3u);
    SpuSetCommonAttr((SpuCommonAttr *)psx_addr(0x80073F78u, sizeof(SpuCommonAttr)));
    sub_8001FE94();
    sub_8004B89C(volume);
    for (voice = 0u; voice < 24u; ++voice)
        memset(psx_addr(0x80073FA0u + 0x20u * voice, 0x20u), 0, 0x20u);
    return 0u;
}

uint32 sub_8004B294(void)
{
    FUNCTION_MARKER(0x8004B294u, "SLUS_009.69");
    return _SpuInit(0u);
}

sint32 sub_8001FE94(void)
{
    FUNCTION_MARKER(0x8001FE94u, "SLUS_009.69");
    SpuReverbAttr attr;
    SpuReverbControl control;
    sint32 reserved;
    SpuSetReverb(1);
    w_u32(0x8005FF04u, 1u);
    attr.mask = 7u;
    attr.mode = 0x103;
    attr.depth.left = 0;
    attr.depth.right = 0;
    if (SpuSetReverbModeParam(&attr) == 0)
    {
        spu_reverb_export(&control);
        w_u32(0x8005FF14u, (uint32)control.mode);
        w_u32(0x8005FF0Cu, control.work_address);
        w_u32(0x8005FF1Cu, (uint32)control.delay);
        w_u32(0x8005FF20u, (uint32)control.feedback);
        w_u16(0x8005FF18u, (uint16)control.depth_left);
        w_u16(0x8005FF1Au, (uint16)control.depth_right);
    }
    SpuSetReverbDepth(&attr);
    w_u16(0x8005FF18u, 0u);
    w_u16(0x8005FF1Au, 0u);
    SpuSetReverbVoice(1, 0xFFFFFFu);
    reserved = SpuReserveReverbWorkArea(1);
    w_u32(0x8005FF08u, (uint32)reserved);
    return reserved;
}

uint32 sub_80058C38(uint32 owner)
{
    FUNCTION_MARKER(0x80058C38u, "SLUS_009.69");
    uint32 callback = r_u32(0x80060580u);
    uint32 result = xport_missing_pad_owner_callback(callback, owner);
    uint32 packet = r_u32(owner + 0x3Cu);
    w_u32(0x800649F8u, result);
    w_u8(packet, 0u);
    return sub_80057160(owner, -2);
}

uint32 sub_8004B89C(const uint8 *volume)
{
    FUNCTION_MARKER(0x8004B89Cu, "SLUS_009.69");
    CD_vol((CdlATV *)volume);
    return 1u;
}

uint32 sub_80041E24(uint32 path)
{
    FUNCTION_MARKER(0x80041E24u, "SLUS_009.69");
    uint32 file[6];
    uint32 minute;
    uint32 second;
    uint32 frame;
    uint32 base_sector;
    uint32 index;
    CdReset(0);
    printf((const char *)psx_addr(0x800123BCu, 1u), (const char *)psx_addr(path, 1u));
    for (;;)
    {
        if (CdSearchFile((CdlFILE *)file, (const char *)psx_addr(path, 1u)) == NULL)
        {
            CdReset(0);
            continue;
        }
        if (CdControlBPSX(0x15u, (uint8 *)file, NULL) == 0)
        {
            CdReset(0);
            continue;
        }
        if (CdRead(10, (uint32 *)psx_addr(0x800839D4u, 20480u), 0x80) == 0)
        {
            CdReset(0);
            continue;
        }
        break;
    }
    while (CdReadSync(1, NULL) > 0)
        ;
    minute = ((uint8 *)file)[0];
    second = ((uint8 *)file)[1];
    frame = ((uint8 *)file)[2];
    minute = 10u * (minute >> 4) + (minute & 15u);
    second = 10u * (second >> 4) + (second & 15u);
    frame = 10u * (frame >> 4) + (frame & 15u);
    base_sector = 4500u * minute + 75u * second + frame;
    index = 0u;
    for (;;)
    {
        uint32 offset;
        uint32 sector;
        uint32 count;
        if (index >= 512u)
            return index;
        offset = 40u * index;
        sector = r_u32(0x800839F8u + offset);
        w_u32(0x800839F8u + offset, sector + 10u + base_sector);
        if (r_u8(0x800839D4u + offset) == 0u)
            return offset;
        count = r_u32(0x80065180u);
        w_u32(0x80065180u, count + 1u);
        index += 1u;
    }
}

void sub_8002B88C(uint32 buffer, uint32 path)
{
    uint32 vertical, horizontal, index, destination, offset, image;
    uint16 value;
    char pathname[64];
    PSX_RECT rectangle;
    FUNCTION_MARKER(0x8002B88Cu, "SLUS_009.69");
    VSync(0);
    DrawSync(0);
    SetDispMask(0);
    sub_80039718(0u);
    image = buffer + 0x14u;
    sub_800399D4();
    vertical = r_u32(0x80064BF4u);
    horizontal = r_u32(0x80064BF0u);
    index = r_u32(0x80065128u);
    w_u16(0x80081A9Eu, (uint16)vertical);
    w_u16(0x80081B12u, (uint16)vertical);
    destination = r_u32(0x80065118u);
    w_u16(0x80081A9Cu, (uint16)horizontal);
    w_u16(0x80081B10u, (uint16)horizontal);
    offset = (index ^ 1u) * 0x74u;
    value = r_u16(0x80081A3Au + offset);
    w_u16(destination + 2u, value);
    value = r_u16(0x80081A42u + offset);
    w_u16(destination + 10u, value);
    sub_80039934();
    strcpy(pathname, (const char *)psx_addr(path, 1u));
    sub_8004225C(pathname, buffer, 0x28800u);
    index = r_u32(0x80065128u);
    rectangle.w = 320;
    rectangle.h = 256;
    rectangle.x = 0;
    rectangle.y = 0;
    w_u32(0x80065218u, index);
    LoadImagePSX(&rectangle, (uint32 *)psx_addr(image, 1u));
    rectangle.w = 320;
    rectangle.h = 256;
    rectangle.x = 0;
    rectangle.y = 256;
    LoadImagePSX(&rectangle, (uint32 *)psx_addr(image, 1u));
    DrawSync(0);
    VSync(0);
    w_u32(0x80065344u, buffer + 0x28800u);
    SetDispMask(1);
}

uint32 sub_80039718(uint32 mode)
{
    uint32 video_mode, table, offset, i, width, height, incoming_v0;
    sint32 x, y, w, h;
    uint16 value;
    FUNCTION_MARKER(0x80039718u, "SLUS_009.69");
    video_mode = r_u32(0x80065130u);
    w_u32(0x8006516Cu, 0x1000u);
    if (video_mode != 0u)
    {
        table = 0x8005AACCu + mode * 30u;
        w_u32(0x80065124u, 50u);
        sub_8004F39C(1u);
    }
    else
    {
        table = 0x8005AB44u + mode * 30u;
        w_u32(0x80065124u, 60u);
    }
    x = (sint16)r_u16(table);
    y = (sint16)r_u16(table + 2u);
    w = (sint16)r_u16(table + 8u);
    h = (sint16)r_u16(table + 10u);
    SetDefDrawEnv((DRAWENV *)psx_addr(0x80081A38u, sizeof(DRAWENV)), x, y, w, h);
    x = (sint16)r_u16(table + 4u);
    y = (sint16)r_u16(table + 6u);
    w = (sint16)r_u16(table + 8u);
    h = (sint16)r_u16(table + 10u);
    SetDefDrawEnv((DRAWENV *)psx_addr(0x80081AACu, sizeof(DRAWENV)), x, y, w, h);
    x = (sint16)r_u16(table + 4u);
    y = (sint16)r_u16(table + 6u);
    w = (sint16)r_u16(table + 12u);
    h = (sint16)r_u16(table + 14u);
    SetDefDispEnv((DISPENV *)psx_addr(0x80081A94u, sizeof(DISPENV)), x, y, w, h);
    x = (sint16)r_u16(table);
    y = (sint16)r_u16(table + 2u);
    w = (sint16)r_u16(table + 12u);
    h = (sint16)r_u16(table + 14u);
    SetDefDispEnv((DISPENV *)psx_addr(0x80081B08u, sizeof(DISPENV)), x, y, w, h);
    width = (uint32)(sint32)(sint16)r_u16(table + 8u);
    height = (uint32)(sint32)(sint16)r_u16(table + 10u);
    x = (sint16)r_u16(table + 22u);
    y = (sint16)r_u16(table + 24u);
    w_u32(0x8006511Cu, width);
    w_u32(0x80065120u, height);
    w_u32(0x80065168u, (uint32)x);
    w_u32(0x80065164u, (uint32)y);
    for (i = 0u, offset = 0u; i < 2u; ++i, offset += 0x74u)
    {
        value = r_u16(table + 26u);
        w_u8(0x80081A50u + offset, (uint8)value);
        w_u8(0x80081A51u + offset, 0u);
        w_u8(0x80081A52u + offset, 0u);
        w_u8(0x80081A53u + offset, 0u);
        value = r_u16(table + 28u);
        w_u8(0x80081A4Eu + offset, (uint8)value);
        value = r_u16(table + 16u);
        w_u16(0x80081A9Cu + offset, value);
        value = r_u16(table + 18u);
        w_u16(0x80081A9Eu + offset, value);
        value = r_u16(table + 20u);
        w_u16(0x80081AA0u + offset, value);
        value = r_u16(table + 10u);
        w_u16(0x80081AA2u + offset, value);
    }
    width = r_u32(0x8006511Cu);
    height = r_u32(0x80065120u);
    w_u32(0x80065138u, mode);
    incoming_v0 = height >> 31;
    sub_8003BF0C((uint32)((sint32)width / 2), (uint32)((sint32)height / 2), incoming_v0);
    return sub_8003BC1C();
}

uint32 sub_8004F3B0(void)
{
    FUNCTION_MARKER(0x8004F3B0u, "SLUS_009.69");
    return r_u32(0x8005F2F8u);
}

uint32 sub_8003BF0C(uint32 x, uint32 y, uint32 incoming_v0)
{
    FUNCTION_MARKER(0x8003BF0Cu, "SLUS_009.69");
    sub_8004FACC(x, y);
    w_u32(0x80065154u, x);
    w_u32(0x80065158u, y);
    return incoming_v0;
}

void sub_8004FACC(uint32 x, uint32 y)
{
    FUNCTION_MARKER(0x8004FACCu, "SLUS_009.69");
    xport_gte_write_control(24u, x << 16);
    xport_gte_write_control(25u, y << 16);
}

uint32 sub_8003BC1C(void)
{
    FUNCTION_MARKER(0x8003BC1Cu, "SLUS_009.69");
    uint32 width_word = r_u32(0x8006511Cu);
    uint32 width_scale = r_u32(0x80065168u);
    uint32 product = width_word * width_scale;
    uint32 scale;
    uint32 width_adjusted, height_adjusted;
    uint32 width, height, lens, lens_square, square;
    uint32 denominator, numerator, first, second, divisor, projected_first, projected_second;
    if ((sint32)product < 0)
        product += 0xFFFu;
    scale = r_u32(0x8006516Cu);
    width_adjusted = (uint32)((sint32)product >> 12) * scale;
    if ((sint32)width_adjusted < 0)
        width_adjusted += 0xFFFu;
    product = r_u32(0x80065120u);
    product *= r_u32(0x80065164u);
    width = (uint32)((sint32)width_adjusted >> 12);
    if ((sint32)product < 0)
        product += 0xFFFu;
    height_adjusted = (uint32)((sint32)product >> 12) * scale;
    square = width * width;
    if ((sint32)height_adjusted < 0)
        height_adjusted += 0xFFFu;
    lens = r_u32(0x80065150u);
    height = (uint32)((sint32)height_adjusted >> 12);
    if ((sint32)square < 0)
        square += 3u;
    lens_square = lens * lens;
    denominator = sub_8003B674((uint32)((sint32)square >> 2) + lens_square);
    lens <<= 12;
    divisor = denominator;
    if (divisor == 0u)
        xport_mips_break(7u);
    if (divisor == 0xFFFFFFFFu && lens == 0x80000000u)
        xport_mips_break(6u);
    first = (uint32)((sint32)lens / (sint32)divisor);
    numerator = 0u - ((uint32)((sint32)(width + (width_adjusted >> 31)) >> 1) << 12);
    if (divisor == 0u)
        xport_mips_break(7u);
    if (divisor == 0xFFFFFFFFu && numerator == 0x80000000u)
        xport_mips_break(6u);
    second = (uint32)((sint32)numerator / (sint32)divisor);
    divisor = r_u32(0x8006516Cu);
    numerator = first << 12;
    if (divisor == 0u)
        xport_mips_break(7u);
    if (divisor == 0xFFFFFFFFu && numerator == 0x80000000u)
        xport_mips_break(6u);
    projected_first = (uint32)((sint32)numerator / (sint32)divisor);
    numerator = second << 12;
    if (divisor == 0u)
        xport_mips_break(7u);
    if (divisor == 0xFFFFFFFFu && numerator == 0x80000000u)
        xport_mips_break(6u);
    projected_second = (uint32)((sint32)numerator / (sint32)divisor);
    square = height * height;
    w_u16(0x80065552u, 0u);
    w_u16(0x80065550u, (uint16)projected_first);
    w_u16(0x80065554u, (uint16)projected_second);
    if ((sint32)square < 0)
        square += 3u;
    denominator = sub_8003B674((uint32)((sint32)square >> 2) + lens_square);
    divisor = denominator;
    if (divisor == 0u)
        xport_mips_break(7u);
    if (divisor == 0xFFFFFFFFu && lens == 0x80000000u)
        xport_mips_break(6u);
    first = (uint32)((sint32)lens / (sint32)divisor);
    numerator = 0u - ((uint32)((sint32)(height + (height_adjusted >> 31)) >> 1) << 12);
    if (divisor == 0u)
        xport_mips_break(7u);
    if (divisor == 0xFFFFFFFFu && numerator == 0x80000000u)
        xport_mips_break(6u);
    second = (uint32)((sint32)numerator / (sint32)divisor);
    divisor = r_u32(0x8006516Cu);
    numerator = first << 12;
    if (divisor == 0u)
        xport_mips_break(7u);
    if (divisor == 0xFFFFFFFFu && numerator == 0x80000000u)
        xport_mips_break(6u);
    projected_first = (uint32)((sint32)numerator / (sint32)divisor);
    numerator = second << 12;
    if (divisor == 0u)
        xport_mips_break(7u);
    if (divisor == 0xFFFFFFFFu && numerator == 0x80000000u)
        xport_mips_break(6u);
    projected_second = (uint32)((sint32)numerator / (sint32)divisor);
    w_u16(0x80065560u, 0u);
    w_u16(0x80065562u, (uint16)projected_first);
    w_u16(0x80065564u, (uint16)projected_second);
    return projected_second;
}

uint32 sub_8003B674(uint32 remainder)
{
    FUNCTION_MARKER(0x8003B674u, "SLUS_009.69");
    uint32 root = 0u;
    uint32 trial;
    if ((sint32)remainder > 0xFFFF)
    {
        if ((sint32)remainder > 0xFFFFFF)
        {
            trial = root + 0x40000000u;
            if (remainder >= trial)
            {
                remainder -= trial;
                root = (trial + 0x40000000u) >> 1;
            }
            else
                root >>= 1;
            trial = root + 0x10000000u;
            if (remainder >= trial)
            {
                remainder -= trial;
                root = (trial + 0x10000000u) >> 1;
            }
            else
                root >>= 1;
            trial = root + 0x4000000u;
            if (remainder >= trial)
            {
                remainder -= trial;
                root = (trial + 0x4000000u) >> 1;
            }
            else
                root >>= 1;
            trial = root + 0x1000000u;
            if (remainder >= trial)
            {
                remainder -= trial;
                root = (trial + 0x1000000u) >> 1;
            }
            else
                root >>= 1;
        }
        trial = root + 0x400000u;
        if (remainder >= trial)
        {
            remainder -= trial;
            root = (trial + 0x400000u) >> 1;
        }
        else
            root >>= 1;
        trial = root + 0x100000u;
        if (remainder >= trial)
        {
            remainder -= trial;
            root = (trial + 0x100000u) >> 1;
        }
        else
            root >>= 1;
        trial = root + 0x40000u;
        if (remainder >= trial)
        {
            remainder -= trial;
            root = (trial + 0x40000u) >> 1;
        }
        else
            root >>= 1;
        trial = root + 0x10000u;
        if (remainder >= trial)
        {
            remainder -= trial;
            root = (trial + 0x10000u) >> 1;
        }
        else
            root >>= 1;
    }
    trial = root + 0x4000u;
    if (remainder >= trial)
    {
        remainder -= trial;
        root = (trial + 0x4000u) >> 1;
    }
    else
        root >>= 1;
    trial = root + 0x1000u;
    if (remainder >= trial)
    {
        remainder -= trial;
        root = (trial + 0x1000u) >> 1;
    }
    else
        root >>= 1;
    trial = root + 0x400u;
    if (remainder >= trial)
    {
        remainder -= trial;
        root = (trial + 0x400u) >> 1;
    }
    else
        root >>= 1;
    trial = root + 0x100u;
    if (remainder >= trial)
    {
        remainder -= trial;
        root = (trial + 0x100u) >> 1;
    }
    else
        root >>= 1;
    trial = root + 0x40u;
    if (remainder >= trial)
    {
        remainder -= trial;
        root = (trial + 0x40u) >> 1;
    }
    else
        root >>= 1;
    trial = root + 0x10u;
    if (remainder >= trial)
    {
        remainder -= trial;
        root = (trial + 0x10u) >> 1;
    }
    else
        root >>= 1;
    trial = root + 0x4u;
    if (remainder >= trial)
    {
        remainder -= trial;
        root = (trial + 0x4u) >> 1;
    }
    else
        root >>= 1;
    trial = root + 1u;
    if (remainder >= trial)
        root = trial + 1u;
    return root >> 1;
}

uint32 sub_800399D4(void)
{
    FUNCTION_MARKER(0x800399D4u, "SLUS_009.69");
    uint32 next = r_u32(0x80065128u) ^ 1u;
    uint32 offset = (((next << 3) - next) << 2) + next;
    offset <<= 2;
    w_u32(0x80065128u, next);
    w_u32(0x80065118u, 0x80081A38u + offset);
    return 0x80081A38u;
}
