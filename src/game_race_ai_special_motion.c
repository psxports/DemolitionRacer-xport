#include "draft_signatures.h"
#include <limits.h>
extern uint32 sub_8001685C(uint32);
extern sint32 draft_agent3_missing_800437EC(uint32);
extern sint32 draft_agent3_missing_8004388C(uint32);
extern void dr_driving_gears(uint32, uint32);

static sint32 special_add(sint32 a,sint32 b){return (sint32)((uint32)a+(uint32)b);}
static sint32 special_sub(sint32 a,sint32 b){return (sint32)((uint32)a-(uint32)b);}
static sint32 special_mul(sint32 a,sint32 b){return (sint32)((uint32)a*(uint32)b);}
static sint32 special_neg(sint32 a){return (sint32)(0u-(uint32)a);}
static sint32 special_shift(sint32 a,uint32 n){return (sint32)((uint32)a+(a<0?(1u<<n)-1u:0u))>>n;}
static sint32 special_high(sint32 a,sint32 b){return (sint32)(((sint64)a*b)>>32);}
static sint32 special_magic(sint32 a,uint32 n){return special_sub(special_high(a,0x0C907DA5)>>n,a>>31);}
static sint32 special_read(uint32 a){return (sint32)r_u32(a);}
static sint32 special_half(uint32 a){return (sint16)r_u16(a);}
static sint32 special_div(sint32 a,sint32 b)
{if(!b)xport_mips_break(7u);if(a==INT_MIN&&b==-1)xport_mips_break(6u);return a/b;}
static sint32 special_limit(uint32 address,sint32 force,sint32 limit)
{
    if(force>limit){w_u32(address,(uint32)special_sub(force,limit));return limit;}
    if(force<special_neg(limit)){w_u32(address,(uint32)special_sub(special_neg(force),limit));return special_neg(limit);}
    w_u32(address,0u);return force;
}

void game_8009C58C(uint32 object)
{
    uint32 model=r_u32(object+4u);
    sint32 vx,vz,angular,heading,steering,limit,throttle;
    sint32 cosine,sine,front_cosine,front_sine,steer_cosine,steer_sine;
    sint32 lateral,longitudinal,front_longitudinal,engine=0;
    sint32 denominator,side_term,brake_term,sum,force_side,force_front;
    sint32 angular251,lateral320,term,numerator,rotation,linear_x,linear_z,timestep;
    w_u8(object+932u,(uint8)sub_8001685C(object+88u));
    w_u32(object+112u,(uint32)special_sub(special_read(object+112u),special_shift(special_mul(special_half(object+752u),1500),12u)));
    w_u32(object+120u,(uint32)special_sub(special_read(object+120u),special_shift(special_mul(special_half(object+756u),1500),12u)));
    if(!special_half(object+16u)){
        sint32 damping=special_shift(special_mul(special_read(0x800651C4u),6000),8u);
        vx=special_read(object+112u);
        w_u32(object+112u,(uint32)special_add(vx,special_shift(special_mul(special_shift(special_neg(vx),8u),damping),12u)));
        vz=special_read(object+120u);
        w_u32(object+120u,(uint32)special_add(vz,special_shift(special_mul(special_shift(special_neg(vz),8u),damping),12u)));
    }
    heading=special_read(object+128u)>>8;
    steering=special_shift(special_read(object+8u),8u);
    vx=special_read(object+112u);vz=special_read(object+120u);angular=special_read(object+140u);
    throttle=special_half(object+16u);limit=special_half(r_u32(object+4u)+44u);
    w_u16(object+904u,(uint16)dr_race_motion_sqrt((uint32)special_add(special_mul(vx>>8,vx>>8),special_mul(vz>>8,vz>>8))));
    cosine=draft_agent3_missing_800437EC((uint32)heading);
    sine=draft_agent3_missing_8004388C((uint32)heading);
    front_cosine=draft_agent3_missing_800437EC((uint32)special_add(heading,steering));
    front_sine=draft_agent3_missing_8004388C((uint32)special_add(heading,steering));
    steer_cosine=draft_agent3_missing_800437EC((uint32)steering);
    steer_sine=draft_agent3_missing_8004388C((uint32)steering);
    lateral=special_shift(special_sub(special_mul(vx,cosine),special_mul(vz,sine)),12u);
    longitudinal=special_shift(special_add(special_mul(vx,sine),special_mul(vz,cosine)),12u);
    front_longitudinal=special_sub(special_shift(special_add(special_mul(vx,front_sine),special_mul(vz,front_cosine)),12u),special_magic(special_mul(angular,special_mul(steer_sine,320)),17u));
    w_u32(object+888u,(uint32)longitudinal);w_u32(object+892u,(uint32)front_longitudinal);
    if(throttle && !((sint32)((uint32)special_half(model+116u)<<8u)<longitudinal)){
        dr_driving_gears(object,model);game_80016404(object,model);engine=game_8001675C(object,model);
    }
    denominator=special_add(special_shift(special_mul(special_shift(special_mul(steer_sine,260),12u),steer_sine),12u),special_shift(special_mul(special_shift(special_mul(steer_cosine,400),12u),steer_cosine),12u));
    side_term=special_shift(special_mul(special_shift(special_mul(special_sub(special_magic(special_mul(angular,320),5u),lateral),160),12u),steer_sine),12u);
    brake_term=special_shift(special_mul(special_shift(special_neg(special_mul(special_add(engine,longitudinal),60)),12u),steer_cosine),12u);
    sum=special_add(side_term,brake_term);
    angular251=special_mul(angular,251);lateral320=special_mul(lateral,320);
    term=special_shift(special_mul(special_shift(special_mul(special_shift(special_sub(angular251,lateral320),10u),640),12u),steer_cosine),12u);
    numerator=special_add(special_mul(sum,steer_sine),special_mul(term,steer_cosine));
    force_side=special_div(numerator,denominator);
    term=special_shift(special_mul(special_shift(special_neg(special_add(lateral320,angular251)),10u),640),12u);
    numerator=special_add(special_mul(term,steer_cosine),special_mul(special_shift(special_mul(special_add(engine,longitudinal),260),12u),steer_sine));
    force_front=special_div(numerator,denominator);
    force_side=special_limit(object+896u,force_side,limit);
    force_front=special_limit(object+900u,force_front,limit);
    term=special_sub(special_mul(force_front,special_shift(special_mul(steer_cosine,320),12u)),special_mul(force_side,320));
    rotation=special_sub(special_add(special_high(term,(sint32)0x828CBFBFu),term)>>7,term>>31);
    if(rotation>=1401)rotation=1400;if(rotation<-1400)rotation=-1400;
    linear_x=special_add(special_add(special_shift(special_mul(force_side,cosine),12u),special_shift(special_mul(force_front,front_cosine),12u)),special_shift(special_mul(engine,sine),12u));
    linear_z=special_add(special_add(special_shift(special_mul(special_neg(force_side),sine),12u),special_shift(special_mul(special_neg(force_front),front_sine),12u)),special_shift(special_mul(engine,cosine),12u));
    timestep=special_read(0x800651C4u);
    w_u32(object+140u,(uint32)special_add(special_read(object+140u),special_shift(special_mul(rotation,timestep),8u)));
    w_u32(object+112u,(uint32)special_add(special_read(object+112u),special_shift(special_mul(linear_x,timestep),8u)));
    w_u32(object+120u,(uint32)special_add(special_read(object+120u),special_shift(special_mul(linear_z,timestep),8u)));
}
