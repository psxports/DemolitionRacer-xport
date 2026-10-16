#include "draft_signatures.h"
#include "psx_gpu.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))

uint32 sub_8001BCAC(void);
uint32 sub_80013A9C(void);
uint32 sub_80016C68(void);
uint32 sub_8002F4A4(uint32 state, uint32 position);
uint32 game_800AE420(uint32 track);
void game_800A7B6C(uint32 player, uint32 lap, uint32 lap_limit);
uint32 game_8009A114(uint32 object);
void game_8008F9A8(uint32 port);
uint32 sub_800131C8(void);
uint32 sub_80037B18(void);
uint32 sub_80017D38(void);
uint32 sub_80019F54(void);
void game_800BD4B8(void);
uint32 sub_8001DEDC(uint32 player);
uint32 sub_8001DFBC(uint32 player);
uint32 sub_8001D1DC(void);
uint32 sub_8001EE94(void);
uint32 sub_80012E90(uint32 player);
uint32 sub_80020AB8(uint32 font, sint32 x, sint32 y, const char *text);
void game_800AE17C(void);
void game_800AEB4C(void);
void game_800B77CC(void);
void game_8009564C(void);
void game_8008FCA4(uint32 port);
uint32 sub_80042754(void);
void game_80090040(uint32 player, uint32 port);
void game_8008F6F8(void);
void game_800B3924(void);
void game_800A7098(uint32 player);
void game_800A23CC(uint32 port);
void game_800B8704(void);
void game_8009545C(void);
void game_800B9518(void);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
static void level_pipe_init(void);
static void level_pipe_update(void);
static void level_pipe_draw(void);
static void level_texture_init(void);
static void level_texture_update(void);
static void level_particles(void);
static void level_crane_init(void);
static void level_crane_update(void);
static void level_crane_draw(void);

uint32 sub_800131C8(void)
{
    uint32 buffer = sub_80042544(1600u), packet, index, texture, x, y, tile = 0u;
    uint32 bottom;
    R32(0x800651C0u) = buffer;
    R32(0x80064A74u) = 0u;
    R32(0x800651D4u) = sub_80039ADC(0x80064A78u);
    packet = R32(0x800651C0u);
    for (index = 0u; index < 40u; ++index, packet += 40u)
    {
        R8(packet + 3u) = 9u;
        R8(packet + 7u) = 44u;
        R8(packet + 12u) = R8(R32(0x800651D4u) + 24u);
        R8(packet + 13u) = R8(R32(0x800651D4u) + 25u);
        texture = R32(0x800651D4u);
        R8(packet + 20u) = (uint8)(R8(texture + 24u) + R8(texture + 22u));
        R8(packet + 21u) = R8(R32(0x800651D4u) + 25u);
        R8(packet + 28u) = R8(R32(0x800651D4u) + 24u);
        texture = R32(0x800651D4u);
        R8(packet + 29u) = (uint8)(R8(texture + 25u) + R8(texture + 23u));
        texture = R32(0x800651D4u);
        R8(packet + 36u) = (uint8)(R8(texture + 24u) + R8(texture + 22u));
        texture = R32(0x800651D4u);
        bottom = R8(texture + 25u) + R8(texture + 23u);
        R8(packet + 4u) = 128u;
        R8(packet + 5u) = 128u;
        R8(packet + 6u) = 128u;
        R8(packet + 37u) = (uint8)bottom;
        texture = R32(0x800651D4u);
        R16(packet + 22u) = R16(texture + 8u);
        R16(packet + 14u) = R16(texture + 10u);
    }
    for (y = 0u; y < 256u; y += 64u)
        for (x = 0u; x < 320u; x += 64u, ++tile)
        {
            R16(0x8006564Cu + tile * 4u) = (uint16)x;
            R16(0x8006564Eu + tile * 4u) = (uint16)y;
        }
    return 0u;
}

uint32 sub_80037B18(void)
{
    R32(0x800650E0u) = 0u;
    R32(0x800650E4u) = 31u;
    R32(0x800650E8u) = 0u;
    return 31u;
}

/* TODO: Replace pending boundaries only after the callee ABI and image are reviewed */
static uint32 race_pending(uint32 target, uint32 argument_count, const uint32 *arguments)
{
    uint32 index;
    draft_agent1_missing_diagnostic("Untranslated race dependency %08X (%u arguments)", target, argument_count);
    for (index = 0u; index < argument_count; ++index)
        draft_agent1_missing_diagnostic(" %08X", arguments[index]);
    draft_agent1_missing_diagnostic("\n");
    abort();
}

static uint32 race_call0(uint32 target)
{
    switch (target)
    {
        case 0x800BDA5C:
            level_pipe_init();
            return 0;
        case 0x800BDA7C:
            level_pipe_update();
            return 0;
        case 0x800BDA9C:
            level_pipe_draw();
            return 0;
        case 0x800BDD68:
            level_texture_init();
            return 0;
        case 0x800BDD88:
            level_texture_update();
            return 0;
        case 0x800BE0BC:
            level_particles();
            return 0;
        case 0x800BE6A0:
            level_crane_init();
            return 0;
        case 0x800BE6C0:
            level_crane_update();
            return 0;
        case 0x800BE6E0:
            level_crane_draw();
            return 0;
        /* Empty callbacks in the otherwise nonempty level rows */
        case 0x800BDDA8:
        case 0x800BE0B4:
        case 0x800BE0DC:
            return 0;
        /* Remaining reviewed empty entries in the level callback table */
        case 0x800BDDB0:
        case 0x800BDDB8:
        case 0x800BDDC0:
        case 0x800BDDC8:
        case 0x800BDDD0:
        case 0x800BDDD8:
        case 0x800BDDE0:
        case 0x800BDDE8:
        case 0x800BDDF0:
        case 0x800BDE10:
        case 0x800BDE18:
        case 0x800BDE20:
        case 0x800BDE28:
        case 0x800BDE30:
        case 0x800BDE38:
        case 0x800BE700:
        case 0x800BE708:
        case 0x800BE710:
        case 0x800BE718:
        case 0x800BE720:
        case 0x800BE728:
            return 0;
        /* Level 4 callbacks each contain JR RA and a NOP delay slot */
        case 0x800BDAD4:
        case 0x800BDADC:
        case 0x800BDAE4:
            return 0;
        /* Level 3 callbacks each contain JR RA and a NOP delay slot */
        case 0x800BDABC:
        case 0x800BDAC4:
        case 0x800BDACC:
            return 0;
        /* Level 9 callbacks each contain JR RA and a NOP delay slot */
        case 0x800BDDF8:
        case 0x800BDE00:
        case 0x800BDE08:
            return 0;
        case 0x800B8704:
            game_800B8704();
            return 0;
        case 0x8009545C:
            game_8009545C();
            return 0;
        case 0x800B9518:
            game_800B9518();
            return 0;
        case 0x8002AD28:
            return sub_8002AD28();
        /* The complete original BD530 consists of JR RA and its NOP delay slot */
        case 0x800AA78Cu:
            game_800AA78C();
            return 0u;
        case 0x800AA544u:
            game_800AA544();
            return 0u;
        case 0x800A9A68u:
            game_800A9A68();
            return 0u;
        case 0x800BC0B4u:
            game_800BC0B4();
            return 0u;
        case 0x800BB558u:
            game_800BB558();
            return 0u;
        case 0x800B3810u:
            game_800B3810();
            return 0u;
        case 0x8001372Cu:
            return sub_8001372C();
        case 0x80013364u:
            sub_80013364();
            return 0u;
        case 0x800BA7B4u:
            game_800BA7B4();
            return 0u;
        case 0x8001E328u:
            return sub_8001E328();
        case 0x800BD538u:
            return 0u;
        case 0x800BD540u:
            return 0u;
        case 0x800BD530u:
            return 0u;
        case 0x80016C68u:
            return sub_80016C68();
        case 0x8003BC1Cu:
            return sub_8003BC1C();
        case 0x8003A578u:
            return sub_8003A578();
        case 0x800399D4u:
            return sub_800399D4();
        case 0x80039908u:
            return sub_80039908();
        case 0x80039934u:
            return sub_80039934();
        case 0x80039A10u:
            return sub_80039A10();
        case 0x800200E0u:
            return (uint32)sub_800200E0();
        case 0x80017D38u:
            return sub_80017D38();
        case 0x80019F54u:
            return sub_80019F54();
        case 0x800BD4B8u:
            game_800BD4B8();
            return 0u;
        case 0x80012C3Cu:
            sub_80012C3C();
            return 0u;
        case 0x800208E4u:
            return sub_800208E4();
        case 0x8001697Cu:
            return sub_8001697C();
        case 0x80015664u:
            return sub_80015664();
        case 0x8001CD48u:
            return sub_8001CD48();
        case 0x800A9E24u:
            game_800A9E24();
            return 0u;
        case 0x8009076Cu:
            return game_8009076C();
        case 0x8001F354u:
            return sub_8001F354();
        case 0x8001D1DCu:
            return sub_8001D1DC();
        case 0x8001EE94u:
            return sub_8001EE94();
        case 0x800AE17Cu:
            game_800AE17C();
            return 0u;
        case 0x800AEB4Cu:
            game_800AEB4C();
            return 0u;
        case 0x800B77CCu:
            game_800B77CC();
            return 0u;
        case 0x8009564Cu:
            game_8009564C();
            return 0u;
        case 0x80042754u:
            return sub_80042754();
        case 0x800B053Cu:
            game_800B053C();
            return 0u;
        case 0x800B13A8u:
            game_800B13A8();
            return 0u;
        case 0x80042784u:
            return sub_80042784();
        case 0x80017E68u:
            return sub_80017E68();
        case 0x80017C9Cu:
            return sub_80017C9C();
        case 0x80017D80u:
            return sub_80017D80();
        case 0x8003408Cu:
            sub_8003408C();
            return 0u;
        case 0x8008F6F8u:
            game_8008F6F8();
            return 0u;
        case 0x800B3924u:
            game_800B3924();
            return 0u;
        default:
            return race_pending(target, 0u, NULL);
    }
}

static uint32 race_call1(uint32 target, uint32 argument)
{
    if (target == 0x8001BAC0u)
        return sub_8001BAC0(argument);
    if (target == 0x8001BBB4u)
        return sub_8001BBB4(argument);
    if (target == 0x8003997Cu)
    {
        sub_8003997C(argument);
        return 0u;
    }
    switch (target)
    {
        case 0x8008F784u:
            game_8008F784(argument);
            return 0u;
        case 0x8003D9ACu:
            return (uint32)DrawSync((sint32)argument);
        case 0x80047044u:
            return (uint32)VSync((sint32)argument);
        case 0x80020430u:
            return sub_80020430(argument);
        case 0x8004261Cu:
            return sub_8004261C(argument);
        case 0x80021430u:
            return sub_80021430(argument);
        case 0x8001DEDCu:
            return sub_8001DEDC(argument);
        case 0x8001DFBCu:
            return sub_8001DFBC(argument);
        case 0x80012E90u:
            return sub_80012E90(argument);
        case 0x8008FCA4u:
            game_8008FCA4(argument);
            return 0u;
        case 0x800A7794u:
            game_800A7794(argument);
            return 0u;
        case 0x800B3638u:
            game_800B3638(argument);
            return 0u;
        case 0x800B9374u:
            game_800B9374(argument);
            return 0u;
        case 0x800A7098u:
            game_800A7098(argument);
            return 0u;
        case 0x800A23CCu:
            game_800A23CC(argument);
            return 0u;
        default:
            return race_pending(target, 1u, &argument);
    }
}

static uint32 race_call2(uint32 target, uint32 first, uint32 second)
{
    uint32 arguments[2] = {first, second};
    if (target == 0x800A730Cu)
    {
        game_800A730C(first, second);
        return 0u;
    }
    if (target == 0x80020A14u)
        return sub_80020A14(first, second);
    if (target == 0x80037DC8u)
        return sub_80037DC8(first, second);
    if (target == 0x80090C50u)
    {
        game_80090C50(first, second);
        return 0u;
    }
    if (target == 0x80090858u)
    {
        game_80090858(first, second);
        return 0u;
    }
    if (target == 0x80090040u)
    {
        game_80090040(first, second);
        return 0u;
    }
    return race_pending(target, 2u, arguments);
}

static uint32 race_call3(uint32 target, uint32 first, uint32 second, uint32 third)
{
    uint32 arguments[3] = {first, second, third};
    if (target == 0x80015780)
        return sub_80015780(first, second, third);
    if (target == 0x800B8DB8u)
    {
        game_800B8DB8(first, (sint32)second, (sint32)third);
        return 0u;
    }
    if (target == 0x8001CDBCu)
        return sub_8001CDBC(first, second, third);
    if (target == 0x80036914u)
        return sub_80036914(first, second, third);
    return race_pending(target, 3u, arguments);
}

static void race_empty1E310(uint32 argument)
{
}

static void race_empty1E318(uint32 argument)
{
}

static void race_empty42C44(void)
{
}

uint32 sub_8003AAC8(uint32 player, uint32 first, uint32 second)
{
    uint32 address = 0x800654F8u + (player << 2);
    if (first != ~0u)
        R8(address) = (uint8)first;
    if (second != ~0u)
        R8(address + 1u) = (uint8)second;
    return ~0u;
}

uint32 sub_800151DC(void)
{
    if (R32(0x800593A8u))
        sub_8001BCAC();
    else
        sub_80013A9C();
    sub_8003AAC8(0u, 0u, 0u);
    return sub_8003AAC8(1u, 0u, 0u);
}

static void race_dispatch_slot(uint32 slot)
{
    uint32 target = R32(0x800BFD60u + (R32(0x80064C08u) - 1u) * 12u + slot);
    race_call0(target);
}

static void race_copy_camera(uint32 destination)
{
    uint32 offset, first, second;
    for (offset = 0u; offset < 32u; offset += 8u)
    {
        first = R32(0x80081BA4u + offset);
        second = R32(0x80081BA8u + offset);
        R32(destination + offset) = first;
        R32(destination + offset + 4u) = second;
    }
}

static void race_message(uint32 messages, uint32 width)
{
    uint32 measured = race_call2(0x80020A14u, 5u, R32(messages + R32(0x80064B8Cu) * 4u));
    uint32 remaining = width - measured;
    uint32 text = R32(messages + R32(0x80064B8Cu) * 4u);
    sub_80020AB8(5u, (sint32)(remaining + (remaining >> 31)) >> 1, 24, (const char *)psx_addr(text, 1u));
}

uint32 sub_80013A9C(void)
{
    uint32 players, index, count, result, argument, value, pointer;
    /* Native C uses the host stack instead of the original 46FF4 stack swap */
    R32(0x800651B0u) = 0u;
    R32(0x800655A4u) = 0u;
    R32(0x800651D0u) = 0u;
    R32(0x800651D8u) = 0u;
    R32(0x800651DCu) = 1u;
    R32(0x800651ACu) = 0u;
    R32(0x80064A30u) = 0u;
    R32(0x80064A38u) = 0u;
    R32(0x800651B8u) = 0u;
    race_empty1E318(0x80066D78u);
    race_empty1E310(0x80066D84u + R32(0x800651D8u) * 952u);
    race_call0(0x80016C68u);
    value = R32(0x8006511Cu);
    pointer = R32(0x80065120u);
    R32(0x8006522Cu) = 0u;
    R32(0x80065230u) = 0u;
    R32(0x80065240u) = value;
    R32(0x8006524Cu) = pointer;
    R32(0x80065198u) = race_call1(0x80047044u, ~0u);
    race_dispatch_slot(0u);
    players = R32(0x80065254u);
    R32(0x800651A0u) = 0u;
    if ((sint32)players > 0)
    {
        do
        {
            game_8009A114(0x80066D14u + R32(0x800651A0u) * 952u);
            index = R32(0x800651A0u) + 1u;
            count = R32(0x80065254u);
            R32(0x800651A0u) = index;
        } while ((sint32)index < (sint32)count);
    }
    race_call1(0x8001DEDCu, 0u);
    sub_800131C8();
    R32(0x80064A58u) = 0u;
    R32(0x80064A2Cu) = 0u;
    sub_80037B18();
    race_call1(0x80020430u, R8(0x800593ABu + R32(0x80064C08u)));
    value = R32(0x80065338u);
    result = R32(0x80064A58u);
    R32(0x800651A4u) = value;
    while (!result)
    {
        if (R32(0x800651D0u) || R32(0x800651ACu))
            R32(0x80064A40u) = 1u;
        race_empty42C44();
        race_call0(0x80017D38u);
        race_call0(0x80019F54u);
        race_call0(0x800BD4B8u);
        players = R32(0x80065254u);
        R32(0x80064A34u) = 0u;
        R32(0x800651A0u) = 0u;
        if ((sint32)players > 0)
        {
            do
            {
                index = R32(0x800651A0u);
                if (R8(0x80066E8Eu + index * 952u) & 1u)
                    R32(0x80064A34u) += 1u;
                R32(0x800651A0u) = index + 1u;
            } while ((sint32)(index + 1u) < (sint32)players);
        }
        sub_8003A578();
        value = R32(0x80065538u);
        if (R32(0x800651A4u) != value)
        {
            game_8008F9A8(0u);
            value = R32(0x80065538u);
        }
        pointer = R32(0x80064B5Cu);
        R32(0x800651A4u) = value;
        if ((pointer || R32(0x80065110u) == 1u) && (R16(0x80065524u) || R16(0x80065526u)))
            R32(0x80064A40u) = 1u;
        if (R32(0x80064B5Cu) && (R32(0x80064A30u) & 15u) < 8u)
        {
            race_call1(0x80021430u, 3u);
            race_message(0x80059378u, 320u);
        }
        else if (R32(0x80065110u) == 1u && (R32(0x80064A30u) & 15u) < 8u)
        {
            race_call1(0x80021430u, 3u);
            race_message(0x8005938Cu, 320u);
        }
        race_call0(0x80012C3Cu);
        race_call0(0x800208E4u);
        race_call1(0x8008FCA4u, 0u);
        if (!R32(0x80064B5Cu))
        {
            if (R8(0x80065538u) == 255u)
            {
                R32(0x80064A80u) += 1u;
                if ((sint32)R32(0x80064A80u) >= 31)
                    R32(0x80064A80u) = 0u;
            }
            else
            {
                R32(0x80064A80u) = 0u;
                R32(0x8006519Cu) = 0u;
            }
        }
        sub_800399D4();
        sub_80039908();
        race_call0(0x80042754u);
        race_call2(0x80090040u, 0u, 0u);
        if (R32(0x800593A0u))
        {
            race_call0(0x800B8704u);
            race_call0(0x8009545Cu);
        }
        else
        {
            race_call0(0x800B77CCu);
            race_call0(0x8009564Cu);
        }
        value = R32(0x80064A54u);
        if (value)
        {
            race_call3(0x80015780u, 0u, value, 0x80066D78u);
            R32(0x80064A54u) = 0u;
        }
        race_call0(0x8001697Cu);
        race_call1(0x8001DFBCu, 0u);
        race_call0(0x8001D1DCu);
        race_call0(0x8001EE94u);
        race_call0(0x80015664u);
        race_call0(0x800AE17Cu);
        R32(0x80064A58u) = race_call1(0x80012E90u, 0u);
        race_call0(0x800AEB4Cu);
        race_call0(0x80016C68u);
        race_call0(0x8003408Cu);
        race_call2(0x80037DC8u, 0x80066D14u + R32(0x800651D8u) * 952u, 0u);
        race_call2(0x80090C50u, 0x80066D14u, R8(0x800670B8u));
        race_call3(0x8001CDBCu, 0u, 0x80066D14u, R8(0x800670B8u));
        race_call0(0x8008F6F8u);
        race_call0(0x800B3924u);
        race_call1(0x800A7098u, 0x80066D14u + R32(0x800651D8u) * 952u);
        if (R32(0x80064B5Cu) || R32(0x80065110u) == 1u || !R32(0x80059764u))
            sub_8003AAC8(0u, 0u, 0u);
        else
            race_call2(0x80090858u, 0u, 0u);
        argument = ~0u;
        if (!R8(0x800670C2u) && R32(0x80065110u) != 1u && !R32(0x80064B5Cu))
            argument = 0u;
        race_call1(0x800A7794u, argument);
        race_call0(0x800B053Cu);
        race_call0(0x800B13A8u);
        race_call1(0x800B3638u, 0x80066D14u + R32(0x800651D8u) * 952u);
        if (R32(0x800593A0u))
            race_call0(0x800B9518u);
        else
        {
            race_call3(0x800B8DB8u, 0u, (uint32)(sint32)(sint16)R16(0x80066D6Eu), (uint32)(sint32)(sint16)R16(0x80066D70u));
            race_call1(0x800B9374u, 0u);
        }
        race_dispatch_slot(4u);
        race_dispatch_slot(8u);
        race_call0(0x80017E68u);
        race_call0(0x80017C9Cu);
        race_call0(0x80017D80u);
        race_call0(0x800AA78Cu);
        race_call0(0x800AA544u);
        race_call1(0x8008F784u, 0u);
        race_call0(0x800A9A68u);
        race_call0(0x800BC0B4u);
        race_call0(0x800BB558u);
        race_call0(0x80042784u);
        race_call1(0x8003D9ACu, 0u);
        race_call0(0x800B3810u);
        race_call1(0x8003D9ACu, 0u);
        do
        {
            value = race_call1(0x80047044u, ~0u);
        } while ((sint32)(value - R32(0x80065198u)) < 2);
        R32(0x80065198u) = race_call1(0x80047044u, ~0u);
        sub_800200E0();
        sub_80039934();
        sub_80039A10();
        R32(0x80064A30u) += 1u;
        race_call0(0x8001372Cu);
        if (R32(0x80064B5Cu) || R32(0x80065110u) == 1u)
        {
            if (R16(0x80065520u))
                R32(0x800651ACu) = 1u;
            if (R16(0x80065522u))
                R32(0x800651ACu) = 1u;
            if (R32(0x80064B5Cu))
            {
                R32(0x800651B8u) += 1u;
                if ((sint32)R32(0x800651B8u) >= 901)
                    R32(0x800651ACu) = 1u;
            }
        }
        else if (!R32(0x80064A44u))
        {
            race_call0(0x800BA7B4u);
            if (R32(0x80064A30u) >= 0x4651u)
                R32(0x800651ACu) = 1u;
        }
        race_call0(0x80013364u);
        if (R32(0x80064A84u) == 2u)
            race_call0(0x8001CBACu);
        result = R32(0x80064A58u);
    }
    race_call0(0x8001CD48u);
    race_call0(0x8001E328u);
    sub_8003AAC8(0u, 0u, 0u);
    race_call1(0x8004261Cu, R32(0x800651BCu));
    if (!R32(0x80064A2Cu) && !R32(0x80064B5Cu) && !R8(0x80064EA1u))
        race_call0(0x8002AD28u);
    race_call1(0x8003D9ACu, 0u);
    race_call1(0x80047044u, 0u);
    race_call0(0x8001E328u);
    return race_call0(0x8001CD48u);
}

static void race_render_screen(uint32 player)
{
    uint32 object = player ? 0x800670CCu : 0x80066D14u;
    uint32 argument;
    race_call1(player ? 0x8001BBB4u : 0x8001BAC0u, R32(0x8006527Cu));
    sub_800399D4();
    sub_80039908();
    if (!player)
    {
        uint32 ordering_table = R32(0x80065118u);
        uint32 slot = R32(ordering_table + 112u);
        R32(0x80073D70u) = (R32(0x80073D70u) & 0xFF000000u) | (R32(slot) & 0xFFFFFFu);
        slot = R32(ordering_table + 112u);
        R32(slot) = (R32(slot) & 0xFF000000u) | 0x00073D70u;
    }
    race_call0(0x80042754u);
    if (!player)
        race_call0(0x800208E4u);
    race_call2(0x80037DC8u, object, player);
    race_copy_camera(player ? 0x80073DD0u : 0x80073DB0u);
    if (player)
        R32(0x80064D18u) = 1u;
    race_call2(0x800A730Cu, object, player);
    race_call1(0x800A23CCu, player);
    if (R32(0x800593A0u))
        race_call0(0x800B9518u);
    else
    {
        race_call3(0x800B8DB8u, player, (uint32)(sint32)(sint16)R16(object + 90u), (uint32)(sint32)(sint16)R16(object + 92u));
        race_call1(0x800B9374u, player);
    }
    race_call0(0x80017C9Cu);
    race_call0(0x80042784u);
    race_dispatch_slot(8u);
    race_call0(0x800BC0B4u);
    race_call0(player ? 0x800A9E24u : 0x800A9A68u);
    race_call1(0x8008F784u, player);
    argument = 0u;
    if (R8(object + 942u) || R32(0x80065110u) == 1u || R32(0x80064B5Cu))
        argument = ~0u;
    race_call1(0x800A7794u, argument);
    if (R32(0x80065110u) == 1u && (R32(0x80064A30u) & 15u) < 8u)
    {
        if (!player)
            race_call1(0x80021430u, 3u);
        race_message(0x8005938Cu, R32(0x80064B68u) ? 160u : 320u);
    }
    if (player)
        race_call0(0x80017D80u);
    else
        sub_800200E0();
    R32(0x80064D0Cu) = race_call1(0x80012E90u, player);
    race_call1(0x8003997Cu, R32(0x8006527Cu));
    sub_80039A10();
}

uint32 sub_8001BCAC(void)
{
    uint32 viewport, players, index, count, first, second, result;
    /* Native C uses the host stack instead of the original 46FF4 stack swap */
    viewport = R32(0x80064B68u);
    R32(0x800655A4u) = 0u;
    R32(0x800651D0u) = 0u;
    R32(0x800651D8u) = 0u;
    R32(0x800651DCu) = 1u;
    R32(0x8006528Cu) = 0u;
    R32(0x80064A30u) = 0u;
    R32(0x80064B68u) = viewport;
    if (viewport)
    {
        R32(0x8006511Cu) = 160u;
        R32(0x80065120u) = 240u;
        sub_8003BC1C();
        R8(0x80073D73u) = 3u;
        R8(0x80073D77u) = 64u;
        R16(0x80073D78u) = 159u;
        R16(0x80073D7Cu) = 159u;
        R8(0x80073D74u) = 0u;
        R8(0x80073D75u) = 0u;
        R8(0x80073D76u) = 0u;
        R16(0x80073D7Au) = 0u;
        R16(0x80073D7Eu) = 256u;
    }
    else
    {
        R32(0x8006511Cu) = 320u;
        R32(0x80065120u) = 120u;
        sub_8003BC1C();
        R8(0x80073D73u) = 3u;
        R8(0x80073D77u) = 64u;
        R8(0x80073D74u) = 0u;
        R8(0x80073D75u) = 0u;
        R8(0x80073D76u) = 0u;
        R16(0x80073D78u) = 0u;
        R16(0x80073D7Au) = 119u;
        R16(0x80073D7Cu) = 320u;
        R16(0x80073D7Eu) = 119u;
    }
    R32(0x8006527Cu) = R32(0x80065128u);
    R32(0x8006526Cu) = (uint32)VSync(-1);
    R32(0x800651D0u) = 0u;
    race_dispatch_slot(0u);
    players = R32(0x80065254u);
    R32(0x80065274u) = 0u;
    if ((sint32)players > 0)
    {
        do
        {
            game_8009A114(0x80066D14u + R32(0x80065274u) * 952u);
            count = R32(0x80065254u);
            index = R32(0x80065274u) + 1u;
            R32(0x80065274u) = index;
        } while ((sint32)index < (sint32)count);
    }
    race_call3(0x80036914u, 0x80066D3Cu, 1u, 0u);
    race_call3(0x80036914u, 0x800670F4u, 1u, 1u);
    race_empty1E318(0x80066D78u + R32(0x800651D8u) * 952u);
    race_empty1E310(0x80066D84u + R32(0x800651D8u) * 952u);
    race_call1(0x8001DEDCu, 0u);
    sub_800131C8();
    R32(0x80064D0Cu) = 0u;
    R32(0x80064A2Cu) = 0u;
    sub_80037B18();
    race_call1(0x80020430u, R8(0x800593ABu + R32(0x80064C08u)));
    first = R32(0x80065338u);
    second = R32(0x8006533Cu);
    result = R32(0x80064D0Cu);
    R32(0x80065280u) = first;
    R32(0x80065284u) = second;
    while (!result)
    {
        if (R32(0x800651D0u) || R32(0x8006528Cu))
            R32(0x80064A40u) = 1u;
        race_call0(0x80019F54u);
        race_call0(0x800BD4B8u);
        players = R32(0x80065254u);
        R32(0x80064A34u) = 0u;
        R32(0x80065274u) = 0u;
        if ((sint32)players > 0)
        {
            do
            {
                index = R32(0x80065274u);
                if (R8(0x80066E8Eu + index * 952u) & 1u)
                    R32(0x80064A34u) += 1u;
                R32(0x80065274u) = index + 1u;
            } while ((sint32)(index + 1u) < (sint32)players);
        }
        sub_8003A578();
        if (R32(0x80065280u) != R32(0x80065538u))
            game_8008F9A8(0u);
        if (R32(0x80065284u) != R32(0x8006553Cu))
            game_8008F9A8(1u);
        first = R32(0x80065538u);
        second = R32(0x8006553Cu);
        R32(0x80065280u) = first;
        R32(0x80065284u) = second;
        race_call1(0x8008FCA4u, 0u);
        race_call1(0x8008FCA4u, 1u);
        race_call2(0x80090040u, 0u, 0u);
        race_call2(0x80090040u, 1u, 1u);
        race_call0(0x8009076Cu);
        race_call0(0x80012C3Cu);
        first = R32(0x800593A0u);
        R32(0x8006527Cu) ^= 1u;
        if (first)
        {
            race_call0(0x800B8704u);
            race_call0(0x8009545Cu);
        }
        else
        {
            race_call0(0x800B77CCu);
            race_call0(0x8009564Cu);
        }
        race_call0(0x8001697Cu);
        race_call1(0x8001DFBCu, 0u);
        race_call0(0x8001D1DCu);
        race_call0(0x8001F354u);
        race_call0(0x80015664u);
        race_call0(0x800AE17Cu);
        race_call0(0x8001372Cu);
        if (R32(0x80065110u) == 1u)
        {
            if (R16(0x80065520u))
                R32(0x8006528Cu) = 1u;
            if (R16(0x80065522u))
                R32(0x8006528Cu) = 1u;
        }
        else
        {
            race_call0(0x800BA7B4u);
            if (R32(0x80064A30u) >= 0x4651u)
                R32(0x8006528Cu) = 1u;
        }
        R32(0x80065128u) = 1u;
        /* TODO: Review the full 16C68 ABI before replacing its pending boundary */
        race_call0(0x80016C68u);
        race_call0(0x8003408Cu);
        race_call0(0x8008F6F8u);
        race_call0(0x800AEB4Cu);
        if (R32(0x80064B5Cu) || R32(0x80065110u) == 1u || !R32(0x80059764u))
            sub_8003AAC8(0u, 0u, 0u);
        else
            race_call2(0x80090858u, 0u, 0u);
        if (R32(0x80064B5Cu) || R32(0x80065110u) == 1u || !R32(0x80059768u))
            sub_8003AAC8(1u, 0u, 0u);
        else
            race_call2(0x80090858u, 1u, 1u);
        race_dispatch_slot(4u);
        R32(0x80064A30u) += 1u;
        race_empty42C44();
        race_call1(0x8003D9ACu, 0u);
        race_call0(0x80017D38u);
        race_render_screen(0u);
        race_render_screen(1u);
        /* Service host events after both viewports are complete */
        psx_native_poll_events();
        race_call0(0x80013364u);
        result = R32(0x80064D0Cu);
    }
    race_call0(0x8001CD48u);
    race_call0(0x8001E328u);
    sub_8003AAC8(0u, 0u, 0u);
    sub_8003AAC8(1u, 0u, 0u);
    race_call1(0x8004261Cu, R32(0x800651BCu));
    if (!R32(0x80064A2Cu) && !R8(0x80064EA1u))
        race_call0(0x8002AD28u);
    race_call1(0x8003D9ACu, 0u);
    return (uint32)VSync(0);
}

uint32 sub_80016C68(void)
{
    uint16 progress_cache[44];
    uint32 result = R32(0x800593A0u);
    uint32 count, index, swaps, first, second, first_object, second_object;
    uint32 first_distance, second_distance, first_progress, second_progress;
    uint32 length, baseline, object, lap, next_lap, threshold, crossed, sum, previous_laps;
    if (result == 1u)
        return result;
    result = R32(0x80064A44u);
    if (result == 1u)
        return result;
    count = R32(0x80065254u);
    for (index = 0u; (sint32)index < (sint32)count; ++index)
        progress_cache[index] = 0xC001u;
    do
    {
        swaps = 0u;
        for (index = 0u; (sint32)index < (sint32)(R32(0x80065254u) - 1u); ++index)
        {
            first = R8(0x80066B1Cu + index);
            first_object = 0x80066D14u + first * 952u;
            if (R8(first_object + 945u))
                continue;
            second = R8(0x80066B1Du + index);
            first_distance = R16(first_object + 92u);
            second_object = 0x80066D14u + second * 952u;
            second_distance = R16(second_object + 92u);
            if (R32(0x80059738u) == 2u)
            {
                uint32 turnaround = R32(0x80064C14u);
                if ((sint32)turnaround >= (sint32)first)
                    first_distance = R32(R32(0x800651E4u) + 60u) - first_distance;
                if ((sint32)turnaround >= (sint32)second)
                    second_distance = R32(R32(0x800651E4u) + 60u) - second_distance;
            }
            if (R8(0x800670C5u + first * 952u) || R8(0x800670C5u + second * 952u))
                continue;
            if ((sint16)first_distance < (sint16)second_distance)
                goto swap_pair;
            if ((sint16)first_distance != (sint16)second_distance)
                continue;
            if ((sint16)progress_cache[first] == -16383)
            {
                first_progress = sub_8002F4A4(first_object + 88u, first_object + 100u);
                if (R32(0x80059738u) == 2u && (sint32)first < (sint32)R32(0x8005973Cu))
                    first_progress = 256u - first_progress;
                progress_cache[first] = (uint16)first_progress;
            }
            else
                first_progress = (uint32)(sint32)(sint16)progress_cache[first];
            if ((sint16)progress_cache[second] == -16383)
            {
                second_progress = sub_8002F4A4(second_object + 88u, second_object + 100u);
                if (R32(0x80059738u) == 2u && (sint32)second < (sint32)R32(0x8005973Cu))
                    second_progress = 256u - second_progress;
                progress_cache[second] = (uint16)second_progress;
            }
            else
                second_progress = (uint32)(sint32)(sint16)progress_cache[second];
            if ((sint32)first_progress >= (sint32)second_progress)
                continue;
        swap_pair:
            swaps += 1u;
            R8(0x80066B1Cu + index) = (uint8)second;
            R8(0x80066B1Du + index) = (uint8)first;
        }
    } while (swaps);
    for (index = 0u; (sint32)index < (sint32)R32(0x80065254u); ++index)
        R8(0x800670C4u + R8(0x80066B1Cu + index) * 952u) = (uint8)index;
    length = R32(R32(0x800651E4u) + 60u);
    baseline = game_800AE420(length);
    for (index = 0u; (sint32)index < (sint32)R32(0x80065254u); ++index)
    {
        uint32 mode;
        object = 0x80066D14u + index * 952u;
        mode = R32(0x80059738u);
        lap = (uint32)(sint32)(sint16)R16(object + 928u);
        next_lap = lap + 1u;
        if (mode == 2u)
        {
            if (index == 0u || (R32(0x8005973Cu) == mode && index == 1u))
            {
                threshold = next_lap * R32(0x800653A4u);
                crossed = (sint32)((uint32)(sint32)(sint16)R16(object + 92u) - baseline) < (sint32)(0u - threshold);
            }
            else
            {
                threshold = next_lap * R32(0x800653A4u);
                crossed = (sint32)(baseline + threshold) < (sint32)(sint16)R16(object + 94u);
            }
        }
        else
        {
            threshold = next_lap * R32(0x800653A4u);
            crossed = (sint32)(baseline + threshold) < (sint32)(sint16)R16(object + 92u);
        }
        if (!crossed)
            continue;
        if ((sint32)lap >= (sint32)(R32(0x80064BD4u) - 1u))
        {
            if (!R8(object + 945u))
            {
                R8(object + 945u) = 1u;
                R16(object + 910u) = (uint16)R32(0x800BEEA4u);
            }
            continue;
        }
        R16(object + 928u) = (uint16)next_lap;
        game_800A7B6C(index, next_lap + 1u, R32(0x80064BD4u));
        previous_laps = next_lap - 1u;
        sum = 0u;
        for (count = 0u; (sint32)count < (sint32)previous_laps; ++count)
            sum += R16(object + 912u + count * 2u);
        R16(object + 912u + previous_laps * 2u) = (uint16)(R32(0x800BEEA4u) - sum);
    }
    result = R8(0x800651CAu);
    if (result)
    {
        result = 1u;
        if (!R8(0x800670C5u))
        {
            uint32 elapsed = R32(0x800BEEA4u);
            uint32 score = R16(0x80066E98u);
            R8(0x800670C5u) = 1u;
            R8(0x800651CAu) = 0u;
            R16(0x800670A2u) = (uint16)elapsed;
            R16(0x80066E98u) = (uint16)(score + 10000u);
        }
    }
    return result;
}

static void level_set_position(uint32 record, uint32 x, uint32 y, uint32 z)
{
    w_u32(record + 4, x);
    w_u32(record + 8, y);
    w_u32(record + 12, z);
}

/* Original BD548 */
static void level_pipe_init(void)
{
    uint32 index, record;
    w_u32(0x800C8CB8, sub_800162D8(0x8008BFA0));
    w_u32(0x800C8CD4, sub_800162D8(0x8008BFB4));
    w_u32(0x800C8CF0, sub_800162D8(0x8008BFA0));
    w_u32(0x800C8D0C, sub_800162D8(0x8008BFB4));
    for (index = 0; index < 4; ++index)
    {
        record = 0x800C8CB8 + index * 28;
        w_u32(record + 24, 0);
        w_u16(record + 16, 0);
        w_u16(record + 18, 0);
        w_u16(record + 20, 0);
        if (index < 2)
            level_set_position(record, 0x027B94FC, 0x000315D8, 0x029C3680);
        else
            level_set_position(record, 0x01CDD100, 0x0014EAB0, 0x017A15D8);
    }
    for (index = 0; index < 3; ++index)
    {
        record = 0x800C8C60 + index * 28;
        w_u32(record, sub_800162D8(0x8008BFC8));
        w_u16(record + 16, 0);
        w_u16(record + 18, 0);
        w_u16(record + 20, 0);
    }
    w_u16(0x800C8C72, 1024);
    level_set_position(0x800C8C60, 0x02449FD8, 0x002F7FC4, 0x005CBBD8);
    level_set_position(0x800C8C7C, 0x00CFDED8, 0x0019B3B0, 0x01F107D8);
    level_set_position(0x800C8C98, 0x02C037B0, 0x001E81C4, 0x00BE68EC);
}

static void level_oscillate(uint32 base, uint32 count, uint32 angle_offset)
{
    uint32 index, record, phase;
    sint32 delta;
    for (index = 0; index < count; ++index)
    {
        record = base + index * 28;
        phase = r_s32(record + 24) < 4096 ? r_u32(record + 24) + 16 : 0;
        w_u32(record + 24, phase);
        delta = draft_agent3_missing_8004388C(phase) / 512;
        w_u16(record + angle_offset, (uint16)(r_u16(record + angle_offset) + (uint32)delta));
    }
}

/* Original BD7B8 */
static void level_pipe_update(void)
{
    uint32 index, address;
    level_oscillate(0x800C8CB8, 4, 18);
    for (index = 0; index < 3; ++index)
    {
        address = 0x800C8C70 + index * 28;
        w_u16(address, r_u16(address) + 24);
    }
}

static void level_draw_model(uint32 record, uint32 all_axes)
{
    uint32 model = r_u32(record), axis;
    MATRIX matrix;
    for (axis = 0; axis < 3; ++axis)
        w_u32(model + 16 + axis * 4, r_u32(record + 4 + axis * 4));
    if (!sub_8003D188(model))
        return;
    memset(&matrix, 0, sizeof(matrix));
    matrix.m[0][0] = matrix.m[1][1] = matrix.m[2][2] = 4096;
    if (all_axes)
        RotMatrixX(r_s16(record + 16), &matrix);
    RotMatrixY(r_s16(record + 18), &matrix);
    if (all_axes)
        RotMatrixZ(r_s16(record + 20), &matrix);
    sub_8003C164(&matrix);
    sub_8003CED8(&matrix, model);
    sub_800416E0(model);
}

/* Original BD88C */
static void level_pipe_draw(void)
{
    uint32 index;
    for (index = 0; index < 4; ++index)
        level_draw_model(0x800C8CB8 + index * 28, 0);
    for (index = 0; index < 3; ++index)
        level_draw_model(0x800C8C60 + index * 28, 1);
}

/* Original BE730 compares eight bytes including the empty sentinel */
static uint32 level_texture_find(uint32 name)
{
    uint32 texture = r_u32(0x800654F4);
    for (;;)
    {
        if (!memcmp(psx_addr(texture, 8), psx_addr(name, 8), 8))
            return texture;
        if (!r_u8(texture))
            break;
        texture += 26;
    }
    draft_agent1_missing_diagnostic((const char *)psx_addr(0x8008C03C, 1), (const char *)psx_addr(name, 1));
    return 0;
}

/* Original BDAEC with the formatted filename on the native stack */
static void level_texture_init(void)
{
    uint32 texture = level_texture_find(0x8008C000), page = r_u16(texture + 8);
    uint32 x = ((page << 6) & 1023) + (r_u8(texture + 24) >> 2);
    uint32 y = ((page << 4) & 256) + ((page >> 2) & 512) + r_u8(texture + 25);
    uint32 index, record, size, image;
    char path[64];
    PSX_RECT rectangle;
    for (index = 0; index < 2; ++index)
    {
        record = 0x800BFBF8 + index * 60;
        snprintf(path, sizeof(path), (const char *)psx_addr(0x8008C008, 1), (const char *)psx_addr(r_u32(record), 1));
        size = sub_800423EC(path, 1);
        image = sub_80042544(size);
        sub_8004225C(path, image, size);
        w_u32(record + 20, r_u32(record + 20) + x);
        w_u32(record + 24, r_u32(record + 24) + y);
        rectangle.x = (sint16)r_u32(record + 20);
        rectangle.y = (sint16)r_u32(record + 24);
        rectangle.w = (sint16)(r_s32(record + 12) / 4);
        rectangle.h = (sint16)r_u32(record + 16);
        LoadImagePSX(&rectangle, (uint32 *)psx_addr(image + 64, 1));
        DrawSync(0);
        sub_8004261C(image);
    }
}

/* Original BDC58 computes an unused local rectangle without issuing a transfer */
static void level_texture_update(void)
{
    uint32 index, record;
    for (index = 0; index < 2; ++index)
    {
        record = 0x800BFBF8 + index * 60;
        w_u32(record + 44, r_u32(record + 44) + 1);
        if (r_u32(record + 44) != r_u32(record + 40))
            continue;
        w_u32(record + 44, 0);
        w_u32(record + 48, r_s32(record + 48) < r_s32(record + 36) ? r_u32(record + 48) + 1 : 0);
        (void)r_u32(record + 52);
        (void)r_u32(record + 56);
        (void)r_u32(record + 28);
        (void)r_u32(record + 32);
    }
}

/* Original BDE40 */
static uint32 level_visible(uint32 position)
{
    VECTOR input, output;
    uint32 axis;
    sint32 depth, x, y, product;
    for (axis = 0; axis < 3; ++axis)
    {
        sint32 value = (sint32)((uint32)(r_s32(position + axis * 4) >> 8) - r_u32(0x80081BB8 + axis * 4));
        if (axis == 0)
            input.vx = value;
        else if (axis == 1)
            input.vy = value;
        else
            input.vz = value;
    }
    ApplyMatrixLV((MATRIX *)psx_addr(0x80081BA4, sizeof(MATRIX)), &input, &output);
    depth = output.vz;
    if (depth <= 0 || depth >= (r_s32(0x8006514C) >> (r_u32(0x80065170) & 31)))
        return 0;
    x = (sint32)((uint32)output.vx * (uint32)(sint32)r_s16(0x80065550));
    product = (sint32)((uint32)depth * (uint32)(sint32)r_s16(0x80065554));
    if ((sint32)((uint32)x + (uint32)product) >= 0 || (sint32)((uint32)(0 - (uint32)x) + (uint32)product) >= 0)
        return 0;
    y = (sint32)((uint32)output.vy * (uint32)(sint32)r_s16(0x80065562));
    product = (sint32)((uint32)depth * (uint32)(sint32)r_s16(0x80065564));
    if ((sint32)((uint32)y + (uint32)product) >= 0)
        return 0;
    return ((0 - (uint32)y) + (uint32)product) >> 31;
}

extern uint32 game_800A8848(uint32 template_address, const VECTOR *position, const SVECTOR *velocity, const SVECTOR *acceleration, uint32 flags);

/* Original BDF88 advances the CRT RNG even on frames without particles */
static void level_particles(void)
{
    SVECTOR velocity = {0}, acceleration;
    sint32 random = (sint32)randPSX();
    uint32 index;
    velocity.vy = (sint16)(random % 400 + 1000);
    memcpy(&acceleration, psx_addr(0x8008C014, 8), 8);
    if ((r_u32(0x80064A30) & 7) >= 3)
        return;
    for (index = 0; index < 9; ++index)
    {
        uint32 position = 0x800BFC90 + index * 12;
        if (level_visible(position))
            game_800A8848(0x800BFC70, (const VECTOR *)psx_addr(position, 12), &velocity, &acceleration, 60);
    }
}

/* Original BE0E4 */
static void level_crane_init(void)
{
    uint32 index, record, axis;
    for (index = 0; index < 3; ++index)
        w_u32(0x800C8BD0 + index * 28, sub_800162D8(0x8008C01C));
    for (index = 0; index < 3; ++index)
    {
        record = 0x800C8BD0 + index * 28;
        w_u32(record + 24, 0);
        w_u16(record + 16, 0);
        w_u16(record + 18, (sint32)randPSX() % 1024 + 2048);
        w_u16(record + 20, 0);
        for (axis = 0; axis < 3; ++axis)
            w_u32(record + 4 + axis * 4, r_u32(0x800BFD24 + index * 12 + axis * 4));
    }
    for (index = 0; index < 2; ++index)
        w_u32(0x800C8C28 + index * 28, sub_800162D8(0x8008C02C));
    for (index = 0; index < 2; ++index)
    {
        record = 0x800C8C28 + index * 28;
        w_u32(record + 24, 0);
        w_u16(record + 16, 0);
        w_u16(record + 18, 0);
        w_u16(record + 20, 0);
        for (axis = 0; axis < 3; ++axis)
            w_u32(record + 4 + axis * 4, r_u32(0x800BFCFC + index * 12 + axis * 4));
    }
    w_u16(0x800C8C3A, 2000);
    w_u16(0x800C8C56, 2600);
}

/* Original BE2E4 */
static void level_crane_update(void)
{
    level_oscillate(0x800C8BD0, 3, 18);
    level_oscillate(0x800C8C28, 2, 20);
}

/* Original BE40C */
static void level_crane_draw(void)
{
    uint32 player = r_u32(0x8005973C) == 2 ? r_u32(0x80065128) : 0, index;
    sint32 progress = r_s16(0x80066D6E + player * 952), low, high;
    for (index = 0; index < 3; ++index)
    {
        low = r_s32(0x800BFD48 + index * 8);
        high = r_s32(0x800BFD4C + index * 8);
        if (progress < low && high < progress)
            continue;
        level_draw_model(0x800C8BD0 + index * 28, 0);
    }
    for (index = 0; index < 2; ++index)
    {
        low = r_s32(0x800BFD14 + index * 8);
        high = r_s32(0x800BFD18 + index * 8);
        if (progress < low || high < progress)
            continue;
        level_draw_model(0x800C8C28 + index * 28, 1);
    }
}
