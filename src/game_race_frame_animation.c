#include "draft_signatures.h"
#define R32(a) (*(uint32*)psx_addr((a),4u))
#define R16(a) (*(uint16*)psx_addr((a),2u))
#define R8(a) (*(uint8*)psx_addr((a),1u))
void game_800A81F0(uint32 packet,uint32 frames)
{
    uint32 frame=R32(frames)+R8(frames+9u)*28u,texture;
    uint32 color,uv0,uv1;uint16 uv2,uv3;uint8 length;
    if(R32(0x800C4200u)!=(uint32)(sint32)(sint16)R16(frame+10u)){
        texture=sub_80039ADC(frame);R32(frame+12u)=texture;
    }else texture=R32(frame+12u);
    R8(packet+3u)=9u;R8(packet+7u)=0x2Cu;
    R16(packet+22u)=(uint16)((R16(texture+8u)&0xFF9Fu)|((R8(frame+26u)&3u)<<5));
    R16(packet+14u)=R16(texture+10u);
    R8(packet+12u)=R8(texture+24u);R8(packet+13u)=R8(texture+25u);
    R8(packet+20u)=(uint8)(R8(texture+22u)+R8(texture+24u)+255u);R8(packet+21u)=R8(texture+25u);
    R8(packet+28u)=R8(texture+24u);R8(packet+29u)=(uint8)(R8(texture+23u)+R8(texture+25u)+255u);
    R8(packet+36u)=(uint8)(R8(texture+22u)+R8(texture+24u)+255u);
    R8(packet+37u)=(uint8)(R8(texture+23u)+R8(texture+25u)+255u);
    R8(packet+4u)=R8(frames+4u);R8(packet+5u)=R8(frames+5u);
    color=(R8(packet+7u)|2u)&0xFEu;
    R8(packet+6u)=R8(frames+6u);R8(packet+7u)=(uint8)color;
    length=R8(packet+3u);color=R32(packet+4u);uv0=R32(packet+12u);uv1=R32(packet+20u);uv2=R16(packet+28u);uv3=R16(packet+36u);
    R8(packet+43u)=length;R32(packet+44u)=color;R32(packet+52u)=uv0;R32(packet+60u)=uv1;R16(packet+68u)=uv2;R16(packet+76u)=uv3;
}
static sint16 animation_coordinate(uint32 value,uint32 camera)
{
    if((sint32)value<0)value+=255u;
    return (sint16)((uint32)((sint32)value>>8)-camera);
}
void game_800A8DD8(uint32 object)
{
    uint32 packet=object+12u+R32(0x80065128u)*20u,axis,slot,head;
    SVECTOR first,second;sint32 screen,flags,depth;
    sint16 a[3],b[3];
    for(axis=0u;axis<3u;++axis)a[axis]=animation_coordinate(R32(object+52u+axis*4u),R32(0x80081BB8u+axis*4u));
    first.vx=a[0];first.vy=a[1];first.vz=a[2];first.pad=0;
    gte_project_full_depth(&first,&screen,&flags);
    for(axis=0u;axis<3u;++axis)b[axis]=animation_coordinate(R32(object+64u+axis*4u),R32(0x80081BB8u+axis*4u));
    second.vx=b[0];second.vy=b[1];second.vz=b[2];second.pad=0;
    if((uint32)flags&0x01C66000u)return;
    R32(packet+8u)=(uint32)screen;
    depth=gte_project_full_depth(&second,&screen,&flags);
    if((uint32)flags&0x01C66000u)return;
    R32(packet+16u)=(uint32)screen;
    depth>>=2;
    if(depth<=0||depth>=(sint32)R32(0x800654F0u))return;
    slot=R32(R32(0x80065118u)+112u)+(uint32)depth*4u;
    head=R32(slot);R32(packet)=(R32(packet)&0xFF000000u)|(head&0xFFFFFFu);
    slot=R32(R32(0x80065118u)+112u)+(uint32)depth*4u;
    R32(slot)=(R32(slot)&0xFF000000u)|(packet&0xFFFFFFu);
}
