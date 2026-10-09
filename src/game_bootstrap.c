#include "draft_signatures.h"
#include "psx_spu.h"
#include "psx_gpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

uint32 sub_80041694(uint32 first_name, uint32 second_name)
{
    uint32 texture = sub_80039ADC(first_name);
    w_u32(0x80065568u, texture);
    texture = sub_80039ADC(second_name);
    w_u32(0x8006556Cu, texture);
    return texture;
}

static uint32 dr_tim_blocks(uint32 tim, uint32 *palette_pixels, uint32 *image_pixels)
{
    uint32 block, palette_words = 0u, image_words;
    w_u32(0x800889E0u, tim);
    if (r_u32(tim) != 16u) {
        /* The original would consume an uninitialized TIM result after this failure */
        fprintf(stderr, "TODO: Invalid model TIM at %08X\n", tim);
        abort();
    }
    block = tim + 8u;
    if (r_u32(tim + 4u) & 8u) {
        palette_words = r_u32(block) >> 2;
        *palette_pixels = block + 12u;
        block += palette_words << 2;
    } else *palette_pixels = 0u;
    image_words = r_u32(block) >> 2;
    *image_pixels = block + 12u;
    w_u32(0x800889E0u, tim + ((palette_words + image_words + 2u) << 2));
    return 1u;
}

static void dr_model_texture_position(uint32 index, uint16 *x, uint16 *y)
{
    uint32 row;
    if ((sint32)index < 4) { *x = 576u; row = index; }
    else if ((sint32)index < 8) { *x = 600u; row = index - 4u; }
    else if ((sint32)index < 12) { *x = 640u; row = index - 8u; }
    else { *x = 664u; row = index - 12u; }
    *y = (uint16)(row * 48u);
}

uint32 sub_800182D8(uint32 index, uint32 object, uint32 data, uint32 offset)
{
    uint32 tim = data + r_u32(data + (offset << 3) + 4u) + 2048u;
    uint32 palette, pixels, result, page;
    uint16 x, y;
    dr_tim_blocks(tim, &palette, &pixels);
    w_u16(0x80064C52u, (uint16)(r_u32(0x80064C74u) + index));
    LoadImagePSX((PSX_RECT *)psx_addr(0x80064C50u, sizeof(PSX_RECT)),
        (uint32 *)psx_addr(palette + 416u, 1u));
    dr_model_texture_position(index, &x, &y);
    w_u16(0x80064C58u, x);
    w_u16(0x80064C5Au, y);
    LoadImagePSX((PSX_RECT *)psx_addr(0x80064C58u, sizeof(PSX_RECT)),
        (uint32 *)psx_addr(pixels, 1u));
    result = (y & 0x200u) << 2;
    page = ((y >> 4) & 16u) | ((x & 0x3FFu) >> 6) | 128u | result;
    w_u16(object + 74u, (uint16)page);
    w_u16(object + 76u, (uint16)((x & 63u) << 1));
    w_u16(object + 78u, (uint16)(y & 255u));
    return result;
}

sint32 sub_8003367C(sint32 x, sint32 y, sint32 bank)
{
    uint32 index;
    for (index = 0u; index < 13u; ++index) {
        uint32 mask = 1u << index;
        uint32 record = 0x80066C56u + index * 4u;
        if (bank < 0) mask <<= 16;
        if ((mask & 0x1D5F1ABFu) != 0u
            && (sint32)r_u8(record) < x && (sint32)r_u8(record + 1u) < y
            && x < (sint32)r_u8(record + 2u) && y < (sint32)r_u8(record + 3u))
            return (sint32)index;
    }
    if ((sint32)r_u8(0x80066C8Au) < x && (sint32)r_u8(0x80066C8Bu) < y
        && x < (sint32)r_u8(0x80066C8Cu))
        return y < (sint32)r_u8(0x80066C8Du) ? 0 : -1;
    return -1;
}

void sub_80035308(uint32 indices, uint32 records, uint32 vertices, uint32 weights)
{
    uint32 base = records - 16u;
    for (;;) {
        sint32 index = (sint16)r_u16(indices);
        uint32 first, current, x, y, z, link;
        SVECTOR vector;
        VECTOR product;
        indices += 2u;
        if (index < 0) return;
        if (index == 0) continue;
        first = base + ((uint32)index << 4);
        x = r_u32(first);
        y = r_u32(first + 4u);
        z = r_u32(first + 8u);
        link = r_u32(first + 12u);
        vector.vx = (sint16)x;
        vector.vy = (sint16)y;
        vector.vz = (sint16)z;
        vector.pad = 0;
        gte_gpf0(&vector, (sint16)r_u16(weights + (link & 255u) * 2u), &product);
        if (((link >> 8) & 0xFFFu) != 0u) {
            for (;;) {
                current = base + (((link >> 8) & 0xFFFu) << 4);
                link = r_u32(current + 12u);
                if (current == first) break;
                vector.vx = (sint16)r_u32(current);
                vector.vy = (sint16)r_u32(current + 4u);
                vector.vz = (sint16)r_u32(current + 8u);
                gte_gpl0(&vector, (sint16)r_u16(weights + (link & 255u) * 2u), &product);
            }
        }
        current = vertices + ((link >> 20) << 3);
        w_u16(current, (uint16)(((sint32)x >> 16) + (product.vx >> 10)));
        w_u16(current + 2u, (uint16)((y >> 16) + (uint32)(product.vy >> 10)));
        w_u16(current + 4u, (uint16)((z >> 16) + (uint32)(product.vz >> 10)));
    }
}

uint32 psx_bios_init_kernel_heap(uint32 base, uint32 bytes);
uint32 psx_bios_kernel_malloc(uint32 bytes);
uint32 psx_bios_init_rcnt(uint32 priority);
uint32 psx_bios_init_default_interrupts(uint32 priority);
uint32 psx_bios_enqueue_timer_vblank(uint32 priority);
uint32 psx_bios_reset_entry_interrupt(void);
uint32 psx_bios_init_user_heap(uint32 base, uint32 bytes);
static sint32 dr_native_exec(void *context, uint32 entry, uint32 argc, uint32 argv)
{
    (void)context; (void)argc; (void)argv;
    if (entry == 0x800C14B8u) return dr_movie_entry();
    fprintf(stderr, "Unsupported native PS-X EXE entry %08X\n", entry);
    abort();
}

static sint32 dr_guest_callback(void *context, uint32 address)
{
    (void)context;
    if (address == 0x8002B6ACu) {
        sub_8002B6AC();
        return 1;
    }
    fprintf(stderr, "Unsupported game callback %08X\n", address);
    abort();
}

void dr_native_scanout(void)
{
    /* Host scanout continues when the game removes its VBlank callback */
    if (!gpu_present()) {
        fprintf(stderr, "Native display presentation failed\n");
        abort();
    }
}

static uint32 dr_pad_owner_begin(uint32 owner)
{
    return psx_pad_owner_begin(owner, 0x8004F5ACu);
}

void xport_bind_native_spu_transfer(void)
{
    SpuNativeTransferGuestBinding binding = {
        0x8005FF00u, 0x8005FF8Cu, 0x8005FF88u, 0x8005FFA4u,
        0x8005FFA8u, 0x8005FFC4u, 0x8005FFC8u, 0u
    };
    binding.address_shift = r_u32(0x8005FF98u) & 31u;
    if (!spu_bind_native_transfer_guest(&binding)) abort();
}

static uint32 dr_kernel_alloc(uint32 bytes)
{
    uint32 result = psx_bios_kernel_malloc(bytes);
    return result == 0xFFFFFFFFu ? 0u : result;
}

uint32 sub_8004F39C(uint32 mode)
{
    uint32 previous = r_u32(0x8005F2F8u);
    w_u32(0x8005F2F8u, mode);
    return previous;
}

void sub_8002B2FC(void)
{
    uint32 index;
    w_u32(0x80065334u, 3000u);
    for (index = 0u; index < 32u; ++index) w_u8(0x80078A97u - index, 0u);
    for (index = 0u; index < 32u; ++index) w_u8(0x80078A67u - index, 0u);
    w_u8(0x80078A4Cu, 0u);
    for (index = 0u; index < 16u; ++index) w_u8(0x80078A77u - index, 0u);
    w_u8(0x80078A68u, 0u);
    for (index = 0u; index < 32u; ++index) w_u8(0x80078A47u - index, 0u);
    w_u8(0x80078A2Bu, 0u);
    for (index = 0u; index < 32u; ++index) w_u8(0x80078A97u - index, 0u);
    w_u8(0x80078AE0u, 255u); w_u8(0x80078AE1u, 255u); w_u8(0x80078AE2u, 255u);
    w_u8(0x80078AE5u, 24u); w_u8(0x80078AE4u, 0u); w_u8(0x80078AE6u, 248u);
    w_u8(0x80078AE8u, 0u); w_u8(0x80078AE9u, 0u); w_u8(0x80078AEAu, 0u);
    w_u8(0x80078AECu, 0u); w_u8(0x80078AEDu, 0u); w_u8(0x80078AEEu, 0u);
}

uint32 sub_800200B0(void)
{
    uint8 parameter = 0x80u;
    return (uint32)CdControlBPSX(14u, &parameter, NULL);
}

uint32 sub_8001DE6C(void)
{
    uint32 original = r_u32(0x80064BF8u);
    uint32 volume = original;
    uint32 result;
    while ((sint32)volume >= 0) {
        w_u32(0x80064BF8u, volume - 32u);
        sub_8001FF98();
        VSync(0);
        volume = r_u32(0x80064BF8u);
    }
    result = sub_800200B0();
    w_u32(0x80064BF8u, original);
    return result;
}

static void dr_save_copy(uint32 load, uint32 field, uint32 bytes)
{
    uint32 cursor = r_u32(0x8006531Cu);
    uint32 destination = load ? field : cursor;
    uint32 source = load ? cursor : field;
    uint32 index;
    for (index = 0u; index < bytes; ++index)
        w_u8(destination + index, r_u8(source + index));
    w_u32(0x8006531Cu, r_u32(0x8006531Cu) + bytes);
    w_u32(0x80065320u, r_u32(0x80065320u) + bytes);
}

uint32 sub_8002409C(uint32 load)
{
    static const uint32 fields[][2] = {
        {0x80064BF0u, 4u}, {0x80064BF4u, 4u}, {0x80064BFCu, 4u}, {0x80064BF8u, 4u},
        {0x8005A990u, 72u}, {0x8005975Cu, 4u}, {0x80059760u, 4u}, {0x8005976Cu, 4u},
        {0x80059764u, 8u}, {0x80059770u, 10u}, {0x8005978Cu, 4u}, {0x800594CCu, 256u}
    };
    uint32 index;
    for (index = 0u; index < sizeof(fields) / sizeof(fields[0]); ++index)
        dr_save_copy(load, fields[index][0], fields[index][1]);
    dr_save_copy(load, 0x800596CCu, r_u32(0x80064B18u));
    dr_save_copy(load, 0x800596DCu, r_u32(0x80064B1Cu));
    dr_save_copy(load, 0x800596ECu, r_u32(0x80064B20u));
    dr_save_copy(load, 0x800596F4u, r_u32(0x80064B24u));
    for (index = 0u; index < 5u; ++index)
        dr_save_copy(load, 0x80064B28u + index * 4u, 4u);
    dr_save_copy(load, 0x8006649Cu, 1152u);
    dr_save_copy(load, 0x80078CA8u, 588u);
    return 1u;
}

sint32 openPSX(const char *path, uint32 mode);
sint32 readPSX(sint32 descriptor, void *buffer, uint32 bytes);
sint32 closePSX(sint32 descriptor);

uint32 sub_800246B0(uint32 slot)
{
    char path[32];
    sint32 descriptor;
    uint32 index, checksum = 0u;
    sprintf(path, "bu%d0:BASLUS-00969", (sint32)slot);
    memset(psx_addr(0x80076838u, 0x2000u), 0, 0x2000u);
    descriptor = openPSX(path, 1u);
    if (descriptor < 0) return 0u;
    if (readPSX(descriptor, psx_addr(0x80076838u, 0x2000u), 0x2000u) != 0x2000) {
        closePSX(descriptor);
        return 0u;
    }
    for (index = 0u; index < 7676u; ++index)
        checksum += r_u8(0x80076A3Cu + index);
    if (checksum != r_u32(0x80076A38u)) {
        closePSX(descriptor);
        return 0u;
    }
    w_u32(0x80065320u, 0u);
    w_u32(0x8006531Cu, 0x80076A3Cu);
    if (sub_8002409C(1u) == 1u) {
        closePSX(descriptor);
        sub_8001FF98();
        strcpy((char *)psx_addr(0x8006647Cu, 1u), (const char *)psx_addr(0x80059770u, 1u));
    } else closePSX(descriptor);
    return 1u;
}

sint32 sub_80047C94(uint32 first, uint32 second, uint32 third)
{
    uint32 block, descriptor, bytes, end, current, events, threads, table;
    psx_bios_init_kernel_heap(0xA000E800u, 0x1000u);
    block = dr_kernel_alloc(32u);
    if (!block) return 0;
    for (current = 0u; current < 32u; ++current)
        w_u8(block + current, 0u);
    descriptor = r_u32(0x8005E818u);
    w_u32(descriptor, block);
    w_u32(descriptor + 4u, 32u);
    psx_bios_init_rcnt(0u);
    psx_bios_init_default_interrupts(3u);
    bytes = 28u * first;
    events = dr_kernel_alloc(bytes);
    if (!events) return 0;
    descriptor = r_u32(0x8005E818u);
    w_u32(descriptor + 36u, bytes);
    w_u32(descriptor + 32u, events);
    end = events + bytes;
    for (current = events; current < end; current += 28u)
        w_u32(current + 4u, 0u);
    if (!bytes) return 0;
    descriptor = r_u32(0x8005E818u);
    w_u32(descriptor + 12u, 8u);
    bytes = 192u * second;
    w_u32(descriptor + 20u, bytes);
    table = dr_kernel_alloc(8u);
    if (!table) return 0;
    threads = dr_kernel_alloc(bytes);
    if (!threads) return 0;
    w_u32(table, 0u);
    end = threads + 192u * second;
    for (current = threads; current < end; current += 192u)
        w_u32(current, 0x1000u);
    w_u32(threads, 0x4000u);
    w_u32(table, threads);
    descriptor = r_u32(0x8005E818u);
    w_u32(descriptor + 8u, table);
    w_u32(descriptor + 16u, threads);
    if (8u + bytes == 0u) return 0;
    psx_bios_enqueue_timer_vblank(1u);
    psx_bios_reset_entry_interrupt();
    block = r_u32(0x474u);
    current = ((uint32)r_u16(block) << 16) - 8u
        + (uint32)(sint32)(sint16)r_u16(block + 4u);
    w_u32(current, second);
    w_u32(current + 4u, first);
    w_u32(current + 8u, third);
    return 1;
}

void dr_startup(void)
{
    static const PsxPadCallbackBinding pad_callbacks[] = {
        {0x8004F5ACu, psx_pad_owner_cleanup},
        {0x800585CCu, dr_pad_owner_begin}
    };
    SpuMallocGuestBinding heap_binding = {
        0x8005FF98u, 0x8005FFA0u, 0x8005FFD0u, 0x8005FFD4u,
        0x8005FFD8u, 0x8005FF08u, 0x8005FF0Cu
    };
    SpuNativeTransferGuestBinding transfer_binding = {
        0x8005FF00u, 0x8005FF8Cu, 0x8005FF88u, 0x8005FFA4u,
        0x8005FFA8u, 0x8005FFC4u, 0x8005FFC8u, 0u
    };
    char *program_path;
    char directory[1024];
    char *separator;
    const PSX_EXE executable = {
        "DATA/SLUS_009.69", 0x80010000u,
        0x80065194u, 0x8008B434u,
        0x8005E7F8u, 0x8008B434u, 0x0076CBC4u
    };
    if (!_get_pgmptr(&program_path) && strlen(program_path) < sizeof(directory)) {
        strcpy(directory, program_path);
        separator = strrchr(directory, '\\');
        if (separator) {
            *separator = 0;
            if (_chdir(directory)) { xport_set_exit_code(2); return; }
        }
    }
    _set_abort_behavior(0u, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
    if (!xport_psx_exe_load(&executable)) {
        xport_set_exit_code(2);
        xport_message_error("Demolition Racer", "Cannot load DATA/SLUS_009.69");
        return;
    }
    if (!cd_mount_cue("../iso/Demolition Racer (USA).cue")) {
        xport_set_exit_code(2);
        xport_message_error("Demolition Racer", "Cannot mount configured game disc");
        return;
    }
    if (!spu_bind_malloc_guest(&heap_binding)) { xport_set_exit_code(2); return; }
    transfer_binding.address_shift = r_u32(0x8005FF98u) & 31u;
    if (!spu_bind_native_transfer_guest(&transfer_binding)) { xport_set_exit_code(2); return; }
    spu_bind_reverb_presets((const uint32 *)psx_addr(0x8005FFE0u, 40u),
        (const uint8 *)psx_addr(0x80060010u, 680u), transfer_binding.address_shift);
    psx_bios_init_user_heap(0x8008B438u, 0x0076CBC4u);
    psx_bios_bind_native_exec(dr_native_exec, NULL);
    psx_bios_bind_guest_callback_service(dr_guest_callback, NULL);
    psx_pad_bind_callbacks(pad_callbacks, sizeof(pad_callbacks) / sizeof(pad_callbacks[0]));
    /* Original card patch destinations derived from the active BIOS handlers */
    psx_bios_bind_card_patches(0x800582D8u, 0x80058348u, 0x8005835Cu, 0x6444u, 0x4D98u);
    sub_80015E38();
}

uint32 sub_80036914(uint32 object, uint32 reset, uint32 player)
{
    uint32 slot = player << 2, record, value, result;
    record = 0x8007A9ECu + (r_u32(0x800654C4u + slot) << 4);
    w_u32(0x80065494u + slot, (uint32)(sint32)(sint16)r_u16(record + 2u) << 8);
    record = 0x8007A9ECu + (r_u32(0x800654C4u + slot) << 4);
    w_u32(0x8006549Cu + slot, (uint32)(sint32)(sint16)r_u16(record + 4u) << 8);
    record = 0x8007A9ECu + (r_u32(0x800654C4u + slot) << 4);
    w_u32(0x800654A4u + slot, (uint32)(sint32)(sint16)r_u16(record + 6u) << 8);
    record = 0x8007A9ECu + (r_u32(0x800654C4u + slot) << 4);
    value = r_u32(record + 8u);
    result = r_u32(record + 12u);
    w_u32(0x8007A980u + (player << 3), value);
    w_u32(0x8007A984u + (player << 3), result);
    if (reset || r_u32(0x800654E8u + slot) != (uint32)(sint32)(sint16)r_u16(record)) {
        w_u32(0x80065464u + slot, ((r_u32(0x800654B4u + slot) << 11) +
            (uint32)(sint32)(sint16)r_u16(object + 2u)) << 8);
        w_u32(0x800654D0u + slot, r_u32(0x80065494u + slot));
        w_u32(0x8006547Cu + slot, r_u32(0x8006549Cu + slot));
        w_u32(0x800654ACu + slot, r_u32(0x800654A4u + slot));
    }
    value = r_u32(0x80064C14u);
    w_u32(0x8007A990u, r_u32(0x80066D6Cu));
    w_u32(0x8007A994u, r_u32(0x80066D70u));
    w_u16(0x8007A998u, r_u16(0x80066D74u));
    w_u32(0x800654BCu, 0u);
    if (value) {
        w_u32(0x8007A99Au, r_u32(0x80067124u));
        w_u32(0x8007A99Eu, r_u32(0x80067128u));
        w_u16(0x8007A9A2u, r_u16(0x8006712Cu));
        w_u32(0x800654C0u, 0u);
    }
    record = 0x8007A9ECu + (r_u32(0x800654C4u + slot) << 4);
    value = (uint32)(sint32)(sint16)r_u16(record);
    result = r_u32(0x800650ECu);
    w_u32(0x80065484u + slot, 0u);
    w_u32(0x8006548Cu + slot, 0u);
    w_u32(0x8006546Cu + slot, result);
    w_u32(0x800654E8u + slot, value);
    return result;
}

uint32 sub_80040640(uint32 far_address, uint32 color_address)
{
    uint32 index;
    SVECTOR color, far_color;
    VECTOR interpolated;
    xport_gte_write_data(6u, r_u32(0x80065178u));
    for (index = 0u; index < 256u; ++index) {
        far_color.vx = (sint16)r_u16(far_address);
        far_color.vy = (sint16)r_u16(far_address + 2u);
        far_color.vz = (sint16)r_u16(far_address + 4u);
        far_color.pad = 0;
        color.vx = (sint16)r_u16(color_address);
        color.vy = (sint16)r_u16(color_address + 2u);
        color.vz = (sint16)r_u16(color_address + 4u);
        color.pad = 0;
        gte_intpl12(&color, &far_color, (sint32)(index << 4), &interpolated);
        w_u32(0x800835D4u + index * 4u, xport_gte_read_data(22u));
    }
    return 0u;
}

uint32 sub_800152DC(void)
{
    sint32 count = (sint32)r_u32(0x80065254u), index = 0;
    uint32 object = 0x80066E94u, value;
    if (count <= 0) return 0x80066D14u;
    value = r_u32(0x80064A30u);
    do {
        ++index;
        w_u16(object + 4u, 0u);
        w_u16(object + 6u, 0u);
        w_u32(object, value);
        object += 952u;
    } while (index < count);
    return 0u;
}

uint32 sub_80019F30(void)
{
    uint32 result = r_u32(0x800593A8u) ? 60u : 120u;
    w_u32(0x80064CB0u, result);
    return result;
}

sint32 sub_8003909C(void)
{
    uint32 index;
    w_u16(0x8007AA40u, 0u);
    w_u16(0x8007AA42u, 0u);
    w_u32(0x80065114u, 0u);
    if (r_u32(0x8005973Cu) == 2u) {
        w_u16(0x8007AA3Cu, 3576u);
        w_u32(0x8007AA48u, 0u);
        w_u32(0x8007AA4Cu, 3576u);
    } else {
        w_u16(0x8007AA3Cu, 7152u);
        w_u32(0x8007AA48u, 0u);
    }
    for (index = 0u; index < 2u; ++index) {
        w_u16(0x8007AA44u + index * 2u, 0u);
        w_u32(0x8007AA50u + index * 4u, 0u);
        w_u32(0x8007AA58u + index * 4u, 0u);
    }
    if (r_u32(0x80064B5Cu) == 1u) w_u32(0x80065110u, 1u);
    if (r_u8(0x80064EA0u) == 1u) {
        /* Original 80039688 is an empty leaf */
        w_u8(0x80064EA0u, 0u);
        w_u32(0x80065110u, 1u);
    }
    return printf((const char *)psx_addr(0x80011FF4u, 1u), 2044);
}

uint32 sub_80012D58(void)
{
    PSX_RECT rectangle = {960, 256, 1, 1};
    uint32 pixel = 0xC210u, bank, index, packet;
    LoadImagePSX(&rectangle, &pixel);
    DrawSync(0);
    for (bank = 0u; bank < 2u; ++bank) {
        for (index = 0u; index < 2u; ++index) {
            uint32 vertex;
            packet = 0x800655ACu + bank * 40u + index * 80u;
            SetPolyFT4(psx_addr(packet, 40u));
            for (vertex = 0u; vertex < 4u; ++vertex) {
                w_u8(packet + 12u + vertex * 8u, 0u);
                w_u8(packet + 13u + vertex * 8u, 0u);
            }
            w_u8(packet + 4u, 128u);
            w_u8(packet + 5u, 128u);
            w_u8(packet + 6u, 128u);
            w_u8(packet + 7u, (r_u8(packet + 7u) | 2u) & 0xFEu);
            w_u16(packet + 22u, GetTPage(2, 1, 960, 256));
        }
    }
    return 0u;
}

uint32 sub_800162D8(uint32 path)
{
    const char *name = (const char *)psx_addr(path, 1u);
    uint32 size = sub_800423EC(name, 1u);
    uint32 buffer = sub_80042544(size);
    sub_8004225C(name, buffer, size);
    return buffer;
}

static sint32 dr_glyph_width(uint32 font, sint32 character, uint32 pixels)
{
    uint32 base = 0x80059DD8u + font * 144u;
    sint32 divisor = r_u8(base + 6u), rows = r_u8(base + 5u);
    sint32 stride = ((r_u8(r_u32(base + 12u) + 22u) + 3u) & 0x1FCu) >> 1;
    sint32 width = r_u8(base + 4u), column, result;
    uint32 source;
    if (!divisor) abort();
    source = pixels + (uint32)((character % divisor) * (width >> 1)) +
        (uint32)((character / divisor) * rows * stride);
    column = width - 1;
    result = column;
    if (column <= 0) return result + 1;
    do {
        sint32 row = 0, occupied = 0;
        uint32 offset = 0u;
        if (rows) {
            do {
                uint32 byte = r_u8(source + (uint32)(column / 2) + offset);
                if ((column & 1) ? (byte & 15u) : (byte >> 4)) ++occupied;
                ++row;
                offset += (uint32)stride;
            } while (row < r_u8(base + 5u));
        }
        if (!occupied) --result;
        --column;
        if (column <= 0 || occupied) return result + 1;
    } while (1);
}

static PSX_RECT dr_font_rectangle(uint32 texture, uint32 width, sint32 height)
{
    PSX_RECT rectangle;
    uint32 page = r_u16(texture + 8u);
    rectangle.x = (sint16)((r_u8(texture + 24u) >> 2) + ((page << 6) & 1023u));
    rectangle.y = (sint16)(r_u8(texture + 25u) + ((page << 4) & 256u) + ((page >> 2) & 512u));
    rectangle.w = (sint16)(width / 4u);
    rectangle.h = (sint16)height;
    return rectangle;
}

uint32 sub_8002060C(void)
{
    uint32 font;
    for (font = 0u; font < 7u; ++font) {
        uint32 base = 0x80059DD8u + font * 144u;
        uint32 name = r_u32(base), texture = sub_80039ADC(name);
        w_u32(base + 12u, texture);
        if (texture) {
            uint32 width = (r_u8(texture + 22u) + 3u) & 0xFFFCu;
            sint32 height = r_u8(texture + 23u), character;
            uint32 pixels;
            PSX_RECT rectangle;
            if (!strcmp((const char *)psx_addr(name, 1u),
                (const char *)psx_addr(0x80064D50u, 1u))) {
                uint32 image = sub_800162D8(0x80010584u);
                rectangle = dr_font_rectangle(r_u32(base + 12u), width, height);
                LoadImagePSX(&rectangle, (uint32 *)psx_addr(image + 64u, 1u));
                DrawSync(0);
                sub_8004261C(image);
            }
            pixels = sub_80042544((width >> 1) * (uint32)height);
            rectangle = dr_font_rectangle(r_u32(base + 12u), width, height);
            StoreImage(&rectangle, (uint32 *)psx_addr(pixels, 1u));
            DrawSync(0);
            for (character = 0; character < 128; ++character)
                w_u8(base + 16u + (uint32)character,
                    (uint32)dr_glyph_width(font, character, pixels) - r_u8(base + 7u));
            w_u8(base + 16u, r_u8(base + 4u) >> 1);
            sub_8004261C(pixels);
        }
    }
    return 0u;
}

uint32 sub_80020890(void)
{
    uint32 packets, index;
    sub_8002060C();
    packets = sub_80042544(40000u);
    w_u32(0x800652DCu, packets);
    for (index = 0u; index < 1000u; ++index) {
        w_u8(packets + index * 40u + 3u, 9u);
        w_u8(packets + index * 40u + 7u, 46u);
    }
    return 0u;
}

uint32 sub_80020A14(uint32 font, uint32 text)
{
    uint32 total = 0u, index = 0u, base = 0x80059DD8u + font * 144u;
    while ((sint32)index < (sint32)strlen((const char *)psx_addr(text, 1u))) {
        uint32 character = (r_u8(text + index) - 32u) & 255u;
        ++index;
        total += r_u8(base + character + 16u);
    }
    return total + index;
}

uint32 sub_80021430(uint32 index)
{
    uint32 base = 0x80059D48u + (index << 4);
    uint32 a = r_u32(base), b = r_u32(base + 4u), c = r_u32(base + 8u), result;
    w_u32(0x80074580u, a);
    w_u32(0x80074584u, b);
    w_u32(0x80074588u, c);
    result = r_u32(base + 12u);
    w_u32(0x8007458Cu, result);
    return result;
}

void sub_80012C3C(void)
{
    uint32 sequence = r_u32(0x80064A5Cu);
    uint32 buttons = (uint32)(sint32)(sint16)r_u16(0x80065524u);
    uint32 index, progress, target;
    if (sequence == ~0u) {
        for (index = 0u; index < 8u; ++index) {
            if (buttons & (uint32)(sint32)(sint16)r_u16(0x800593CCu + index * 16u)) {
                w_u32(0x80064A5Cu, index);
                w_u32(0x80065194u, 1u);
                return;
            }
        }
        return;
    }
    if (!buttons) return;
    progress = r_u32(0x80065194u);
    if (buttons & (uint32)(sint32)(sint16)r_u16(0x800593CCu + (sequence << 4) + (progress << 1))) {
        w_u32(0x80065194u, progress + 1u);
        if (progress + 1u != 8u) return;
        target = r_u32(0x8005944Cu + (sequence << 2));
        if (target) {
            /* TODO: Translate the original optional cheat callback */
            fprintf(stderr, "Untranslated cheat callback %08X\n", target);
            abort();
        }
        sequence = r_u32(0x80064A5Cu);
        w_u8(0x800651C8u + sequence, r_u8(0x800651C8u + sequence) ^ 1u);
    }
    w_u32(0x80064A5Cu, ~0u);
}

uint32 sub_800208E4(void)
{
    uint32 result = r_u32(0x80065128u) * 500u;
    w_u32(0x800652E4u, 0u);
    w_u32(0x800652E0u, result);
    return result;
}

uint32 sub_8001697C(void)
{
    uint32 result = r_u32(0x800593A0u), table, object = 0x80066D6Cu;
    sint32 index = 0;
    if (result) return result;
    result = r_u32(0x80065254u);
    if ((sint32)result <= 0) return result;
    table = r_u32(0x800653B0u);
    do {
        sint32 position = (sint16)r_u16(object);
        if (r_u32(table + 4u) < (uint32)position) {
            uint32 segment, count = r_u32(table + 20u);
            for (segment = 0u; segment < count; ++segment) {
                uint32 record = table + segment * 6u;
                sint32 start = r_u16(record + 32u), end = r_u16(record + 34u);
                if (position >= start && end >= position) {
                    w_u16(object + 2u, r_u16(record + 36u) + (uint32)(position - start));
                    break;
                }
            }
        } else w_u16(object + 2u, (uint32)position);
        ++index;
        result = (uint32)(index < (sint32)r_u32(0x80065254u));
        object += 952u;
    } while (result);
    return result;
}

uint32 sub_80015664(void)
{
    uint32 object = 0x80066D14u, result = r_u32(0x80065254u);
    sint32 index = 0;
    if ((sint32)result < 0) return result;
    do {
        uint32 state = object + 378u;
        if ((sint32)r_u32(0x80064C14u) >= index) {
            sint32 score = (sint16)r_u16(state + 10u);
            sint32 pending = (sint16)r_u16(state + 12u);
            if (r_u8(state + 567u)) {
                if (pending > 0) { score += pending; pending = 0; }
            } else if (pending > 0 && r_u32(state + 6u) + 25u < r_u32(0x80064A30u)) {
                --pending;
                ++score;
            }
            if (pending < 0 && r_u32(object + 384u) + 50u < r_u32(0x80064A30u)) pending = 0;
            w_u16(object + 388u, (uint32)score);
            w_u16(object + 390u, (uint32)pending);
        }
        if ((r_u8(state) & 64u) && r_u32(state - 62u) &&
            r_u32(0x80064A30u) >= r_u32(state - 62u))
            w_u8(state, r_u8(state) & 0xBFu);
        ++index;
        object += 952u;
        result = (uint32)((sint32)r_u32(0x80065254u) < index);
    } while (!result);
    return result;
}

/* Original GAME leaf returns without work */
void game_800AE17C(void) {}

void game_800AEB4C(void)
{
    sint32 player = 0;
    uint32 state = 0x800C6BC0u, deadline = 0x800C6AFCu;
    if ((sint32)r_u32(0x80065254u) <= 0) return;
    do {
        sint32 value = (sint8)r_u8(state);
        if (value && value < 3 && r_u32(deadline) < r_u32(0x80064A30u)) {
            w_u8(state, (uint32)(value + 1));
            w_u32(deadline, r_u32(0x80064A30u) + (r_u32(0x80064C14u) ? 3u : 7u));
        }
        ++state;
        ++player;
        deadline += 4u;
    } while (player < (sint32)r_u32(0x80065254u));
}

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))
uint32 sub_8001ED2C(uint32 sound, uint32 voice, uint32 mode, sint32 frequency, uint32 position, uint32 velocity);

static uint32 dr_body_quarter(uint32 value)
{
    if ((sint32)value < 0) value += 3u;
    return (uint32)((sint32)value >> 2);
}

static void dr_body_detach(uint32 player, uint32 object, uint32 side)
{
    /* TODO: Translate full 34C74 and its detached-object dependencies */
    fprintf(stderr, "Untranslated body detachment 80034C74: %08X %08X %08X\n", player, object, side);
    abort();
}

static void dr_body_panel(uint32 player, uint32 object, uint32 side, uint32 impulse, uint32 drag, uint8 *flags)
{
    uint32 base = side ? 328u : 326u;
    uint32 mark_lower = (side ? 0x800653ECu : 0x800653F4u) + player;
    uint32 mark_upper = (side ? 0x800653E8u : 0x800653F0u) + player;
    uint32 lower = (uint32)(sint32)(sint16)R16(side ? 0x80066C54u : 0x80066C46u);
    uint32 upper = (uint32)(sint32)(sint16)R16(object + base + 4u);
    uint32 velocity = (uint32)(sint32)(sint16)R16(object + base + 8u);
    uint32 position;
    velocity += impulse - drag - (uint32)((sint32)velocity >> 2);
    position = (uint32)(sint32)(sint16)R16(object + base) + velocity;
    if ((sint32)R32(0x80064C14u) >= (sint32)player) {
        if (!R8(mark_lower) && (sint32)(lower + 256u) < (sint32)position) R8(mark_lower) = 1u;
        if (!R8(mark_upper) && (sint32)position < (sint32)(upper - 256u)) R8(mark_upper) = 1u;
    }
    if ((sint32)upper < (sint32)position) {
        if (*flags & (side ? 8u : 32u)) {
            *flags &= (uint8)~(side ? 4u : 16u);
            dr_body_detach(player, object, side);
        } else {
            velocity -= dr_body_quarter(position - upper);
            if ((sint32)R32(0x80064C14u) >= (sint32)player && R8(mark_upper)) {
                uint32 volume = velocity << 2;
                if ((sint32)volume < 0) volume = 0u - volume;
                if ((sint32)volume >= 2049) volume = 2048u;
                sub_8001ED2C(17u, ~0u, volume, 4096, object + 100u, 0u);
                R8(mark_upper) = 0u;
            }
        }
    } else if ((sint32)position < (sint32)lower) {
        velocity += dr_body_quarter(lower - position);
        if ((sint32)R32(0x80064C14u) >= (sint32)player && R8(mark_lower)) {
            uint32 volume = velocity << 2;
            if ((sint32)volume >= 2049) volume = 2048u;
            sub_8001ED2C(16u, ~0u, volume, 4096, object + 100u, 0u);
            R8(mark_lower) = 0u;
        }
    }
    R16(object + base) = (uint16)position;
    R16(object + base + 8u) = (uint16)velocity;
}

void sub_80033918(uint32 player, uint32 object)
{
    uint8 flags = R8(object + 378u);
    uint32 impulse, speed, drag = 11u;
    if (!(flags & 20u)) return;
    impulse = (uint32)((sint32)(R32(object + 312u) - R32(object + 116u)) >> 4);
    R32(object + 312u) = R32(object + 116u);
    if (impulse + 9u < 44u) impulse = 0u;
    speed = (uint32)((sint32)R32(object + 888u) >> 8) - 500u;
    if ((sint32)speed > 0) drag -= speed;
    if (flags & 16u) dr_body_panel(player, object, 0u, impulse, drag, &flags);
    if (flags & 4u) dr_body_panel(player, object, 1u, impulse, drag, &flags);
    R8(object + 378u) = flags;
}

uint32 sub_8003408C(void)
{
    uint32 count = R32(0x80065254u), index = 0u, object = 0x80066D14u;
    if ((sint32)count <= 0) return count;
    do {
        sub_80033918(index, object);
        count = R32(0x80065254u);
        ++index;
        object += 952u;
    } while ((sint32)index < (sint32)count);
    return 0u;
}

uint32 sub_8001E2E8(uint32 voice);

uint32 sub_8001CDBC(uint32 player, uint32 object, uint32 view)
{
    uint32 voice = player + 8u, offset = voice * 32u, sound, volume;
    if (R8(object + 940u) && R8(object + 942u) == 15u) {
        sub_8001E2E8(voice);
        R32(0x80073FB4u + offset) = 0u;
        R32(0x80073FB0u + offset) = 0u;
        R8(0x800652A8u + player) = 255u;
        return 255u;
    }
    sound = (uint32)(sint32)(sint8)R8(0x80059BACu + view);
    if ((uint32)R8(0x800652A8u + player) != sound) {
        R8(0x800652A8u + player) = (uint8)sound;
        if (sound == ~0u) {
            sub_8001E2E8(voice);
            R32(0x80073FB4u + offset) = 0u;
            R32(0x80073FB0u + offset) = 0u;
        } else sub_8001ED2C(sound, voice, 4096u, 4096, object + 100u, object + 112u);
    }
    volume = (uint32)((sint32)(R32(object + 892u) + R32(object + 888u)) >> 6);
    if ((sint32)volume < 0) volume = 0u - volume;
    if ((sint32)volume > 4096) volume = 4096u;
    R32(0x80073FB4u + offset) = 4096u;
    R32(0x80073FB0u + offset) = volume;
    return 4096u;
}

uint32 sub_80042784(void)
{
    uint32 maximum = R32(0x80065588u);
    uint32 used = R32(0x80065570u) - R32(0x8006557Cu + R32(0x80065128u) * 4u);
    uint32 result = maximum < used;
    R32(0x8006558Cu) = used;
    if (result) maximum = used;
    R32(0x80065588u) = maximum;
    return result;
}

uint32 sub_8001D070(uint32 classification,uint32 strength,uint32 player)
{
    uint32 voice=player+4u,offset=voice*32u;
    sint32 third,volume,frequency,sound;
    if((sint32)r_u32(0x80064C14u)<(sint32)player)return 1u;
    third=(sint32)strength/3;
    volume=third<0?0:(third>4096?4096:third);
    frequency=volume<=0?4096:third;
    if(frequency<3072)frequency=3072;
    else if(frequency>4096)frequency=4096;
    sound=(sint8)r_u8(0x80059BD0u+classification);
    if(sound!=(sint16)r_u16(0x80073FB8u+offset)) {
        if(sound!=-1)sub_8001ED2C((uint32)sound,voice,(uint32)volume,(uint32)frequency,
            0x80066D78u+player*952u,0x80066D84u+player*952u);
        else {w_u16(0x80073FB8u+offset,0xFFFFu);sub_8001E2E8(voice);}
    }
    w_u32(0x80073FB0u+offset,(uint32)volume);
    w_u32(0x80073FB4u+offset,(uint32)frequency);
    return offset;
}
