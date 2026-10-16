#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
extern void dr_race_normalize_vector(VECTOR *, VECTOR *);
extern uint32 sub_8003B674(uint32);
extern uint32 sub_8001ED2C(uint32, uint32, uint32, sint32, uint32, uint32);
extern uint32 sub_80043450(uint32, uint32);
extern sint32 draft_agent3_missing_800437EC(uint32);
extern sint32 draft_agent3_missing_8004388C(uint32);
extern uint32 game_8009A114(uint32);

static sint32 rec_add(sint32 a, sint32 b)
{
    return (sint32)((uint32)a + (uint32)b);
}

static sint32 rec_sub(sint32 a, sint32 b)
{
    return (sint32)((uint32)a - (uint32)b);
}

static sint32 rec_mul(sint32 a, sint32 b)
{
    return (sint32)((uint32)a * (uint32)b);
}

static sint32 rec_neg(sint32 a)
{
    return (sint32)(0u - (uint32)a);
}

static sint32 rec_shift(sint32 a, uint32 n)
{
    return rec_add(a, a < 0 ? (sint32)((1u << n) - 1u) : 0) >> n;
}

static sint32 rec_div(sint32 a, sint32 b)
{
    if (!b)
        xport_mips_break(7u);
    if (a == INT_MIN && b == -1)
        xport_mips_break(6u);
    return a / b;
}

static sint32 rec_product(sint32 a, sint32 b)
{
    return (sint32)(((sint64)a * b + 0x40000000) >> 30);
}

static void rec_transform(uint32 matrix, const VECTOR *input, VECTOR *output)
{
    uint32 row;
    for (row = 0; row < 3; ++row)
    {
        sint32 x = rec_product(r_s32(matrix + row * 12), input->vx);
        sint32 y = rec_product(r_s32(matrix + row * 12 + 4), input->vy);
        sint32 z = rec_product(r_s32(matrix + row * 12 + 8), input->vz);
        ((sint32 *)output)[row] = rec_add(rec_add(x, y), z);
    }
}

static sint32 rec_sample(uint32 index)
{
    uint32 table = r_u32(0x80065174);
    return index < 2048 ? r_s32(table + index * 4) : rec_neg(r_s32(table + index * 4 - 0x2000));
}

static sint32 rec_trig(sint32 angle, uint32 phase)
{
    uint32 index = ((uint32)(angle >> 18) + phase) & 4095;
    sint32 first = rec_sample(index), next = rec_sample((index + 1) & 4095);
    sint32 fraction = (sint32)(((uint32)angle & 0x3FFFF) << 12);
    sint32 upper = rec_product(next, fraction);
    return rec_add(upper, rec_product(first, rec_sub(0x40000000, fraction)));
}

static sint32 rec_sine(sint32 angle)
{
    return rec_trig(angle, 0);
}

static sint32 rec_cosine(sint32 angle)
{
    return rec_trig(angle, 1024);
}

static void rec_compose(const uint32 *a, const uint32 *b, uint32 *out)
{
    uint32 row, column;
    for (row = 0; row < 3; ++row)
        for (column = 0; column < 3; ++column)
        {
            sint32 x = rec_product((sint32)a[row * 3], (sint32)b[column]);
            sint32 y = rec_product((sint32)a[row * 3 + 1], (sint32)b[column + 3]);
            sint32 z = rec_product((sint32)a[row * 3 + 2], (sint32)b[column + 6]);
            out[row * 3 + column] = (uint32)rec_add(rec_add(x, y), z);
        }
}

static void rec_cross(const VECTOR *a, const VECTOR *b, VECTOR *out)
{
    MATRIX rotation;
    uint32 saved[3], index;
    ReadRotMatrix(&rotation);
    saved[0] = (uint16)rotation.m[0][0] | ((uint32)(uint16)rotation.m[0][1] << 16);
    saved[1] = (uint16)rotation.m[1][1] | ((uint32)(uint16)rotation.m[1][2] << 16);
    saved[2] = (uint32)(sint32)rotation.m[2][2];
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(index * 2, (uint32)((const sint32 *)a)[index]);
    xport_gte_write_data(11, (uint32)b->vz);
    xport_gte_write_data(9, (uint32)b->vx);
    xport_gte_write_data(10, (uint32)b->vy);
    xport_gte_execute(0x178000C);
    for (index = 0; index < 3; ++index)
        ((sint32 *)out)[index] = (sint32)xport_gte_read_data(25 + index);
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(index * 2, saved[index]);
}

static void rec_axis_rotation(uint32 *out, const VECTOR *axis, sint32 angle)
{
    sint32 cosine = rec_cosine(angle), sine = rec_sine(angle);
    sint32 y = axis->vy, z = axis->vz, x = axis->vx;
    sint32 yz = rec_add(rec_mul(y, y), rec_mul(z, z));
    sint32 transverse = (sint32)sub_8003B674((uint32)yz), length;
    sint32 inverse_length, inverse_product, inverse_transverse;
    sint32 b0, b3, b6, nx, ny, nz, tz, ty;
    uint32 basis[9], rotation[9], temporary[9];
    if (transverse <= 0)
    {
        if (x < 0)
            sine = rec_neg(sine);
        out[0] = 0x40000000;
        out[1] = out[2] = out[3] = 0;
        out[5] = (uint32)rec_neg(sine);
        out[6] = 0;
        out[7] = (uint32)sine;
        out[4] = out[8] = (uint32)cosine;
        return;
    }
    length = (sint32)sub_8003B674((uint32)rec_add(rec_mul(x, x), yz));
    if (length <= 0)
    {
        memset(out, 0, 36);
        out[0] = out[4] = out[8] = 0x40000000;
        return;
    }
    inverse_length = rec_div(0x40000000, length);
    /* Original divides by the wrapped product, despite the pseudocode denominator */
    inverse_product = rec_div(0x40000000, rec_mul(transverse, length));
    inverse_transverse = rec_div(0x40000000, transverse);
    b0 = rec_mul(transverse, inverse_length);
    b3 = rec_mul(rec_mul(rec_neg(x), y), inverse_product);
    nx = rec_mul(x, inverse_length);
    tz = rec_mul(z, inverse_transverse);
    ny = rec_mul(y, inverse_length);
    ty = rec_mul(rec_neg(y), inverse_transverse);
    nz = rec_mul(z, inverse_length);
    b6 = rec_mul(rec_mul(rec_neg(x), z), inverse_product);
    basis[0] = (uint32)b0;
    basis[1] = 0;
    basis[2] = (uint32)nx;
    basis[3] = (uint32)b3;
    basis[4] = (uint32)tz;
    basis[5] = (uint32)ny;
    basis[6] = (uint32)b6;
    basis[7] = (uint32)ty;
    basis[8] = (uint32)nz;
    rotation[0] = rotation[4] = (uint32)cosine;
    rotation[1] = (uint32)sine;
    rotation[3] = (uint32)rec_neg(sine);
    rotation[2] = rotation[5] = rotation[6] = rotation[7] = 0;
    rotation[8] = 0x40000000;
    rec_compose(basis, rotation, temporary);
    basis[0] = (uint32)b0;
    basis[1] = (uint32)b3;
    basis[2] = (uint32)b6;
    basis[3] = 0;
    basis[4] = (uint32)tz;
    basis[5] = (uint32)ty;
    basis[6] = (uint32)nx;
    basis[7] = (uint32)ny;
    basis[8] = (uint32)nz;
    rec_compose(temporary, basis, out);
}

static void rec_orientation(uint32 object)
{
    sint32 heading, cosine, sine, pitch, roll, dx, dz;
    sint16 normal_x, normal_y, normal_z;
    uint32 wrapped_x, wrapped_z, quadrants;
    if (r_s16(object + 906) < 3)
        return;
    heading = r_s16(object + 42);
    cosine = draft_agent3_missing_800437EC((uint32)heading);
    sine = draft_agent3_missing_8004388C((uint32)heading);
    normal_x = (sint16)rec_shift(rec_sub(rec_mul(r_s16(object + 752), cosine), rec_mul(r_s16(object + 756), sine)), 12);
    normal_y = r_s16(object + 754);
    normal_z = (sint16)rec_shift(rec_add(rec_mul(r_s16(object + 752), sine), rec_mul(r_s16(object + 756), cosine)), 12);
    pitch = (sint32)sub_80043450((uint32)rec_neg(normal_z), (uint32)rec_neg(normal_y));
    roll = (sint32)sub_80043450((uint32)(sint32)normal_x, sub_8003B674((uint32)rec_add(rec_mul(normal_y, normal_y), rec_mul(normal_z, normal_z))));
    wrapped_x = (uint32)rec_add(rec_sub(pitch, r_s16(object + 40)), 512);
    wrapped_z = (uint32)rec_add(rec_sub(roll, r_s16(object + 44)), 512);
    dx = (sint32)(wrapped_x & 1023) - 512;
    dz = (sint32)(wrapped_z & 1023) - 512;
    if ((dx < 0 ? rec_neg(dx) : dx) >= 24 || (dz < 0 ? rec_neg(dz) : dz) >= 24)
        return;
    if ((((r_u32(object + 136) + 2048) & 4095) - 2017) >= 63 || (((r_u32(object + 144) + 2048) & 4095) - 2017) >= 63)
        return;
    quadrants = ((wrapped_x & 0xC00) >> 10) | ((wrapped_z & 0xC00) >> 8);
    if (quadrants == 0 || quadrants == 10)
        game_8009A114(object);
}

/* Full GAME 964B0 recovery with native matrix and vector locals */
void game_800964B0(uint32 object, uint32 contact_mask)
{
    VECTOR contact, relative, axis;
    uint32 basis[9], rotation[9], temporary[9], index, component;
    sint32 count = 0, height = 0, difference, length, normal_speed, impulse, angle;
    sint32 cosine, sine, inverse, along_x, along_z, saved_y, correction;
    contact.vx = 0;
    contact.vy = 0;
    contact.vz = 0;
    for (index = 0u; index < 8u; ++index)
    {
        uint32 sample = object + 480u + index * 12u;
        if (!(contact_mask & (1u << index)))
            continue;
        contact.vx = rec_add(contact.vx, rec_sub(r_s32(sample), r_s32(object + 100u)) >> 4);
        contact.vy = rec_add(contact.vy, rec_sub(r_s32(sample + 4u), r_s32(object + 104u)) >> 4);
        contact.vz = rec_add(contact.vz, rec_sub(r_s32(sample + 8u), r_s32(object + 108u)) >> 4);
        difference = rec_sub(r_s32(0x800C08D8u + index * 4u), r_s32(sample + 4u));
        if (height < difference)
            height = difference;
        ++count;
    }
    contact.vx = rec_div(contact.vx, count);
    contact.vy = rec_div(contact.vy, count);
    contact.vz = rec_div(contact.vz, count);
    rec_transform(object + 216u, &contact, &relative);
    relative.vx = rec_add(relative.vx, rec_sub(r_s32(object + 112u) >> 4, contact.vx));
    relative.vy = rec_add(relative.vy, rec_sub(r_s32(object + 116u) >> 4, contact.vy));
    relative.vz = rec_add(relative.vz, rec_sub(r_s32(object + 120u) >> 4, contact.vz));
    length = (sint32)sub_8003B674((uint32)rec_add(rec_mul(contact.vx, contact.vx), rec_mul(contact.vz, contact.vz)));
    normal_speed = rec_neg(rec_add(rec_add(rec_mul(r_s16(object + 752u), relative.vx), rec_mul(r_s16(object + 754u), relative.vy)), rec_mul(r_s16(object + 756u), relative.vz))) >> 12;
    if (normal_speed < 0)
    {
        if (normal_speed < -4000)
        {
            sub_8001ED2C((randPSX() & 1u) + 19u, ~0u, 1024u, 4096, object + 100u, 0u);
            game_8009082C(r_u8(object + 937u), 1024u);
        }
        if (length > 128)
        {
            impulse = rec_div(rec_mul(normal_speed, -212500), rec_add(rec_shift(rec_mul(length, length), 8u), 250000));
            angle = rec_shift(rec_mul(rec_shift(rec_mul(length, impulse), 4u), 854), 4u);
            cosine = rec_cosine(angle);
            sine = rec_sine(angle);
            inverse = rec_div(0x40000000, length);
            along_z = rec_mul(contact.vz, inverse);
            along_x = rec_mul(contact.vx, inverse);
            basis[0] = (uint32)along_z;
            basis[1] = 0u;
            basis[2] = (uint32)along_x;
            basis[3] = 0u;
            basis[4] = 0x40000000u;
            basis[5] = 0u;
            basis[6] = (uint32)rec_neg(along_x);
            basis[7] = 0u;
            basis[8] = (uint32)along_z;
            rotation[0] = 0x40000000u;
            rotation[1] = 0u;
            rotation[2] = 0u;
            rotation[3] = 0u;
            rotation[4] = (uint32)cosine;
            rotation[5] = (uint32)sine;
            rotation[6] = 0u;
            rotation[7] = (uint32)rec_neg(sine);
            rotation[8] = (uint32)cosine;
            rec_compose(basis, rotation, temporary);
            basis[2] = (uint32)rec_neg(along_x);
            basis[6] = (uint32)along_x;
            rec_compose(temporary, basis, rotation);
            rec_compose(rotation, (const uint32 *)psx_addr(object + 216u, 36u), temporary);
        }
        else
        {
            impulse = rec_shift(rec_mul(rec_neg(normal_speed), 272), 8u);
            memcpy(temporary, psx_addr(object + 216u, 36u), 36u);
        }
        length = (sint32)sub_8003B674((uint32)rec_add(rec_mul(relative.vx, relative.vx), rec_mul(relative.vz, relative.vz)));
        if (length > 0)
        {
            correction = rec_shift(rec_add(normal_speed, 1024), 5u);
            saved_y = contact.vy;
            contact.vx >>= 1;
            contact.vy >>= 1;
            contact.vz >>= 1;
            dr_race_normalize_vector(&contact, &contact);
            relative.vy = 0;
            dr_race_normalize_vector(&relative, &relative);
            rec_cross(&relative, &contact, &axis);
            if (length < correction)
                correction = length;
            angle = rec_shift(rec_mul(rec_shift(rec_mul(saved_y, correction), 4u), 854), 4u);
            rec_axis_rotation(basis, &axis, angle);
            rec_compose(temporary, basis, (uint32 *)psx_addr(object + 216u, 36u));
            w_u32(object + 112u, (uint32)rec_sub(r_s32(object + 112u), rec_shift(rec_mul(correction, relative.vx), 8u)));
            w_u32(object + 120u, (uint32)rec_sub(r_s32(object + 120u), rec_shift(rec_mul(correction, relative.vz), 8u)));
        }
        else
            memcpy(psx_addr(object + 216u, 36u), temporary, 36u);
        for (component = 0u; component < 3u; ++component)
        {
            uint32 velocity = object + 112u + component * 4u;
            difference = rec_shift(rec_mul(r_s16(object + 752u + component * 2u), impulse), 8u);
            w_u32(velocity, (uint32)rec_sub(r_s32(velocity), difference));
        }
    }
    w_u32(object + 104u, (uint32)rec_add(r_s32(object + 104u), height));
    rec_orientation(object);
    for (component = 0u; component < 3u; ++component)
        w_u32(object + 168u + component * 4u, (uint32)(r_s32(object + 100u + component * 4u) >> 8));
    if (normal_speed < -2000)
        sub_8001ED2C(10u, ~0u, 4096u, 4096, object + 100u, 0u);
}
