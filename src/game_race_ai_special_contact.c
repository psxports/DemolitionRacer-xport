#include "draft_signatures.h"
extern uint32 game_800B5790(uint32 track);
extern uint32 sub_80043450(uint32 x,uint32 z);
extern void dr_race_normalize(VECTOR *input,SVECTOR *output);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
static sint32 contact_add(sint32 a,sint32 b){return(sint32)((uint32)a+(uint32)b);}
static sint32 contact_sub(sint32 a,sint32 b){return(sint32)((uint32)a-(uint32)b);}
static sint32 contact_mul(sint32 a,sint32 b){return(sint32)((uint32)a*(uint32)b);}
static sint32 contact_round(sint32 value,uint32 bits)
{
    return (value<0?contact_add(value,(sint32)((1u<<bits)-1u)):value)>>bits;
}
static void contact_edge(uint32 object,uint32 descriptor,uint32 first,uint32 second,sint32 extent)
{
    VECTOR input;SVECTOR normal;
    sint32 x,z,dx,dz,dot,angle,c,s,depth,vx,vz,tangent,approach,friction,reflected;
    input.vx=(sint16)(r_u16(second+4u)-r_u16(first+4u));input.vy=0;
    input.vz=(sint16)(r_u16(first)-r_u16(second));
    dr_race_normalize(&input,&normal);
    x=(sint16)r_u16(first);z=(sint16)r_u16(first+4u);
    dx=contact_sub(contact_sub((sint32)r_u32(object+100u)>>8,x),(sint32)r_u32(descriptor+12u));
    dz=contact_sub(contact_sub((sint32)r_u32(object+108u)>>8,z),(sint32)r_u32(descriptor+20u));
    dot=contact_sub(contact_add(contact_mul(dx,normal.vx),contact_mul(dz,normal.vz)),extent);
    if(dot>=0)return;
    angle=(sint32)sub_80043450((uint32)contact_sub((sint16)r_u16(second+4u),z),(uint32)contact_sub((sint16)r_u16(second),x));
    c=draft_agent3_missing_800437EC((uint32)angle);s=draft_agent3_missing_8004388C((uint32)angle);
    depth=contact_add(contact_add(dot,4095)>>12,-4);
    dx=contact_round(contact_mul(contact_sub(0,depth),s),4u);dz=contact_round(contact_mul(depth,c),4u);
    vx=(sint32)r_u32(object+112u);vz=(sint32)r_u32(object+120u);
    w_u32(object+100u,(uint32)contact_add((sint32)r_u32(object+100u),dx));
    w_u32(object+108u,(uint32)contact_add((sint32)r_u32(object+108u),dz));
    tangent=contact_round(contact_add(contact_mul(vx,c),contact_mul(vz,s)),12u);
    approach=contact_round(contact_sub(contact_mul(vz,c),contact_mul(vx,s)),12u);
    if(approach<0)return;
    reflected=contact_sub(0,approach)/2;
    friction=tangent>384?contact_add(tangent,-384):tangent<-384?contact_add(tangent,384):0;
    w_u32(object+112u,(uint32)contact_round(contact_sub(contact_mul(friction,c),contact_mul(reflected,s)),12u));
    w_u32(object+120u,(uint32)contact_round(contact_add(contact_mul(reflected,c),contact_mul(friction,s)),12u));
    w_u8(object+941u,0u);
}
static sint32 contact_extent(uint32 object,uint32 segment)
{
    uint32 angle=game_800B5790(segment)&2047u,model;
    sint32 c,s,width,length;
    if(angle>=1024u)angle=2047u-angle;
    c=draft_agent3_missing_800437EC(angle);s=draft_agent3_missing_8004388C(angle);
    model=r_u32(object);width=(sint16)r_u16(model+8u);length=(sint16)r_u16(model+12u);
    return contact_add(contact_mul(c,width),contact_mul(s,length));
}
void game_8009141C(uint32 car_index)
{
    uint32 object=0x80066D14u+car_index*952u,segment=(uint32)(sint32)(sint16)r_u16(object+88u);
    uint32 descriptor=r_u32(0x800653D4u)+segment*24u,count=r_u8(descriptor+3u)&15u,progress=r_u8(object+96u);
    uint32 table,vertices,first,second,kind;sint32 extent;
    if(progress<2u){
        extent=contact_extent(object,segment);
        table=r_u32(0x800653D4u)+segment*24u;vertices=r_u32(0x800653C4u);
        first=vertices+r_u16(table+6u)*6u;second=vertices+r_u16(table+4u)*6u;
        contact_edge(object,descriptor,first,second,extent);
    }
    if((sint32)progress<(sint32)count-2)return;
    descriptor=r_u32(0x800653D4u)+segment*24u;kind=r_u8(descriptor);
    extent=contact_extent(object,(uint32)(sint32)(sint16)r_u16(object+88u));
    table=r_u32(0x800653D4u)+segment*24u;vertices=r_u32(0x800653C4u);
    first=vertices+(r_u16(table+4u)+count+(uint32)(sint32)(sint8)r_u8(0x800BE970u+kind*2u))*6u;
    second=vertices+(r_u16(table+6u)+count+(uint32)(sint32)(sint8)r_u8(0x800BE971u+kind*2u))*6u;
    contact_edge(object,descriptor,first,second,extent);
}
