#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define R32(a) (*(uint32*)psx_addr((a),4u))
#define R16(a) (*(uint16*)psx_addr((a),2u))
#define R8(a) (*(uint8*)psx_addr((a),1u))
extern uint32 sub_80043450(uint32 z,uint32 x);
extern void game_8009D5F0(uint32 angles,uint32 output);
extern uint32 sub_8002C188(uint32 state,uint32 position);
extern uint32 sub_8002E408(uint32 state,uint32 position,uint32 normal,uint32 flags);
extern uint32 sub_8002F374(uint32 state,uint32 position);
extern uint32 game_800B5790(uint32 track);
extern uint32 game_8009A114(uint32 object);
extern uint32 game_800B5B7C(uint32 order);
extern void dr_race_normalize_vector(VECTOR *input,VECTOR *output);
void dr_driving_matrix_expand(uint32 *output,const sint16 *input)
{ uint32 i;for(i=0u;i<9u;++i)output[i]=(uint32)(sint32)input[i]<<18; }
static void driving_matrix_narrow(sint16 *output,const uint32 *input)
{uint32 i;for(i=0u;i<9u;++i)output[i]=(sint16)((sint32)(input[i]+0x20000u)>>18);}
static uint32 driving_product(uint32 a,uint32 b)
{
    uint64 product=(uint64)((sint64)(sint32)a*(sint64)(sint32)b);
    uint32 low=(uint32)product,high=(uint32)(product>>32),rounded=low+0x40000000u;
    high+=(rounded<0x40000000u);return (high<<2)|(rounded>>30);
}
static void driving_compose(const uint32 *a,const uint32 *b,uint32 *output)
{uint32 row,col;for(row=0u;row<3u;++row)for(col=0u;col<3u;++col){uint32 x=driving_product(a[row*3u],b[col]),y=driving_product(a[row*3u+1u],b[3u+col]),z=driving_product(a[row*3u+2u],b[6u+col]);output[row*3u+col]=x+y+z;}}
static void driving_angles(const sint16 *matrix,sint16 *angles)
{
    uint32 value=(uint32)(sint32)matrix[3]*(uint32)(sint32)matrix[3]+(uint32)(sint32)matrix[4]*(uint32)(sint32)matrix[4];
    uint32 root=sub_8003B674(value),angle=sub_80043450(0u-(uint32)(sint32)matrix[5],root);
    angles[0]=(sint16)angle;
    if((angle&0x7FFu)!=1024u){angles[1]=(sint16)sub_80043450((uint32)(sint32)matrix[2],(uint32)(sint32)matrix[8]);angles[2]=(sint16)sub_80043450((uint32)(sint32)matrix[3],(uint32)(sint32)matrix[4]);}
    else {angles[1]=(sint16)sub_80043450(0u-(uint32)(sint32)matrix[6],(uint32)(sint32)matrix[0]);angles[2]=0;}
}
static uint32 driving_pending(uint32 target,uint32 a,uint32 b,uint32 c,uint32 d,uint32 e)
{fprintf(stderr,"TODO GAME driving %08X(%08X,%08X,%08X,%08X,%08X)\n",target,a,b,c,d,e);abort();return 0u;}
void game_80096CA0(uint32 object)
{
    sint16 matrix[9],angles[3];uint32 axis;
    driving_angles((const sint16*)psx_addr(object+148u,18u),(sint16*)psx_addr(object+40u,6u));
    game_8009D5F0(object+40u,object+252u);
    driving_matrix_narrow(matrix,(const uint32*)psx_addr(object+216u,36u));
    driving_angles(matrix,angles);
    for(axis=0u;axis<3u;++axis)R32(object+136u+axis*4u)=(uint32)(sint32)angles[axis];
    if((R32(0x80064A30u)&3u)==0u){
        sub_8003C960((sint16*)psx_addr(object+148u,18u),(const sint16*)psx_addr(object+40u,6u));
        dr_driving_matrix_expand((uint32*)psx_addr(object+180u,36u),(const sint16*)psx_addr(object+148u,18u));
        sub_8003C960(matrix,angles);dr_driving_matrix_expand((uint32*)psx_addr(object+216u,36u),matrix);
    }
}
void game_80096E10(uint32 object)
{
    uint32 old,value,temp[9],result,mode,axis;
    old=R32(object+112u);value=old-(uint32)((sint32)old/256);R32(object+112u)=value;R32(object+100u)+=value;
    old=R32(object+116u);value=old-3000u-(uint32)((sint32)old/256);R32(object+116u)=value;R32(object+104u)+=value;
    old=R32(object+120u);value=old-(uint32)((sint32)old/256);R32(object+120u)=value;R32(object+108u)+=value;
    driving_compose((const uint32*)psx_addr(object+216u,36u),(const uint32*)psx_addr(object+180u,36u),temp);
    memcpy(psx_addr(object+180u,36u),temp,36u);
    driving_matrix_narrow((sint16*)psx_addr(object+148u,18u),temp);
    sub_8002C188(object+88u,object+100u);result=sub_8002E408(object+88u,object+100u,object+752u,0u);
    R32(object+824u)=result;
    for(axis=0u;axis<3u;++axis)R32(object+168u+axis*4u)=(uint32)((sint32)R32(object+100u+axis*4u)>>8);
    result=game_80095D6C(object);
    if(result)game_800964B0(object,result);
    if((sint16)R16(object+906u)<90)return;
    if(R32(0x800593A0u)!=0u){game_8009A114(object);return;}
    sub_8002F374(object+88u,object+100u);
    value=game_800B5790((uint32)(sint32)(sint16)R16(object+88u))<<8;R32(object+128u)=value;
    mode=R32(0x80059738u);
    if(mode==2u){if(R8(object+937u)==0u)R32(object+128u)=value+0x80000u;if(R8(object+937u)==1u&&R32(0x8005973Cu)==mode)R32(object+128u)+=0x80000u;}
    game_8009A114(object);game_800B5B7C(R8(object+937u));
    if(R8(0x80059A28u+R8(object+937u))==0u){R8(object+942u)=15u;R32(object+104u)+=0x7D000u;}
}
static void driving_callback(uint32 target,uint32 object)
{
    switch(target){
    case 0x8009A8E8u:game_8009A8E8(object);return;
    case 0x80096D84u:game_80096D84(object);return;
    default:driving_pending(target,object,0u,0u,0u,0u);return;
    }
}
void game_80091FDC(uint32 player,uint32 callback,uint32 wheels)
{
    uint32 object=0x80066D14u+player*952u,index,alternate;
    R8(object+941u)=0u;alternate=R32(0x800593A0u);
    for(index=0u;index<8u;++index){
        sint32 wheel=(sint8)R8(wheels+index);uint32 record,point,current,other,hit;
        VECTOR normal;sint32 ax,az,bx,bz;uint32 distance,angle,record_offset_x=0u,record_offset_z=0u;
        if(wheel<0)return;
        point=object+480u+(uint32)wheel*12u;
        if(!alternate){
            uint32 descriptor=object+398u+(uint32)wheel*10u;
            sint32 segment=(sint16)R16(descriptor);uint32 count,progress,table;
            record=R32(0x800653D4u)+(uint32)segment*24u;count=R8(record+3u)&15u;progress=R8(descriptor+8u);
            table=R32(0x800653C4u);
            if(progress==0u){current=table+R16(record+6u)*6u;other=table+R16(record+4u)*6u;hit=1u;}
            else{
                uint32 shape,first,second;
                if((sint32)progress<(sint32)(count-1u))continue;
                shape=R8(record)*2u;first=R16(record+4u)+count+(uint32)(sint32)(sint8)R8(0x800BE970u+shape);
                second=R16(record+6u)+count+(uint32)(sint32)(sint8)R8(0x800BE971u+shape);
                current=table+first*6u;other=table+second*6u;hit=2u;
            }
            ax=(sint16)R16(current);az=(sint16)R16(current+4u);bx=(sint16)R16(other);bz=(sint16)R16(other+4u);
        }else{
            sint32 segment=(sint16)R16(object+398u+(uint32)wheel*10u);
            other=R32(0x800653ACu)+(uint32)segment*16u;current=other+16u;hit=1u;
            ax=(sint32)R32(current);az=(sint32)R32(current+8u);bx=(sint32)R32(other);bz=(sint32)R32(other+8u);
        }
        normal.vx=(sint32)((uint32)bz-(uint32)az);normal.vy=0;normal.vz=(sint32)((uint32)ax-(uint32)bx);normal.pad=0;
        dr_race_normalize_vector(&normal,&normal);
        if(!alternate){
            ax=(sint16)R16(current);az=(sint16)R16(current+4u);
            /* The original reloads translated geometry after normalization */
            record_offset_x=R32(record+12u);record_offset_z=R32(record+20u);
        }else{ax=(sint32)R32(current);az=(sint32)R32(current+8u);}
        distance=((uint32)((sint32)R32(point)>>8)-(uint32)ax-record_offset_x)*(uint32)normal.vx;
        distance+=((uint32)((sint32)R32(point+8u)>>8)-(uint32)az-record_offset_z)*(uint32)normal.vz;
        if((sint32)distance>=0)continue;
        if(!alternate){bz=(sint16)R16(other+4u);bx=(sint16)R16(other);}
        else{bz=(sint32)R32(other+8u);bx=(sint32)R32(other);}
        angle=sub_80043450((uint32)bz-(uint32)az,(uint32)bx-(uint32)ax);
        game_80091A6C(object,point,angle,(sint32)(distance+4095u)>>12,index&3u);
        driving_callback(callback,object);R8(object+941u)=(uint8)hit;
    }
}
