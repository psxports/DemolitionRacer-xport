#include "draft_signatures.h"

static sint32 df_read(uint32 address){return (sint32)r_u32(address);}
static sint32 df_add(sint32 a,sint32 b){return (sint32)((uint32)a+(uint32)b);}
static sint32 df_sub(sint32 a,sint32 b){return (sint32)((uint32)a-(uint32)b);}
static sint32 df_mul(sint32 a,sint32 b){return (sint32)((uint32)a*(uint32)b);}
static sint32 df_shift(sint32 value,uint32 shift)
{
    return df_add(value,value<0?(sint32)((1u<<shift)-1u):0)>>shift;
}
static sint32 df_product(sint32 a,sint32 b){return df_shift(df_mul(a,b),8u);}

/* Full GAME 9745C leaf with wrapping fixed-point arithmetic */
void game_8009745C(uint32 object,uint32 model,sint32 force_x,sint32 force_z)
{
    sint32 stiffness=(sint16)r_u16(model+94u);
    sint32 damping=(sint16)r_u16(model+96u);
    sint32 coupling=(sint16)r_u16(model+98u);
    sint32 limit=(sint16)r_u16(model+100u);
    sint32 auxiliary=(sint16)r_u16(model+102u);
    sint32 negative_limit=(sint32)(0u-(uint32)limit);
    sint32 velocity,position,projection,x,z,timestep;
    uint32 index,axis,sample;
    for(index=0u;index<4u;++index) {
        axis=object+index*4u;sample=object+760u+index*16u;
        x=df_sub(df_read(sample),df_shift(df_read(object+100u),8u));
        z=df_sub(df_read(sample+8u),df_shift(df_read(object+108u),8u));
        projection=df_shift(df_add(df_mul(force_x,x),df_mul(force_z,z)),8u);
        velocity=df_add(df_read(axis+856u),df_product(projection,coupling));
        velocity=df_add(velocity,df_product(df_read(axis+872u),auxiliary));
        velocity=df_sub(velocity,df_add(df_product(df_read(axis+840u),stiffness),df_product(velocity,damping)));
        if((uint32)velocity+15u<31u)velocity=0;
        timestep=df_read(0x800651C4u);
        w_u32(axis+856u,(uint32)velocity);
        position=df_add(df_read(axis+840u),df_product(velocity,timestep));
        w_u32(axis+840u,(uint32)position);
        if(position>limit){w_u32(axis+840u,(uint32)limit);w_u32(axis+856u,0u);}
        position=df_read(axis+840u);
        if(position<negative_limit){w_u32(axis+840u,(uint32)negative_limit);w_u32(axis+856u,0u);}
    }
    x=df_shift(df_add(df_read(object+760u),df_read(object+776u)),1u);
    x=df_sub(x,df_shift(df_read(object+100u),8u));
    z=df_shift(df_add(df_read(object+768u),df_read(object+784u)),1u);
    z=df_sub(z,df_shift(df_read(object+108u),8u));
    projection=df_shift(df_add(df_mul(force_x,x),df_mul(force_z,z)),8u);
    velocity=df_add(df_read(object+836u),df_product(projection,coupling));
    velocity=df_sub(velocity,df_add(df_product(df_read(object+832u),stiffness),df_product(velocity,damping)));
    timestep=df_read(0x800651C4u);
    w_u32(object+836u,(uint32)velocity);
    position=df_add(df_read(object+832u),df_product(velocity,timestep));
    w_u32(object+832u,(uint32)position);
    if(position>limit){w_u32(object+832u,(uint32)limit);w_u32(object+836u,0u);}
    position=df_read(object+832u);
    if(position<negative_limit){w_u32(object+832u,(uint32)negative_limit);w_u32(object+836u,0u);}
}
