#include "draft_signatures.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RACE_GP 0x80064A10u

uint32 game_800B7E20(void);
uint32 game_800B44F0(void);
void game_800BCB60(void);
uint32 game_8009FB80(void);
uint32 game_8008C058(void);
uint32 game_800B8AA0(void);
uint32 game_800B88D0(void);
uint32 sub_800388AC(void);
uint32 sub_8001CBE8(void);
uint32 sub_8001CBAC(void);
void game_8008F9A8(uint32 port);
extern uint32 draft_agent3_missing_8001F860(void);
extern uint32 draft_agent3_missing_80049F84(uint32 enabled, uint32 voices);
extern uint32 draft_agent3_missing_8004BB74(const void *attributes);
void game_800AE510(void);
void game_800B3640(void);
void game_800910F0(void);
void game_800B0068(void);
void game_800AA450(void);
void game_800B3630(void);
void game_800BB4FC(void);
void game_800A7CA8(uint32 count);

uint32 sub_800162A4(void)
{
    char level[20];
    sint32 length = snprintf(level, sizeof(level), "LEVEL%d\\", (int32_t)r_u32(RACE_GP + 0x1F8u));
    strcpy((char *)psx_addr(0x8006569Cu, strlen(level) + 1u), level);
    return (uint32)length;
}

/* Full original MIPS and complete canonical pseudocode 800161CC */
uint32 sub_800161CC(void)
{
    uint32 alternate = r_u32(RACE_GP + 0x1BCu);
    uint32 mode = r_u32(0x80059738u);
    uint32 value, scale, setting, state;
    w_u32(RACE_GP + 0x838u, 0u);
    w_u32(RACE_GP + 0x208u, 0u);
    w_u32(RACE_GP + 0x7FCu, alternate ^ 1u);
    value = mode == 1u ? r_u32(0x80059760u) : r_u32(RACE_GP + 0x1B4u) + 1u;
    w_u32(RACE_GP + 0x1C4u, value);
    scale = r_u32(RACE_GP + 0x1B0u);
    setting = r_u32(RACE_GP + 0x1B8u);
    w_u32(0x800650ECu, scale * 5u + 15u);
    value = setting * 7u;
    w_u32(0x800BE848u, value);
    if ((int32_t)value < 0)
        value = 0u;
    else if ((int32_t)value >= 65)
        value = 64u;
    state = r_u32(RACE_GP + 0x200u);
    w_u32(0x800BE848u, value);
    if (state == 0u || state == 5u)
    {
        w_u32(RACE_GP + 0x1F8u, r_u32(RACE_GP + 0x1F4u));
        return sub_800162A4();
    }
    return 5u;
}

/* TODO: Translate direct dependencies after full callee ABI and loaded-image review */
static void race_pending0(uint32 entry)
{
    draft_agent1_missing_diagnostic("Untranslated race startup dependency 0x%08X\n", entry);
    abort();
}

static void race_pending1(uint32 entry, uint32 argument)
{
    race_pending0(entry);
}

static void race_pending2(uint32 entry, uint32 first, uint32 second)
{
    race_pending0(entry);
}

static void race_pending3(uint32 entry, uint32 first, uint32 second, uint32 third)
{
    race_pending0(entry);
}

extern uint32 sub_8001DE6C(void);
uint32 sub_800200B0(void);

/* Full caller algorithm 80014BC4 with explicit unresolved dependency stops */
uint32 sub_8001488C(void);
uint32 sub_80042710(uint32 bytes);
uint32 sub_800427C8(uint32 buffer, uint32 capacity);
uint32 game_800BB568(uint32 count);
uint32 game_8009DBB0(void);
uint32 sub_8002BF28(void);
uint32 sub_80041694(uint32 first_name, uint32 second_name);
uint32 sub_8001C9F8(uint32 mode);
uint32 game_800B4340(void);
uint32 sub_80016A54(void);
uint32 sub_80019F70(void);
uint32 sub_80016AB4(void);
uint32 sub_80017B74(void);
uint32 sub_80033FAC(void);
uint32 sub_80017C14(void);
void game_8009A200(uint32 object);
uint32 sub_80033DC8(void);
uint32 sub_80017E44(void);
uint32 sub_80014544(void);
uint32 game_800AE420(uint32 track);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
uint32 game_8009AE08(void);
uint32 game_8009EEE8(void);

extern uint32 draft_agent3_missing_8003DC58(uint32 rectangle, const void *pixels);

uint32 sub_80014BC4(void)
{
    uint32 heap = r_u32(0x80065220u);
    uint32 value, other, buffer, capacity, index, offset;
    sub_8004250C(heap, 0x801FF400u - heap);
    if (r_u32(0x800593A8u) != 0u)
    {
        w_u32(RACE_GP + 0x7B4u, 320u);
        value = 8000u;
    }
    else
    {
        w_u32(RACE_GP + 0x7B4u, 256u);
        value = 19200u;
    }
    w_u32(0x80065190u, value);
    sub_8001488C();
    sub_80039690(0x1201u);
    sub_8003970C(2u);
    sub_80039718(0u);
    value = r_u32(0x80064BF0u);
    other = r_u32(0x80064BF4u);
    w_u16(0x80081A9Cu, (uint16)value);
    w_u16(0x80081B10u, (uint16)value);
    w_u16(0x80081A9Eu, (uint16)other);
    w_u16(0x80081B12u, (uint16)other);
    sub_8003BF48(144u);
    if (r_u32(0x800593A8u) != 0u)
    {
        sub_80042710(0xE000u);
        value = 44000u;
    }
    else
    {
        sub_80042710(0x16000u);
        value = 0x10000u;
    }
    sub_8003BF7C(value);
    if (r_u32(0x800593A0u) != 0u)
    {
        buffer = sub_80042544(0x48000u);
        capacity = 0x48000u;
    }
    else if (r_u32(0x800593A8u) != 0u)
    {
        buffer = sub_80042544(0x58000u);
        capacity = 0x58000u;
    }
    else
    {
        buffer = sub_80042544(0x3C000u);
        capacity = 0x3C000u;
    }
    w_u32(RACE_GP + 0x7ACu, buffer);
    sub_800427C8(buffer, capacity);
    game_800BB568(64u);
    game_8009DBB0();
    sub_8002BF28();
    sub_80041694(0x80064B08u, 0x80064B10u);
    sub_8001C9F8(0u);
    game_800B4340();
    sub_80016A54();
    sub_80019F70();
    sub_80016AB4();
    sub_80017B74();
    sub_8001860C();
    sub_80033FAC();
    for (index = 0u, offset = 0u; (int32_t)index < (int32_t)r_u32(0x80065254u); ++index, offset += 0x3B8u)
    {
        uint32 count;
        sub_80018430(index, r_u32(0x80066D14u + offset));
        count = r_u32(0x80065254u);
        w_u32(0x80067050u + offset, 0xEB312789u);
        if ((int32_t)(index + 1u) >= (int32_t)count)
            break;
    }
    sub_800197B8(47u);
    sub_80017C14();
    sub_80033DC8();
    sub_80017E44();
    if (r_u32(0x800593A0u) != 0u)
        game_800B7E20();
    else
        game_800B44F0();
    sub_80014544();
    game_800BCB60();
    game_8009FB80();
    w_u32(0x800654C4u, 2u);
    w_u32(0x800654C8u, 2u);
    sub_80036914(0x80066D3Cu, 1u, 0u);
    sub_80036914(0x800670F4u, 1u, 1u);
    game_800B3640();
    game_800910F0();
    game_800B0068();
    game_800AA450();
    game_800B3630();
    sub_800152DC();
    game_800BB4FC();
    offset = r_u32(0x800593A4u) * 8u;
    buffer = r_u32(RACE_GP + 0x7D4u);
    sub_80040640(buffer + offset + 4u, buffer + offset + 28u);
    game_800A7CA8(128u);
    game_800AE510();
    sub_80020890();
    sub_8003909C();
    game_8008C058();
    sub_80019F30();
    sub_80012D58();
    value = r_u32(0x800593A0u);
    w_u32(RACE_GP + 0x30u, 0u);
    w_u32(RACE_GP + 0x34u, 0u);
    w_u32(RACE_GP + 0x3Cu, 60u);
    w_u32(RACE_GP + 0x40u, 60u);
    w_u32(0x8006512Cu, 0u);
    if (value != 0u)
        game_800B8AA0();
    else
        game_800B88D0();
    w_u32(0x800654C4u, 2u);
    sub_80036914(0x80066D3Cu, 1u, 0u);
    sub_800388AC();
    /* Finish native loading without the timed CD fade and thirteen idle fields */
    value = r_u32(0x80064BF8);
    w_u32(0x80064BF8, 0);
    sub_8001FF98();
    sub_800200B0();
    w_u32(0x80064BF8, value);
    sub_8001CBE8();
    sub_8001CBAC();
    value = r_u32(RACE_GP + 0x2Cu);
    w_u8(0x80064EA1u, 0u);
    w_u32(RACE_GP + 0x2Cu, value + 1u);
    game_8008F9A8(0u);
    game_8008F9A8(1u);
    sub_8002B840();
    w_u32(0x80064ED8u, 1u);
    return 1u;
}

extern uint32 draft_agent3_missing_8003D9AC(uint32 mode);

static void race_relocate_directory(uint32 base)
{
    uint32 index = 1u;
    uint32 slot = base + 4u;
    if (r_u32(base) == 0u)
        return;
    do
    {
        w_u32(slot, r_u32(slot) + base);
        ++index;
        slot += 4u;
    } while (r_u32(base) >= index);
}

/* Full canonical pseudocode and MIPS 8001488C, including the six texture arguments */
uint32 sub_8001488C(void)
{
    char path[64];
    uint32 allocation, bytes, level, fixed, data, sky, index;
    allocation = sub_80042544(0xB0000u);
    snprintf(path, sizeof(path), "%sSTREAM.VRM", (const char *)psx_addr(0x8006569Cu, 1u));
    sub_8004225C(path, allocation, 0xB0000u);
    sub_80015238(allocation);
    sub_8004261C(allocation);
    allocation = sub_80042544(0xB0000u);
    strcpy(path, "STATIC\\STATIC.VRM");
    sub_8004225C(path, allocation, 0xB0000u);
    sub_80015238(allocation);
    sub_8004261C(allocation);
    snprintf(path, sizeof(path), "%sLEVEL.DAT", (const char *)psx_addr(0x8006569Cu, 1u));
    bytes = sub_800423EC(path, 1u);
    allocation = sub_80042544(bytes);
    w_u32(RACE_GP + 0x7E0u, allocation);
    sub_8004225C(path, allocation, bytes);
    strcpy(path, "STATIC\\STATIC.DAT");
    bytes = sub_800423EC(path, 1u);
    allocation = sub_80042544(bytes);
    w_u32(RACE_GP + 0x7D0u, allocation);
    sub_8004225C(path, allocation, bytes);
    race_relocate_directory(r_u32(RACE_GP + 0x7E0u));
    race_relocate_directory(r_u32(RACE_GP + 0x7D0u));
    level = r_u32(RACE_GP + 0x7E0u);
    data = r_u32(level + 8u);
    fixed = r_u32(RACE_GP + 0x7D0u);
    w_u32(0x800593A0u, r_u32(data));
    allocation = r_u32(fixed + 4u);
    w_u32(RACE_GP + 0x7D4u, data);
    sub_80039A4C(allocation);
    snprintf(path, sizeof(path), "%sSKY%1d.DAT", (const char *)psx_addr(0x8006569Cu, 1u), (int32_t)r_u32(0x800593A4u));
    bytes = sub_800423EC(path, 1u);
    sky = sub_80042544(bytes);
    sub_8004225C(path, sky, bytes);
    for (index = 0u; index < 16u; ++index)
    {
        uint32 offset = r_u32(sky + 4u + index * 4u);
        uint32 name = r_u32(0x80059470u + index * 4u);
        uint32 tim = sky + offset;
        uint32 texture = sub_80039ADC(name);
        uint32 x = (uint32)(sint16)r_u16(texture + 12u);
        uint32 y = (uint32)(sint16)r_u16(texture + 14u);
        uint32 clut_x = (uint32)(sint16)r_u16(texture + 16u);
        uint32 clut_y = (uint32)(sint16)r_u16(texture + 18u);
        sub_8001B7B0(tim, x, y, clut_x, clut_y, 1u);
        draft_agent3_missing_8003D9AC(0u);
    }
    return sub_8004261C(sky);
}

/* Complete original 80042710 allocates both buffers in order */
uint32 sub_80042710(uint32 bytes)
{
    uint32 first = sub_80042544(bytes);
    uint32 second;
    w_u32(RACE_GP + 0xB6Cu, first);
    second = sub_80042544(bytes);
    w_u32(0x80065580u, second);
    w_u32(RACE_GP + 0xB74u, bytes);
    w_u32(RACE_GP + 0xB78u, 0u);
    return second;
}

/* Full original MIPS and complete canonical pseudocode 800427C8 */
uint32 sub_800427C8(uint32 buffer, uint32 capacity)
{
    uint32 units = (capacity >> 4u) << 1u;
    w_u32(RACE_GP + 0xB80u, buffer);
    w_u32(RACE_GP + 0xB84u, buffer);
    w_u32(buffer, buffer);
    w_u32(buffer + 4u, buffer);
    w_u32(buffer + 12u, units);
    w_u32(RACE_GP + 0xB8Cu, capacity);
    w_u32(RACE_GP + 0xB94u, 0u);
    w_u32(RACE_GP + 0xB88u, capacity);
    w_u32(RACE_GP + 0xB90u, capacity);
    w_u32(RACE_GP + 0x778u, 0u);
    return units;
}

/* Full GAME MIPS CFG 800BB568 through 800BB658 */
uint32 game_800BB568(uint32 count)
{
    uint32 bytes = count * 204u;
    uint32 buffer = sub_80042544(bytes);
    uint32 previous = 0x800C89E8u;
    uint32 limit = count - 1u;
    uint32 index = 0u;
    uint32 node = buffer;
    uint32 offset = 204u;
    uint32 end;
    w_u32(0x800C8AB8u, 0x800C8AB4u);
    w_u32(0x800C8AB4u, 0x800C8AB4u);
    w_u32(0x800C89ECu, buffer + bytes - 204u);
    w_u32(0x800C89E8u, buffer);
    w_u32(0x800BFBC0u, buffer);
    w_u32(0x800BFBC4u, count);
    if ((int32_t)limit > 0)
    {
        do
        {
            w_u32(node + 4u, previous);
            previous = node;
            w_u32(previous, buffer + offset);
            node = previous + 204u;
            index += 1u;
            offset += 204u;
        } while ((int32_t)index < (int32_t)limit);
    }
    end = r_u32(0x800BFBC0u) + count * 204u;
    w_u32(end - 204u, 0x800C89E8u);
    w_u32(end - 200u, previous);
    return end;
}

/* Full GAME MIPS CFG 8009DBB0 through 8009DBCC */
uint32 game_8009DBB0(void)
{
    uint32 value = r_u32(0x800651E4u) + 200u;
    w_u32(0x800C0A10u, value);
    return value;
}

/* Full original MIPS and complete canonical pseudocode 8002BF28 */
uint32 sub_8002BF28(void)
{
    uint32 mode = r_u32(0x800593A0u);
    uint32 data = r_u32(0x800651F0u);
    uint32 block = r_u32(data + 4u);
    uint32 result;
    if (mode != 0u)
    {
        uint32 records = r_u32(data + 80u);
        uint32 first = (uint32)(int32_t)(sint16)r_u16(block);
        uint32 second = (uint32)(int32_t)(sint16)r_u16(block + 2u);
        w_u32(RACE_GP + 0x9B0u, block + 4u);
        result = r_u32(data + 80u) + 4u;
        w_u32(RACE_GP + 0x9A4u, first);
        w_u32(RACE_GP + 0x9ACu, second);
        first = r_u32(records);
        w_u32(RACE_GP + 0x99Cu, result);
        w_u32(RACE_GP + 0x9C0u, first);
    }
    else
    {
        uint32 first_offset = r_u32(block);
        uint32 third_offset = r_u32(block + 8u);
        uint32 count = r_u32(block + 4u);
        uint32 fifth = r_u32(block + 16u);
        uint32 seventh_offset = r_u32(block + 24u);
        uint32 eighth_offset = r_u32(block + 28u);
        uint32 first = block + first_offset;
        w_u32(RACE_GP + 0x9A0u, block);
        w_u32(RACE_GP + 0x9A8u, block + eighth_offset);
        w_u32(RACE_GP + 0x9C4u, first);
        w_u32(RACE_GP + 0x9B4u, block + third_offset);
        w_u32(RACE_GP + 0x994u, count);
        w_u32(RACE_GP + 0x998u, fifth);
        w_u32(RACE_GP + 0x990u, block + seventh_offset);
        w_u8(first, 9u);
        result = r_u32(RACE_GP + 0x994u) * 24u + r_u32(RACE_GP + 0x9C4u);
        w_u16(first + 8u, (uint16)(count - 1u));
        w_u16(result - 16u, 0u);
        w_u8(result - 24u, 10u);
    }
    return result;
}

/* Full original MIPS and complete canonical pseudocode 8001C9F8 */
uint32 sub_8001C9F8(uint32 mode)
{
    uint32 selected, index, info, path, capacity, buffer, table, data;
    if (mode != 0u)
    {
        selected = r_u32(RACE_GP + 0x314u);
        if (selected == 20u)
            return selected;
        w_u32(RACE_GP + 0x314u, 20u);
    }
    else
    {
        selected = r_u32(0x80065210u) + 1u;
        if (selected == r_u32(RACE_GP + 0x314u))
            return selected;
        w_u32(RACE_GP + 0x314u, selected);
    }
    info = 0x800744B4u;
    for (index = 39u; index < 42u; ++index, info += 12u)
    {
        uint32 address = r_u32(info);
        if (address != 0u)
        {
            /* Startup binds the allocator to original guest SPU heap */
            SpuFree(address);
            w_u32(info, 0u);
        }
    }
    path = r_u32(RACE_GP + 0x304u);
    if (mode != 0u)
        strcpy((char *)psx_addr(path + 5u, 1u), "PRES.DAT");
    else
    {
        char name[20];
        snprintf(name, sizeof(name), "CAR%d.DAT", (int32_t)r_u32(RACE_GP + 0x314u));
        strcpy((char *)psx_addr(path + 5u, strlen(name) + 1u), name);
    }
    path = r_u32(RACE_GP + 0x304u);
    capacity = (sub_80042488((const char *)psx_addr(path, 1u), 1u) + 2047u) & 0xFFFFFE00u;
    buffer = sub_80042544(capacity);
    path = r_u32(RACE_GP + 0x304u);
    sub_8004225C((const char *)psx_addr(path, 1u), buffer, capacity);
    table = buffer;
    data = buffer + 2048u;
    index = 39u;
    if ((int32_t)capacity > 0)
    {
        info = 0x800744B4u;
        for (;;)
        {
            uint32 bytes = r_u32(table);
            table += 8u;
            if ((int32_t)bytes <= 0)
                break;
            sub_8001E20C(data, bytes, (uint32 *)psx_addr(info, 12u));
            draft_agent1_missing_diagnostic("VAG %d Loaded\n", (int32_t)index);
            info += 12u;
            index += 1u;
            data += bytes;
        }
    }
    return sub_8004261C(buffer);
}

/* Full GAME MIPS CFG 800B4340 through 800B4428 */
uint32 game_800B4340(void)
{
    uint32 index;
    uint32 count;
    if (r_u32(0x800593A8u) == 0u)
    {
        uint32 mode = r_u32(0x80064B5Cu);
        w_u32(0x80065254u, 16u);
        w_u8(0x80059A28u, mode != 0u ? 1u : 0u);
        index = 1u;
        if ((int32_t)r_u32(0x80065254u) > 1)
        {
            do
            {
                w_u8(0x80059A28u + index, 2u);
                count = r_u32(0x80065254u);
                index += 1u;
            } while ((int32_t)index < (int32_t)count);
        }
    }
    else
    {
        w_u32(0x80065254u, 12u);
        w_u8(0x80059A28u, 0u);
        w_u8(0x80059A29u, 0u);
        index = 2u;
        do
        {
            w_u8(0x80059A28u + index, 2u);
            count = r_u32(0x80065254u);
            index += 1u;
        } while ((int32_t)index < (int32_t)count);
    }
    count = r_u32(0x80065254u);
    w_u32(0x800C08D0u, count);
    return count;
}

/* Full original MIPS and complete canonical pseudocode 80016A54 */
uint32 sub_80016A54(void)
{
    uint32 result = r_u32(RACE_GP + 0x844u);
    uint32 index = 0u;
    uint32 object = 0x80066D14u;
    if ((int32_t)result > 0)
    {
        do
        {
            memset(psx_addr(object, 952u), 0, 952u);
            index += 1u;
            result = (int32_t)index < (int32_t)r_u32(RACE_GP + 0x844u) ? 1u : 0u;
            object += 952u;
        } while (result != 0u);
    }
    return result;
}

/* Full original MIPS and complete canonical pseudocode 80016AB4 */
uint32 sub_80016AB4(void)
{
    uint32 index = 0u, offset = 0u;
    if ((int32_t)r_u32(RACE_GP + 0x844u) > 0)
    {
        do
        {
            uint32 object = 0x80066D14u + offset;
            uint32 part;
            w_u8(object + 0x3A9u, (uint8)index);
            if (r_u8(0x80059A28u + index) != 0u)
            {
                w_u32(object + 4u, 0x80066B2Cu);
                w_u8(object + 0x3B5u, 32u);
            }
            else
            {
                w_u32(object + 4u, 0x80066C90u);
                w_u8(object + 0x3B5u, 13u);
            }
            w_u8(object + 18u, 2u);
            w_u32(object, 0x80066BB0u);
            w_u8(object + 0x3B2u, 255u);
            w_u8(object + 0x3B4u, 0u);
            w_u8(object + 0x3A4u, 1u);
            for (part = 0u; part < 4u; ++part)
            {
                uint32 source, first, second;
                w_u8(object + 0x3A5u + part, 255u);
                source = r_u32(object) + part * 8u;
                first = r_u32(source + 64u);
                second = r_u32(source + 68u);
                w_u32(object + 0x270u + part * 8u, first);
                w_u32(object + 0x274u + part * 8u, second);
            }
            w_u8(0x80066B1Cu + index, (uint8)index);
            game_8009A200(object);
            index += 1u;
            offset += 952u;
        } while ((int32_t)index < (int32_t)r_u32(RACE_GP + 0x844u));
    }
    return game_8009AE08();
}

/* Full original MIPS and complete canonical pseudocode 80017B74 */
uint32 sub_80017B74(void)
{
    uint32 first, second, result;
    game_8009EEE8();
    if (r_u32(0x80065110u) == 1u || r_u32(0x80064B5Cu) != 0u)
    {
        first = 1000000u;
        second = 16000000u;
        result = second;
    }
    else if (r_u32(0x800593A8u) != 0u)
    {
        first = 15625u;
        second = 1000000u;
        result = first;
    }
    else
    {
        first = 190969u;
        second = 1000000u;
        result = second;
    }
    w_u32(RACE_GP + 0x848u, first);
    w_u32(RACE_GP + 0x850u, second);
    return result;
}

/* Full original MIPS and complete canonical pseudocode 80033FAC */
uint32 sub_80033FAC(void)
{
    uint32 width0 = r_u16(0x80066C48u);
    uint32 width1 = r_u16(0x80066C3Au);
    uint16 height0 = r_u16(0x80066C4Au);
    uint16 depth0 = r_u16(0x80066C4Cu);
    uint16 height1 = r_u16(0x80066C3Cu);
    uint16 depth1 = r_u16(0x80066C3Eu);
    uint32 result = 0u - width0;
    w_u16(RACE_GP + 0x9D0u, 0u);
    w_u16(RACE_GP + 0x9C8u, 0u);
    w_u16(0x8007A3A2u, height0);
    w_u16(0x8007A39Au, height0);
    w_u16(0x8007A3C2u, height1);
    w_u16(0x8007A3BAu, height1);
    w_u16(0x8007A3A8u, (uint16)result);
    w_u16(0x8007A398u, (uint16)result);
    w_u16(0x8007A3D0u, (uint16)(0u - width1));
    w_u16(0x8007A3C0u, (uint16)(0u - width1));
    w_u16(0x8007A3B0u, (uint16)width0);
    w_u16(0x8007A3A0u, (uint16)width0);
    w_u16(0x8007A3C8u, (uint16)width1);
    w_u16(0x8007A3B8u, (uint16)width1);
    w_u16(RACE_GP + 0x9CAu, (uint16)(height0 + 10u));
    w_u16(0x8007A3A4u, depth0);
    w_u16(0x8007A39Cu, depth0);
    w_u16(RACE_GP + 0x9CCu, depth0);
    w_u16(RACE_GP + 0x9D2u, (uint16)(height1 + 10u));
    w_u16(0x8007A3C4u, depth1);
    w_u16(0x8007A3BCu, depth1);
    w_u16(RACE_GP + 0x9D4u, depth1);
    return result;
}

/* Full original MIPS and complete canonical pseudocode 80017C14 */
uint32 sub_80017C14(void)
{
    uint32 result = r_u32(0x80065254u);
    uint32 index = 0u, position = 0x800661D0u, rotation = 0x800661D4u;
    if ((int32_t)result > 0)
    {
        do
        {
            sub_80019C6C(index, 0u);
            sub_80019A3C(index, (const uint8 *)psx_addr(position, 3u), (const uint8 *)psx_addr(rotation, 3u));
            result = r_u32(0x80065254u);
            rotation += 44u;
            index += 1u;
            result = (int32_t)index < (int32_t)result ? 1u : 0u;
            position += 44u;
        } while (result != 0u);
    }
    return result;
}

/* Full GAME MIPS CFG 8009AE08 through 8009AE48 */
uint32 game_8009AE08(void)
{
    int32_t value = (int32_t)(sint16)r_u16(0x80066C32u) * 181;
    uint32 result = (uint32)(value / 256);
    w_u32(0x800C0920u, result);
    return result;
}

/* Full GAME MIPS CFG 8009EEE8 through 8009F02C */
uint32 game_8009EEE8(void)
{
    uint32 texture, u, v, group, buffer_index, column;
    w_u32(0x800C0A18u, sub_80042544(0x500u));
    w_u32(0x800C0A1Cu, sub_80042544(0x500u));
    texture = sub_80039ADC(0x8008B608u);
    u = r_u8(texture + 24u);
    v = r_u8(texture + 25u);
    for (group = 0u; group < 16u; ++group)
    {
        for (buffer_index = 0u; buffer_index < 2u; ++buffer_index)
        {
            for (column = 0u; column < 2u; ++column)
            {
                uint32 packet = r_u32(0x800C0A18u + buffer_index * 4u) + (group * 2u + column) * 40u;
                uint32 left = u + column * 32u;
                uint32 right = left + 31u;
                uint16 clut;
                uint8 command;
                w_u8(packet + 3u, 9u);
                w_u8(packet + 7u, 0x2Cu);
                w_u16(packet + 22u, (uint16)(r_u16(texture + 8u) | 0x40u));
                clut = r_u16(texture + 10u);
                command = r_u8(packet + 7u);
                w_u8(packet + 12u, (uint8)left);
                w_u8(packet + 20u, (uint8)left);
                w_u8(packet + 4u, 128u);
                w_u8(packet + 5u, 128u);
                w_u8(packet + 6u, 128u);
                w_u8(packet + 13u, (uint8)v);
                w_u8(packet + 21u, (uint8)(v + 31u));
                w_u8(packet + 28u, (uint8)right);
                w_u8(packet + 29u, (uint8)v);
                w_u8(packet + 36u, (uint8)right);
                w_u8(packet + 37u, (uint8)(v + 31u));
                w_u8(packet + 7u, (uint8)((command & 0xFEu) | 2u));
                w_u16(packet + 14u, clut);
            }
        }
    }
    return 0u;
}

/* Full original MIPS and complete canonical pseudocode 80033DC8 */
uint32 sub_80033DC8(void)
{
    uint16 tpage = r_u16(sub_80039ADC(0x800150D8u) + 8u);
    uint32 group, buffer;
    for (group = 0u; group < 16u; ++group)
    {
        uint32 palette = ((r_u32(0x80064C74u) + group) << 6u) | ((r_u32(0x80064C70u) >> 4u) & 63u);
        for (buffer = 0u; buffer < 2u; ++buffer)
        {
            uint32 p = 0x80078F24u + group * 160u + buffer * 2560u;
            w_u8(p + 3u, 9u);
            w_u8(p + 7u, 44u);
            w_u16(p + 22u, tpage);
            w_u16(p + 14u, (uint16)palette);
            w_u8(p + 12u, 100u);
            w_u8(p + 13u, 72u);
            w_u8(p + 20u, 100u);
            w_u8(p + 21u, 96u);
            w_u8(p + 28u, 127u);
            w_u8(p + 29u, 72u);
            w_u8(p + 36u, 127u);
            w_u8(p + 37u, 96u);
            w_u8(p + 4u, 128u);
            w_u8(p + 5u, 128u);
            w_u8(p + 6u, 128u);
            w_u8(p + 83u, 9u);
            w_u8(p + 87u, 44u);
            w_u16(p + 102u, tpage);
            w_u16(p + 94u, (uint16)palette);
            w_u8(p + 92u, 100u);
            w_u8(p + 93u, 96u);
            w_u8(p + 100u, 100u);
            w_u8(p + 101u, 120u);
            w_u8(p + 108u, 127u);
            w_u8(p + 109u, 96u);
            w_u8(p + 116u, 127u);
            w_u8(p + 117u, 120u);
            w_u8(p + 84u, 128u);
            w_u8(p + 85u, 128u);
            w_u8(p + 86u, 128u);
            w_u8(p + 43u, 9u);
            w_u8(p + 47u, 44u);
            w_u16(p + 62u, tpage);
            w_u16(p + 54u, (uint16)palette);
            w_u8(p + 140u, 56u);
            w_u8(p + 156u, 56u);
            w_u8(p + 52u, 8u);
            w_u8(p + 53u, 128u);
            w_u8(p + 60u, 32u);
            w_u8(p + 61u, 128u);
            w_u8(p + 68u, 8u);
            w_u8(p + 69u, 192u);
            w_u8(p + 76u, 32u);
            w_u8(p + 77u, 192u);
            w_u8(p + 44u, 128u);
            w_u8(p + 45u, 128u);
            w_u8(p + 46u, 128u);
            w_u8(p + 123u, 9u);
            w_u8(p + 127u, 44u);
            w_u16(p + 142u, tpage);
            w_u16(p + 134u, (uint16)palette);
            w_u8(p + 132u, 32u);
            w_u8(p + 133u, 128u);
            w_u8(p + 141u, 128u);
            w_u8(p + 148u, 32u);
            w_u8(p + 149u, 192u);
            w_u8(p + 157u, 192u);
            w_u8(p + 124u, 128u);
            w_u8(p + 125u, 128u);
            w_u8(p + 126u, 128u);
        }
    }
    w_u16(0x8007A3F0u, 0u);
    w_u16(0x8007A3E8u, 0u);
    w_u16(0x8007A3E0u, 0u);
    w_u16(0x8007A3D8u, 0u);
    return 0u;
}

/* Full original MIPS and complete canonical pseudocode 80017E44 */
uint32 sub_80017E44(void)
{
    uint32 value = 15u;
    uint32 address = 0x8006A8D0u;
    do
    {
        w_u32(address, value);
        value -= 1u;
        address -= 4u;
    } while ((int32_t)value >= 0);
    return value;
}

/* Full GAME MIPS CFG 800AE420 through 800AE4B8 */
uint32 game_800AE420(uint32 track)
{
    uint32 base, index = 0u, offset = 32u;
    if ((int32_t)r_u32(0x800653A4u) >= (int32_t)track)
        return track;
    base = r_u32(0x800653B0u);
    if (r_u32(base + 20u) != 0u)
    {
        do
        {
            uint32 record = base + offset;
            uint32 first = r_u16(record);
            uint32 last = r_u16(record + 2u);
            if ((int32_t)track >= (int32_t)first && (int32_t)last >= (int32_t)track)
                return track - first + r_u16(record + 4u);
            base = r_u32(0x800653B0u);
            index += 1u;
            offset += 6u;
        } while (index < r_u32(base + 20u));
    }
    return track;
}

static uint32 race_signed_divide(uint32 numerator, uint32 denominator)
{
    if (denominator == 0u || (numerator == 0x80000000u && denominator == 0xFFFFFFFFu))
        abort();
    return (uint32)((int32_t)numerator / (int32_t)denominator);
}

/* Player positioning preserves the original fifth argument */
uint32 game_800B5C60(uint32 order, uint32 track, uint32 lateral, uint32 flag, uint32 sequence);

/* Full original MIPS and complete canonical pseudocode 80014544 */
uint32 sub_80014544(void)
{
    uint32 result = r_u32(0x800593A0u);
    uint32 index = 0u;
    if (result != 0u)
    {
        uint32 count = r_u32(0x80065254u);
        uint32 offset = 0u;
        if ((int32_t)count > 0)
        {
            do
            {
                uint32 numerator = index << 12u;
                uint32 angle = race_signed_divide(numerator, count);
                uint32 sine = (uint32)draft_agent3_missing_8004388C(angle);
                uint32 cosine;
                index += 1u;
                count = r_u32(0x80065254u);
                angle = race_signed_divide(numerator, count);
                w_u32(0x80066D7Cu + offset, 0u);
                w_u32(0x80066D78u + offset, sine * 800u);
                cosine = (uint32)draft_agent3_missing_800437EC(angle);
                count = r_u32(0x80065254u);
                angle = race_signed_divide(numerator, count);
                result = (int32_t)index < (int32_t)count ? 1u : 0u;
                w_u32(0x80066D80u + offset, cosine * 800u);
                w_u32(0x80066D94u + offset, (angle + 2048u) << 8u);
                offset += 952u;
            } while (result != 0u);
        }
    }
    else
    {
        uint32 track = r_u32(r_u32(RACE_GP + 0x7D4u) + 60u);
        uint32 sequence = game_800AE420(track);
        uint32 count = r_u32(0x80065254u);
        uint32 lateral = r_u32(r_u32(RACE_GP + 0x7D4u) + 64u);
        result = sequence;
        if ((int32_t)count > 0)
        {
            do
            {
                uint32 type, order, flag = 0u;
                if ((int32_t)track < 0)
                    track += r_u32(0x800653A4u);
                type = r_u8(r_u32(0x800653D4u) + track * 24u);
                lateral += (uint32)(int32_t)(sint8)r_u8(0x80059464u + type);
                order = r_u32(0x80065254u) - (index + 1u);
                if (r_u32(0x80059738u) == 2u)
                {
                    flag = order == 0u ? 1u : 0u;
                    if (r_u32(0x8005973Cu) == 2u && order == 1u)
                        flag = 1u;
                }
                game_800B5C60(order, track, lateral + (order & 1u), flag, sequence);
                type = r_u8(r_u32(0x800653D4u) + track * 24u);
                index += 1u;
                count = r_u32(0x80065254u);
                track -= 1u;
                sequence -= 1u;
                result = (int32_t)index < (int32_t)count ? 1u : 0u;
                lateral += (uint32)(int32_t)(sint8)r_u8(0x80059458u + type);
            } while (result != 0u);
        }
    }
    return result;
}

/* Full original 8002F374 coordinate interpolation */
uint32 sub_8002F374(uint32 position, uint32 destination)
{
    uint32 record = r_u32(RACE_GP + 0x9C4u) + (uint32)(int32_t)(int16_t)r_u16(position) * 24u;
    uint32 type = r_u8(record), lanes = r_u8(record + 3u) & 15u;
    uint32 first = r_u16(record + 4u) + (uint32)(int32_t)(int8_t)r_u8(0x8005AA30u + type * 2u);
    uint32 second = r_u16(record + 6u) + (uint32)(int32_t)(int8_t)r_u8(0x8005AA31u + type * 2u);
    uint32 lane = r_u8(position + 8u), base, result = 0u, axis;
    if (lane >= lanes)
    {
        lane = lanes - 1u;
        w_u8(position + 8u, (uint8)lane);
    }
    base = r_u32(RACE_GP + 0x9B4u);
    first = base + (first + lane) * 6u;
    second = base + (second + lane) * 6u;
    for (axis = 0u; axis < 3u; ++axis)
    {
        uint32 sum = (uint32)(int32_t)(int16_t)r_u16(first + axis * 2u);
        sum += (uint32)(int32_t)(int16_t)r_u16(first + 6u + axis * 2u);
        sum += (uint32)(int32_t)(int16_t)r_u16(second + axis * 2u);
        sum += (uint32)(int32_t)(int16_t)r_u16(second + 6u + axis * 2u);
        result = sum << 6;
        w_u32(destination + axis * 4u, (r_u32(record + 12u + axis * 4u) << 8) + result);
    }
    return result;
}

/* Full original 800168C8 track-range remapping */
uint32 sub_800168C8(uint32 order)
{
    uint32 position = 0x80066D6Cu + order * 952u;
    uint32 table = r_u32(0x800653B0u);
    uint32 track = (uint32)(int32_t)(int16_t)r_u16(position), count, index;
    if (r_u32(table + 4u) >= track)
    {
        w_u16(position + 2u, (uint16)track);
        return 0u;
    }
    count = r_u32(table + 20u);
    for (index = 0u; index < count; ++index, table += 6u)
    {
        uint32 first = r_u16(table + 32u);
        if ((int32_t)track >= (int32_t)first && (int32_t)track <= (int32_t)r_u16(table + 34u))
        {
            uint32 result = r_u16(table + 36u) + track - first;
            w_u16(position + 2u, (uint16)result);
            return result;
        }
    }
    return 0u;
}

/* Full original 80043450 octants and signed table lookup */
uint32 sub_80043450(uint32 x, uint32 z)
{
    uint32 numerator, denominator, origin;
    int subtract;
    if (x == 0u && z == 0u)
        return 0u;
    if ((int32_t)x >= 0)
    {
        if ((int32_t)z > 0)
        {
            if ((int32_t)x < (int32_t)z)
            {
                numerator = (x << 10) + (uint32)((int32_t)z / 2);
                denominator = z;
                origin = 0u;
                subtract = 0;
            }
            else
            {
                numerator = (z << 10) + (uint32)((int32_t)x / 2);
                denominator = x;
                origin = 1024u;
                subtract = 1;
            }
        }
        else if ((int32_t)z < (int32_t)(0u - x))
        {
            numerator = (uint32)((int32_t)z / 2) - (x << 10);
            denominator = z;
            origin = 2048u;
            subtract = 1;
        }
        else
        {
            numerator = (uint32)((int32_t)x / 2) - (z << 10);
            denominator = x;
            origin = 1024u;
            subtract = 0;
        }
    }
    else if ((int32_t)z > 0)
    {
        if ((int32_t)(0u - x) < (int32_t)z)
        {
            numerator = (uint32)((int32_t)z / 2) - (x << 10);
            denominator = z;
            origin = 4096u;
            subtract = 1;
        }
        else
        {
            numerator = (uint32)((int32_t)x / 2) - (z << 10);
            denominator = x;
            origin = 3072u;
            subtract = 0;
        }
    }
    else if ((int32_t)z < (int32_t)x)
    {
        numerator = (x << 10) + (uint32)((int32_t)z / 2);
        denominator = z;
        origin = 2048u;
        subtract = 0;
    }
    else
    {
        numerator = (z << 10) + (uint32)((int32_t)x / 2);
        denominator = x;
        origin = 3072u;
        subtract = 1;
    }
    {
        uint32 index = race_signed_divide(numerator, denominator);
        uint32 value = (uint32)(int32_t)(int16_t)r_u16(0x8005CDB0u + index * 2u);
        return subtract ? origin - value : origin + value;
    }
}

/* Full original GAME B5790 eleven-entry switch */
uint32 game_800B5790(uint32 track)
{
    uint32 record = r_u32(0x800653D4u) + track * 24u, base = r_u32(0x800653C4u);
    uint32 first = base + r_u16(record + 4u) * 6u, second = base + r_u16(record + 6u) * 6u;
    uint32 type = r_u8(record), first_offset = 0u, second_offset = 0u;
    int32_t x = 0, z = 0;
    if (type >= 1u && type <= 11u)
    {
        if (type == 3u || type == 4u)
            second_offset = 6u;
        else if (type == 6u || type == 7u)
            first_offset = 6u;
        first += first_offset;
        second += second_offset;
        x = (int16_t)r_u16(second) - (int16_t)r_u16(first) + (int16_t)r_u16(second + 6u) - (int16_t)r_u16(first + 6u);
        z = (int16_t)r_u16(second + 4u) - (int16_t)r_u16(first + 4u) + (int16_t)r_u16(second + 10u) - (int16_t)r_u16(first + 10u);
        x /= 4;
        z /= 4;
    }
    return sub_80043450((uint32)x, (uint32)z);
}

/* Full original GAME 9A114 object-reset writes and renderer call */
uint32 game_8009A114(uint32 object)
{
    uint32 i, angle, x, y, z;
    w_u32(object + 0x68u, 0xC0000000u);
    w_u8(object + 0x12u, 2u);
    w_u8(object + 0x3AAu, 0u);
    w_u8(object + 0x3ACu, 0u);
    w_u32(object + 0x88u, 0u);
    w_u32(object + 0x8Cu, 0u);
    w_u32(object + 0x90u, 0u);
    w_u32(object + 0x70u, 0u);
    w_u32(object + 0x74u, 0u);
    w_u32(object + 0x78u, 0u);
    w_u16(object + 0x38Au, 0u);
    w_u8(object + 0x3AEu, 0u);
    w_u16(object + 12u, 400u);
    for (i = 0u; i < 4u; ++i)
    {
        w_u32(object + 0x358u + i * 4u, 0u);
        w_u32(object + 0x348u + i * 4u, 0u);
    }
    angle = r_u32(object + 0x80u);
    x = r_u32(object + 0x64u);
    y = r_u32(object + 0x68u);
    z = r_u32(object + 0x6Cu);
    w_u16(object + 0x28u, 0u);
    w_u32(object + 0x7Cu, 0u);
    w_u16(object + 0x2Cu, 0u);
    w_u32(object + 0x84u, 0u);
    w_u32(object + 0xACu, (uint32)((int32_t)y >> 8));
    w_u16(object + 0x2Au, (uint16)((int32_t)angle >> 8));
    w_u32(object + 0xA8u, (uint32)((int32_t)x >> 8));
    w_u32(object + 0xB0u, (uint32)((int32_t)z >> 8));
    game_8009A200(object);
    for (i = 4u; i != 0u; --i)
        w_u32(object + 0x368u + (i - 1u) * 4u, 0u);
    w_u32(object + 0x88u, 0u);
    w_u32(object + 0x90u, 0u);
    w_u32(object + 0x74u, 0u);
    return object - 4u;
}

/* Full original GAME B4B58 inverse range lookup */
uint32 game_800B4B58(uint32 track)
{
    uint32 base = r_u32(0x800653B0u), index = 0u, offset = 0u;
    if (r_u32(base + 20u) != 0u)
    {
        do
        {
            uint32 record = base + offset, mapped = r_u16(record + 36u);
            uint32 first = r_u16(record + 32u);
            uint32 last = mapped + r_u16(record + 34u) - first - 1u;
            if ((int32_t)track >= (int32_t)mapped && (int32_t)track <= (int32_t)last)
                return track - mapped + first;
            base = r_u32(0x800653B0u);
            ++index;
            offset += 6u;
        } while (index < r_u32(base + 20u));
    }
    return 0xFFFFFFFFu;
}

/* Full original GAME B4BEC cache window update */
void game_800B4BEC(uint32 cache)
{
    int32_t distance = (int16_t)r_u16(cache + 0x36u) / 64;
    uint32 track, count, result;
    if (distance < 2)
        distance = 2;
    else if (distance >= 5)
        distance = 4;
    track = (uint32)(int32_t)(int16_t)r_u16(0x80066D6Eu + r_u8(cache + 0x61u) * 952u) + (uint32)distance;
    count = r_u32(0x800653A4u);
    if ((int32_t)track >= (int32_t)count)
        track -= count;
    if (r_u32(cache) == r_u32(0x800C7690u))
    {
        w_u16(cache + 0x3Eu, (uint16)track);
        w_u16(cache + 0x40u, (uint16)track);
    }
    else
    {
        result = game_800B4B58(track);
        w_u16(cache + 0x40u, (uint16)track);
        w_u16(cache + 0x3Eu, (uint16)(result == 0xFFFFFFFFu ? track : result));
    }
}

/* Full original GAME B5D68 projected along-track coordinate */
uint32 game_800B5D68(uint32 track, uint32 position)
{
    uint32 record, base, index, first, second, dx, dz, x, z, squares, dot, denominator;
    if (r_u32(0x800593A0u) == 1u)
        return 0u;
    record = r_u32(0x800653D4u) + track * 24u;
    base = r_u32(0x800653C4u);
    index = r_u16(record + 6u);
    first = base + index * 6u;
    second = base + (index + (r_u8(record + 3u) & 15u) + r_u32(0x800BF99Cu + r_u8(record) * 8u)) * 6u;
    x = (uint32)(int32_t)(int16_t)r_u16(first);
    z = (uint32)(int32_t)(int16_t)r_u16(first + 4u);
    dx = (uint32)(int32_t)(int16_t)r_u16(second) - x;
    dz = (uint32)(int32_t)(int16_t)r_u16(second + 4u) - z;
    squares = dx * dx + dz * dz;
    dot = ((uint32)((int32_t)r_u32(position) >> 8) - (r_u32(record + 12u) + x)) * dx;
    dot += ((uint32)((int32_t)r_u32(position + 8u) >> 8) - (r_u32(record + 20u) + z)) * dz;
    if ((int32_t)squares < 0)
        squares += 255u;
    denominator = (uint32)((int32_t)squares >> 8);
    return race_signed_divide(dot, denominator);
}

/* Full original GAME B5EA4 length-scaled along-track difference */
uint32 game_800B5EA4(uint32 track, uint32 along, uint32 baseline)
{
    uint32 record = r_u32(0x800653D4u) + track * 24u;
    uint32 base = r_u32(0x800653C4u), index = r_u16(record + 6u);
    uint32 first = base + index * 6u;
    uint32 second = base + (index + (r_u8(record + 3u) & 15u) + r_u32(0x800BF99Cu + r_u8(record) * 8u)) * 6u;
    uint32 dx = (uint32)(int32_t)(int16_t)r_u16(second) - (uint32)(int32_t)(int16_t)r_u16(first);
    uint32 dz = (uint32)(int32_t)(int16_t)r_u16(second + 4u) - (uint32)(int32_t)(int16_t)r_u16(first + 4u);
    uint32 result = sub_8003B674(dx * dx + dz * dz) * (along - baseline);
    if ((int32_t)result < 0)
        result += 255u;
    return (uint32)((int32_t)result >> 8);
}

/* Full original GAME B5B7C player cache update */
uint32 game_800B5B7C(uint32 order)
{
    uint32 cache = 0x800C7698u + order * 104u, position = 0x80066D6Cu + order * 952u;
    uint32 result, mapped;
    game_800B4BEC(cache);
    result = game_800B5D68((uint32)(int32_t)(int16_t)r_u16(position), position + 12u);
    w_u16(cache + 56u, (uint16)result);
    mapped = (uint32)(int32_t)(int16_t)r_u16(position + 2u);
    result = game_800B5EA4((uint32)(int32_t)(int16_t)r_u16(position), (uint32)(int32_t)(int16_t)r_u16(cache + 56u), r_u8(r_u32(cache) + mapped * 3u));
    w_u16(cache + 26u, (uint16)result);
    return result;
}

/* Full original GAME B5C60 five-argument player positioning */
uint32 game_800B5C60(uint32 order, uint32 track, uint32 lateral, uint32 flag, uint32 sequence)
{
    uint32 offset = order * 952u, position = 0x80066D6Cu + offset;
    w_u16(position, (uint16)track);
    w_u8(position + 8u, (uint8)lateral);
    sub_8002F374(position, position + 12u);
    w_u32(0x80066D94u + offset, game_800B5790((uint32)(int32_t)(int16_t)r_u16(position)) << 8);
    if (flag != 0u)
    {
        draft_agent1_missing_diagnostic("%s", (const char *)psx_addr(0x8008BC9Cu, 1u));
        w_u32(0x80066D94u + offset, r_u32(0x80066D94u + offset) + 0x80000u);
    }
    sub_800168C8(order);
    w_u16(position + 4u, (uint16)sequence);
    w_u16(position + 6u, (uint16)sequence);
    game_8009A114(0x80066D14u + offset);
    return game_800B5B7C(order);
}

/* Full original GAME B7E20 alternate-race cache initialization */
uint32 game_800B7E20(void)
{
    uint32 result, index = 0u, offset = 0u, cache = 0x800C7698u;
    memset(psx_addr(cache, 0x680u), 0, 0x680u);
    result = r_u32(0x80065254u);
    if ((int32_t)result > 0)
    {
        do
        {
            uint32 configuration = 0x800661DCu + offset;
            uint32 count, numerator, remainder, height;
            w_u8(cache + 0x57u, r_u8(configuration));
            w_u8(cache + 0x59u, r_u8(configuration + 1u));
            w_u8(cache + 0x58u, r_u8(configuration + 2u));
            w_u8(cache + 0x5Au, r_u8(configuration + 3u));
            w_u16(cache + 0x14u, r_u16(0x800BFAACu + r_u8(cache + 0x59u) * 2u));
            w_u8(cache + 0x5Bu, (uint8)r_u16(0x800BFA98u + r_u8(configuration + 4u) * 2u));
            height = r_u16(0x800BFAC0u + r_u8(cache + 0x58u) * 2u);
            w_u8(cache + 0x61u, (uint8)index);
            w_u8(cache + 0x5Fu, 1u);
            count = r_u32(0x80065254u);
            numerator = index + (uint32)((int32_t)count / 2);
            race_signed_divide(numerator, count);
            remainder = (uint32)((int32_t)numerator % (int32_t)count);
            w_u8(cache + 0x5Eu, (uint8)remainder);
            count = r_u32(0x80065254u);
            offset += 44u;
            ++index;
            result = 600u;
            w_u16(cache + 0x3Cu, (uint16)result);
            w_u16(cache + 0x4Eu, (uint16)height);
            cache += 104u;
            if ((int32_t)index >= (int32_t)count)
                break;
        } while (1);
    }
    return result;
}

/* Full original GAME B44F0 track-file loading and player cache initialization */
uint32 game_800B44F0(void)
{
    char path[256];
    uint32 result, index = 0u, configuration = 0x800661DCu, cache = 0x800C7698u;
    memset(psx_addr(cache, 0x680u), 0, 0x680u);
    if (r_u32(0x800593A0u) == 0u)
    {
        uint32 bytes, buffer;
        snprintf(path, sizeof(path), "%sLEFT.TRK", (const char *)psx_addr(0x8006569Cu, 1u));
        bytes = sub_800423EC(path, 1u);
        buffer = sub_80042544(bytes);
        w_u32(0x800C7690u, buffer);
        sub_8004225C(path, buffer, bytes);
        snprintf(path, sizeof(path), "%sRIGHT.TRK", (const char *)psx_addr(0x8006569Cu, 1u));
        bytes = sub_800423EC(path, 1u);
        buffer = sub_80042544(bytes);
        w_u32(0x800C7694u, buffer);
        sub_8004225C(path, buffer, bytes);
    }
    result = r_u32(0x80065254u);
    if ((int32_t)result > 0)
    {
        do
        {
            uint32 side;
            w_u8(cache + 0x57u, r_u8(configuration));
            w_u8(cache + 0x59u, r_u8(configuration + 1u));
            w_u8(cache + 0x58u, r_u8(configuration + 2u));
            w_u8(cache + 0x5Au, r_u8(configuration + 3u));
            w_u16(cache + 0x48u, r_u16(0x800BF9F8u + r_u8(cache + 0x57u) * 2u));
            w_u16(cache + 0x3Au, r_u16(0x800BFA0Cu + r_u8(cache + 0x59u) * 2u));
            w_u16(cache + 0x14u, r_u16(0x800BFA20u + r_u8(cache + 0x59u) * 2u));
            w_u16(cache + 0x4Au, r_u16(0x800BFA34u + r_u8(cache + 0x59u) * 2u));
            w_u16(cache + 0x4Eu, r_u16(0x800BFA48u + r_u8(cache + 0x58u) * 2u));
            w_u8(cache + 0x5Cu, (uint8)r_u16(0x800BFA5Cu + r_u8(configuration + 5u) * 2u));
            w_u8(cache + 0x5Du, (uint8)r_u16(0x800BFA70u + r_u8(configuration + 6u) * 2u));
            w_u8(cache + 0x5Bu, (uint8)r_u16(0x800BF9F8u + r_u8(configuration + 4u) * 2u));
            w_u16(cache + 0x50u, r_u16(0x800BFA84u + r_u8(configuration + 7u) * 2u));
            w_u8(cache + 0x61u, (uint8)index);
            w_u8(cache + 0x5Fu, 5u);
            w_u16(cache + 0x3Cu, 360u);
            side = r_u32(r_u32(0x800651E4u) + 0x44u);
            if (side == 0u)
            {
                w_u8(cache + 0x60u, 0u);
                w_u32(cache, r_u32(0x800C7690u));
            }
            else if (side == 1u)
            {
                w_u8(cache + 0x60u, 1u);
                w_u32(cache, r_u32(0x800C7694u));
            }
            w_u16(0x800C7D18u + index * 4u, (uint16)index);
            result = r_u32(0x80065254u);
            configuration += 44u;
            ++index;
            result = (uint32)((int32_t)index < (int32_t)result);
            cache += 104u;
        } while (result != 0u);
    }
    return result;
}

/* Full original GAME 8C058 direction scaling and descending clear */
uint32 game_8008C058(void)
{
    uint32 x = r_u32(0x800BE7B0u), y, z, scaled_x, scaled_y, scaled_z, index;
    x = (uint32)((int32_t)(0u - ((x * 5u) << 14)) >> 12);
    scaled_x = x * 163u;
    y = r_u32(0x800BE7B4u);
    y = (uint32)((int32_t)(0u - ((y * 5u) << 14)) >> 12);
    scaled_y = y * 163u;
    z = r_u32(0x800BE7B8u);
    w_u32(0x800BFED0u, 163u);
    w_u32(0x800BFED8u, 163u);
    w_u32(0x800BFF98u, 4096u);
    z = (uint32)((int32_t)(0u - ((z * 5u) << 14)) >> 12);
    scaled_z = z * 163u;
    w_u32(0x800BFF78u, x);
    w_u32(0x800BFFA0u, 204u);
    w_u32(0x800BFEC8u, 204u);
    w_u32(0x800BFF7Cu, y);
    w_u32(0x800BFF80u, z);
    scaled_x = (uint32)((int32_t)scaled_x >> 12);
    scaled_y = (uint32)((int32_t)scaled_y >> 12);
    w_u32(0x800BFF40u, scaled_x);
    w_u32(0x800BFF44u, scaled_y);
    w_u32(0x800BFF90u, scaled_y);
    scaled_z = (uint32)((int32_t)scaled_z >> 12);
    w_u32(0x800BFF48u, scaled_z);
    for (index = 0u; index < 10u; ++index)
        w_u32(0x800BFF74u - index * 4u, 0u);
    return scaled_z;
}

/* Original CIR filename formatting and actual file allocation */
static void race_load_circuit(void)
{
    char path[256];
    uint32 bytes, buffer;
    snprintf(path, sizeof(path), "%sCIR%d.DAT", (const char *)psx_addr(0x8006569Cu, 1u), (int32_t)r_u32(0x80064C08u));
    bytes = sub_800423EC(path, 1u);
    buffer = sub_80042544(bytes);
    w_u32(0x800C7D58u, buffer);
    w_u32(0x800C7D5Cu, buffer);
    sub_8004225C(path, buffer, bytes);
}

/* Full original GAME B8AA0 circuit pointer relocation and coordinate scaling */
uint32 game_800B8AA0(void)
{
    uint32 base, index = 1u, entry, pointer, value, result;
    race_load_circuit();
    base = r_u32(0x800C7D58u);
    result = r_u32(base);
    if (result != 0u)
    {
        entry = base + 4u;
        do
        {
            pointer = r_u32(entry) + base;
            w_u32(entry, pointer);
            value = r_u32(pointer + 16u);
            if ((int32_t)value < 0)
                value += 255u;
            w_u32(pointer + 16u, (uint32)((int32_t)value >> 8));
            value = r_u32(pointer + 20u);
            if ((int32_t)value < 0)
                value += 255u;
            w_u32(pointer + 20u, (uint32)((int32_t)value >> 8));
            value = r_u32(pointer + 24u);
            if ((int32_t)value < 0)
                value += 255u;
            w_u32(pointer + 24u, (uint32)((int32_t)value >> 8));
            result = r_u32(base);
            ++index;
            result = (uint32)(result < index);
            entry += 4u;
        } while (result == 0u);
    }
    return result;
}

/* Full original signed MULT high-word operation */
static uint32 race_count_product_high(uint32 value)
{
    int64_t product = (int64_t)(int32_t)value * INT64_C(0x2AAAAAAB);
    return (uint32)((uint64_t)product >> 32);
}

/* Full original GAME B88D0 circuit relocation and paired object slots */
uint32 game_800B88D0(void)
{
    uint32 base, index = 1u, entry, value, high, result, record;
    w_u32(0x800BFAD8u, r_u32(0x800593A8u) == 1u ? 35u : 47u);
    race_load_circuit();
    base = r_u32(0x800C7D58u);
    if (r_u32(base) != 0u)
    {
        entry = base + 4u;
        do
        {
            w_u32(entry, r_u32(entry) + base);
            ++index;
            entry += 4u;
        } while (r_u32(base) >= index);
    }
    value = r_u32(0x800C7D60u) * 24u;
    memset(psx_addr(0x800C7DB0u, value), 0, (size_t)value);
    value = r_u32(0x800653A4u) + 11u;
    high = race_count_product_high(value);
    w_u32(0x800C7D64u, (uint32)((int32_t)high >> 1) - (uint32)((int32_t)value >> 31));
    value = r_u32(0x800BFAD8u) + 1u;
    result = race_count_product_high(value) - (uint32)((int32_t)value >> 31) + 1u;
    w_u32(0x800C7D60u, result);
    index = 0u;
    record = 0x800C7DB0u;
    if ((int32_t)result > 0)
    {
        do
        {
            w_u8(record + 8u, 0u);
            w_u8(record + 20u, 0u);
            result = r_u32(0x800C7D60u);
            w_u32(record, 0u);
            w_u32(record + 12u, 0u);
            w_u16(record + 6u, 0xFFFFu);
            w_u16(record + 18u, 0xFFFFu);
            ++index;
            result = (uint32)((int32_t)index < (int32_t)result);
            record += 24u;
        } while (result != 0u);
    }
    return result;
}

/* Full original MIPS and complete canonical pseudocode 800388AC */
uint32 sub_800388AC(void)
{
    uint32 list = r_u32(0x800651E4u) + 1800u, count = 0u, track, lateral, record, vertex, value, random;
    w_u32(RACE_GP + 0x6ECu, 0u);
    w_u32(RACE_GP + 0x6F0u, 0u);
    w_u32(RACE_GP + 0xA3Cu, 0xFFFFFFFFu);
    w_u32(RACE_GP + 0xA40u, 0xFFFFFFFFu);
    w_u32(RACE_GP + 0x9F8u, 0u);
    w_u32(RACE_GP + 0x9F4u, list);
    if (r_u16(list) != 0xFFFFu)
    {
        do
        {
            count = r_u32(RACE_GP + 0x9F8u) + 1u;
            value = r_u16(list + count * 16u);
            w_u32(RACE_GP + 0x9F8u, count);
        } while (value != 0xFFFFu);
    }
    list = r_u32(RACE_GP + 0x9F4u);
    track = r_u16(list + 6u);
    lateral = r_u16(list + 8u);
    record = r_u32(0x800653D4u) + track * 24u;
    w_u32(RACE_GP + 0xA18u, lateral);
    w_u32(RACE_GP + 0xA14u, lateral);
    w_u32(0x800654E4u, track);
    w_u32(RACE_GP + 0xAD0u, track);
    vertex = r_u32(0x800653C4u) + (r_u16(record + 4u) + lateral) * 6u;
    value = ((uint32)(int32_t)(int16_t)r_u16(vertex) + r_u32(record + 12u)) << 8;
    w_u32(RACE_GP + 0xA00u, value);
    w_u32(RACE_GP + 0x9FCu, value);
    vertex = r_u32(0x800653C4u) + (r_u16(record + 4u) + lateral) * 6u;
    value = ((uint32)(int32_t)(int16_t)r_u16(vertex + 4u) + r_u32(record + 20u)) << 8;
    w_u32(RACE_GP + 0xA08u, value);
    w_u32(RACE_GP + 0xA04u, value);
    value = r_u16(list + 10u);
    w_u32(RACE_GP + 0xA50u, 0u);
    w_u32(RACE_GP + 0xA4Cu, 0u);
    w_u32(RACE_GP + 0xA10u, value);
    w_u32(RACE_GP + 0xA0Cu, value);
    random = randPSX();
    w_u32(RACE_GP + 0x6E4u, (uint32)((int32_t)random % 10000) + 10000u);
    random = randPSX();
    w_u32(RACE_GP + 0xA2Cu, 8u);
    w_u32(RACE_GP + 0xA30u, 8u);
    w_u32(RACE_GP + 0xA24u, 0u);
    w_u32(RACE_GP + 0xA28u, 0u);
    count = r_u32(RACE_GP + 0x9F8u);
    w_u32(RACE_GP + 0x6E8u, (uint32)((int32_t)random % 10000) + 10000u);
    if ((int32_t)count < 2 || (int32_t)r_u32(0x80064C08u) >= 24)
    {
        w_u32(RACE_GP + 0xA4Cu, 4u);
        w_u32(RACE_GP + 0xA50u, 4u);
    }
    return 4u;
}

/* Full original MIPS and complete canonical pseudocode 8001ED2C */
uint32 sub_8001ED2C(uint32 sound, uint32 voice, uint32 mode, sint32 frequency, uint32 position, uint32 velocity)
{
    uint32 slot, mask;
    if (voice == 0xFFFFFFFFu)
    {
        voice = draft_agent3_missing_8001F860();
        if (voice == 0xFFFFFFFFu)
            return voice;
    }
    slot = 0x80073FA0u + voice * 32u;
    w_u16(slot + 24u, (uint16)sound);
    w_u32(slot, position);
    w_u32(slot + 4u, velocity);
    w_u32(slot + 16u, mode);
    w_u32(slot + 20u, (uint32)frequency);
    w_u8(slot + 30u, 0u);
    w_u8(slot + 31u, 1u);
    mask = 1u << (voice & 31u);
    if (frequency >= 16384)
        frequency = 16383;
    draft_agent3_missing_80049F84(0u, mask);
    position = r_u32(0x800742E0u + sound * 12u);
    w_u32(0x800742A0u, mask);
    w_u32(0x800742A4u, 128u);
    w_u32(0x800742BCu, position);
    draft_agent3_missing_8004BB74(psx_addr(0x800742A0u, 1u));
    sub_8001F8FC(voice, frequency, 0, 0);
    draft_agent3_missing_80049F84(1u, mask);
    return voice;
}

/* Full original MIPS and complete canonical pseudocode 8001CBE8 */
uint32 sub_8001CBE8(void)
{
    uint32 voice = 0u, count = r_u32(0x80065254u), result, index = 0u, offset = 0u;
    while ((int32_t)voice < ((int32_t)count >= 5 ? 4 : (int32_t)count))
    {
        uint32 position = 0x80066D78u + voice * 952u;
        sub_8001ED2C(39u, voice, 0u, 4096, position, position + 12u);
        count = r_u32(0x80065254u);
        ++voice;
    }
    result = r_u32(0x80064C14u);
    if ((int32_t)result >= 0)
    {
        voice = 4u;
        do
        {
            uint32 frame, limit;
            sub_8001ED2C(23u, voice, 0u, 4096, 0x80066D78u + offset, 0u);
            w_u8(0x800652A4u + voice, 0u);
            w_u8(0x8006529Cu + voice, 0u);
            w_u16(0x800652A4u + index * 2u, 0u);
            frame = r_u32(0x80064A30u);
            limit = r_u32(0x80064C14u);
            offset += 952u;
            ++voice;
            ++index;
            w_u32(0x80065290u + (index - 1u) * 4u, frame);
            result = (uint32)((int32_t)limit < (int32_t)index);
        } while (result == 0u);
    }
    w_u32(RACE_GP + 0x308u, 0u);
    return result;
}
