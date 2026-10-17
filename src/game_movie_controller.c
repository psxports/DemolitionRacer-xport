#include "draft_signatures.h"
#include "psx_spu.h"
#include "psx_stream.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
extern uint32 mov_800C1524(void);
extern void mov_800C14F4(uint32 *, uint32);
extern void mov_800CC724(void);
extern void mov_800CC840(void);
extern void mov_800CA584(void);
extern void mov_800C4DE4(void);
extern uint32 mov_800C8DF4(void);

#define MOV_GP 0x800E0454u
static uint32 *movie_vlc[2], *movie_slice, *movie_ring;
static uint32 movie_logical[12];
static uint32 movie_active;
static size_t movie_vlc_bytes, movie_slice_bytes;

static sint32 movie_div(sint32 a, sint32 b)
{
    if (!b)
        xport_mips_break(7u);
    if (a == INT_MIN && b == -1)
        xport_mips_break(6u);
    return a / b;
}

static sint32 movie_half(sint32 a)
{
    return (sint32)((uint32)a + ((uint32)a >> 31u)) >> 1;
}

static void movie_pump(void)
{
    stream_pump();
    mdec_psyq_pump(0);
    (void)VSync(-1);
}

static sint32 movie_slice_words(void)
{
    sint32 mode = r_s16(MOV_GP + 92u), height = r_s16(0x800E054Au);
    sint32 columns = movie_div(r_s16(0x800E0548u) - 1, mode * 8) + 1;
    sint32 rows = (height - 1 < 0 ? height + 14 : height - 1) >> 4;
    return (sint32)((uint32)columns * 128u * (uint32)mode * (uint32)(rows + 1)) >> 1;
}

static uint32 movie_validate_slice(void)
{
    sint32 words = movie_slice_words();
    if (words <= 0 || (uint64)(uint32)words * 4u > movie_slice_bytes)
    {
        draft_agent1_missing_diagnostic("Movie slice output exceeds allocated buffer\n");
        w_u8(MOV_GP + 72u, 1u);
        return 0u;
    }
    return 1u;
}

static void movie_decode_frame(uint32 *frame, uint32 slot)
{
    uint64 required = ((uint64)(frame[0] & 65535u) + 1u) * 4u;
    if (required <= 4u || required > movie_vlc_bytes)
    {
        draft_agent1_missing_diagnostic("Movie VLC header exceeds allocated buffer\n");
        w_u8(MOV_GP + 72u, 1u);
        return;
    }
    if (DecDCTvlc(frame, movie_vlc[slot]) != 0)
    {
        draft_agent1_missing_diagnostic("Movie VLC decoder returned an incomplete chunk\n");
        w_u8(MOV_GP + 72u, 1u);
    }
}

static void movie_load_rect(uint32 address, uint32 *pixels)
{
    PSX_RECT rectangle;
    rectangle.x = (sint16)r_u16(address);
    rectangle.y = (sint16)r_u16(address + 2u);
    rectangle.w = (sint16)r_u16(address + 4u);
    rectangle.h = (sint16)r_u16(address + 6u);
    LoadImagePSX(&rectangle, pixels);
}

void mov_800C08BC(void)
{
    uint32 display = r_u32(0x800E0540u), base = 0x800E0530u + display * 8u;
    sint32 mode = r_s16(MOV_GP + 92u), remainder;
    if (mode == 3 && r_u32(0x801E2E34u))
    {
        mov_800C4DE4();
        w_u32(0x801E2E34u, 0u);
    }
    movie_load_rect(0x800E0544u, movie_slice);
    remainder = r_s16(base + 4u) % (mode * 8);
    if (r_u32(MOV_GP) && remainder)
    {
        w_u32(MOV_GP, 0u);
        w_u16(0x800E0544u, (uint16)(r_u16(0x800E0544u) + movie_half(remainder * mode)));
    }
    else
        w_u16(0x800E0544u, (uint16)(r_u16(0x800E0544u) + mode * 8));
    if (r_s16(0x800E0544u) >= r_s16(base) + r_s16(base + 4u))
    {
        display = !display;
        w_u32(0x800E0540u, display);
        w_u32(0x800E054Cu, 1u);
        w_u16(0x800E0544u, r_u16(0x800E0530u + display * 8u));
        w_u16(0x800E0546u, r_u16(0x800E0532u + display * 8u));
        w_u32(MOV_GP, 1u);
    }
    else if (movie_validate_slice())
        DecDCTout(movie_slice, movie_slice_words());
}

uint32 *mov_800C0AE8(uint32 *logical_buffers)
{
    uint32 *frame = NULL, *raw_header = NULL, timeout = 0x800000u;
    StHEADER *header;
    PSX_RECT clear;
    while (StGetNext(&frame, &raw_header))
    {
        if (!--timeout)
            return NULL;
        movie_pump();
    }
    header = (StHEADER *)raw_header;
    if (header->frameSize < 12u)
    {
        draft_agent1_missing_diagnostic("Movie frame has a truncated VLC header\n");
        StFreeRing(frame);
        return NULL;
    }
    if (frame[0] != header->dummy1 || frame[1] != header->dummy2)
    {
        StFreeRing(frame);
        return NULL;
    }
    {
        uint32 previous = r_u32(MOV_GP + 68u);
        w_u32(MOV_GP + 68u, header->frameCount);
        if ((sint32)header->frameCount < (sint32)previous)
            w_u32(MOV_GP + 4u, 1u);
        if (header->frameCount >= (uint32)(r_s16(MOV_GP + 88u) - 2))
            w_u32(MOV_GP + 4u, 1u);
    }
    if (r_u32(MOV_GP + 16u) != header->width || r_u32(MOV_GP + 20u) != header->height)
    {
        sint32 width = (sint32)r_u32(MOV_GP + 96u);
        clear.x = 0;
        clear.y = 0;
        clear.w = (sint16)movie_half(width * r_s16(MOV_GP + 92u));
        clear.h = (sint16)width;
        ClearImage(&clear, 0, 0, 0);
        w_u32(MOV_GP + 16u, header->width);
        w_u32(MOV_GP + 20u, header->height);
    }
    {
        uint16 *fields = (uint16 *)logical_buffers;
        uint32 width = (uint32)header->width * (uint32)r_s16(MOV_GP + 92u);
        fields[15] = header->height;
        fields[11] = header->height;
        fields[21] = header->height;
        fields[14] = (uint16)((width + (width >> 31u)) >> 1u);
        fields[10] = fields[14];
        // Mirror original logical rectangles while pointers remain native
        w_u16(0x800E0534u, fields[10]);
        w_u16(0x800E0536u, fields[11]);
        w_u16(0x800E053Cu, fields[14]);
        w_u16(0x800E053Eu, fields[15]);
        w_u16(0x800E054Au, fields[21]);
    }
    return frame;
}

uint32 mov_800C0E10(const char *path)
{
    uint32 width = r_u32(MOV_GP + 96u), height = r_u32(MOV_GP + 100u), mode = (uint32)r_s16(MOV_GP + 92u);
    uint32 x = r_u32(MOV_GP + 116u), y = r_u32(MOV_GP + 120u), frame_buffer, *frame, timeout;
    CdlFILE file;
    PSX_RECT clear = {0, 0, 1024, 512};
    DRAWENV draw;
    DISPENV display;
    w_u32(MOV_GP + 108u, 10u);
    w_u32(MOV_GP + 104u, 0u);
    w_u32(MOV_GP + 112u, 4u);
    w_u32(MOV_GP + 68u, 0u);
    w_u8(MOV_GP + 72u, 0u);
    w_u32(MOV_GP + 4u, 0u);
    {
        uint64 vlc_size = (uint64)width * height * 2u;
        uint64 slice_size = (uint64)mode * ((height + 15u) & ~15u) * 32u;
        if (!vlc_size || !slice_size || vlc_size > (size_t)-1 || slice_size > (size_t)-1)
            abort();
        movie_vlc_bytes = (size_t)vlc_size;
        movie_slice_bytes = (size_t)slice_size;
    }
    movie_vlc[0] = (uint32 *)malloc(movie_vlc_bytes);
    movie_vlc[1] = (uint32 *)malloc(movie_vlc_bytes);
    movie_slice = (uint32 *)malloc(movie_slice_bytes);
    if (!movie_vlc[0] || !movie_vlc[1] || !movie_slice)
        abort();
    w_u32(0x800E0528u, 0u);
    w_u32(0x800E0540u, 0u);
    w_u32(0x800E054Cu, 0u);
    w_u16(0x800E0530u, (uint16)movie_half((sint32)(x * mode)));
    w_u16(0x800E0538u, r_u16(0x800E0530u));
    w_u16(0x800E0544u, r_u16(0x800E0530u));
    w_u16(0x800E0532u, (uint16)y);
    w_u16(0x800E053Au, (uint16)(y + 256u));
    w_u16(0x800E0546u, (uint16)y);
    w_u16(0x800E0534u, (uint16)movie_half((sint32)(width * mode)));
    w_u16(0x800E053Cu, r_u16(0x800E0534u));
    w_u16(0x800E0536u, (uint16)height);
    w_u16(0x800E053Eu, (uint16)height);
    w_u16(0x800E054Au, (uint16)height);
    w_u16(0x800E0548u, (uint16)(mode * 8u));
    memset(movie_logical, 0, sizeof(movie_logical));
    {
        uint32 field;
        for (field = 4u; field < 12u; ++field)
            movie_logical[field] = r_u32(0x800E0520u + field * 4u);
    }
    if (!dr_data_stream(&file, path))
    {
        draft_agent1_missing_diagnostic("File %s not found\n", path);
        goto done;
    }
    memcpy(psx_addr(MOV_GP + 76u, 3u), &file.pos, 3u);
    w_u32(MOV_GP + 84u, file.size);
    DecDCTReset(0);
    DecDCToutCallback(mov_800C08BC);
    movie_ring = (uint32 *)malloc(0x20000u);
    if (!movie_ring)
        abort();
    mov_800C14F4(movie_ring, 64u);
    StSetStream(mode == 3u, 1u, 0x0FFFFFFFu, NULL, NULL);
    while (!CdControlF(0x15u, (uint8 *)&file.pos))
        movie_pump();
    while (!CdRead2(448))
        movie_pump();
    frame = mov_800C0AE8(movie_logical);
    if (frame)
    {
        frame_buffer = !r_u32(0x800E0528u);
        w_u32(0x800E0528u, frame_buffer);
        movie_decode_frame(frame, frame_buffer);
        StFreeRing(frame);
    }
    else
        w_u8(MOV_GP + 72u, 1u);
    ClearImage(&clear, 0, 0, 0);
    // A missing first frame cannot submit uninitialized compressed data
    if (r_u8(MOV_GP + 72u) == 1u)
        goto stop_stream;
    while ((sint32)r_u32(MOV_GP + 68u) < r_s16(MOV_GP + 88u))
    {
        DecDCTin(movie_vlc[r_u32(0x800E0528u) & 1u], (sint32)mode);
        if (!movie_validate_slice())
            goto stop_stream;
        DecDCTout(movie_slice, movie_slice_words());
        if (r_u8(MOV_GP + 72u) == 1u)
            goto stop_stream;
        frame = mov_800C0AE8(movie_logical);
        if (frame)
        {
            frame_buffer = !r_u32(0x800E0528u);
            w_u32(0x800E0528u, frame_buffer);
            movie_decode_frame(frame, frame_buffer);
            StFreeRing(frame);
        }
        else
            w_u8(MOV_GP + 72u, 1u);
        if (r_u8(MOV_GP + 72u) == 1u)
            break;
        timeout = 0x800000u;
        while (!r_u32(0x800E054Cu))
        {
            movie_pump();
            if (!--timeout)
            {
                uint32 alternate = !r_u32(0x800E0540u);
                w_u32(0x800E054Cu, 1u);
                w_u32(0x800E0540u, alternate);
                w_u16(0x800E0544u, r_u16(0x800E0530u + alternate * 8u));
                w_u16(0x800E0546u, r_u16(0x800E0532u + alternate * 8u));
            }
        }
        w_u32(0x800E054Cu, 0u);
        VSync(0);
        frame_buffer = !r_u32(0x800E0540u);
        w_u32(MOV_GP + 128u, frame_buffer);
        SetDefDispEnv(&display, 0, (sint32)(frame_buffer << 8u), r_s16(0x800E0534u + frame_buffer * 8u), r_s16(0x800E0536u + frame_buffer * 8u));
        SetDefDrawEnv(&draw, r_s16(0x800E0530u + frame_buffer * 8u), r_s16(0x800E0532u + frame_buffer * 8u), r_s16(0x800E0534u + frame_buffer * 8u), r_s16(0x800E0536u + frame_buffer * 8u));
        display.screen.x = (sint16)r_u32(MOV_GP + 108u);
        display.screen.y = (sint16)r_u32(MOV_GP + 112u);
        if (mode == 3u)
        {
            display.isrgb24 = 1u;
            display.disp.w = (sint16)(display.disp.w * 2 / 3);
        }
        PutDispEnv(&display);
        PutDrawEnv(&draw);
        SetDispMask(1);
        if (r_u32(MOV_GP + 4u) == 1u)
        {
            CdControlF(8u, NULL);
            break;
        }
        mov_800CC840();
        if (r_u16(0x800E04ECu) & 0x4008u)
        {
            w_u32(MOV_GP + 8u, 1u);
            break;
        }
    }
stop_stream:
    StClearRing();
    StUnSetRing();
    DecDCToutCallback(NULL);
    CdControlBPSX(8u, NULL, NULL);
    free(movie_ring);
    movie_ring = NULL;
done:
    free(movie_vlc[0]);
    free(movie_vlc[1]);
    free(movie_slice);
    movie_vlc[0] = movie_vlc[1] = movie_slice = NULL;
    return 0u;
}

uint32 mov_800C0C74(void)
{
    uint32 descriptors = r_u32(r_u32(MOV_GP + 12u)), index = 0u, entry = descriptors;
    w_u32(MOV_GP + 8u, 0u);
    mov_800C8DF4();
    mov_800C1524();
    mov_800CC724();
    ResetGraph(0);
    SetGraphDebug(0);
    SetDispMask(0);
    draft_agent1_missing_diagnostic("hello\n");
    while ((sint32)index < (sint32)r_u32(descriptors))
    {
        w_u16(MOV_GP + 92u, 3u);
        w_u32(MOV_GP + 96u, r_u32(entry + 8u));
        w_u32(MOV_GP + 100u, r_u32(entry + 12u));
        w_u16(MOV_GP + 88u, r_u16(entry + 16u));
        w_u32(MOV_GP + 116u, r_u32(entry + 20u));
        w_u32(MOV_GP + 120u, r_u32(entry + 24u));
        ++index;
        mov_800C0E10((const char *)psx_addr(r_u32(entry + 4u), 1u));
        entry += 28u;
    }
    DrawSync(0);
    VSync(0);
    VSync(0);
    VSync(0);
    VSync(0);
    draft_agent1_missing_diagnostic("bye\n");
    StopRCnt(0xF2000000u, 0x800E0384u, 0x800E038Cu);
    StopRCnt(0xF2000001u, 0x800E0384u, 0x800E038Cu);
    StopRCnt(0xF2000002u, 0x800E0384u, 0x800E038Cu);
    StopRCnt(0xF2000003u, 0x800E0384u, 0x800E038Cu);
    ResetGraph(3);
    mov_800CA584();
    StopCallback();
    draft_agent1_missing_diagnostic("bye2\n");
    return 0u;
}

sint32 dr_movie_entry(void)
{
    uint32 address;
    if (movie_active)
        abort();
    movie_active = 1u;
    for (address = 0x800E0498u; address < 0x801E46E0u; address += 4u)
        w_u32(address, 0u);
    {
        sint32 result = (sint32)mov_800C0C74();
        movie_active = 0u;
        return result;
    }
}
