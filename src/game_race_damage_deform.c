#include "draft_signatures.h"

/* Full boot 351B8 record-linked GPF0/GPL0 vertex deformation */
uint32 sub_800351B8(uint32 records, uint32 indexed_records, uint32 vertices, uint32 damages)
{
    uint32 descriptor = r_u32(records + 12u), index, linked, words[3], destination, next_descriptor;
    sint32 first_scale = (sint16)r_u16(damages + (descriptor & 255u) * 2u), scale;
    SVECTOR input;
    VECTOR accumulated;
    indexed_records -= 16u;
    for (;;)
    {
        words[0] = r_u32(records);
        words[1] = r_u32(records + 4u);
        words[2] = r_u32(records + 8u);
        input.vx = (sint16)words[0];
        input.vy = (sint16)words[1];
        input.vz = (sint16)words[2];
        gte_gpf0(&input, first_scale, &accumulated);
        index = (descriptor >> 8) & 4095u;
        if (index)
        {
            for (;;)
            {
                linked = indexed_records + (index << 4);
                descriptor = r_u32(linked + 12u);
                if (linked == records)
                    break;
                input.vx = (sint16)r_u32(linked);
                input.vy = (sint16)r_u32(linked + 4u);
                input.vz = (sint16)r_u32(linked + 8u);
                scale = (sint16)r_u16(damages + (descriptor & 255u) * 2u);
                index = (descriptor >> 8) & 4095u;
                gte_gpl0(&input, scale, &accumulated);
            }
        }
        destination = vertices + ((descriptor >> 20) << 3);
        {
            uint32 x = (uint32)((sint32)words[0] >> 16) + (uint32)(accumulated.vx >> 10);
            uint32 y = (words[1] >> 16) + (uint32)(accumulated.vy >> 10);
            uint32 z = (words[2] >> 16) + (uint32)(accumulated.vz >> 10);
            w_u32(destination, (x & 65535u) + (y << 16));
            /* Original next-record descriptor read precedes final halfword store */
            next_descriptor = r_u32(records + 28u);
            w_u16(destination + 4u, (uint16)z);
        }
        records += 16u;
        descriptor = next_descriptor;
        if (descriptor & 128u)
            return 128u;
    }
}
