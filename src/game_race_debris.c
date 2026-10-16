#include "draft_signatures.h"
#include <string.h>

#define R32(a) (*(uint32 *)psx_addr((a), 4u))
#define R16(a) (*(uint16 *)psx_addr((a), 2u))
#define R8(a) (*(uint8 *)psx_addr((a), 1u))

typedef struct
{
    sint32 position[3];
    sint32 velocity[3];
    sint16 orientation[4];
    SVECTOR angular;
} DEBRIS_STATE;

extern void sub_80035458(const sint16 *first, const sint16 *second, sint16 *output);
extern void dr_race_normalize(VECTOR *input, SVECTOR *output);
extern void dr_detached_ground(uint32 object, const sint32 *position, uint32 force);

static uint32 debris_div(uint32 numerator, uint32 denominator)
{
    if (!denominator)
        xport_mips_break(7u);
    if (numerator == 0x80000000u && denominator == 0xFFFFFFFFu)
        xport_mips_break(6u);
    return (uint32)((sint32)numerator / (sint32)denominator);
}

static sint32 debris_mul(uint32 first, uint32 second, uint32 shift)
{
    return (sint32)(first * second) >> shift;
}

static uint32 debris_lz(uint32 value)
{
    xport_gte_write_data(30u, value);
    return xport_gte_read_data(31u);
}

static uint32 debris_reduce(uint32 vector[3])
{
    uint32 index, minimum = 32, leading;
    for (index = 0; index < 3; ++index)
    {
        leading = debris_lz(vector[index]);
        if (leading < minimum)
            minimum = leading;
    }
    if (minimum >= 17)
        return 0;
    minimum = 17 - minimum;
    for (index = 0; index < 3; ++index)
        vector[index] = (uint32)((sint32)vector[index] >> (minimum & 31u));
    return minimum;
}

static void debris_load(const uint32 vector[3])
{
    uint32 index;
    for (index = 0; index < 3; ++index)
        xport_gte_write_data(9u + index, vector[index]);
}

static void debris_diagonal(const sint16 *diagonal)
{
    uint32 index;
    for (index = 0; index < 3; ++index)
        xport_gte_write_control(index * 2u, (uint16)diagonal[index]);
    xport_gte_write_control(1u, 0);
    xport_gte_write_control(3u, 0);
}

/* Original 3621C also supplies the X carrier consumed by 361D4 */
static uint32 debris_dot(const sint16 *normal, const sint32 *input, sint32 *dot_x)
{
    uint32 vector[3], shift, result;
    memcpy(vector, input, 12);
    shift = debris_reduce(vector);
    if (dot_x)
        *dot_x = (sint32)vector[0];
    xport_gte_write_control(0u, (uint16)normal[0] + ((uint32)(uint16)normal[1] << 16));
    xport_gte_write_control(1u, (uint16)normal[2]);
    debris_load(vector);
    xport_gte_execute(0x49E012u);
    result = xport_gte_read_data(25u);
    return result << (shift & 31u);
}

static uint32 debris_short_dot(const SVECTOR *normal, const SVECTOR *input, sint32 dot_x)
{
    uint32 vector[3] = {(uint32)dot_x, (uint32)(sint32)input->vy, (uint32)(sint32)input->vz};
    xport_gte_write_control(0u, (uint16)normal->vx + ((uint32)(uint16)normal->vy << 16));
    xport_gte_write_control(1u, (uint16)normal->vz);
    debris_load(vector);
    xport_gte_execute(0x49E012u);
    return xport_gte_read_data(25u);
}

static void debris_cross(const SVECTOR *first, const SVECTOR *second, SVECTOR *output)
{
    uint32 vector[3] = {(uint32)(sint32)second->vx, (uint32)(sint32)second->vy, (uint32)(sint32)second->vz};
    xport_gte_write_control(0u, (uint16)first->vx);
    xport_gte_write_control(2u, (uint16)first->vy);
    xport_gte_write_control(4u, (uint16)first->vz);
    debris_load(vector);
    xport_gte_execute(0x178000Cu);
    output->vx = (sint16)xport_gte_read_data(25u);
    output->vy = (sint16)xport_gte_read_data(26u);
    output->vz = (sint16)xport_gte_read_data(27u);
}

static void debris_vector_op(const sint16 *first, const VECTOR *input, VECTOR *output, uint32 command)
{
    uint32 vector[3], shift, index;
    memcpy(vector, input, 12);
    shift = debris_reduce(vector);
    debris_diagonal(first);
    debris_load(vector);
    xport_gte_execute(command);
    for (index = 0; index < 3; ++index)
        ((sint32 *)output)[index] = (sint32)(xport_gte_read_data(25u + index) << (shift & 31u));
}

static void debris_short_diagonal(const sint16 *first, const SVECTOR *input, SVECTOR *output)
{
    uint32 vector[3] = {(uint32)(sint32)input->vx, (uint32)(sint32)input->vy, (uint32)(sint32)input->vz};
    debris_diagonal(first);
    debris_load(vector);
    xport_gte_execute(0x49E012u);
    output->vx = (sint16)xport_gte_read_data(25u);
    output->vy = (sint16)xport_gte_read_data(26u);
    output->vz = (sint16)xport_gte_read_data(27u);
}

static void debris_gpf12(uint32 scalar, const uint32 vector[3])
{
    SVECTOR input = {(sint16)vector[0], (sint16)vector[1], (sint16)vector[2], 0};
    VECTOR output;
    gte_gpf12(&input, (sint32)scalar, &output);
}

static sint32 debris_scale_short(uint32 scalar, const SVECTOR *input, SVECTOR *output)
{
    uint32 vector[3] = {(uint32)(sint32)input->vx, (uint32)(sint32)input->vy, (uint32)(sint32)input->vz};
    uint32 result;
    debris_gpf12(scalar, vector);
    output->vx = (sint16)xport_gte_read_data(25u);
    output->vy = (sint16)xport_gte_read_data(26u);
    result = xport_gte_read_data(27u);
    output->vz = (sint16)result;
    return (sint32)result;
}

static void debris_scale_long(uint32 scalar, const VECTOR *input, VECTOR *output)
{
    uint32 vector[3], shift = 0, leading, index;
    memcpy(vector, input, 12);
    /* Original measures the vector before measuring the scalar */
    leading = debris_reduce(vector);
    shift = debris_lz(scalar);
    shift = shift < 18 ? 18 - shift : 0;
    scalar = (uint32)((sint32)scalar >> (shift & 31u));
    debris_gpf12(scalar, vector);
    shift += leading;
    for (index = 0; index < 3; ++index)
        ((sint32 *)output)[index] = (sint32)(xport_gte_read_data(25u + index) << (shift & 31u));
}

static uint32 debris_length_square(const sint32 *input, uint32 *shift_output)
{
    uint32 vector[3], shift, x, y, z;
    memcpy(vector, input, 12);
    shift = debris_reduce(vector);
    debris_load(vector);
    xport_gte_execute(0xA80428u);
    x = xport_gte_read_data(25u);
    y = xport_gte_read_data(26u);
    z = xport_gte_read_data(27u);
    if (shift_output)
        *shift_output = shift;
    /* Both original scaled branches overwrite X plus Y with Y plus Z */
    return shift ? y + z : x + y + z;
}

static uint32 debris_short_square(const SVECTOR *input)
{
    uint32 vector[3] = {(uint32)(sint32)input->vx, (uint32)(sint32)input->vy, (uint32)(sint32)input->vz};
    debris_load(vector);
    xport_gte_execute(0xA80428u);
    return xport_gte_read_data(25u) + xport_gte_read_data(26u) + xport_gte_read_data(27u);
}

static uint32 debris_sqrt12(uint32 value)
{
    uint32 leading = debris_lz(value), even, normalized, table;
    sint32 shift;
    if (leading == 32)
        return 0;
    even = leading & ~1u;
    normalized = even >= 24 ? value << ((even - 24) & 31u) : (uint32)((sint32)value >> ((24 - even) & 31u));
    table = (uint32)(sint32)(sint16)R16(0x800602C0u + (normalized - 64u) * 2u);
    shift = (19 - (sint32)even) >> 1;
    return shift < 0 ? table >> ((0u - (uint32)shift) & 31u) : table << ((uint32)shift & 31u);
}

static void debris_scale_quaternion(uint32 scalar, const sint16 *input, sint16 *output)
{
    SVECTOR v = {input[0], input[1], input[2], 0};
    sint32 w = debris_mul((uint32)(sint32)input[3], scalar, 12);
    debris_scale_short(scalar, &v, &v);
    output[0] = v.vx;
    output[1] = v.vy;
    output[2] = v.vz;
    output[3] = (sint16)w;
}

static void debris_normalize_quaternion(sint16 *q)
{
    SVECTOR v = {q[0], q[1], q[2], 0};
    sint32 scalar = (sint32)(0x3000u - (uint32)debris_mul((uint32)(sint32)q[3], (uint32)(sint32)q[3], 12) - debris_short_square(&v)) >> 1;
    sint16 w = (sint16)debris_mul((uint32)scalar, (uint32)(sint32)q[3], 12);
    debris_scale_short((uint32)scalar, &v, &v);
    q[0] = v.vx;
    q[1] = v.vy;
    q[2] = v.vz;
    q[3] = w;
}

static void debris_rotate(const sint16 *q, const SVECTOR *input, SVECTOR *output)
{
    sint16 conjugate[4] = {(sint16)(0u - (uint16)q[0]), (sint16)(0u - (uint16)q[1]), (sint16)(0u - (uint16)q[2]), q[3]};
    sint16 v[4] = {input->vx, input->vy, input->vz, 0};
    sub_80035458(conjugate, v, v);
    sub_80035458(v, q, v);
    output->vx = v[0];
    output->vy = v[1];
    output->vz = v[2];
}

static void debris_integrate(const DEBRIS_STATE *input, DEBRIS_STATE *output, const sint16 *inertia, const sint16 *inverse_inertia)
{
    uint32 step = R32(0x800BFED8u), shift, speed, linear, angular, position_scale, gravity, index;
    VECTOR v = {input->velocity[0], input->velocity[1], input->velocity[2], 0};
    SVECTOR omega = input->angular, transformed = {0};
    sint16 quaternion[4];
    speed = debris_length_square(input->velocity, &shift);
    speed = debris_sqrt12(speed) << (shift & 31u);
    linear = 4096u - (uint32)debris_mul(R32(0x800BFFA0u), (uint32)debris_mul(speed, step, 12), 12);
    if ((sint32)linear < 2048)
        linear = 2048;
    angular = 4096u - (uint32)debris_mul(R32(0x800BFEC8u), (uint32)debris_mul(debris_sqrt12(debris_short_square(&omega)), step, 12), 12);
    if ((sint32)angular < 2048)
        angular = 2048;
    position_scale = R32(0x800651C4u) << 3;
    gravity = R32(0x800BFF90u);
    if (step != R32(0x800BFED0u))
    {
        position_scale = debris_div(position_scale * step, R32(0x800BFED0u));
        gravity = debris_div(gravity * step, R32(0x800BFED0u));
    }
    v.vy = (sint32)((uint32)v.vy + gravity);
    debris_scale_long(linear, &v, &v);
    memcpy(output->velocity, &v, 12);
    v.vx = (sint32)((uint32)v.vx + (uint32)input->velocity[0]);
    v.vy = (sint32)((uint32)v.vy + (uint32)input->velocity[1]);
    v.vz = (sint32)((uint32)v.vz + (uint32)input->velocity[2]);
    debris_scale_long(position_scale, &v, &v);
    for (index = 0; index < 3; ++index)
        output->position[index] = (sint32)((uint32)input->position[index] + (uint32)((sint32 *)&v)[index]);
    if (inertia)
    {
        debris_scale_short(angular, &omega, &omega);
        debris_short_diagonal(inertia, &omega, &transformed);
        debris_cross(&transformed, &omega, &transformed);
        debris_scale_short(step, &transformed, &transformed);
        debris_short_diagonal(inverse_inertia, &transformed, &transformed);
        output->angular.vx = (sint16)((uint16)omega.vx + (uint16)transformed.vx);
        output->angular.vy = (sint16)((uint16)omega.vy + (uint16)transformed.vy);
        output->angular.vz = (sint16)((uint16)omega.vz + (uint16)transformed.vz);
        output->angular.pad = 0;
    }
    else
        debris_scale_quaternion(angular, (const sint16 *)&input->angular, (sint16 *)&output->angular);
    sub_80035458(input->orientation, (const sint16 *)&output->angular, quaternion);
    debris_scale_quaternion((uint32)((sint32)(step + (step >> 31)) >> 1), quaternion, quaternion);
    for (index = 0; index < 4; ++index)
        output->orientation[index] = (sint16)((uint16)input->orientation[index] + (uint16)quaternion[index]);
    debris_normalize_quaternion(output->orientation);
}

static uint32 debris_contact(uint32 object, const SVECTOR *normal, SVECTOR *point, sint32 *dot_x)
{
    uint32 shape = R8(object + 157u), code = 0;
    uint32 x = R32(object + 80u), y = R32(object + 84u), z = R32(object + 88u);
    SVECTOR candidate = {0};
    VECTOR direction = {0};
    uint32 candidate_depth, point_depth;
    if (shape == 1 || shape >= 6)
    {
        *dot_x = debris_scale_short(0u - (uint32)(sint32)(sint16)R16(object + 96u), normal, point);
        return 0;
    }
    if (shape == 4)
    {
        if (!normal->vx && !normal->vz)
        {
            point->vx = (sint16)x;
            point->vz = 0;
        }
        else
        {
            candidate.vx = normal->vx;
            candidate.vz = normal->vz;
            direction.vx = normal->vx;
            direction.vz = normal->vz;
            dr_race_normalize(&direction, &candidate);
            *dot_x = debris_scale_short(0u - x, &candidate, point);
        }
        point->vy = (sint16)(normal->vy > 0 ? 0u - y : y);
        return 0;
    }
    point->vx = (sint16)(normal->vx > 0 ? 0u - x : x);
    if (normal->vx > 0)
        code = 1;
    if (shape == 0)
    {
        point->vy = (sint16)(normal->vy > 0 ? 0u - y : y);
        if (normal->vy > 0)
            code += 2;
        point->vz = (sint16)(normal->vz > 0 ? 0u - z : z);
        if (normal->vz > 0)
            code += 4;
        return code;
    }
    if (shape == 5)
    {
        point->vy = 0;
        point->vz = (sint16)(normal->vz > 0 ? 0u - z : z);
        if (normal->vz > 0)
            code += 4;
        return code;
    }
    point->vz = (sint16)(normal->vz > 0 ? 0u - z : z);
    if (normal->vz > 0)
        code += 2;
    point->vy = (sint16)(0u - y);
    if (normal->vy < 0)
    {
        candidate.vy = (sint16)y;
        if (shape == 3)
            candidate.vz = point->vz;
        candidate_depth = debris_short_dot(normal, &candidate, *dot_x);
        point_depth = debris_short_dot(normal, point, *dot_x);
        if ((sint32)candidate_depth < (sint32)point_depth)
        {
            *point = candidate;
            code = shape == 2 ? 5 : (code >> 1) + 4;
        }
    }
    return code;
}

static void debris_unrotate(const sint16 *q, const SVECTOR *input, SVECTOR *output)
{
    sint16 conjugate[4] = {(sint16)(0u - (uint16)q[0]), (sint16)(0u - (uint16)q[1]), (sint16)(0u - (uint16)q[2]), q[3]};
    sint16 v[4] = {input->vx, input->vy, input->vz, 0};
    sub_80035458(q, v, v);
    sub_80035458(v, conjugate, v);
    output->vx = v[0];
    output->vy = v[1];
    output->vz = v[2];
}

static void debris_rotate_velocity(const DEBRIS_STATE *state, VECTOR *output)
{
    SVECTOR velocity = {(sint16)(state->velocity[0] >> 8), (sint16)(state->velocity[1] >> 8), (sint16)(state->velocity[2] >> 8), 0};
    debris_rotate(state->orientation, &velocity, &velocity);
    output->vx = (sint32)((uint32)(sint32)velocity.vx << 8);
    output->vy = (sint32)((uint32)(sint32)velocity.vy << 8);
    output->vz = (sint32)((uint32)(sint32)velocity.vz << 8);
}

void game_8008C864(uint32 object)
{
    DEBRIS_STATE original, predicted, current;
    const sint16 *inertia = (const sint16 *)psx_addr(object + 98u, 6u);
    const sint16 *inverse_inertia = (const sint16 *)psx_addr(object + 106u, 6u);
    uint32 plane = 0, edge_distance = 0, depth, old_depth, plane_distance, hit_step, full_step, saved_step;
    uint32 index, component, normal_speed, friction, magnitude, scalar, counter;
    sint32 radius, dot_x = 0;
    SVECTOR plane_normal = {0}, body_normal = {0}, point = {0}, cross = {0}, angular = {0}, direction = {0};
    VECTOR body_velocity = {0}, contact_velocity = {0}, normal_velocity = {0}, tangent = {0}, impulse = {0}, torque = {0}, correction = {0};
    memcpy(psx_addr(object + 20u, 20u), psx_addr(object, 20u), 20u);
    memcpy(psx_addr(object, 12u), psx_addr(object + 40u, 12u), 12u);
    memcpy(psx_addr(object + 12u, 8u), psx_addr(object + 64u, 8u), 8u);
    memcpy(&original, psx_addr(object + 40u, sizeof(original)), sizeof(original));
    debris_integrate(&original, &predicted, inertia, inverse_inertia);
    dr_detached_ground(object, predicted.position, 0);
    radius = (sint16)R16(object + 96u);
    component = debris_dot((const sint16 *)psx_addr(object + 116u, 6u), predicted.position, &dot_x) - R32(object + 124u);
    if ((sint32)component < radius)
    {
        counter = R32(object + 148u) + 1u;
        if ((sint32)counter >= 11)
        {
            R8(object + 156u) = 3;
            return;
        }
        R32(object + 148u) = counter;
        plane = object + 116u;
    }
    else if (R8(object + 155u))
    {
        edge_distance = debris_dot((const sint16 *)psx_addr(object + 132u, 6u), predicted.position, &dot_x) - R32(object + 140u);
        if ((sint32)edge_distance < radius)
            plane = object + 132u;
    }
    if (!plane)
    {
        memcpy(psx_addr(object + 40u, sizeof(predicted)), &predicted, sizeof(predicted));
        return;
    }
    memcpy(&plane_normal, psx_addr(plane, 6u), 6u);
    debris_rotate(predicted.orientation, &plane_normal, &body_normal);
    plane_distance = R32(plane + 8u) - debris_dot((const sint16 *)&plane_normal, predicted.position, &dot_x);
    full_step = R32(0x800BFED0u);
    hit_step = full_step;
    (void)debris_contact(object, &body_normal, &point, &dot_x);
    depth = debris_short_dot(&body_normal, &point, dot_x) - plane_distance;
    if ((!edge_distance || depth) && (sint32)depth < 0)
    {
        normal_speed = debris_dot((const sint16 *)&plane_normal, predicted.velocity, &dot_x);
        dot_x = predicted.angular.vx;
        debris_cross(&body_normal, &predicted.angular, &cross);
        normal_speed += debris_short_dot(&point, &cross, dot_x);
        if ((sint32)normal_speed < 0)
        {
            debris_rotate(original.orientation, &plane_normal, &direction);
            plane_distance = R32(plane + 8u) - debris_dot((const sint16 *)&plane_normal, original.position, &dot_x);
            old_depth = debris_short_dot(&direction, &point, dot_x) - plane_distance;
            if (old_depth == depth)
                hit_step = (uint32)((sint32)(full_step + (full_step >> 31)) >> 1);
            else
                hit_step = (uint32)debris_mul(full_step, debris_div(old_depth << 12, old_depth - depth), 12);
            if ((sint32)hit_step < 0)
                hit_step = 0;
        }
    }
    if ((sint32)hit_step < (sint32)R32(0x800BFED0u))
    {
        saved_step = R32(0x800BFED8u);
        R32(0x800BFED8u) = hit_step;
        if ((sint32)hit_step > 0)
            debris_integrate(&original, &predicted, inertia, inverse_inertia);
        else
            predicted = original;
        debris_rotate(predicted.orientation, &plane_normal, &body_normal);
        plane_distance = R32(plane + 8u) - debris_dot((const sint16 *)&plane_normal, predicted.position, &dot_x);
        angular = predicted.angular;
        debris_cross(&angular, &point, &cross);
        debris_rotate_velocity(&predicted, &body_velocity);
        contact_velocity.vx = (sint32)((uint32)body_velocity.vx + (uint32)(sint32)cross.vx);
        contact_velocity.vy = (sint32)((uint32)body_velocity.vy + (uint32)(sint32)cross.vy);
        contact_velocity.vz = (sint32)((uint32)body_velocity.vz + (uint32)(sint32)cross.vz);
        normal_speed = debris_dot((const sint16 *)&body_normal, (const sint32 *)&contact_velocity, &dot_x);
        normal_velocity.vx = body_normal.vx;
        normal_velocity.vy = body_normal.vy;
        normal_velocity.vz = body_normal.vz;
        debris_scale_long(normal_speed, &normal_velocity, &normal_velocity);
        tangent.vx = (sint32)((uint32)contact_velocity.vx - (uint32)normal_velocity.vx);
        tangent.vy = (sint32)((uint32)contact_velocity.vy - (uint32)normal_velocity.vy);
        tangent.vz = (sint32)((uint32)contact_velocity.vz - (uint32)normal_velocity.vz);
        friction = (uint32)debris_mul((uint32)(sint32)(sint16)R16(plane + 12u), normal_speed, 12);
        if ((sint32)friction < 0)
            friction = 0u - friction;
        magnitude = debris_sqrt12(debris_length_square((const sint32 *)&tangent, NULL));
        if ((sint32)friction < (sint32)magnitude)
        {
            debris_scale_long(debris_div(friction << 12, magnitude), &tangent, &tangent);
            contact_velocity.vx = (sint32)((uint32)tangent.vx + (uint32)normal_velocity.vx);
            contact_velocity.vy = (sint32)((uint32)tangent.vy + (uint32)normal_velocity.vy);
            contact_velocity.vz = (sint32)((uint32)tangent.vz + (uint32)normal_velocity.vz);
        }
        direction.vx = (sint16)(contact_velocity.vx >> 6);
        direction.vy = (sint16)(contact_velocity.vy >> 6);
        direction.vz = (sint16)(contact_velocity.vz >> 6);
        if (!direction.vx && !direction.vy && !direction.vz)
            impulse = contact_velocity;
        else
        {
            correction.vx = direction.vx;
            correction.vy = direction.vy;
            correction.vz = direction.vz;
            dr_race_normalize(&correction, &direction);
            scalar = debris_dot((const sint16 *)&direction, (const sint32 *)&contact_velocity, &dot_x);
            impulse.vx = direction.vx;
            impulse.vy = direction.vy;
            impulse.vz = direction.vz;
            debris_scale_long(scalar, &impulse, &impulse);
            torque.vx = (sint32)((uint32)(sint32)point.vx << 10);
            torque.vy = (sint32)((uint32)(sint32)point.vy << 10);
            torque.vz = (sint32)((uint32)(sint32)point.vz << 10);
            debris_vector_op((const sint16 *)&direction, &torque, &torque, 0x178000Cu);
            debris_vector_op(inverse_inertia, &torque, &torque, 0x49E012u);
            debris_vector_op((const sint16 *)&point, &torque, &torque, 0x178000Cu);
            scalar = debris_dot((const sint16 *)&direction, (const sint32 *)&torque, &dot_x) + 4096u;
            scalar = 0u - debris_div((R32(0x800BFF98u) + 4096u) << 12, scalar);
            impulse.vx = debris_mul(scalar, (uint32)impulse.vx, 12);
            impulse.vy = debris_mul(scalar, (uint32)impulse.vy, 12);
            impulse.vz = debris_mul(scalar, (uint32)impulse.vz, 12);
        }
        direction.vx = (sint16)(impulse.vx >> 8);
        direction.vy = (sint16)(impulse.vy >> 8);
        direction.vz = (sint16)(impulse.vz >> 8);
        debris_unrotate(predicted.orientation, &direction, &direction);
        predicted.velocity[0] = (sint32)((uint32)predicted.velocity[0] + ((uint32)(sint32)direction.vx << 8));
        predicted.velocity[1] = (sint32)((uint32)predicted.velocity[1] + ((uint32)(sint32)direction.vy << 8));
        predicted.velocity[2] = (sint32)((uint32)predicted.velocity[2] + ((uint32)(sint32)direction.vz << 8));
        debris_vector_op((const sint16 *)&point, &impulse, &torque, 0x178000Cu);
        debris_vector_op(inverse_inertia, &torque, &torque, 0x49E012u);
        angular.vx = (sint16)((uint16)angular.vx + ((uint32)torque.vx << 10));
        angular.vy = (sint16)((uint16)angular.vy + ((uint32)torque.vy << 10));
        angular.vz = (sint16)((uint16)angular.vz + ((uint32)torque.vz << 10));
        R16(object + 78u) = 0;
        R16(object + 72u) = (uint16)angular.vx;
        R16(object + 74u) = (uint16)angular.vy;
        R16(object + 76u) = (uint16)angular.vz;
        if ((sint16)R16(plane + 14u) && (sint32)debris_length_square(predicted.velocity, NULL) < 4096)
        {
            torque.vx = angular.vx;
            torque.vy = angular.vy;
            torque.vz = angular.vz;
            debris_vector_op(inertia, &torque, &torque, 0x49E012u);
            if ((sint32)debris_dot((const sint16 *)&angular, (const sint32 *)&torque, &dot_x) < 4096)
            {
                R8(object + 156u) = 3;
                R32(0x800BFED8u) = saved_step;
                return;
            }
        }
        R32(0x800BFED8u) = saved_step - hit_step;
        memcpy(psx_addr(object + 40u, 12u), predicted.position, 12u);
        memcpy(psx_addr(object + 52u, 12u), predicted.velocity, 12u);
        memcpy(psx_addr(object + 64u, 8u), predicted.orientation, 8u);
        memcpy(&current, psx_addr(object + 40u, sizeof(current)), sizeof(current));
        debris_integrate(&current, &predicted, inertia, inverse_inertia);
        R32(0x800BFED8u) = saved_step;
    }
    else if ((sint32)depth < 0)
    {
        correction.vx = plane_normal.vx;
        correction.vy = plane_normal.vy;
        correction.vz = plane_normal.vz;
        debris_scale_long((uint32)((sint32)depth >> 8), &correction, &correction);
        for (index = 0; index < 3; ++index)
            predicted.position[index] = (sint32)((uint32)predicted.position[index] - (uint32)((sint32 *)&correction)[index]);
    }
    memcpy(psx_addr(object + 40u, sizeof(predicted)), &predicted, sizeof(predicted));
    (void)plane_distance;
}
