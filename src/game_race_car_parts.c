#include "draft_signatures.h"
#define PART_GP 0x80064A10u
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);
extern void draft_agent3_missing_80049B74(const MATRIX *left,MATRIX *right);
static void part_vertex(uint32 slot,sint16 x,sint16 y,sint16 z)
{
    xport_gte_write_data(slot*2u,(uint32)(uint16)x|((uint32)(uint16)y<<16));xport_gte_write_data(slot*2u+1u,(uint16)z);
}
static void part_guest_vertex(uint32 slot,uint32 address)
{
    xport_gte_write_data(slot*2u,r_u32(address));xport_gte_write_data(slot*2u+1u,r_u32(address+4u));
}
static void part_link(uint32 packet,uint32 depth)
{
    uint32 ot=r_u32(r_u32(0x80065118u)+112u)+depth*4u;
    w_u32(packet,(r_u32(packet)&0xFF000000u)|(r_u32(ot)&0xFFFFFFu));w_u32(ot,(r_u32(ot)&0xFF000000u)|(packet&0xFFFFFFu));
}
static void part_quad(uint32 packet,const uint32 xy[4],const uint32 z[4])
{
    uint32 i,sum=z[0]+z[1]+z[2]+z[3];
    for(i=0u;i<4u;++i)w_u32(packet+8u+i*8u,xy[i]);
    part_link(packet,(uint32)((sint32)sum>>((r_u32(0x80065134u)+2u)&31u)));
}
static void part_surface(uint32 player,uint32 side,uint32 sine,uint32 cosine)
{
    uint32 base=side?0x8007A3B8u:0x8007A398u,extension=side?0x8007A3E8u:0x8007A3D8u;
    uint32 width=(uint32)(sint32)(sint16)r_u16(side?0x80066C42u:0x80066C50u);
    uint32 y=r_u16(base+2u),z=r_u16(base+4u),dy=(uint32)((sint32)(cosine*width)>>12),dz=(uint32)((sint32)(sine*width)>>12);
    uint32 near_y=y+(uint32)((sint32)(sine*width)>>15),near_z=side?z-(uint32)((sint32)(cosine*width)>>15):z+(uint32)((sint32)(cosine*width)>>15);
    uint32 addresses[4]={base,extension,base+16u,extension+8u},xy[4],depth[4],i;
    uint32 packet=0x80078F24u+player*160u+r_u32(0x80065128u)*2560u+(side?40u:0u);
    w_u16(base+18u,(uint16)(y+dy));w_u16(base+26u,(uint16)(y+dy));
    w_u16(base+20u,(uint16)(side?z+dz:z-dz));w_u16(base+28u,(uint16)(side?z+dz:z-dz));
    w_u16(extension+2u,(uint16)near_y);w_u16(extension+4u,(uint16)near_z);
    w_u16(extension+10u,(uint16)(near_y+dy));w_u16(extension+12u,(uint16)(side?near_z+dz:near_z-dz));
    for(i=0u;i<4u;++i){part_guest_vertex(0u,addresses[i]);xport_gte_execute(0x180001u);xy[i]=xport_gte_read_data(14u);depth[i]=xport_gte_read_data(19u);}
    part_quad(packet,xy,depth);
    part_guest_vertex(0u,base+8u);xport_gte_execute(0x180001u);xy[0]=xport_gte_read_data(14u);depth[0]=xport_gte_read_data(19u);
    part_guest_vertex(0u,base+24u);xport_gte_execute(0x180001u);xy[2]=xport_gte_read_data(14u);depth[2]=xport_gte_read_data(19u);
    {uint32 reverse[4]={xy[1],xy[0],xy[3],xy[2]};part_quad(packet+80u,reverse,depth);}
}
void sub_800340F0(uint32 player,uint32 object)
{
    uint32 side;
    if(!(r_u8(object+378u)&0x14u))return;
    for(side=0u;side<2u;++side){
        sint32 angle,limit,minimum;uint32 sine,cosine,model,i;
        if(!(r_u8(object+378u)&(side?16u:4u)))continue;
        angle=side?(sint16)r_u16(object+326u):-(sint16)r_u16(object+328u);
        limit=(sint16)r_u16(object+(side?330u:332u));minimum=(sint16)r_u16(side?0x80066C46u:0x80066C54u);
        if(limit<angle)angle=limit;if(angle<minimum)angle=minimum;
        angle=(sint32)((uint32)angle+(uint32)(sint32)(sint16)r_u16(side?0x80066C44u:0x80066C52u));
        sine=(uint32)draft_agent3_missing_800437EC((uint32)angle);cosine=(uint32)draft_agent3_missing_8004388C((uint32)angle);
        model=r_u32(object+(side?36u:32u));
        if(model){
            sint16 vertex[3];MATRIX matrix={0};uint32 rotated[3],a=(uint32)((sint16)r_u16(side?0x80066C42u:0x80066C50u)/2),b=(uint32)((sint16)r_u16(side?0x80066C40u:0x80066C4Eu)/2);
            vertex[0]=(sint16)r_u16(PART_GP+(side?0x9D0u:0x9C8u));
            vertex[1]=(sint16)(r_u16(PART_GP+(side?0x9D2u:0x9CAu))+(uint32)((sint32)(cosine*a+sine*b)>>12));
            vertex[2]=(sint16)(r_u16(PART_GP+(side?0x9D4u:0x9CCu))+(side?1u:0xFFFFFFFFu)*(uint32)((sint32)(sine*a-cosine*b)>>12));
            for(i=0u;i<5u;++i)xport_gte_write_control(i,r_u32(object+148u+i*4u));
            part_vertex(0u,vertex[0],vertex[1],vertex[2]);xport_gte_execute(0x486012u);
            for(i=0u;i<3u;++i)rotated[i]=xport_gte_read_data(25u+i);
            for(i=0u;i<3u;++i)w_u32(model+16u+i*4u,r_u32(object+100u+i*4u)+(rotated[i]<<8));
            if(sub_8003D188(model)){
                matrix.m[0][0]=4096;matrix.m[1][1]=(sint16)sine;matrix.m[1][2]=(sint16)(side?cosine:0u-cosine);
                matrix.m[2][1]=(sint16)(side?0u-cosine:cosine);matrix.m[2][2]=(sint16)sine;
                draft_agent3_missing_80049B74((const MATRIX*)psx_addr(object+148u,sizeof(MATRIX)),&matrix);
                sub_8003CED8(&matrix,model);sub_800416E0(model);
            }
        }else part_surface(player,side,sine,cosine);
    }
}
void sub_80019304(uint32 player)
{
    uint32 object=0x80066D14u+player*952u,data=r_u32(object),angle=0u-((uint32)(sint32)(sint16)r_u16(object+68u)<<2);
    uint32 radius=(uint32)(sint32)(sint16)r_u16(data+130u),sine,cosine,i,j,frame,texture,packet;
    sint16 width=(sint16)r_u16(data+132u),vertices[4][3];uint32 centers[4][3];
    sine=(uint32)((sint32)((uint32)draft_agent3_missing_800437EC(angle)*radius)/4096);
    cosine=(uint32)((sint32)((uint32)draft_agent3_missing_8004388C(angle)*radius)/4096);
    frame=(uint32)((sint32)r_u32(object+892u)>>8);if((sint32)frame<0)frame=0u-frame;frame=(uint32)((sint32)frame/64);
    texture=(sint32)frame<4?0x800739D4u+frame*6u:0x800739E6u;
    packet=0x800711D4u+r_u32(0x80065128u)*5120u+player*320u;
    for(i=0u;i<4u;++i){part_guest_vertex(0u,object+624u+i*8u);xport_gte_execute(0x480012u);for(j=0u;j<3u;++j)centers[i][j]=xport_gte_read_data(25u+j);}
    vertices[0][0]=(sint16)(0u-(uint32)(sint32)width);vertices[0][1]=(sint16)cosine;vertices[0][2]=(sint16)sine;
    vertices[1][0]=vertices[0][0];vertices[1][1]=(sint16)sine;vertices[1][2]=(sint16)(0u-cosine);
    vertices[2][0]=vertices[0][0];vertices[2][1]=(sint16)(0u-sine);vertices[2][2]=(sint16)cosine;
    vertices[3][0]=vertices[0][0];vertices[3][1]=(sint16)(0u-cosine);vertices[3][2]=(sint16)(0u-sine);
    for(i=0u;i<4u;++i,packet+=80u){
        uint32 index=r_u32(0x80059A58u+i*4u),u=r_u8(texture+4u),v=r_u8(texture+5u),clip,depth;
        SVECTOR projected[3];sint32 screen[3],project_depth[3],flag;
        for(j=0u;j<3u;++j)xport_gte_write_control(5u+j,centers[index][j]);
        if(i==2u){
            for(j=0u;j<4u;++j)vertices[j][0]=width;
            vertices[1][1]=(sint16)(0u-sine);vertices[1][2]=(sint16)cosine;
            vertices[2][1]=(sint16)sine;vertices[2][2]=(sint16)(0u-cosine);
        }
        for(j=0u;j<4u;++j){w_u8(packet+12u+j*8u,(uint8)(u+((j&1u)?31u:0u)));w_u8(packet+13u+j*8u,(uint8)(v+(j>=2u?31u:0u)));}
        for(j=0u;j<3u;++j){projected[j].vx=vertices[j][0];projected[j].vy=vertices[j][1];projected[j].vz=vertices[j][2];projected[j].pad=0;} gte_project3_full_depth(projected,screen,project_depth,&flag);w_u16(packet+22u,r_u16(texture));w_u16(packet+14u,r_u16(texture+2u));
        for(j=0u;j<3u;++j)w_u32(packet+8u+j*8u,xport_gte_read_data(12u+j));
        if((uint32)flag&0x1C66000u)continue;
        xport_gte_execute(0x1400006u);clip=xport_gte_read_data(24u);
        part_vertex(0u,vertices[3][0],vertices[3][1],vertices[3][2]);xport_gte_execute(0x180001u);w_u32(packet+32u,xport_gte_read_data(14u));
        xport_gte_execute(0x168002Eu);depth=xport_gte_read_data(7u);
        if((sint32)clip>=0){uint32 limit=r_u32(0x800654F0u)-1u;depth+=32u;if((sint32)limit<(sint32)depth)depth=limit;part_link(packet,depth);w_u32(packet+4u,0x2C000000u);}
        else{
            xport_gte_write_data(6u,r_u32(0x80064C64u));part_guest_vertex(0u,0x80059A38u+index*8u);xport_gte_execute(0xE80413u);
            depth-=32u;if((sint32)depth<0)depth=0u;part_link(packet,depth);w_u32(packet+4u,xport_gte_read_data(22u));
        }
    }
}


