#include "draft_signatures.h"
#include "psx_stream.h"
#include "psx_spu.h"
#include <stdio.h>
#include <string.h>

static uint32 movie_pad_active;

uint32 mov_800C1524(void)
{
    uint32 attempt;
    for (attempt = 0u; attempt < 5u; ++attempt)
        if (CdInit() == 1)
            return 1u;
    fprintf(stderr, "CdInit: Init failed\n");
    return 0u;
}

void mov_800C14F4(uint32 *ring, uint32 sectors)
{
    /* Native borrowed ring retains its pointer width and StSetRing clears state */
    StSetRing(ring, sectors);
}

void mov_800CC724(void)
{
    /* Initialize movie packet storage while retaining the shared game PAD binding */
    memset(psx_addr(0x801E2E48u, 68u), 0xFF, 68u);
    movie_pad_active = 1u;
}

void mov_800CC840(void)
{
    uint32 port;
    if (!movie_pad_active)
        return;
    for (port = 0u; port < 2u; ++port)
    {
        uint32 packet = 0x801E2E48u + port * 34u;
        uint32 previous_address = 0x800E048Cu + port * 4u;
        uint16 host = port == 0u ? (uint16)PadRead(0) : 0u;
        uint16 held = (uint16)((host >> 8) | (host << 8));
        uint16 previous = (uint16)r_u32(previous_address);
        /* TODO Analog and NegCon movie PAD branches require a native device report */
        if (port == 0u)
        {
            w_u8(packet, 0u);
            w_u8(packet + 1u, 0x41u);
            w_u16(packet + 2u, (uint16)~held);
            w_u32(0x800E04E0u + port * 4u, 0x4000u);
            w_u8(0x800E04F0u + port * 4u + 2u, 0u);
            w_u16(0x800E04E8u + port * 2u, (uint16)(~previous & held));
            w_u16(0x800E04ECu + port * 2u, held);
            w_u32(previous_address, ~(uint32)r_u16(packet + 2u));
        }
        else
        {
            w_u16(0x800E04E8u + port * 2u, 0u);
            w_u16(0x800E04ECu + port * 2u, 0u);
            w_u32(0x800E04E0u + port * 4u, 0xF0FFu);
        }
    }
}

void mov_800CA584(void)
{
    uint32 port;
    movie_pad_active = 0u;
    for (port = 0u; port < 2u; ++port)
    {
        w_u16(0x800E04E8u + port * 2u, 0u);
        w_u16(0x800E04ECu + port * 2u, 0u);
    }
    /* Release movie polling ownership without stopping the shared game PAD */
}

void mov_800C4DE4(void)
{
    /* Native stream owns sector and ring dispatch for the deferred caller */
    (void)stream_pump();
}

uint32 mov_800C8DF4(void)
{
    uint32 previous = ResetCallbackPSX();
    /* Original SDK initialization exits the critical section before movie VSync */
    ExitCriticalSection();
    return previous;
}
