#include "draft_signatures.h"
#include <stdio.h>
#include <stdlib.h>
extern uint32 sub_8001B884(uint32);
extern uint32 sub_80033554(uint32,sint32,sint32,sint32);
static sint32 region_half(uint32 a){return (sint16)r_u16(a);}
static sint32 region_pending(uint32 target,uint32 a,uint32 b,uint32 c,uint32 d)
{fprintf(stderr,"TODO Damage region %08X(%08X,%08X,%08X,%08X)\n",target,a,b,c,d);abort();return 0;}
static void region_deform(uint32 shape,uint32 data,uint32 vertices,uint32 damage)
{(void)sub_800351B8(shape,data,vertices,damage);}
static void region_part(uint32 target,uint32 object,sint32 damage)
{if(target==0x80033790u)(void)sub_80033790(object,damage);else if(target==0x80033854u)(void)sub_80033854(object,damage);else (void)region_pending(target,object,(uint32)damage,0u,0u);}
static sint32 region_test(uint32 mask,uint32 u,uint32 v,sint32 coordinate)
{return (sint32)sub_80033554(mask,(sint32)u,(sint32)v,coordinate);}
static uint32 region_random_damage(uint32 player,sint32 damage)
{return (uint32)((sub_8001B884(player)&255u)+512u)<(uint32)damage;}

/* Both original geometry streams share the same four packet layouts */
static void region_packets(uint32 mesh,uint32 mask,uint32 stage,uint32 object,uint32 alternate)
{
    uint32 stream,descriptor,packet,vertices,type,texture,replacement,index,vertex,count_vertices;
    uint32 packet_stride,descriptor_stride,uv_stride,clut_offset,sum_u,sum_v,u,v,add_u,add_v;
    sint32 count,entry,excluded,object_clut;
    replacement=alternate?r_u16(0x8007A32Cu):r_u16(0x8007A324u+stage*2u);
    excluded=region_half(0x8007A32Eu);object_clut=region_half(object+74u);
    add_u=(stage&1u)<<6;add_v=(stage&2u)<<6;
    for(stream=0u;stream<2u;++stream) {
        descriptor=mesh+r_u32(mesh+48u);packet=mesh+r_u32(mesh+52u+stream*4u);
        vertices=mesh+r_u32(mesh+44u);
        while(r_u8(descriptor+3u)) {
            count=region_half(descriptor);type=r_u8(descriptor+2u);descriptor+=4u;
            if(type>3u)continue;
            count_vertices=(type&1u)?4u:3u;
            uv_stride=type<2u?8u:12u;
            clut_offset=type<2u?22u:26u;
            packet_stride=type==0u?32u:type==3u?52u:40u;
            descriptor_stride=type<2u?20u:type==2u?44u:56u;
            for(index=0u;(sint32)index<count;++index) {
                texture=r_u16(packet+clut_offset);
                if(alternate?texture==replacement:((sint32)texture!=excluded && (sint32)texture!=object_clut)) {
                    sum_u=0u;sum_v=0u;
                    for(vertex=0u;vertex<count_vertices;++vertex) {
                        sum_u+=r_u8(packet+12u+vertex*uv_stride);
                        sum_v+=r_u8(packet+13u+vertex*uv_stride);
                    }
                    u=sum_u/count_vertices;v=sum_v/count_vertices;
                    if(alternate){u=(u&63u)<<1;v=(v&127u)<<1;}
                    entry=region_half(descriptor);
                    if(region_test(mask,u,v,region_half(vertices+(uint32)entry*8u))) {
                        if(alternate) {
                            /* Read all UV bytes before storing the replacement */
                            uint8 uv[8];
                            for(vertex=0u;vertex<count_vertices;++vertex) {
                                uv[vertex*2u]=(uint8)((r_u8(packet+12u+vertex*uv_stride)&63u)+add_u);
                                uv[vertex*2u+1u]=(uint8)((r_u8(packet+13u+vertex*uv_stride)&127u)+add_v);
                            }
                            for(vertex=0u;vertex<count_vertices;++vertex) {
                                w_u8(packet+12u+vertex*uv_stride,uv[vertex*2u]);
                                w_u8(packet+13u+vertex*uv_stride,uv[vertex*2u+1u]);
                            }
                        } else w_u16(packet+clut_offset,(uint16)replacement);
                    }
                }
                packet+=packet_stride;
            }
            descriptor+=(uint32)count*descriptor_stride;
        }
    }
}

/* Full boot 30470 with the live incoming fourth damage argument */
void sub_80030470(uint32 player,uint32 object,uint32 region,sint32 damage)
{
    uint32 mesh=r_u32(object+24u),vertices=mesh+r_u32(mesh+44u),mapped,slot,mask=0u,stage=0u;
    uint32 part,current,flags,map,alpha,bit;
    sint32 owner_count=(sint32)r_u32(0x80064C14u),limit;
    if((sint32)region<3)mapped=3u-region;
    else if((sint32)region<12)mapped=14u-region;
    else mapped=region;
    if(owner_count>=(sint32)player) {
        region_deform(r_u32(0x8007A330u+mapped*4u),r_u32(0x800650B4u),vertices,object+338u);
    } else if(r_u8(object+82u)) {
        region_deform(r_u32(0x8007A364u+mapped*4u),r_u32(0x800650B8u),vertices,object+338u);
    }
    for(slot=0u;slot<3u;++slot) {
        sint32 signed_part=region_half(0x8005AA48u+region*6u+slot*2u);
        if(signed_part==-1)continue;
        part=(uint32)signed_part;current=r_u8(object+364u+part);
        if(damage>=region_half(0x8005AAAAu+current*2u)) {
            stage=current+1u;
            while(stage<4u && damage>=region_half(0x8005AAAAu+stage*2u))++stage;
            w_u8(object+364u+part,(uint8)stage);
            mask+=1u<<(part&31u);
        }
        if(part==12u && damage>=769)region_part(0x80033790u,object,damage);
        if(part==11u && damage>=769)region_part(0x80033854u,object,damage);
        if(part==2u || part==1u) {
            if(damage>=769 || (damage>=513 && region_random_damage(player,damage)))
                w_u8(object+377u,r_u8(object+377u)&(part==2u?0xBFu:0x7Fu));
        }
        if(part==4u || part==3u) {
            if(damage>=769)w_u8(object+377u,r_u8(object+377u)&(part==4u?0xEAu:0xD5u));
            else if(damage>=513) {
                flags=r_u8(object+377u);
                for(bit=0u;bit<3u;++bit) {
                    if(region_random_damage(player,damage))flags&=~(1u<<(bit*2u+(part==4u?0u:1u)));
                }
                w_u8(object+377u,(uint8)flags);
            }
        }
        /* Original LBU map value is never equal to signed minus one */
        map=r_u8(0x8005AA98u+part);alpha=r_u8(object+379u+map);limit=255-(damage>>4);
        if((sint32)alpha<limit)limit=(sint32)alpha;
        w_u8(object+379u+map,(uint8)limit);
    }
    if(!mask)return;
    if(owner_count>=(sint32)player || r_u8(object+82u))region_packets(r_u32(object+24u),mask,stage,object,0u);
    if(owner_count<(sint32)player)region_packets(r_u32(object+28u),mask,stage,object,1u);
}
