#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
extern void game_800AE184(uint32 object,sint32 impulse,uint32 flags);
typedef struct PairObjectContact {sint16 x,z,offset_x,offset_z;} PairObjectContact;
static sint32 po_add(sint32 a,sint32 b){return(sint32)((uint32)a+(uint32)b);}
static sint32 po_sub(sint32 a,sint32 b){return(sint32)((uint32)a-(uint32)b);}
static sint32 po_step(sint32 value,sint32 scale)
{
    sint32 product=(sint32)((uint32)(value/2)*(uint32)scale);
    return product<0?po_add(product,255)>>8:product>>8;
}
static uint32 po_geometry(uint32 player,uint32 object,const VECTOR positions[2],const sint32 angles[2],PairObjectContact contacts[8])
{
    /* TODO Translate ACEFC with native position, angle and contact arrays */
    (void)positions;(void)angles;(void)contacts;
    fprintf(stderr,"TODO GAME ACEFC(%08X,%08X,native arrays)\n",player,object);abort();
}
static void po_response(uint32 target,uint32 player,uint32 object,const PairObjectContact *contact,sint32 angle,sint32 fraction)
{
    /* TODO Translate the selected five-argument object response */
    (void)contact;(void)angle;(void)fraction;
    fprintf(stderr,"TODO GAME %08X(%08X,%08X,native contact,angle,fraction)\n",target,player,object);abort();
}
static void po_collect(uint32 player,uint32 type,uint32 position)
{
    fprintf(stderr,"TODO boot 80015780(%08X,%08X,%08X)\n",player,type,position);abort();
}
static void po_effect_native(uint32 model,uint32 angles,const VECTOR *velocity,sint32 flags)
{
    /* TODO Adapt the real BC824 engine to a native velocity input */
    (void)velocity;
    fprintf(stderr,"TODO native BC824(%08X,%08X,velocity,%08X)\n",model,angles,(uint32)flags);abort();
}
void game_800AD59C(uint32 player_index,uint32 bucket_object_index)
{
    uint32 first_bounds=0x800C03D0u+player_index*20u,second_bounds=0x800C03D0u+bucket_object_index*20u;
    uint32 player,object,mask,type,i,iteration;
    VECTOR positions[2];sint32 angles[2],delta[2][3],fraction=128,step=128,scale;
    PairObjectContact contacts[8];sint32 distance,extent,high;
    if((sint32)r_u32(first_bounds)>=(sint32)r_u32(second_bounds+8u)||
       (sint32)r_u32(second_bounds)>=(sint32)r_u32(first_bounds+8u)||
       (sint32)r_u32(first_bounds+4u)>=(sint32)r_u32(second_bounds+12u)||
       (sint32)r_u32(second_bounds+4u)>=(sint32)r_u32(first_bounds+12u))return;
    player=0x80066D14u+player_index*952u;
    positions[0].vx=(sint32)r_u32(player+100u);positions[0].vy=(sint32)r_u32(player+104u);positions[0].vz=(sint32)r_u32(player+108u);
    object=0x800C4378u+(bucket_object_index-r_u32(0x800C08D0u))*300u;
    positions[1].vx=(sint32)r_u32(object+12u);positions[1].vy=(sint32)r_u32(object+16u);positions[1].vz=(sint32)r_u32(object+20u);
    distance=po_sub(positions[1].vy,positions[0].vy)>>8;
    distance=(sint32)dr_race_motion_sqrt((uint32)distance*(uint32)distance);
    extent=po_add((sint16)r_u16(r_u32(player)+128u),(sint16)r_u16(r_u32(object+120u)+8u));
    high=(sint32)(((sint64)extent*0x55555556)>>32);
    if(po_sub(high,extent>>31)<distance)return;
    angles[0]=(sint32)r_u32(player+128u);angles[1]=(sint32)r_u32(object+88u);
    mask=po_geometry(player,object,positions,angles,contacts);if(!mask)return;
    type=r_u8(object+292u)>>4;
    w_u8(0x800C42B0u+r_u8(object+298u),1u);
    if(type>=6u){
        VECTOR velocity;uint32 model;
        velocity.vx=(sint32)r_u32(player+112u);velocity.vy=po_add((sint32)r_u32(player+116u),10000);velocity.vz=(sint32)r_u32(player+120u);
        po_collect(player_index,type,object+12u);
        model=r_u32(object+120u);
        w_u32(model+16u,r_u32(object+12u));w_u32(model+20u,r_u32(object+16u));w_u32(model+24u,r_u32(object+20u));
        po_effect_native(r_u32(object+120u),object+96u,&velocity,-500);
        if(!r_u8(object+299u))w_u8(object+299u,4u);
        game_800AE184(object,32,0u);return;
    }
    w_u8(object+297u,1u);scale=(sint32)r_u32(0x800651C4u);
    delta[0][0]=po_step((sint32)r_u32(player+112u),scale);delta[0][1]=po_step((sint32)r_u32(player+120u),scale);
    delta[1][0]=po_step((sint32)r_u32(object+24u),scale);delta[1][1]=po_step((sint32)r_u32(object+32u),scale);
    delta[0][2]=po_step((sint32)r_u32(player+140u),scale);delta[1][2]=po_step((sint32)r_u32(object+108u),scale);
    for(i=0;i<2u;++i){positions[i].vx=po_sub(positions[i].vx,delta[i][0]);positions[i].vz=po_sub(positions[i].vz,delta[i][1]);angles[i]=po_sub(angles[i],delta[i][2]);}
    for(iteration=0;iteration<7u;++iteration){
        uint32 current;step/=2;
        for(i=0;i<2u;++i){delta[i][0]/=2;delta[i][1]/=2;delta[i][2]/=2;}
        current=po_geometry(player,object,positions,angles,contacts);
        if(current){fraction-=step;mask=current;}else fraction+=step;
        for(i=0;i<2u;++i){sint32 dx=delta[i][0],dz=delta[i][1],da=delta[i][2];
            positions[i].vx=current?po_sub(positions[i].vx,dx):po_add(positions[i].vx,dx);
            positions[i].vz=current?po_sub(positions[i].vz,dz):po_add(positions[i].vz,dz);
            angles[i]=current?po_sub(angles[i],da):po_add(angles[i],da);}
    }
    fraction-=16;
    switch(mask){
    case 1u:case 2u:case 4u:case 8u:
        for(i=0;!(mask&(1u<<i));++i){}
        po_response(0x800AC1F0u,player,object,&contacts[i],angles[0]>>8,fraction);return;
    case 16u:case 32u:case 64u:case 128u:
        for(i=4;!(mask&(1u<<i));++i){}
        po_response(0x800AB4F8u,player,object,&contacts[i],angles[1]>>8,fraction);return;
    default:break;
    }
    for(i=0;i<4u;++i)if(mask&(1u<<i))po_response(0x800AC1F0u,player,object,&contacts[i],angles[0]>>8,fraction);
    for(i=4;i<8u;++i)if(mask&(1u<<i))po_response(0x800AB4F8u,player,object,&contacts[i],angles[1]>>8,fraction);
}
