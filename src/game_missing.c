#include "draft_signatures.h"
#include "psx_gpu.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdint.h>

typedef struct DraftVertex { sint16 x, y, z, padding; } DraftVertex;
uint32 draft_agent3_missing_8003DE98(uint32 ordering_table, uint32 entries);
uint32 draft_agent3_missing_8004BB74(const void *attributes);

static void dr_missing(const char *name)
{
    fprintf(stderr, "Missing native implementation: %s\n", name);
    xport_message_error("Demolition Racer: missing implementation", name);
    abort();
}

/* Reject chains that the synchronous software backend cannot complete */
static void dr_validate_gpu_packet(uint32 address, uint32 words)
{
    uint32 command = r_u32(address + 4u) >> 24, minimum = 0u;
    /* A packet of only GP0 NOP words has no GPU effect */
    if (command == 0u) {
        uint32 index;
        for (index = 1u; index <= words; ++index) {
            if ((r_u32(address + index * 4u) >> 24) != 0u)
                dr_missing("Mixed commands after GPU NOP");
        }
        return;
    }
    if (command >= 0xE1u && command <= 0xE6u) {
        uint32 index = 1u;
        while (index <= words) {
            command = r_u32(address + index * 4u) >> 24;
            if (command < 0xE1u || command > 0xE6u) break;
            ++index;
        }
        if (index > words) return;
        if ((command & 0xFCu) != 0x60u || words - index + 1u != 3u)
            dr_missing("Unsupported commands after GPU drawing environment");
        return;
    }
    /* GP0 VRAM copy consumes command, source, destination and dimensions */
    if ((command & 0xE0u) == 0x80u) {
        if (words != 4u) dr_missing("Invalid GPU VRAM copy word count");
        return;
    }
    switch (command & 0xFCu) {
        case 0x20u: minimum = 4u; break;
        case 0x28u: minimum = 5u; break;
        case 0x30u: minimum = 6u; break;
        case 0x38u: minimum = 8u; break;
        case 0x24u: minimum = 7u; break;
        case 0x2Cu: minimum = 9u; break;
        case 0x34u: minimum = 9u; break;
        case 0x3Cu: minimum = 12u; break;
        case 0x40u: minimum = 3u; break;
        case 0x64u: minimum = 4u; break;
        case 0x6Cu: case 0x74u: case 0x7Cu: minimum = 3u; break;
        case 0x60u: minimum = 3u; break;
        case 0x68u: case 0x70u: case 0x78u: minimum = 2u; break;
        default: dr_missing("Unsupported GPU primitive command"); return;
    }
    if (words != minimum) dr_missing("Invalid GPU primitive word count");
}

static void dr_validate_gpu_chain(uint32 head)
{
    uint32 count = 0u;
    while ((head & 0xFFFFFFu) != 0xFFFFFFu) {
        uint32 address = head & 0xFFFFFFu, tag, bytes, limit;
        if ((address & 3u) || ++count > 65536u) dr_missing("Invalid or cyclic GPU ordering table");
        if (address < PSX_DRAM_SIZE) limit = PSX_DRAM_SIZE;
        else if (address >= 0x800000u && address < 0x800400u) {
            address += 0x1F000000u;
            limit = 0x1F800400u;
        } else dr_missing("GPU link does not address RAM or scratchpad");
        tag = r_u32(address);
        bytes = ((tag >> 24) + 1u) * 4u;
        if (bytes > limit - address) dr_missing("GPU packet exceeds its memory region");
        if (tag >> 24) dr_validate_gpu_packet(address, tag >> 24);
        head = tag;
    }
}

sint32 draft_agent1_missing_draw_mode(sint32 mode)
{
    if (!mode) memset(psx_addr(0x8005CC94u, 20u), 0xFF, 20u);
    SetDispMask(mode);
    w_u8(0x80081CCFu, mode ? 0u : 1u);
    return 3;
}

sint32 draft_agent1_missing_draw_sync(sint32 mode)
{
    return DrawSync(mode);
}

sint32 draft_agent1_missing_load_image(const sint16 *rect, uint32 pixels)
{
    return LoadImagePSX((PSX_RECT *)rect, (uint32 *)psx_addr(pixels, 1u));
}

sint32 draft_agent1_missing_clear_image(const sint16 *rect, uint32 red, uint32 green, uint32 blue)
{
    return ClearImage((PSX_RECT *)rect, (uint8)red, (uint8)green, (uint8)blue);
}

uint32 draft_agent1_missing_vsync_callback(uint32 callback)
{
    return VSyncCallbackPSX(callback);
}

void draft_agent1_missing_store_image(uint32 rect, uint32 pixels)
{
    StoreImage((PSX_RECT *)psx_addr(rect, sizeof(PSX_RECT)), (uint32 *)psx_addr(pixels, 1u));
}

void draft_agent1_missing_palette_update(uint32 colors, uint32 x, uint32 y)
{
    LoadClut((uint32 *)psx_addr(colors, 1u), (sint32)x, (sint32)y);
}

sint32 draft_agent1_missing_cd_pause(sint32 mode)
{
    sint32 enabled;
    if (mode != 0 && mode != 1) return (sint32)r_u32(0x8005FF04u);
    enabled = SpuSetReverb(mode);
    w_u32(0x8005FF04u, (uint32)enabled);
    return enabled;
}

sint32 draft_agent1_missing_cd_command(uint32 command, const uint8 *parameters, uint8 *result)
{
    return CdControl((uint8)command, (uint8 *)parameters, result);
}

sint32 draft_agent1_missing_cd_command_wait(uint32 command, const uint8 *parameters)
{
    return CdControlF((uint8)command, (uint8 *)parameters);
}

sint32 draft_agent1_missing_cd_position_to_sector(const uint8 *position)
{
    uint32 minutes = (position[0] >> 4) * 10u + (position[0] & 15u);
    uint32 seconds = (position[1] >> 4) * 10u + (position[1] & 15u);
    uint32 frames = (position[2] >> 4) * 10u + (position[2] & 15u);
    return (sint32)((minutes * 60u + seconds) * 75u + frames - 150u);
}

void draft_agent1_missing_cd_sector_to_position(uint32 sector, uint8 *position)
{
    sint32 absolute = (sint32)(sector + 150u);
    sint32 frames = absolute % 75, seconds = absolute / 75 % 60, minutes = absolute / 75 / 60;
    position[2] = (uint8)(((uint32)(frames / 10) << 4) + (uint32)(frames % 10));
    position[1] = (uint8)(((uint32)(seconds / 10) << 4) + (uint32)(seconds % 10));
    position[0] = (uint8)(((uint32)(minutes / 10) << 4) + (uint32)(minutes % 10));
}

sint32 draft_agent1_missing_diagnostic(const char *format, ...)
{
    va_list arguments;
    sint32 result;
    va_start(arguments, format);
    result = vprintf(format, arguments);
    va_end(arguments);
    return result;
}

uint32 draft_agent1_missing_add_primitives(uint32 ordering_table, uint32 primitive)
{
    return draft_agent3_missing_8003DE98(ordering_table, primitive);
}

uint32 draft_agent1_missing_event_operation(uint32 event)
{
    return (uint32)ResetRCnt(event);
}

uint32 draft_agent1_missing_bios_b0_42(uint32 pattern, uint32 directory_entry)
{
    return (uint32)(uintptr_t)firstfile((const char *)psx_addr(pattern, 1u), psx_addr(directory_entry, 1u));
}

sint32 draft_agent1_missing_8003367C(sint32 x, sint32 y, sint32 coordinate)
{
    return sub_8003367C(x, y, coordinate);
}

void draft_agent1_missing_80035308(uint32 table, uint32 parameter, uint32 coordinates, uint32 output)
{
    sub_80035308(table, parameter, coordinates, output);
}

uint32 draft_agent2_missing_800182D8(uint32 index, uint32 object, uint32 data, uint32 offset)
{
    return sub_800182D8(index, object, data, offset);
}

uint32 draft_agent2_missing_draw_otag_result(uint32 ordering_table)
{
    dr_validate_gpu_chain(ordering_table);
    DrawOTag((uint32 *)psx_addr(ordering_table, 4u));
    if (gpu_linked_submit_failed() || gpu_transfer_observation_failed())
        dr_missing("GPU submission did not complete");
    /* Native synchronous rasterization has no pending queue after completion */
    /* TODO: Original asynchronous SDK V0 carrier remains outside this policy */
    return 0u;
}

uint32 draft_agent2_missing_spu_voice_attr_result(SpuVoiceAttr *attributes)
{
    return draft_agent3_missing_8004BB74(attributes);
}

uint32 draft_agent2_missing_8004D9D4(uint32 port)
{
    return (uint32)_card_info((sint32)port);
}

uint32 draft_agent2_missing_8004DCF4(uint32 event)
{
    return (uint32)TestEvent(event);
}

uint32 draft_agent2_missing_8004DC84(uint32 port)
{
    _new_card();
    return psx_bios_card_write_guest(port, 63u, 0u);
}

uint32 draft_agent2_missing_8004DFD4(uint32 port)
{
    return psx_bios_card_load_guest(port);
}

uint32 draft_agent3_missing_8003DC58(uint32 rectangle, const void *pixels)
{
    return (uint32)LoadImagePSX((PSX_RECT *)psx_addr(rectangle, sizeof(PSX_RECT)), (uint32 *)pixels);
}

sint32 draft_agent3_missing_800437EC(uint32 angle)
{
    uint32 index;
    sint32 value;
    if ((sint32)angle < 0) angle = 0u - angle;
    angle &= 4095u;
    if (angle <= 1024u) index = 1024u - angle;
    else if (angle <= 2048u) index = angle - 1024u;
    else if (angle <= 3072u) index = 3072u - angle;
    else index = angle - 3072u;
    value = (sint16)r_u16(0x8005F358u + index * 2u);
    return angle > 1024u && angle <= 3072u ? -value : value;
}

sint32 draft_agent3_missing_8004388C(uint32 angle)
{
    uint32 negative = (sint32)angle < 0, index;
    sint32 value;
    if (negative) angle = 0u - angle;
    angle &= 4095u;
    if (angle <= 1024u) index = angle;
    else if (angle <= 2048u) index = 2048u - angle;
    else if (angle <= 3072u) index = angle - 2048u;
    else index = 4096u - angle;
    value = (sint16)r_u16(0x8005F358u + index * 2u);
    if (angle > 2048u) value = -value;
    return negative ? -value : value;
}

uint32 draft_agent3_missing_80049F84(uint32 enabled, uint32 voices)
{
    uint32 mask = voices & 0xFFFFFFu, result, opposite, queued, upper;
    if (enabled > 1u) return 1u;
    if (!(r_u32(0x8005FF5Cu) & 1u)) {
        SpuSetKey((sint32)enabled, mask);
        result = enabled ? r_u32(0x8005FEFCu) | mask : r_u32(0x8005FEFCu) & ~mask;
        w_u32(0x8005FEFCu, result);
        return result;
    }
    queued = enabled ? 0x8008B398u : 0x8008B39Cu;
    opposite = enabled ? 0x8008B39Cu : 0x8008B398u;
    w_u32(queued, mask);
    w_u32(0x8005FF28u, r_u32(0x8005FF28u) | 1u);
    w_u32(0x8005FF24u, enabled ? r_u32(0x8005FF24u) | mask : r_u32(0x8005FF24u) & ~mask);
    if (r_u16(opposite) & (uint16)mask) w_u16(opposite, (uint16)(r_u16(opposite) & ~(uint16)mask));
    upper = mask >> 16;
    result = r_u16(opposite + 2u) & upper;
    if (result) {
        result = r_u16(opposite + 2u) & ~upper;
        w_u16(opposite + 2u, (uint16)result);
    }
    return result;
}

uint32 draft_agent3_missing_8004AD94(uint32 bytes)
{
    return (uint32)SpuMalloc((sint32)bytes);
}

uint32 draft_agent3_missing_8004A224(uint32 mode)
{
    return (uint32)SpuSetTransferMode((sint32)mode);
}

uint32 draft_agent3_missing_8004A254(uint32 address)
{
    return SpuSetTransferStartAddr(address);
}

uint32 draft_agent3_missing_8004BAC4(uint32 mode)
{
    return (uint32)SpuIsTransferCompleted((sint32)mode);
}

uint32 draft_agent3_missing_8001F860(void)
{
    uint32 voice = r_u32(0x80064D30u);
    while ((sint32)voice < 24) {
        if (!r_u16(0x80073FBAu + (voice << 5))) {
            w_u32(0x80064D30u, voice + 1u);
            return voice;
        }
        ++voice;
    }
    voice = 0;
    while ((sint32)voice < (sint32)r_u32(0x80064D30u)) {
        if (!r_u16(0x80073FBAu + (voice << 5))) {
            w_u32(0x80064D30u, voice + 1u);
            return voice;
        }
        ++voice;
    }
    return 0xFFFFFFFFu;
}

uint32 draft_agent3_missing_8001FBE4(uint32 voice, sint32 volume, uint32 mode, sint32 pitch)
{
    uint32 sound = (uint32)(sint32)(sint16)r_u16(0x80073FB8u + (voice << 5));
    sint32 scaled = (sint32)(r_u32(0x800742E4u + sound * 12u) * (uint32)volume) / 44100;
    uint32 left = (uint32)pitch, right = (uint32)pitch;
    sint32 delta = (sint32)((uint32)pitch * mode) / 4096;
    sint32 left_volume, right_volume;
    if (scaled >= 0x4000) scaled = 0x3FFF;
    if ((sint32)mode < 0) right += (uint32)delta;
    else left -= (uint32)delta;
    left_volume = (sint32)(left << 2);
    right_volume = (sint32)(right << 2);
    if (left_volume < -16384) left_volume = -16384;
    else if (left_volume >= 16384) left_volume = 16383;
    if (right_volume < -16384) right_volume = -16384;
    else if (right_volume >= 16384) right_volume = 16383;
    w_u32(0x800742A4u, 19u);
    w_u32(0x800742A0u, 1u << (voice & 31u));
    w_u16(0x800742B4u, (uint16)scaled);
    w_u16(0x800742A8u, (uint16)left_volume);
    w_u16(0x800742AAu, (uint16)right_volume);
    return draft_agent3_missing_8004BB74(psx_addr(0x800742A0u, 1u));
}

uint32 draft_agent3_missing_8004BB74(const void *attributes)
{
    SpuSetVoiceAttr((SpuVoiceAttr *)attributes);
    /* Original final loop leaves SLTI(iteration, 2) equal to zero */
    return 0u;
}

uint32 draft_agent3_missing_8003D9AC(uint32 mode)
{
    return (uint32)DrawSync((sint32)mode);
}

uint32 draft_agent3_missing_8003D914(uint32 mode)
{
    return (uint32)draft_agent1_missing_draw_mode((sint32)mode);
}

uint32 draft_agent3_missing_8003DE98(uint32 ordering_table, uint32 entries)
{
    ClearOTagR((uint32 *)psx_addr(ordering_table, 1u), (sint32)entries);
    w_u32(0x8005CCE8u, 0x0405CCD4u);
    w_u32(ordering_table, 0x0005CCE8u);
    return ordering_table;
}

void draft_agent3_missing_8003DD18(const sint16 *rectangle, uint32 x, uint32 y)
{
    MoveImage((PSX_RECT *)rectangle, (sint32)x, (sint32)y);
}

void draft_agent3_missing_8003DF44(uint32 packet)
{
    DrawSync(0);
    DrawPrim(psx_addr(packet, 1u));
}

void draft_agent3_missing_8004FB3C(uint32 red, uint32 green, uint32 blue)
{
    SetBackColor((sint32)red, (sint32)green, (sint32)blue);
}

void draft_agent3_missing_80049A64(const MATRIX *left, const MATRIX *right, MATRIX *out)
{
    MulMatrix0((MATRIX *)left, (MATRIX *)right, out);
}

void draft_agent3_missing_80049B74(const MATRIX *left, MATRIX *right)
{
    MulMatrix0((MATRIX *)left, right, right);
}

void draft_agent3_missing_80049C84(const MATRIX *matrix)
{
    SetRotMatrix((MATRIX *)matrix);
}

void draft_agent3_missing_8004FB0C(const MATRIX *matrix)
{
    SetLightMatrix((MATRIX *)matrix);
}

void draft_agent3_missing_8004FB5C(const sint16 *light_matrix)
{
    SetColorMatrix((MATRIX *)light_matrix);
}

void draft_agent3_missing_800556CC(uint32 source, uint32 bytes)
{
    SpuWrite(psx_addr(source, 1u), bytes);
}

void sdk_model_80050D50(uint32 model);
uint32 scene_800456AC(uint32 packet);
uint32 scene_8004FDDC(uint32 packet);
void scene_8004FCBC(uint32 packet);
void scene_80050700(uint32 packet);

void draft_agent3_missing_dispatch_packet(uint32 target, uint32 packet)
{
    if (target == 0x8004FCBCu) { scene_8004FCBC(packet); return; }
    if (target == 0x80050700u) { scene_80050700(packet); return; }
    if (target == 0x8004FDDCu) { scene_8004FDDC(packet); return; }
    if (target == 0x800456ACu) { scene_800456AC(packet); return; }
    if (target == 0x80050D50u)
    {
        sdk_model_80050D50(packet);
        return;
    }
    /* TODO: Translate each remaining renderer from its complete original CFG */
    fprintf(stderr, "Untranslated packet renderer %08X for model %08X\n", target, packet);
    dr_missing("Untranslated packet renderer");
}

uint32 draft_agent3_missing_first_file(const char *pattern, uint8 *entry)
{
    return (uint32)(uintptr_t)firstfile(pattern, entry);
}

uint32 draft_agent3_missing_next_file(uint8 *entry)
{
    return (uint32)(uintptr_t)nextfile(entry);
}

void draft_agent3_missing_8004DCC4(void *destination, const void *source)
{
    strcat((char *)destination, (const char *)source);
}

void draft_agent3_missing_8004DD04(void *destination, const void *source)
{
    strcpy((char *)destination, (const char *)source);
}

sint32 draft_agent3_missing_compare_name(const char *left, const char *right, uint32 count)
{
    return strncmp(left, right, count);
}

void draft_agent3_missing_project_three(const DraftVertex *vertices, uint32 *screen, uint32 *flags)
{
    sint32 depth[3], output_flags;
    gte_project3_full_depth((const SVECTOR *)vertices, (sint32 *)screen, depth, &output_flags);
    if (flags) *flags = (uint32)output_flags;
}

uint32 draft_agent3_missing_project_one(const DraftVertex *vertex)
{
    sint32 screen;
    gte_project_full_depth((const SVECTOR *)vertex, &screen, NULL);
    return (uint32)screen;
}

sint32 draft_agent3_missing_normal_clip(void)
{
    PsxGteSnapshot state;
    psx_gte_snapshot(&state);
    return NormalClip(state.sxy[0], state.sxy[1], state.sxy[2]);
}

sint32 draft_agent3_missing_average_depth_four(void)
{
    PsxGteSnapshot state;
    psx_gte_snapshot(&state);
    return AverageZ4(state.sz[0], state.sz[1], state.sz[2], state.sz[3]);
}

void draft_agent3_missing_rotate_vertex(const DraftVertex *vertex, sint32 *transformed)
{
    xport_gte_write_data(0u, (uint32)(uint16)vertex->x | (uint32)(uint16)vertex->y << 16);
    xport_gte_write_data(1u, (uint32)(uint16)vertex->z);
    xport_gte_execute(0x480012u);
    transformed[0] = (sint32)xport_gte_read_data(25u);
    transformed[1] = (sint32)xport_gte_read_data(26u);
    transformed[2] = (sint32)xport_gte_read_data(27u);
}

void draft_agent3_missing_set_translation(const sint32 *translation)
{
    xport_gte_write_control(5u, (uint32)translation[0]);
    xport_gte_write_control(6u, (uint32)translation[1]);
    xport_gte_write_control(7u, (uint32)translation[2]);
}

uint32 draft_agent3_missing_light_color(uint32 packed_color, const DraftVertex *normal)
{
    return gte_normal_color_col_depth((const SVECTOR *)normal, packed_color);
}

extern void pres_8008FD38(void);
extern void pres_80092410(void);

void dr_overlay_call(uint32 entry)
{
    char message[128];
    if (entry == 0x8008FD38u) { pres_8008FD38(); return; }
    if (entry == 0x80092410u) { pres_80092410(); return; }
    sprintf(message, "PRES.BIN entry 0x%08X has no native translation", entry);
    dr_missing(message);
}

uint32 sub_80057160(uint32 owner, sint32 value)
{
    return (uint32)psx_pad_serial_transfer(owner, value, r_u32(0x800605A8u), 0u, -20);
}

/* Original SLUS SDK storage: 4B4B8, 4B4CC, 4B61C and 4BB74 */
const uint32 xport_cd_ready_callback_address = 0x8005FBD4u;
const uint32 xport_cd_sync_callback_address = 0x8005FBD0u;
const uint32 xport_spu_register_pointer_address = 0x8005FF70u;
const uint32 xport_cd_status_address = 0x8005FBDCu;
const uint32 xport_cd_setloc_table_address = 0x8005E848u;

/* TODO: Translate these complete PRES menu dependencies before entering the menu */
/* Original leaf returns without work */
void pres_80012D48(void) { }

extern void pres_8009880C(void);
extern void pres_80093CC0(void);
extern void pres_8009A4A4(void);
extern void pres_80096E38(void);
extern void pres_800977B4(void);
extern void pres_800964DC(void);
extern void pres_8009686C(void);
extern void pres_80098404(void);
extern void pres_800985A8(void);
extern void pres_80099268(void);
extern void pres_80099610(void);
extern void pres_8009AE08(void);
extern void pres_80094144(void);
extern void pres_80094238(void);
extern void pres_8009748C(void);
extern void pres_800983F4(void);
extern void pres_800991DC(void);
extern void pres_8009B270(void);
extern void pres_80094BF4(void);
extern void pres_80095484(void);
extern void pres_8009B2A8(void);
void pres_dispatch_callback(uint32 target)
{
    if (target == 0x80094144u) { pres_80094144(); return; }
    if (target == 0x80094238u) { pres_80094238(); return; }
    if (target == 0x8009748Cu) { pres_8009748C(); return; }
    if (target == 0x800983F4u) { pres_800983F4(); return; }
    if (target == 0x800991DCu) { pres_800991DC(); return; }
    if (target == 0x8009B270u) { pres_8009B270(); return; }
    if (target == 0x80094BF4u) { pres_80094BF4(); return; }
    if (target == 0x80095484u) { pres_80095484(); return; }
    if (target == 0x8009B2A8u) { pres_8009B2A8(); return; }
    if (target == 0x8009880Cu) { pres_8009880C(); return; }
    if (target == 0x800964DCu) { pres_800964DC(); return; }
    if (target == 0x8009686Cu) { pres_8009686C(); return; }
    if (target == 0x80098404u) { pres_80098404(); return; }
    if (target == 0x800985A8u) { pres_800985A8(); return; }
    if (target == 0x80099268u) { pres_80099268(); return; }
    if (target == 0x80099610u) { pres_80099610(); return; }
    if (target == 0x8009AE08u) { pres_8009AE08(); return; }
    if (target == 0x800977B4u) { pres_800977B4(); return; }
    if (target == 0x80096E38u) { pres_80096E38(); return; }
    if (target == 0x8009A4A4u) { pres_8009A4A4(); return; }
    if (target == 0x80093CC0u) { pres_80093CC0(); return; }
    dr_missing("Untranslated PRES descriptor callback");
}

/* TODO: Translate the remaining direct menu helper dependencies */

extern void pres_80098814(uint32 object);
extern void pres_80093D1C(uint32 object);
extern void pres_8009A8CC(uint32 object);
extern void pres_80096ECC(uint32 object);
extern void pres_80097A78(uint32 object);
extern void pres_80096608(uint32 object);
extern void pres_80096AD4(uint32 object);
extern void pres_8009847C(uint32 object);
extern void pres_800986F4(uint32 object);
extern void pres_80099508(uint32 object);
extern void pres_800998E4(uint32 object);
extern void pres_8009B094(uint32 object);

extern void pres_8009414C(uint32 object);
extern void pres_80094240(uint32 object);
extern void pres_80097494(uint32 object);
extern void pres_800983FC(uint32 object);
extern void pres_800991E4(uint32 object);
extern void pres_8009B278(uint32 object);
extern void pres_80094E4C(uint32 object);
extern void pres_800957C0(uint32 object);
extern void pres_8009B2C8(uint32 object);
void pres_dispatch_callback1(uint32 target, uint32 object)
{
    char message[128];
    if (target == 0x8009414Cu) { pres_8009414C(object); return; }
    if (target == 0x80094240u) { pres_80094240(object); return; }
    if (target == 0x80097494u) { pres_80097494(object); return; }
    if (target == 0x800983FCu) { pres_800983FC(object); return; }
    if (target == 0x800991E4u) { pres_800991E4(object); return; }
    if (target == 0x8009B278u) { pres_8009B278(object); return; }
    if (target == 0x80094E4Cu) { pres_80094E4C(object); return; }
    if (target == 0x800957C0u) { pres_800957C0(object); return; }
    if (target == 0x8009B2C8u) { pres_8009B2C8(object); return; }
    if (target == 0x80098814u) { pres_80098814(object); return; }
    if (target == 0x80096608u) { pres_80096608(object); return; }
    if (target == 0x80096AD4u) { pres_80096AD4(object); return; }
    if (target == 0x8009847Cu) { pres_8009847C(object); return; }
    if (target == 0x800986F4u) { pres_800986F4(object); return; }
    if (target == 0x80099508u) { pres_80099508(object); return; }
    if (target == 0x800998E4u) { pres_800998E4(object); return; }
    if (target == 0x8009B094u) { pres_8009B094(object); return; }
    if (target == 0x80097A78u) { pres_80097A78(object); return; }
    if (target == 0x80096ECCu) { pres_80096ECC(object); return; }
    if (target == 0x8009A8CCu) { pres_8009A8CC(object); return; }
    if (target == 0x80093D1Cu) { pres_80093D1C(object); return; }
    sprintf(message, "TODO: Translate PRES callback1 0x%08X", target);
    dr_missing(message);
}

/* The synchronous native serial service completes the original ACK-release wait */
uint32 sub_80057570(void)
{
    return psx_pad_serial_release_ack(r_u32(0x800605A8u));
}

/* Return the original RX-ready status bit after the actual native transfer */
uint32 sub_80057600(void)
{
    uint32 ready;
    do { ready = psx_pad_serial_ready(r_u32(0x800605A8u)); } while (ready == 0u);
    return ready;
}

/* TODO: Replace these explicit stops with their complete PRES text translations */
