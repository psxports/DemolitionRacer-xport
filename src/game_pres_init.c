#include "psx.h"
#include <stdint.h>
#include <string.h>

#define PRES_WORD(address) (*(uint32 *)psx_addr((address), 4u))

/* Preserve the signed MULT high word and wrapping reciprocal arithmetic */
static uint32 pres_random_name(void)
{
    uint32 random = randPSX();
    int64_t product = (int64_t)(int32_t)random * (int64_t)(int32_t)0x84210843u;
    uint32 high = (uint32)((uint64_t)product >> 32);
    uint32 quotient = (uint32)((int32_t)(high + random) >> 6);
    uint32 remainder;
    quotient -= (uint32)((int32_t)random >> 31);
    remainder = random - quotient * 124u;
    return 0x800A9BDCu + remainder * 44u;
}

static void pres_copy_random_name(uint32 destination)
{
    uint32 source = pres_random_name();
    strcpy((char *)psx_addr(destination, 1u), (const char *)psx_addr(source, 1u));
}

static void pres_init_fixed_group(uint32 base, uint32 bytes)
{
    uint32 offset;
    for (offset = 0u; offset < bytes; offset += 32u)
    {
        pres_copy_random_name(base + offset);
        PRES_WORD(base + offset + 20u) = 0u;
        PRES_WORD(base + offset + 24u) = 2000u;
    }
}

static void pres_init_scaled_group(uint32 base, uint32 bytes)
{
    uint32 offset;
    for (offset = 0u; offset < bytes; offset += 32u)
    {
        uint32 scale;
        uint32 half;
        pres_copy_random_name(base + offset);
        scale = PRES_WORD(0x80065124u);
        PRES_WORD(base + offset + 24u) = 0u;
        half = (uint32)((int32_t)(scale + (scale >> 31)) >> 1);
        PRES_WORD(base + offset + 20u) = half * 300u;
    }
}

/* Full MIPS source: PRES.BIN 8008FD38 through the return delay slot 80090178 */
void pres_8008FD38(void)
{
    const uint32 base = 0x800668BCu;
    pres_init_fixed_group(0x8006649Cu, 0x140u);
    pres_init_scaled_group(0x800665DCu, 0x140u);
    pres_init_scaled_group(0x8006671Cu, 0x140u);
    pres_init_fixed_group(0x8006685Cu, 0x60u);
    pres_init_scaled_group(base, 0x60u);

    PRES_WORD(base - 0x164u) = 1u;
    PRES_WORD(base - 0x2A4u) = 1u;
    PRES_WORD(base - 0x3E4u) = 1u;
    PRES_WORD(base - 0x144u) = 2u;
    PRES_WORD(base - 0x284u) = 2u;
    PRES_WORD(base - 0x3C4u) = 2u;
    PRES_WORD(base - 0x124u) = 3u;
    PRES_WORD(base - 0x264u) = 3u;
    PRES_WORD(base - 0x3A4u) = 3u;
    PRES_WORD(base - 0x104u) = 7u;
    PRES_WORD(base - 0x244u) = 7u;
    PRES_WORD(base - 0x384u) = 7u;
    PRES_WORD(base - 0xE4u) = 9u;
    PRES_WORD(base - 0x224u) = 9u;
    PRES_WORD(base - 0x364u) = 9u;
    PRES_WORD(base - 0xC4u) = 10u;
    PRES_WORD(base - 0x204u) = 10u;
    PRES_WORD(base - 0x344u) = 10u;
    PRES_WORD(base - 0xA4u) = 11u;
    PRES_WORD(base - 0x1E4u) = 11u;
    PRES_WORD(base - 0x324u) = 11u;
    PRES_WORD(base - 0x84u) = 12u;
    PRES_WORD(base - 0x1C4u) = 12u;
    PRES_WORD(base - 0x304u) = 12u;
    PRES_WORD(base - 0x64u) = 13u;
    PRES_WORD(base - 0x1A4u) = 13u;
    PRES_WORD(base - 0x2E4u) = 13u;
    PRES_WORD(base + 0x1Cu) = 6u;
    PRES_WORD(base - 0x44u) = 6u;
    PRES_WORD(base - 0x184u) = 0u;
    PRES_WORD(base - 0x2C4u) = 0u;
    PRES_WORD(base - 0x404u) = 0u;
    PRES_WORD(base + 0x3Cu) = 8u;
    PRES_WORD(base - 0x24u) = 8u;
    PRES_WORD(base + 0x5Cu) = 14u;
    PRES_WORD(base - 4u) = 14u;
}
