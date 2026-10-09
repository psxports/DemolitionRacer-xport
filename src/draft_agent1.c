#include "draft_signatures.h"
#include <string.h>

/* Unverified draft bodies; dependencies remain declarations */
/* TODO: Supply native-buffer adapters without guest stack emulation */
sint32 draft_agent1_missing_draw_mode(sint32 mode);
sint32 draft_agent1_missing_draw_sync(sint32 mode);
sint32 draft_agent1_missing_load_image(const sint16 *rect, uint32 pixels);
sint32 draft_agent1_missing_clear_image(const sint16 *rect, uint32 red, uint32 green, uint32 blue);
uint32 draft_agent1_missing_vsync_callback(uint32 callback);
void draft_agent1_missing_store_image(uint32 rect, uint32 pixels);
void draft_agent1_missing_palette_update(uint32 colors, uint32 x, uint32 y);
sint32 draft_agent1_missing_cd_pause(sint32 mode);
sint32 CdReset(sint32 mode);
sint32 draft_agent1_missing_cd_command(uint32 command, const uint8 *parameters, uint8 *result);
sint32 draft_agent1_missing_cd_command_wait(uint32 command, const uint8 *parameters);
sint32 CdControlBPSX(uint8 command, uint8 *parameters, uint8 *result);
sint32 draft_agent1_missing_cd_position_to_sector(const uint8 *position);
void draft_agent1_missing_cd_sector_to_position(uint32 sector, uint8 *position);
sint32 draft_agent1_missing_diagnostic(const char *format, ...);
uint32 draft_agent1_missing_add_primitives(uint32 ordering_table, uint32 primitive);
uint32 draft_agent1_missing_event_operation(uint32 event);
uint32 draft_agent1_missing_bios_b0_42(uint32 pattern, uint32 directory_entry);

static void draft_agent1_volume(uint8 volume[4])
{
    uint32 word = r_u32(0x80064D34u);
    sint32 level = (sint32)r_u32(0x80064BF8u) / 16;
    volume[0] = (uint8)word;
    volume[1] = (uint8)(word >> 8);
    volume[2] = (uint8)(word >> 16);
    volume[3] = (uint8)(word >> 24);
    if (level < 0) level = 0;
    if (level > 255) level = 255;
    volume[0] = volume[2] = (uint8)level;
}

sint32 sub_80015238(uint32 image)
{
    uint32 index;
    draft_agent1_missing_draw_mode(1);
    for (index = 0; index < 2u; ++index) {
        uint32 record = image + 20u * index;
        sint16 rect[4];
        rect[0] = (sint16)r_u32(record);
        rect[1] = (sint16)r_u32(record + 4u);
        rect[2] = (sint16)r_u32(record + 8u);
        rect[3] = (sint16)r_u32(record + 12u);
        draft_agent1_missing_load_image(rect, image + r_u32(record + 16u));
    }
    return draft_agent1_missing_draw_sync(0);
}

sint32 sub_8001993C(uint32 offset, uint32 clut)
{
    w_u16(0x80064C78u, (uint16)((clut & 63u) << 4));
    w_u16(0x80064C7Au, (uint16)((sint16)clut >> 6));
    draft_agent1_missing_store_image(0x80064C78u, 0x800739ECu);
    draft_agent1_missing_palette_update(0x800739ECu, r_u32(0x80064C70u), r_u32(0x80064C74u) + offset);
    return (sint16)(((r_u32(0x80064C74u) + offset) << 6) |
        (((sint32)r_u32(0x80064C70u) >> 4) & 63));
}

sint32 sub_8001FEF4(void)
{
    return draft_agent1_missing_cd_pause(1);
}

uint32 sub_8001FF98(void)
{
    uint8 volume[4];
    draft_agent1_volume(volume);
    return sub_8004B89C(volume);
}

sint32 sub_800200E0(void)
{
    uint32 previous = r_u32(0x80064D38u);
    sint32 result = (sint32)previous < 61;
    w_u32(0x80064D38u, previous + 1u);
    if (!result) {
        uint8 *status = (uint8 *)psx_addr(0x800652D0u, 8u);
        w_u32(0x80064D38u, 0);
        draft_agent1_missing_cd_command(1, NULL, status);
        result = r_u8(0x800652D0u) & 32;
        if (result) {
            uint8 position[8];
            sint32 sector;
            draft_agent1_missing_cd_command(16, NULL, status);
            position[0] = r_u8(0x800652D0u);
            position[1] = r_u8(0x800652D1u);
            position[2] = r_u8(0x800652D2u);
            position[3] = 0;
            sector = draft_agent1_missing_cd_position_to_sector(position);
            w_u32(0x800652BCu, (uint32)sector);
            result = (sint32)r_u32(0x800652B4u) < sector;
            if (!result) result = sector < (sint32)r_u32(0x800652B0u);
            if (result) {
                uint8 parameters[8], restart[8];
                uint32 start = r_u32(0x800652C4u);
                uint32 end = r_u32(0x800652C8u);
                uint32 channel = r_u32(0x800652C0u);
                parameters[0] = 1;
                parameters[1] = (uint8)channel;
                w_u32(0x800652B0u, start);
                w_u32(0x800652B4u, end);
                w_u32(0x800652BCu, start);
                w_u32(0x800652B8u, channel);
                draft_agent1_missing_cd_command_wait(13, parameters);
                draft_agent1_missing_cd_sector_to_position(r_u32(0x800652B0u), restart);
                return draft_agent1_missing_cd_command_wait(27, restart);
            }
        }
    }
    return result;
}

sint32 sub_800201EC(uint32 path, uint32 channel)
{
    uint8 file[24], mode[8], parameters[8], position[8], volume[4];
    uint32 start, end, track;
    sint32 bytes;
    CdReset(1);
    if (!CdSearchFile((CdlFILE *)file, (const char *)psx_addr(path, 1u)))
        return draft_agent1_missing_diagnostic("Got here: PlayXA\n");
    start = (uint32)draft_agent1_missing_cd_position_to_sector(file);
    track = r_u32(0x800652CCu);
    bytes = (sint32)r_u32(0x80059C18u + 20u * track);
    w_u32(0x800652C4u, start);
    w_u32(0x800652C0u, channel);
    mode[0] = 0xCC;
    end = start + 8u * (uint32)(bytes / 2336);
    w_u32(0x800652C8u, end);
    CdControlBPSX(14, mode, NULL);
    w_u32(0x800652B0u, start);
    w_u32(0x800652B4u, end);
    w_u32(0x800652BCu, start);
    w_u32(0x800652B8u, channel);
    parameters[0] = 1;
    parameters[1] = (uint8)channel;
    draft_agent1_missing_cd_command_wait(13, parameters);
    draft_agent1_missing_cd_sector_to_position(r_u32(0x800652B0u), position);
    draft_agent1_missing_cd_command_wait(27, position);
    draft_agent1_volume(volume);
    sub_8004B89C(volume);
    return sub_8001FEF4();
}

sint32 sub_80023FDC(uint32 index)
{
    uint32 row = 0x80074700u + 4396u * index;
    uint32 count;
    for (count = 0; count < 15u; ++count, row += 292u)
        if ((sint16)r_u16(row) == 0) return 0;
    return 1;
}

uint32 sub_8002B6CC(void)
{
    uint32 pixels, page;
    w_u32(0x80065128u, 0);
    sub_8004225C("PRES\\CD.TIM", r_u32(0x80065344u), 10240u);
    pixels = r_u32(0x80065344u) + 20u;
    page = r_u32(0x80065218u);
    w_u32(0x80065344u, pixels);
    w_u16(0x800650AAu, (uint16)((page << 8) + 128u));
    draft_agent1_missing_load_image((const sint16 *)psx_addr(0x800650A8u, 8u), pixels);
    draft_agent1_missing_draw_sync(0);
    w_u8(0x80078C5Bu, 9); w_u8(0x80078C5Fu, 44);
    w_u8(0x80078C75u, 191); w_u8(0x80078C7Du, 191);
    w_u8(0x80078C6Cu, 63); w_u8(0x80078C7Cu, 63);
    w_u16(0x80078C62u, 180); w_u16(0x80078C6Au, 180);
    w_u8(0x80078C64u, 0); w_u8(0x80078C65u, 128);
    w_u8(0x80078C6Du, 128); w_u8(0x80078C74u, 0);
    w_u16(0x80078C60u, 256); w_u16(0x80078C68u, 319);
    w_u16(0x80078C70u, 256); w_u16(0x80078C72u, 243);
    w_u16(0x80078C78u, 319); w_u16(0x80078C7Au, 243);
    w_u8(0x80078C5Cu, 128); w_u8(0x80078C5Du, 128); w_u8(0x80078C5Eu, 128);
    page = r_u32(0x80065218u) << 8;
    w_u16(0x80078C6Eu, (uint16)(((page & 256u) >> 4) | 256u | ((page & 512u) << 2)));
    return draft_agent1_missing_vsync_callback(0x8002B6ACu);
}

sint32 sub_8002B840(void)
{
    sint16 rect[4] = {0, 0, 320, 512};
    draft_agent1_missing_vsync_callback(0);
    return draft_agent1_missing_clear_image(rect, 0, 0, 0);
}

void sub_8003970C(uint32 value)
{
    w_u32(0x80065134u, value);
}

uint32 sub_80039908(void)
{
    return draft_agent1_missing_add_primitives(r_u32(r_u32(0x80065118u) + 112u), r_u32(0x800654F0u));
}

uint32 dr_texture_lookup(const char *name)
{
    uint32 record = r_u32(0x800654F4u);
    for (;;) {
        uint32 offset;
        sint32 matched = 1;
        for (offset = 0; offset < 8u; ++offset) {
            uint32 left = r_u8(record + offset), right = (uint8)name[offset];
            if (!left) { matched = !right; break; }
            if (!right) { matched = 0; break; }
            if (left >= 97u) left = (left - 32u) & 255u;
            if (right >= 97u) right = (right - 32u) & 255u;
            if (left != right) { matched = 0; break; }
        }
        if (matched) return record;
        if (!r_u8(record)) {
            draft_agent1_missing_diagnostic("couldn't find texture %s\n", name);
            return record - 26u;
        }
        record += 26u;
    }
}

uint32 sub_80039ADC(uint32 name)
{
    return dr_texture_lookup((const char *)psx_addr(name, 1u));
}

void sub_8003BC14(void)
{
}

sint32 sub_80041D50(const char *path)
{
    uint32 index = 0;
    while ((sint32)index < (sint32)r_u32(0x80065180u)) {
        if (!strcmp(path, (const char *)psx_addr(0x800839D4u + 40u * index, 1u)))
            return (sint32)index;
        ++index;
    }
    draft_agent1_missing_diagnostic("can't find file %s\n", path);
    return -1;
}

uint32 sub_80042488(const char *path, uint32 enabled)
{
    if (!enabled) return 0;
    return r_u32(0x800839F4u + 40u * (uint32)sub_80041D50(path));
}

uint32 sub_8004250C(uint32 heap, uint32 size)
{
    uint32 state = r_u32(0x80065184u);
    w_u32(state, heap);
    w_u32(heap, heap);
    heap = r_u32(state);
    w_u32(heap + 4u, size >> 4);
    heap = r_u32(state);
    w_u32(heap + 8u, 0x12345678u);
    w_u32(state + 4u, size);
    w_u32(state + 8u, size);
    return 0x12345678u;
}

uint32 sub_80042E98(uint32 selector)
{
    if (!selector) return draft_agent1_missing_event_operation(0xF2000002u);
    if (selector == 1u) return draft_agent1_missing_event_operation(0xF2000001u);
    return 1u;
}

uint32 sub_80058548(uint32 pattern, uint32 directory_entry)
{
    /* TODO: Provisional BIOS B0:0x42 ABI needs caller review */
    return draft_agent1_missing_bios_b0_42(pattern, directory_entry);
}
