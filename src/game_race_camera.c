#include "draft_signatures.h"
#define CAMERA_GP 0x80064A10u
extern uint32 sub_8001DE6C(void);

uint32 sub_80012E90(uint32 player)
{
    uint32 packet, head, old, width, height;
    if (!r_u32(CAMERA_GP + 0x30u))
        return 0u;
    if (!player)
    {
        if (!r_u8(CAMERA_GP + 0x58u))
        {
            w_u32(CAMERA_GP + 0x5Cu, 0u);
            w_u8(CAMERA_GP + 0x58u, 1u);
            w_u8(CAMERA_GP + 0x60u, 0u);
        }
        if ((sint32)r_u32(CAMERA_GP + 0x5Cu) < 255)
            w_u32(CAMERA_GP + 0x5Cu, r_u32(CAMERA_GP + 0x5Cu) + 5u);
        else
        {
            w_u8(CAMERA_GP + 0x58u, 0u);
            w_u8(CAMERA_GP + 0x60u, 1u);
            sub_8001DE6C();
        }
    }
    if (r_u32(0x8005973Cu) == 1u)
    {
        packet = 0x800655ACu + 40u * r_u32(0x80065128u);
        w_u8(packet + 4u, (uint8)r_u32(CAMERA_GP + 0x5Cu));
        w_u16(packet + 26u, 256u);
        w_u16(packet + 34u, 256u);
        w_u16(packet + 8u, 0u);
        w_u16(packet + 10u, 0u);
        w_u16(packet + 16u, 320u);
        w_u16(packet + 18u, 0u);
        w_u16(packet + 24u, 0u);
        w_u16(packet + 32u, 320u);
        w_u8(0x800655ACu + 40u * r_u32(0x80065128u) + 5u, (uint8)r_u32(CAMERA_GP + 0x5Cu));
        w_u8(0x800655ACu + 40u * r_u32(0x80065128u) + 6u, (uint8)r_u32(CAMERA_GP + 0x5Cu));
        packet = 0x800655ACu + 40u * r_u32(0x80065128u);
    }
    else
    {
        if (r_u32(0x80064B68u))
        {
            width = 160u;
            height = 256u;
        }
        else
        {
            width = 320u;
            height = 128u;
        }
        packet = 0x800655ACu + 80u * player + 40u * r_u32(0x80065128u);
        w_u16(packet + 8u, 0u);
        w_u16(packet + 10u, 0u);
        w_u16(packet + 16u, (uint16)width);
        w_u16(packet + 18u, 0u);
        w_u16(packet + 24u, 0u);
        w_u16(packet + 26u, (uint16)height);
        w_u16(packet + 32u, (uint16)width);
        w_u16(packet + 34u, (uint16)height);
        w_u8(0x800655B0u + 80u * player + 40u * r_u32(0x80065128u), (uint8)r_u32(CAMERA_GP + 0x5Cu));
        w_u8(0x800655B1u + 80u * player + 40u * r_u32(0x80065128u), (uint8)r_u32(CAMERA_GP + 0x5Cu));
        w_u8(0x800655B2u + 80u * player + 40u * r_u32(0x80065128u), (uint8)r_u32(CAMERA_GP + 0x5Cu));
        packet = 0x800655ACu + 80u * player + 40u * r_u32(0x80065128u);
    }
    head = r_u32(r_u32(0x80065118u) + 0x70u);
    w_u32(packet, (r_u32(packet) & 0xFF000000u) | (r_u32(head) & 0xFFFFFFu));
    head = r_u32(r_u32(0x80065118u) + 0x70u);
    old = r_u32(head);
    w_u32(head, (old & 0xFF000000u) | (packet & 0xFFFFFFu));
    return r_u8(CAMERA_GP + 0x60u);
}

static uint32 race_player_viewport(uint32 buffer, uint32 player)
{
    uint32 vertical = r_u32(0x80064B68u), width = vertical ? 160u : 320u;
    uint32 height = vertical ? 240u : 120u;
    uint32 x = vertical ? player * 160u : 0u;
    uint32 y = (buffer << 8u) | (vertical ? 0u : player * 120u);
    SetDefDrawEnv((DRAWENV *)psx_addr(0x80081A38u + buffer * 116u, sizeof(DRAWENV)), (sint32)x, (sint32)y, (sint32)width, (sint32)height);
    w_u32(0x80065240u, width);
    w_u32(0x8006524Cu, height);
    w_u32(0x8006522Cu, 0u);
    w_u32(0x80065230u, 0u);
    w_u32(0x8006511Cu, width);
    w_u32(0x80065120u, height);
    return sub_8003BF0C(width / 2u, height / 2u, width);
}

uint32 sub_8001BAC0(uint32 buffer)
{
    FUNCTION_MARKER(0x8001BAC0u, "SLUS_009.69");
    return race_player_viewport(buffer, 0u);
}

uint32 sub_8001BBB4(uint32 buffer)
{
    FUNCTION_MARKER(0x8001BBB4u, "SLUS_009.69");
    return race_player_viewport(buffer, 1u);
}

void sub_8003997C(uint32 buffer)
{
    uint32 environment = 0x80081A38u + buffer * 116u;
    FUNCTION_MARKER(0x8003997Cu, "SLUS_009.69");
    PutDrawEnv((DRAWENV *)psx_addr(environment, sizeof(DRAWENV)));
    PutDispEnv((DISPENV *)psx_addr(environment + 92u, sizeof(DISPENV)));
}
