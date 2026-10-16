#include "draft_signatures.h"
#include "psx_stream.h"
#include "psx_spu.h"
#include "xport.h"
#include <stdio.h>
#include <string.h>

static uint32 movie_pad_active;

uint32 mov_800C1524(void)
{
    uint32 attempt;
    for (attempt = 0u; attempt < 5u; ++attempt)
        if (CdInit() == 1)
            return 1u;
    draft_agent1_missing_diagnostic("CdInit: Init failed\n");
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
    memset(psx_addr(0x800E04F0, 8), 0, 8);
    movie_pad_active = 1;
}

static void movie_pad_decode(uint32 port)
{
    uint32 packet = 0x801E2E48 + port * 34;
    uint32 previous_address = 0x800E048C + port * 4;
    uint32 type = r_u16(packet) & 0xF0FF;
    uint32 buttons, axis, actuator, state;
    w_u32(0x800E04E0 + port * 4, type);
    if (type == 0x4000 || type == 0x5000 || type == 0x7000)
    {
        actuator = 0x800E04F0 + port * 4;
        state = PadGetStatePSX(port * 16);
        if (state == 6)
        {
            if (r_u8(actuator + 2))
                PadSetActPSX(port * 16, actuator, 2);
            else if (PadSetActAlignPSX(port * 16, 0x800E0484))
                w_u8(actuator + 2, 1);
        }
        else
            w_u8(actuator + 2, 0);
        if (type != 0x4000)
            w_u32(0x800E04F8 + port * 4, r_u8(packet + 6));
        buttons = ~(uint32)r_u16(packet + 2);
    }
    else if (type == 0x2000)
    {
        w_u32(0x800E04F8 + port * 4, r_u8(packet + 4));
        w_u32(0x800E0500 + port * 4, r_u8(packet + 5));
        w_u32(0x800E0508 + port * 4, r_u8(packet + 6));
        buttons = ~(uint32)r_u16(packet + 2);
        if (r_u8(packet + 5) >= 65)
            buttons |= 0x4000;
        if (r_u8(packet + 7) >= 65)
            buttons |= 0x400;
    }
    else
    {
        w_u16(0x800E04EC + port * 2, 0);
        w_u16(0x800E04E8 + port * 2, 0);
        return;
    }
    w_u16(0x800E04E8 + port * 2, (uint16)(~r_u32(previous_address) & buttons));
    w_u16(0x800E04EC + port * 2, (uint16)buttons);
    w_u32(previous_address, buttons);
    if (type != 0x5000 && type != 0x7000)
        return;
    axis = r_u8(packet + 5);
    if (axis < 128)
    {
        w_u32(0x800E0500 + port * 4, 2 * (128 - axis));
        w_u32(0x800E0508 + port * 4, 0);
    }
    else
    {
        w_u32(0x800E0500 + port * 4, 0);
        w_u32(0x800E0508 + port * 4, 2 * (axis - 127));
    }
    if (r_u32(0x800E04D8 + port * 4))
    {
        buttons = (uint32)(sint32)(sint16)r_u16(0x800E04EC + port * 2);
        if (buttons & r_u32(0x800E0518 + port * 4))
            w_u32(0x800E0500 + port * 4, 256);
        if (buttons & r_u32(0x800E04D8 + port * 4))
            w_u32(0x800E0508 + port * 4, 256);
    }
}

void mov_800CC840(void)
{
    uint32 port;
    if (!movie_pad_active)
        return;
    for (port = 0; port < 2; ++port)
    {
        uint32 packet = 0x801E2E48 + port * 34;
        uint16 host = (uint16)PadRead((sint32)port);
        uint16 held = (uint16)((host >> 8) | (host << 8));
        uint8 axes[4];
        sint32 analog = xport_input_analog((sint32)port, axes);
        /* Sample both host ports without rebinding the shared game PAD owner */
        analog = analog && PadInfoMode((sint32)(port * 16), 1, 0) == 7;
        w_u8(packet, 0);
        w_u8(packet + 1, analog ? 0x73 : 0x41);
        w_u16(packet + 2, (uint16)~held);
        memcpy(psx_addr(packet + 4, 4), axes, 4);
        movie_pad_decode(port);
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
