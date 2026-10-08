#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>
extern void dr_race_normalize_vector(VECTOR*,VECTOR*);
extern uint32 sub_8003B674(uint32);
extern uint32 sub_8001ED2C(uint32,uint32,uint32,sint32,uint32,uint32);

static sint32 rec_add(sint32 a,sint32 b){return (sint32)((uint32)a+(uint32)b);}
static sint32 rec_sub(sint32 a,sint32 b){return (sint32)((uint32)a-(uint32)b);}
static sint32 rec_mul(sint32 a,sint32 b){return (sint32)((uint32)a*(uint32)b);}
static sint32 rec_neg(sint32 a){return (sint32)(0u-(uint32)a);}
static sint32 rec_read(uint32 a){return (sint32)r_u32(a);}
static sint32 rec_half(uint32 a){return (sint16)r_u16(a);}
static sint32 rec_shift(sint32 a,uint32 n){return rec_add(a,a<0?(sint32)((1u<<n)-1u):0)>>n;}
static sint32 rec_div(sint32 a,sint32 b)
{if(!b)xport_mips_break(7u);if(a==INT_MIN&&b==-1)xport_mips_break(6u);return a/b;}
static void rec_pending(uint32 target)
{fprintf(stderr,"TODO Driving recovery native adapter %08X\n",target);abort();}
static void rec_transform(uint32 matrix,const VECTOR*input,VECTOR*output)
{(void)matrix;(void)input;(void)output;rec_pending(0x8003B0ECu);}
static sint32 rec_sine(sint32 angle){(void)angle;rec_pending(0x8003AB90u);return 0;}
static sint32 rec_cosine(sint32 angle){(void)angle;rec_pending(0x8003AC64u);return 0;}
static void rec_compose(const uint32*a,const uint32*b,uint32*out)
{(void)a;(void)b;(void)out;rec_pending(0x8003AE88u);}
static void rec_cross(const VECTOR*a,const VECTOR*b,VECTOR*out)
{(void)a;(void)b;(void)out;rec_pending(0x80049998u);}
static void rec_axis_rotation(uint32*out,const VECTOR*axis,sint32 angle)
{(void)out;(void)axis;(void)angle;rec_pending(0x8003B1E8u);}

/* Full GAME 964B0 recovery with native matrix and vector locals */
void game_800964B0(uint32 object,uint32 contact_mask)
{
    VECTOR contact,relative,axis;
    uint32 basis[9],rotation[9],temporary[9],index,component;
    sint32 count=0,height=0,difference,length,normal_speed,impulse,angle;
    sint32 cosine,sine,inverse,along_x,along_z,saved_y,correction;
    contact.vx=0;contact.vy=0;contact.vz=0;
    for(index=0u;index<8u;++index) {
        uint32 sample=object+480u+index*12u;
        if(!(contact_mask&(1u<<index)))continue;
        contact.vx=rec_add(contact.vx,rec_sub(rec_read(sample),rec_read(object+100u))>>4);
        contact.vy=rec_add(contact.vy,rec_sub(rec_read(sample+4u),rec_read(object+104u))>>4);
        contact.vz=rec_add(contact.vz,rec_sub(rec_read(sample+8u),rec_read(object+108u))>>4);
        difference=rec_sub(rec_read(0x800C08D8u+index*4u),rec_read(sample+4u));
        if(height<difference)height=difference;
        ++count;
    }
    contact.vx=rec_div(contact.vx,count);contact.vy=rec_div(contact.vy,count);contact.vz=rec_div(contact.vz,count);
    rec_transform(object+216u,&contact,&relative);
    relative.vx=rec_add(relative.vx,rec_sub(rec_read(object+112u)>>4,contact.vx));
    relative.vy=rec_add(relative.vy,rec_sub(rec_read(object+116u)>>4,contact.vy));
    relative.vz=rec_add(relative.vz,rec_sub(rec_read(object+120u)>>4,contact.vz));
    length=(sint32)sub_8003B674((uint32)rec_add(rec_mul(contact.vx,contact.vx),rec_mul(contact.vz,contact.vz)));
    normal_speed=rec_neg(rec_add(rec_add(rec_mul(rec_half(object+752u),relative.vx),rec_mul(rec_half(object+754u),relative.vy)),rec_mul(rec_half(object+756u),relative.vz)))>>12;
    if(normal_speed<0) {
        if(normal_speed<-4000) {
            sub_8001ED2C((randPSX()&1u)+19u,~0u,1024u,4096,object+100u,0u);
            game_8009082C(r_u8(object+937u),1024u);
        }
        if(length>128) {
            impulse=rec_div(rec_mul(normal_speed,-212500),rec_add(rec_shift(rec_mul(length,length),8u),250000));
            angle=rec_shift(rec_mul(rec_shift(rec_mul(length,impulse),4u),854),4u);
            cosine=rec_cosine(angle);sine=rec_sine(angle);inverse=rec_div(0x40000000,length);
            along_z=rec_mul(contact.vz,inverse);along_x=rec_mul(contact.vx,inverse);
            basis[0]=(uint32)along_z;basis[1]=0u;basis[2]=(uint32)along_x;
            basis[3]=0u;basis[4]=0x40000000u;basis[5]=0u;
            basis[6]=(uint32)rec_neg(along_x);basis[7]=0u;basis[8]=(uint32)along_z;
            rotation[0]=0x40000000u;rotation[1]=0u;rotation[2]=0u;
            rotation[3]=0u;rotation[4]=(uint32)cosine;rotation[5]=(uint32)sine;
            rotation[6]=0u;rotation[7]=(uint32)rec_neg(sine);rotation[8]=(uint32)cosine;
            rec_compose(basis,rotation,temporary);
            basis[2]=(uint32)rec_neg(along_x);basis[6]=(uint32)along_x;
            rec_compose(temporary,basis,rotation);
            rec_compose(rotation,(const uint32*)psx_addr(object+216u,36u),temporary);
        } else {
            impulse=rec_shift(rec_mul(rec_neg(normal_speed),272),8u);
            memcpy(temporary,psx_addr(object+216u,36u),36u);
        }
        length=(sint32)sub_8003B674((uint32)rec_add(rec_mul(relative.vx,relative.vx),rec_mul(relative.vz,relative.vz)));
        if(length>0) {
            correction=rec_shift(rec_add(normal_speed,1024),5u);
            saved_y=contact.vy;contact.vx>>=1;contact.vy>>=1;contact.vz>>=1;
            dr_race_normalize_vector(&contact,&contact);
            relative.vy=0;dr_race_normalize_vector(&relative,&relative);
            rec_cross(&relative,&contact,&axis);
            if(length<correction)correction=length;
            angle=rec_shift(rec_mul(rec_shift(rec_mul(saved_y,correction),4u),854),4u);
            rec_axis_rotation(basis,&axis,angle);
            rec_compose(temporary,basis,(uint32*)psx_addr(object+216u,36u));
            w_u32(object+112u,(uint32)rec_sub(rec_read(object+112u),rec_shift(rec_mul(correction,relative.vx),8u)));
            w_u32(object+120u,(uint32)rec_sub(rec_read(object+120u),rec_shift(rec_mul(correction,relative.vz),8u)));
        } else memcpy(psx_addr(object+216u,36u),temporary,36u);
        for(component=0u;component<3u;++component) {
            uint32 velocity=object+112u+component*4u;
            difference=rec_shift(rec_mul(rec_half(object+752u+component*2u),impulse),8u);
            w_u32(velocity,(uint32)rec_sub(rec_read(velocity),difference));
        }
    }
    w_u32(object+104u,(uint32)rec_add(rec_read(object+104u),height));
    /* TODO Full GAME 962CC object orientation update */
    rec_pending(0x800962CCu);
    for(component=0u;component<3u;++component)w_u32(object+168u+component*4u,(uint32)(rec_read(object+100u+component*4u)>>8));
    if(normal_speed<-2000)sub_8001ED2C(10u,~0u,4096u,4096,object+100u,0u);
}
