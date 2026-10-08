#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
extern uint32 sub_80016340(uint32);
extern uint32 sub_8001ED2C(uint32,uint32,uint32,sint32,uint32,uint32);
extern uint32 sub_8001685C(uint32);
extern sint32 draft_agent3_missing_800437EC(uint32);
extern sint32 draft_agent3_missing_8004388C(uint32);
extern void dr_driving_matrix_expand(uint32*,const sint16*);
extern uint32 sub_8003B674(uint32);

static sint32 dc_add(sint32 a,sint32 b){return (sint32)((uint32)a+(uint32)b);}
static sint32 dc_sub(sint32 a,sint32 b){return (sint32)((uint32)a-(uint32)b);}
static sint32 dc_mul(sint32 a,sint32 b){return (sint32)((uint32)a*(uint32)b);}
static sint32 dc_neg(sint32 a){return (sint32)(0u-(uint32)a);}
static sint32 dc_shift(sint32 a,uint32 n){return (sint32)((uint32)a+(a<0?(1u<<n)-1u:0u))>>n;}
static sint32 dc_product(sint32 a,sint32 b,uint32 n){return dc_shift(dc_mul(a,b),n);}
static sint32 dc_div(sint32 a,sint32 b){if(!b)xport_mips_break(7u);if(a==INT_MIN&&b==-1)xport_mips_break(6u);return a/b;}
static sint32 dc_high(sint32 a,sint32 b){return (sint32)(((int64_t)a*b)>>32);}
static sint32 dc_magic(sint32 a,uint32 shift){return dc_sub(dc_high(a,0x0C907DA5)>>shift,a>>31);}
static sint32 dc_rpm_div(sint32 a){return dc_sub(dc_add(dc_high(a,(sint32)0xB60B60B7u),a)>>5,a>>31);}
static sint32 dc_read(uint32 a){return (sint32)r_u32(a);}
static sint32 dc_half(uint32 a){return (sint16)r_u16(a);}
static uint32 dc_sqrt(uint32 input)
{
    uint32 leading,even,index,shift;
    xport_gte_write_data(30u,input);leading=xport_gte_read_data(31u);
    if(leading==32u)return 0u;
    even=leading&~1u;shift=(uint32)((31-(sint32)even)>>1)&31u;
    index=(sint32)even>=24?input<<((even-24u)&31u):(uint32)((sint32)input>>((24u-even)&31u));
    return ((uint32)(sint32)(sint16)r_u16(0x800602C0u+(index-64u)*2u)<<shift)>>12;
}
static void dc_gears(uint32 object,uint32 model)
{
    sint32 throttle=dc_half(object+16u),rpm,gear;
    uint32 sound,value;
    if(throttle<0){w_u8(object+18u,0u);return;}
    gear=r_u8(object+18u);
    if(!gear && throttle<=0)return;
    rpm=dc_half(object+12u);
    if(!gear)w_u8(object+18u,2u);
    if(dc_half(model+62u+(uint32)gear*2u)<rpm && gear<8 && dc_read(object+888u)>0){
        w_u8(object+18u,r_u8(object+18u)+1u);sound=29u;
    }else if(rpm<dc_half(model+78u+(uint32)gear*2u) && gear>=3){
        w_u8(object+18u,r_u8(object+18u)-1u);sound=30u;
    }else return;
    if(dc_read(0x80064C14u)<(sint32)r_u8(object+937u))return;
    if((randPSX()&7u)!=0u)return;
    value=(uint32)randPSX();
    sub_8001ED2C(sound,0xFFFFFFFFu,4096u,
        (sint32)(value-400u*(uint32)((dc_high((sint32)value,0x51EB851F)>>7)-((sint32)value>>31))+3800u),object+100u,0u);
}

void game_80099F74(uint32 object)
{
    uint32 x=(uint32)(dc_half(object+40u)+2048)&4095u,z=(uint32)(dc_half(object+44u)+2048)&4095u;
    SVECTOR delta,angles;MATRIX rotation,transpose;uint32 i,j; sint32 scale;
    if(x-1195u<1707u && z-1116u<1865u)return;
    scale=dc_read(0x800651C4u);
    delta.vx=(sint16)(dc_sub(dc_read(object+124u),dc_product(dc_read(object+136u),scale,8u))>>8);
    delta.vy=(sint16)(dc_sub(dc_read(object+128u),dc_product(dc_read(object+140u),scale,8u))>>8);
    delta.vz=(sint16)(dc_sub(dc_read(object+132u),dc_product(dc_read(object+144u),scale,8u))>>8);
    dr_driving_quaternion(&delta,&angles);dr_driving_rotation(&angles,&rotation);
    for(i=0;i<3u;++i)for(j=0;j<3u;++j)transpose.m[i][j]=rotation.m[j][i];
    dr_driving_matrix(object+148u,&transpose);
    dr_driving_matrix_expand((uint32*)psx_addr(object+216u,36u),(const sint16*)&transpose);
    dr_driving_matrix_expand((uint32*)psx_addr(object+180u,36u),(const sint16*)psx_addr(object+148u,18u));
    w_u8(object+940u,1u);w_u16(object+906u,0u);game_80095D6C(object);
}

void game_80097AA0(uint32 object)
{
    uint32 model=r_u32(object+4u),angle; sint32 value,scale;
    sub_80016340(object);game_8009745C(object,model,0,0);
    angle=(uint32)dc_neg(dc_half(object+42u));
    w_u8(object+938u,0u);w_u32(object+896u,0u);w_u32(object+900u,0u);
    /* Original rotated local vector has no subsequent reader */
    (void)draft_agent3_missing_800437EC(angle);(void)draft_agent3_missing_8004388C(angle);
    value=dc_read(object+136u);w_u32(object+136u,(uint32)dc_sub(value,dc_shift(value,4u)));
    value=dc_read(object+144u);w_u32(object+144u,(uint32)dc_sub(value,dc_shift(value,4u)));
    scale=dc_read(0x800651C4u);
    w_u16(object+68u,(uint16)(r_u16(object+68u)+dc_product(dc_read(object+892u)>>8,scale,8u)));
    w_u16(object+70u,(uint16)(r_u16(object+70u)+dc_product(dc_read(object+888u)>>8,scale,8u)));
}

static sint32 dc_brake(sint32 limit,sint32 speed)
{
    sint32 lower=dc_neg(limit),opposite=dc_neg(speed),result=limit;
    if(opposite<lower)lower=opposite;
    if(limit<lower)result=lower;
    return dc_shift(result,1u);
}

static sint32 dc_engine(uint32 object,uint32 model,sint32 speed,uint32 disabled_gear)
{
    sint32 rpm=dc_half(object+12u),target,step=200,velocity;
    uint32 gear=r_u8(object+18u);
    if(gear==disabled_gear)velocity=0;
    else velocity=(sint32)((uint32)dc_div(dc_rpm_div((sint32)((uint32)dc_sub(rpm,400)<<12)),dc_half(model+46u+gear*2u))<<8);
    if(dc_half(object+16u)>0 && gear==2u) {
        sint32 value=dc_shift(speed,6u);
        target=dc_sub(dc_add(dc_half(model+114u),value<0?0:value),1800);
    } else {if(r_u8(object+19u))step*=2;step*=2;target=0;}
    if(target<rpm) {rpm=dc_sub(rpm,step);if(rpm<target)rpm=target;}
    else if(rpm<dc_sub(target,step*4))rpm=dc_add(rpm,step);
    else rpm=dc_add(rpm,dc_shift(dc_sub(target,rpm),2u));
    w_u16(object+12u,(uint16)rpm);return velocity;
}

static void dc_slip(uint32 object,sint32 wheel_speed,uint32 command_address,sint32 coefficient,
    sint32 lateral,sint32 *limit,sint32 *longitudinal)
{
    sint32 command=dc_read(command_address),difference,slip,scaled,norm,reduced;
    if(wheel_speed>=0) {difference=dc_sub(command,wheel_speed);if(difference<0)difference=dc_sub(wheel_speed,command);}
    else difference=command<0?dc_neg(command):command;
    slip=dc_shift(difference,8u);
    if(slip<65)w_u8(object+938u,0u);
    w_u16(object+908u,(uint16)slip);
    scaled=dc_product(slip,coefficient,8u);reduced=dc_shift(lateral,8u);
    norm=dc_add((sint32)dc_sqrt((uint32)dc_add(dc_product(reduced,lateral,8u),dc_mul(scaled,scaled))),1);
    *limit=dc_div(dc_mul(*limit,reduced<0?dc_neg(reduced):reduced),norm);
    *longitudinal=dc_div(dc_mul(dc_shift(*longitudinal,1u),scaled),norm);
}

static sint32 dc_force_limit(sint32 limit,sint32 *lateral,sint32 longitudinal)
{
    sint32 a=dc_shift(*lateral,4u),b=dc_shift(longitudinal,4u);
    sint32 magnitude=(sint32)(dc_sqrt((uint32)dc_add(dc_mul(a,a),dc_mul(b,b)))<<4);
    if(limit<magnitude) {
        sint32 ratio=dc_div((sint32)((uint32)limit<<8),magnitude);
        *lateral=dc_product(*lateral,ratio,8u);return dc_sub(magnitude,limit);
    }
    return 0;
}

void game_80097EC4(uint32 object)
{
    uint32 model=r_u32(object+4u),surface[5],i,drive,gear;
    sint32 timestep,drag,vx,vz,yawrate,yaw,steering,throttle,front,rear,axle_sum,inertia,height,slip_coefficient;
    sint32 cy,sy,ct,st,cs,ss,lateral,longitudinal,wheel_rear,wheel_front;
    sint32 front_weight,rear_weight,grip[4],force[4]={0,0,0,0};
    sint32 sum_rear,sum_front,denominator,coupling,front_lateral,rear_lateral,limit,loss_rear,loss_front;
    sint32 first,second,third,fourth,rotational_inertia,rotational_acceleration,world_x,world_z;
    const uint32 offsets[5]={88u,398u,408u,418u,428u};
    for(i=0;i<5u;++i)surface[i]=sub_8001685C(object+offsets[i]);
    /* Original 16330 restores its stack and returns without side effects */
    w_u8(object+932u,(uint8)surface[0]);
    drag=dc_half(0x800BE8A0u+surface[0]*2u)*256;
    drag=dc_add(drag,dc_half(model+((dc_half(object+16u)<32 || r_u8(object+18u)<2u)?108u:106u)));
    timestep=dc_read(0x800651C4u);
    vx=dc_read(object+112u);
    vx=dc_add(vx,dc_product(dc_product(dc_shift(dc_neg(vx),8u),drag,12u),timestep,8u));
    w_u32(object+112u,(uint32)vx);
    vz=dc_read(object+120u);
    vz=dc_add(vz,dc_product(dc_product(dc_shift(dc_neg(vz),8u),drag,12u),timestep,8u));
    w_u32(object+120u,(uint32)vz);
    steering=dc_shift(dc_read(object+8u),8u);yaw=dc_read(object+128u)>>8;
    yawrate=dc_read(object+140u);throttle=dc_half(object+16u);
    first=vx>>8;second=vz>>8;
    w_u16(object+904u,(uint16)dc_sqrt((uint32)dc_add(dc_mul(first,first),dc_mul(second,second))));
    front=dc_half(model+40u);rear=dc_half(model+42u);axle_sum=dc_add(front,rear);
    inertia=dc_read(model+32u);height=dc_half(model+44u);drive=(uint32)dc_half(model+118u);
    slip_coefficient=dc_half(model+124u);
    cy=draft_agent3_missing_800437EC((uint32)yaw);sy=draft_agent3_missing_8004388C((uint32)yaw);
    ct=draft_agent3_missing_800437EC((uint32)dc_add(yaw,steering));st=draft_agent3_missing_8004388C((uint32)dc_add(yaw,steering));
    cs=draft_agent3_missing_800437EC((uint32)steering);ss=draft_agent3_missing_8004388C((uint32)steering);
    lateral=dc_shift(dc_sub(dc_mul(vx,cy),dc_mul(vz,sy)),12u);
    longitudinal=dc_shift(dc_add(dc_mul(vx,sy),dc_mul(vz,cy)),12u);
    first=dc_read(model+36u);second=dc_read(object+832u);
    front_weight=dc_div(dc_mul(dc_div((sint32)((uint32)rear<<8),axle_sum),dc_sub(first,second)),(sint32)((uint32)first<<1));
    rear_weight=dc_div(dc_mul(dc_div((sint32)((uint32)front<<8),axle_sum),dc_add(first,second)),(sint32)((uint32)first<<1));
    if(front_weight<56)front_weight=56;if(front_weight>80)front_weight=80;
    if(rear_weight<56)rear_weight=56;if(rear_weight>80)rear_weight=80;
    for(i=0;i<4u;++i)grip[i]=dc_product(i<2u?front_weight:rear_weight,dc_half(0x800BE860u+surface[i+1u]*2u),8u);
    if(r_u8(object+20u)){grip[2]=dc_product(grip[2],dc_half(model+122u),8u);grip[3]=dc_product(grip[3],dc_half(model+122u),8u);}
    wheel_rear=dc_shift(dc_add(dc_mul(vx,sy),dc_mul(vz,cy)),12u);
    wheel_front=dc_sub(dc_shift(dc_add(dc_mul(vx,st),dc_mul(vz,ct)),12u),dc_magic(dc_mul(dc_mul(yawrate,front),ss),17u));
    if(r_u8(object+938u)) {
        if(drive==1u) {
            w_u32(object+892u,(uint32)wheel_front);
            first=dc_engine(object,model,wheel_rear,1u);w_u32(object+888u,(uint32)first);
        } else if(drive==2u) {
            first=dc_engine(object,model,wheel_front,1u);w_u32(object+892u,(uint32)first);w_u32(object+888u,(uint32)wheel_rear);
        } else if(drive==3u) {
            first=dc_engine(object,model,dc_shift(dc_add(wheel_front,wheel_rear),1u),1u);
            w_u32(object+892u,(uint32)first);w_u32(object+888u,(uint32)first);
        }
        if(r_u8(object+19u)) {
            force[0]=force[1]=dc_brake(dc_product(dc_half(model+110u),throttle,8u),wheel_front);
        } else if(drive==1u) {
            first=game_8001675C(object,model);force[2]=force[3]=dc_shift(first,1u);
        } else if(drive==2u) {
            first=game_8001675C(object,model);force[0]=force[1]=dc_shift(first,1u);
        } else if(drive==3u) {
            first=game_8001675C(object,model);for(i=0;i<4u;++i)force[i]=dc_shift(first,2u);
        }
    } else {
        w_u32(object+892u,(uint32)wheel_front);w_u32(object+888u,(uint32)wheel_rear);
        if(r_u8(object+19u) || !throttle) {
            if(!throttle)throttle=-32;
            game_80016404(object,model);
            first=dc_product(dc_half(model+110u),throttle,8u);second=dc_product(dc_half(model+112u),throttle,8u);
            if(r_u8(object+20u))force[2]=dc_brake(second,dc_shift(wheel_rear,1u));
            else {
                force[0]=force[1]=dc_brake(first,dc_shift(wheel_front,1u));
                force[2]=force[3]=dc_brake(second,dc_shift(wheel_rear,1u));
            }
        } else {
            dc_gears(object,model);game_80016404(object,model);
            limit=(sint32)((uint32)dc_half(model+116u)<<((r_u8(object+950u)&1u)?9u:8u));
            if(dc_read(object+888u)<limit) {
                first=game_8001675C(object,model);
                if(drive==1u)force[2]=force[3]=dc_shift(first,1u);
                else if(drive==2u)force[0]=force[1]=dc_shift(first,1u);
                else if(drive==3u)for(i=0;i<4u;++i)force[i]=dc_shift(first,2u);
            }
        }
    }
    for(i=0;i<4u;++i)force[i]=dc_product(force[i],dc_half(0x800BE860u+surface[i+1u]*2u),8u);
    sum_front=dc_add(force[0],force[1]);sum_rear=dc_add(force[2],force[3]);
    first=dc_product(dc_product(dc_shift(dc_add(inertia,dc_mul(rear,rear)),10u),ss,12u),ss,12u);
    second=dc_product(dc_product(dc_shift(dc_mul(axle_sum,axle_sum),10u),cs,12u),cs,12u);
    denominator=dc_add(first,second);coupling=dc_shift(dc_sub(dc_mul(rear,front),inertia),10u);
    first=dc_product(dc_product(dc_product(coupling,sum_front,12u),ss,12u),ss,12u);
    second=dc_product(dc_product(dc_shift(inertia,10u),dc_sub(dc_magic(dc_mul(rear,yawrate),5u),lateral),12u),ss,12u);
    first=dc_add(first,second);
    second=dc_product(dc_product(coupling,dc_add(dc_add(dc_product(sum_front,cs,12u),sum_rear),longitudinal),12u),cs,12u);
    first=dc_mul(dc_add(first,second),ss);
    rotational_inertia=dc_mul(yawrate,dc_magic(inertia,5u));
    second=dc_product(dc_product(dc_shift(dc_sub(rotational_inertia,dc_mul(front,lateral)),10u),axle_sum,12u),cs,12u);
    second=dc_mul(second,cs);
    front_lateral=dc_div(dc_add(first,second),denominator);
    first=dc_product(dc_shift(dc_neg(dc_add(dc_mul(rear,lateral),rotational_inertia)),10u),axle_sum,12u);
    first=dc_mul(first,cs);
    second=dc_product(dc_shift(dc_add(dc_sub(dc_mul(dc_mul(rear,2),front),inertia),dc_mul(front,front)),10u),sum_front,12u);
    second=dc_mul(second,cs);
    third=dc_mul(dc_shift(dc_add(inertia,dc_mul(rear,rear)),10u),dc_add(longitudinal,sum_rear));
    second=dc_mul(dc_shift(dc_add(second,third),12u),ss);
    rear_lateral=dc_div(dc_add(first,second),denominator);
    limit=dc_product(height,dc_add(grip[2],grip[3]),8u);w_u16(object+908u,0u);
    if(r_u8(object+938u)&1u)dc_slip(object,wheel_rear,object+888u,slip_coefficient,front_lateral,&limit,&sum_rear);
    loss_rear=dc_force_limit(limit,&front_lateral,sum_rear);w_u32(object+896u,(uint32)loss_rear);
    limit=dc_product(height,dc_add(grip[0],grip[1]),8u);
    if(r_u8(object+938u)&2u)dc_slip(object,wheel_front,object+892u,slip_coefficient,rear_lateral,&limit,&sum_front);
    loss_front=dc_force_limit(limit,&rear_lateral,sum_front);w_u32(object+900u,(uint32)loss_front);
    first=dc_mul(rear_lateral,dc_product(front,cs,12u));second=dc_mul(front_lateral,rear);
    third=dc_mul(dc_sub(dc_add(force[1],force[3]),dc_add(force[0],force[2])),500);
    rotational_acceleration=dc_div(dc_add(dc_sub(first,second),third),dc_magic(inertia,5u));
    if(rotational_acceleration>1400)rotational_acceleration=1400;if(rotational_acceleration<-1400)rotational_acceleration=-1400;
    world_x=dc_add(dc_add(dc_product(front_lateral,cy,12u),dc_product(rear_lateral,ct,12u)),
        dc_add(dc_product(sum_rear,sy,12u),dc_product(sum_front,st,12u)));
    world_z=dc_add(dc_add(dc_product(dc_neg(front_lateral),sy,12u),dc_product(dc_neg(rear_lateral),st,12u)),
        dc_add(dc_product(sum_rear,cy,12u),dc_product(sum_front,ct,12u)));
    timestep=dc_read(0x800651C4u);
    w_u32(object+140u,(uint32)dc_add(dc_read(object+140u),dc_product(rotational_acceleration,timestep,8u)));
    w_u32(object+112u,(uint32)dc_add(dc_read(object+112u),dc_product(world_x,timestep,8u)));
    w_u32(object+120u,(uint32)dc_add(dc_read(object+120u),dc_product(world_z,timestep,8u)));
    w_u16(object+68u,(uint16)(r_u16(object+68u)+dc_product(dc_read(object+892u)>>8,timestep,8u)));
    if(!r_u8(object+20u))w_u16(object+70u,(uint16)(r_u16(object+70u)+dc_product(dc_read(object+888u)>>8,timestep,8u)));
    if(!steering) {
        first=dc_read(object+140u);
        if(first<0){first=dc_add(first,128);if(first>0)first=0;}
        else {first=dc_sub(first,128);if(first<0)first=0;}
        w_u32(object+140u,(uint32)first);
    }
    if(loss_rear>0) {
        first=dc_read(object+140u);
        second=dc_product(dc_product(dc_shift(first,6u),dc_read(0x800651C4u),8u),loss_rear,15u);
        if(second>512)second=512;if(second<-512)second=-512;
        w_u32(object+140u,(uint32)dc_sub(first,second));
    }
    gear=r_u8(object+18u);
    if(gear==2u) {
        first=dc_div(dc_rpm_div((sint32)((uint32)dc_sub(dc_half(object+12u),400)<<12)),dc_half(model+50u));
        first=dc_sub((sint32)((uint32)first<<8),wheel_rear);
        if(first>76800 && dc_half(object+16u)>=128)w_u8(object+938u,(uint8)r_u16(model+118u));
    }
    if(r_u32(object+112u)+63u<127u && r_u32(object+120u)+63u<127u && r_u32(object+140u)+31u<63u) {
        w_u32(object+112u,0u);w_u32(object+120u,0u);w_u32(object+140u,0u);
    }
    fourth=world_z;
    game_8009745C(object,model,world_x,fourth);
    game_80097C44(object);
}
