#include "draft_signatures.h"

extern void sub_80035458(const sint16 *first,const sint16 *second,sint16 *output);

static sint32 pair_vertical_absolute(sint16 value)
{
    return value < 0 ? -(sint32)value : (sint32)value;
}

static void pair_vertical_rotate(uint32 object,const sint16 *input,sint16 *output)
{
    sint16 conjugate[4];
    conjugate[0]=(sint16)(0u-r_u16(object+252u));
    conjugate[1]=(sint16)(0u-r_u16(object+254u));
    conjugate[2]=(sint16)(0u-r_u16(object+256u));
    conjugate[3]=(sint16)r_u16(object+258u);
    output[0]=input[0];output[1]=input[1];output[2]=input[2];output[3]=0;
    sub_80035458(conjugate,output,output);
    sub_80035458(output,(const sint16*)psx_addr(object+252u,8u),output);
}

uint32 game_80094DF0(uint32 upper_object,uint32 lower_object,const VECTOR *delta)
{
    uint32 model=r_u32(lower_object);
    sint32 width=(sint16)r_u16(model+40u),length=(sint16)r_u16(model+44u);
    sint16 input[4],rotated[4];
    width+=width>>1;length+=length>>1;
    if((r_u8(upper_object+378u)&1u)!=0u || (r_u8(lower_object+378u)&1u)!=0u)
        return 0u;
    if((sint32)r_u32(upper_object+116u)>=-32766)
        return 0u;
    input[0]=(sint16)delta->vx;input[1]=0;input[2]=(sint16)delta->vz;input[3]=0;
    pair_vertical_rotate(upper_object,input,rotated);
    if(width<pair_vertical_absolute(rotated[0]) || length<pair_vertical_absolute(rotated[2]))
        return 0u;
    pair_vertical_rotate(lower_object,input,rotated);
    if(width<pair_vertical_absolute(rotated[0]) || length<pair_vertical_absolute(rotated[2]))
        return 0u;
    (void)sub_800302F8(r_u8(lower_object+937u),lower_object);
    (void)sub_8001532C(r_u8(upper_object+937u),12u);
    return 1u;
}
