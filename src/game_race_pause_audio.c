#include "draft_signatures.h"

/* Native CD dispatch retains the original retry and callback shadow sequence */
static uint32 pause_cd_command(uint8 command)
{
    uint32 previous = r_u32(0x8005FBD0u), attempts = 3u;
    for (;;)
    {
        w_u32(0x8005FBD0u, 0u);
        if (command != 1u && (r_u8(0x8005FBDCu) & 0x10u))
            CD_cw(1u, NULL, NULL, 0u);
        w_u32(0x8005FBD0u, previous);
        if (CD_cw(command, NULL, NULL, 0u) == 0)
            return 1u;
        if (--attempts == 0xFFFFFFFFu)
            break;
    }
    w_u32(0x8005FBD0u, previous);
    return 0u;
}

static sint16 pause_decode_volume(sint16 raw)
{
    uint32 value = (uint16)raw;
    if (value >= 0x4000u)
        value -= 0x8000u;
    return (sint16)value;
}

static void pause_voice_volume(uint32 voice, sint16 left, sint16 right)
{
    SpuSetVoiceVolume((sint32)voice, (sint16)((uint16)left & 0x7FFFu), (sint16)((uint16)right & 0x7FFFu));
}

uint32 sub_8001E328(void)
{
    uint32 voice;
    for (voice = 0u; voice < 24u; ++voice)
    {
        SpuVoiceAttr attr = {0};
        sint32 volume;
        sint16 left, right;
        attr.voice = 1u << voice;
        SpuGetVoiceAttr(&attr);
        left = pause_decode_volume(attr.volume.left);
        right = pause_decode_volume(attr.volume.right);
        volume = (sint32)r_u32(0x80064BFCu);
        w_u16(0x80074520u + voice * 4u, (uint16)(volume >= 17 ? ((sint32)left * 4096) / volume : 0));
        volume = (sint32)r_u32(0x80064BFCu);
        w_u16(0x80074522u + voice * 4u, (uint16)(volume >= 17 ? ((sint32)right * 4096) / volume : 0));
        pause_voice_volume(voice, 0, 0);
    }
    return 0u;
}

uint32 sub_8001E450(void)
{
    uint32 voice;
    for (voice = 0u; voice < 24u; ++voice)
    {
        uint32 volume = r_u32(0x80064BFCu);
        sint32 left = (sint32)((uint32)(sint32)(sint16)r_u16(0x80074520u + voice * 4u) * volume);
        sint32 right = (sint32)((uint32)(sint32)(sint16)r_u16(0x80074522u + voice * 4u) * volume);
        pause_voice_volume(voice, (sint16)(left / 4096), (sint16)(right / 4096));
    }
    return 0u;
}

uint32 sub_8002037C(void)
{
    return pause_cd_command(11u);
}

uint32 sub_800203A4(void)
{
    uint8 volume[4];
    uint32 i;
    sint32 level;
    pause_cd_command(12u);
    for (i = 0u; i < 4u; ++i)
        volume[i] = r_u8(0x80064D34u + i);
    level = (sint32)r_u32(0x80064BF8u) / 16;
    if (level < 0)
        level = 0;
    if (level > 255)
        level = 255;
    volume[0] = volume[2] = (uint8)level;
    return sub_8004B89C(volume);
}

uint32 sub_8001B9A4(uint32 mode)
{
    w_u32(0x80064B68u, mode);
    w_u32(0x8006511Cu, mode ? 160u : 320u);
    w_u32(0x80065120u, mode ? 240u : 120u);
    sub_8003BC1C();
    w_u8(0x80073D73u, 3u);
    w_u8(0x80073D77u, 64u);
    w_u8(0x80073D74u, 0u);
    w_u8(0x80073D75u, 0u);
    w_u8(0x80073D76u, 0u);
    w_u16(0x80073D78u, mode ? 159u : 0u);
    w_u16(0x80073D7Au, mode ? 0u : 119u);
    w_u16(0x80073D7Cu, mode ? 159u : 320u);
    w_u16(0x80073D7Eu, mode ? 256u : 119u);
    return mode ? 256u : 320u;
}
