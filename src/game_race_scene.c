#include "draft_signatures.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

extern uint32 sub_8003B898(void);
void sub_8004171C(uint32 object,sint32 maximum);
uint32 sub_80042924(uint32 bytes,uint32 owner);
void sub_80042B3C(uint32 buffer);
void game_800AA078(uint32 records);
void game_800AB380(uint32 object);
void game_800AB0D8(uint32 object);
void game_800AE264(uint32 object);
static sint32 scene_div(sint32 first,sint32 second)
{
    if(!second || (first==INT_MIN && second==-1)) abort();
    return first/second;
}
static sint32 scene_remainder(sint32 first,sint32 second)
{
    scene_div(first,second); return first%second;
}

void game_800B8DB8(uint32 port, sint32 track, sint32 progress)
{
    sint32 sections=scene_div((sint32)(r_u32(0x800653A4u)+11u),12);
    sint32 center=scene_div((sint32)((uint32)track+6u),12);
    sint32 radius=r_u32(0x80064C14u)?2:3;
    sint32 begin=center-radius,end=center+radius,section,i,count=0;
    uint32 slot_base=0x800C7DB0u+port*12u;
    sint32 distance=(sint32)r_u32(0x800BFAD8u);
    w_u16(0x800C7DA8u+port*2u,(uint16)((uint32)progress-(uint32)distance));
    w_u16(0x800C7DACu+port*2u,(uint16)((uint32)progress+(uint32)distance));
    progress=(sint32)((uint32)progress-36u);
    if(begin<0) begin=scene_remainder(begin,sections)+sections;
    end=scene_remainder((sint32)((uint32)end+1u),sections);
    for(section=begin;section!=end;section=scene_remainder(section+1,sections)) {
        w_u32(0x800C7D68u+(uint32)count*4u,(uint32)section);++count;
    }
    for(i=0;i<(sint32)r_u32(0x800C7D60u);++i) {
        uint32 slot=slot_base+(uint32)i*24u, state=r_u8(slot+8u);
        if(state==3u) w_u8(slot+8u,0u);
        if(state==2u) {
            sint32 j,found=0;
            for(j=0;j<count;++j) if((sint16)r_u16(slot+6u)==(sint32)r_u32(0x800C7D68u+(uint32)j*4u)) ++found;
            if(!found) w_u8(slot+8u,3u);
        }
    }
    for(section=begin;section!=end;section=scene_remainder(section+1,sections),progress=(sint32)((uint32)progress+12u)) {
        sint32 free_slot=-1,present=0,selected;
        for(i=0;i<(sint32)r_u32(0x800C7D60u);++i) {
            uint32 slot=slot_base+(uint32)i*24u;
            if(!r_u8(slot+8u) && free_slot==-1) free_slot=i;
            if((sint16)r_u16(slot+6u)==section && r_u8(slot+8u)==2u) ++present;
        }
        selected=present?-1:free_slot;
        if(selected!=-1) {
            uint32 slot=slot_base+(uint32)selected*24u,source,size,buffer;
            if(r_u8(slot+8u)) continue;
            buffer=r_u32(slot);
            if(buffer) sub_80042B3C(buffer);
            source=r_u32(r_u32(0x800C7D5Cu)+(uint32)section*4u+4u);
            size=sub_80015150(source);
            sub_80042924(size,slot);
            buffer=r_u32(slot);
            if(!buffer) break;
            w_u8(slot+8u,1u);
            source=r_u32(r_u32(0x800C7D5Cu)+(uint32)section*4u+4u);
            sub_80015074(source,buffer,(sint32)size);
            w_u8(slot+8u,2u);w_u16(slot+6u,(uint16)section);w_u16(slot+4u,(uint16)track);
            game_800AA078(buffer+4u);
            {
                sint32 length=(sint32)r_u32(0x800653A4u),offset,j;
                uint32 objects=r_u32(buffer);
                if(progress<0) {
                    sint32 magnitude=(sint32)(0u-(uint32)progress);
                    offset=(sint32)(0u-((uint32)scene_div(magnitude,length)*(uint32)length+(uint32)length));
                } else offset=(sint32)((uint32)scene_div(progress,length)*(uint32)length);
                for(j=0;j<(sint32)objects;++j) {
                    uint32 object=buffer+r_u32(buffer+724u+(uint32)j*4u),axis;
                    w_u16(object+6u,(uint16)(r_u16(object+6u)+(uint32)offset));
                    for(axis=0;axis<3;++axis) w_u32(object+16u+axis*4u,(uint32)((sint32)r_u32(object+16u+axis*4u)/256));
                }
            }
        }
    }
}

void game_800B9374(uint32 port)
{
    sint32 translation[3];uint32 axis,i;
    for(axis=0;axis<3;++axis) translation[axis]=(sint32)r_u32(0x80081BB8u+axis*4u);
    w_u32(0x80065170u,0u);w_u32(0x80065134u,0u);
    for(axis=0;axis<3;++axis) w_u32(0x80081BB8u+axis*4u,(uint32)(translation[axis]>>2));
    sub_8003B898();
    for(axis=0;axis<5;++axis) xport_gte_write_control(axis,r_u32(0x80081BA4u+axis*4u));
    for(i=0;i<r_u32(0x800C7D60u) && (sint32)r_u32(0x800C7D60u)>0;++i) {
        uint32 slot=0x800C7DB0u+port*12u+i*24u,buffer=r_u32(slot),count=r_u32(buffer),j;
        if(r_u8(slot+8u)!=2u) continue;
        for(j=0;j<count && (sint32)count>0;++j) sub_8004171C(buffer+r_u32(buffer+724u+j*4u),16384);
    }
    for(axis=0;axis<3;++axis) w_u32(0x80081BB8u+axis*4u,(uint32)translation[axis]);
    w_u32(0x80065170u,2u);w_u32(0x80065134u,2u);sub_8003B898();
}

extern void sub_800416E0(uint32 packet);
static void scene_rotate_long(const sint32 input[3], sint32 output[3])
{
    sint32 high[3],low[3],rotated_high[3];uint32 axis;
    for(axis=0;axis<3;++axis) {
        uint32 magnitude=input[axis]<0?0u-(uint32)input[axis]:(uint32)input[axis];
        high[axis]=(sint32)magnitude>>15;low[axis]=(sint32)(magnitude&32767u);
        if(input[axis]<0) {high[axis]=(sint32)(0u-(uint32)high[axis]);low[axis]=-low[axis];}
        xport_gte_write_data(9u+axis,(uint32)high[axis]);
    }
    xport_gte_execute(0x41E012u);
    for(axis=0;axis<3;++axis) rotated_high[axis]=(sint32)xport_gte_read_data(25u+axis);
    for(axis=0;axis<3;++axis) xport_gte_write_data(9u+axis,(uint32)low[axis]);
    xport_gte_execute(0x49E012u);
    for(axis=0;axis<3;++axis) output[axis]=(sint32)(xport_gte_read_data(25u+axis)+((uint32)rotated_high[axis]<<3));
}

void sub_8004171C(uint32 object,sint32 maximum)
{
    sint32 input[3],output[3],radius,term,side;uint32 axis;
    for(axis=0;axis<3;++axis) input[axis]=(sint32)(r_u32(object+16u+axis*4u)-r_u32(0x80081BB8u+axis*4u));
    radius=(sint16)r_u16(object+8u)>>(r_u32(0x80065170u)&31u);
    scene_rotate_long(input,output);
    if((sint32)((uint32)output[2]+(uint32)radius)<=0 || (sint32)((uint32)output[2]-(uint32)radius)>=maximum) return;
    for(axis=0;axis<2;++axis) {
        sint32 normal=(sint16)r_u16(axis?0x80065562u:0x80065550u);
        sint32 depth=(sint16)r_u16(axis?0x80065564u:0x80065554u);
        term=(sint32)((uint32)output[2]*(uint32)depth);
        side=(sint32)((uint32)output[axis]*(uint32)normal+(uint32)term)>>12;
        if(side>=radius) return;
        side=(sint32)((0u-(uint32)output[axis])*(uint32)normal+(uint32)term)>>12;
        if(side>=radius) return;
    }
    w_u32(0x80065548u,(uint32)output[2]);sub_800416E0(object);
}

extern uint32 draft_agent3_missing_8003D9AC(uint32 mode);
uint32 sub_80042924(uint32 bytes,uint32 owner)
{
    uint32 current=r_u32(0x80065594u),units=((bytes+15u)>>4)+1u;
    do {
        uint32 flags=r_u32(current+12u),available=flags>>1;
        if(!(flags&1u) && (sint32)available>=(sint32)units) {
            uint32 allocated=current;
            if(available!=units) {
                uint32 remaining=(available-units)*2u,next=r_u32(current);
                allocated=current+remaining*8u;
                w_u32(current+12u,remaining);w_u32(allocated+4u,current);
                w_u32(allocated,next);w_u32(current,allocated);
                w_u32(r_u32(allocated)+4u,allocated);
                w_u32(allocated+12u,units*2u|1u);w_u32(allocated+8u,owner);
            } else {w_u32(current+12u,flags|1u);w_u32(current+8u,owner);}
            w_u32(0x80065594u,current);
            w_u32(0x800655A0u,r_u32(0x800655A0u)-units*16u);
            w_u32(owner,allocated+16u);w_u32(0x8006518Cu,0u);return allocated+16u;
        }
        current=r_u32(current);
    } while(current!=r_u32(0x80065594u));
    if(r_u32(0x8006518Cu)) {
        w_u32(0x8006518Cu,0u);w_u32(owner,0u);return 1u;
    }
    w_u32(0x8006518Cu,1u);w_u32(0x80065188u,r_u32(0x80065188u)+1u);
    fprintf(stderr,"%s",(const char *)psx_addr(0x800123DCu,1u));
    draft_agent3_missing_8003D9AC(0u);
    {
        uint32 destination=r_u32(0x80065590u),source=destination,previous=0u,next;
        do {
            uint32 flags=r_u32(source+12u);next=r_u32(source);
            if(flags&1u) {
                if(source!=destination) {
                    uint32 length=(flags>>1)*16u,i;
                    for(i=0;i<length;++i) w_u8(destination+i,r_u8(source+i));
                    w_u32(destination,destination+(r_u32(destination+12u)>>1)*16u);
                    w_u32(r_u32(destination+8u),destination+16u);
                }
                w_u32(destination+4u,previous);previous=destination;
                destination+=(r_u32(destination+12u)>>1)*16u;
            }
            source=next;
        } while(source!=r_u32(0x80065590u));
        w_u32(destination,source);w_u32(destination+4u,previous);
        w_u32(0x80065594u,destination);
        w_u32(destination+12u,r_u32(destination+12u)&~1u);
        w_u32(destination+12u,((r_u32(0x80065598u)-(destination-source))>>4)*2u);
        w_u32(source+4u,destination);
    }
    {uint32 result=sub_80042924(bytes,owner);w_u32(0x8006518Cu,0u);return result;}
}

void sub_80042B3C(uint32 buffer)
{
    uint32 block=buffer-16u,flags=r_u32(buffer-4u)&~1u,next=r_u32(block),previous=r_u32(block+4u);
    w_u32(block+12u,flags);w_u32(0x80065594u,previous);
    w_u32(0x800655A0u,r_u32(0x800655A0u)+(flags<<3));
    if(!(r_u32(next+12u)&1u) && block<next) {
        uint32 joined=((r_u32(block+12u)>>1)+(r_u32(next+12u)>>1))*2u;
        w_u32(block+12u,(r_u32(block+12u)&1u)|joined);
        w_u32(r_u32(next)+4u,block);w_u32(block,r_u32(r_u32(block)));
    }
    previous=r_u32(block+4u);
    if(!(r_u32(previous+12u)&1u) && previous<block) {
        uint32 joined=((r_u32(previous+12u)>>1)+(r_u32(block+12u)>>1))*2u;
        w_u32(previous+12u,joined);w_u32(r_u32(block)+4u,r_u32(block+4u));
        w_u32(r_u32(block+4u),r_u32(block));
    }
}

extern uint32 game_800AE420(uint32 track);
extern uint32 sub_8002C188(uint32 state,uint32 position);
static sint32 scene_free_object(void)
{
    uint32 i;
    for(i=0;i<30;++i) if(!r_u8(0x800C449Fu+i*300u)) return (sint32)i;
    fprintf(stderr,"%s",(const char *)psx_addr(0x8008BB60u,1u));return -1;
}

void game_800AA078(uint32 records)
{
    uint32 record,index;
    for(index=0,record=records;index<30;++index,record+=24u) {
        uint32 identity=r_u16(record),i,blocked=0u;
        sint32 free_index;
        uint32 object,kind,bytes,source,buffer;
        if(!identity) continue;
        if(r_u8(0x800C42B0u+identity)) continue;
        for(i=0;i<30;++i) if(r_u8(0x800C449Fu+i*300u) && r_u8(0x800C44A2u+i*300u)==identity) {
            fprintf(stderr, (const char *)psx_addr(0x8008BB20u,1u),identity);
            blocked=1u;break;
        }
        if(blocked || !(r_u8(0x800C42ACu)&r_u8(record+7u))) continue;
        free_index=scene_free_object();if(free_index==-1) continue;
        object=0x800C4378u+(uint32)free_index*300u;
        kind=(r_u8(record+4u)&15u)*4u;
        bytes=r_u32(0x800C426Cu+kind);
        source=r_u32(r_u32(0x800651F0u)+kind+12u);
        sub_80042924(bytes,object+120u);buffer=r_u32(object+120u);
        if(!buffer) {
            fprintf(stderr,"%s",(const char *)psx_addr(0x8008BB34u,1u));continue;
        }
        for(i=0;i<300;++i) w_u8(object+i,0u);
        w_u32(object+120u,buffer);
        w_u8(object+296u,r_u8(record+8u));w_u8(object+292u,r_u8(record+4u));
        for(i=0;i<bytes;++i) w_u8(buffer+i,r_u8(source+i));
        w_u8(object+298u,r_u16(record));
        w_u32(object+12u,r_u32(record+12u));w_u32(object+16u,r_u32(record+16u));w_u32(object+20u,r_u32(record+20u));
        w_u8(object+297u,0u);w_u8(object+295u,1u);
        w_u32(object+88u,(uint32)r_u8(record+9u)<<12);
        w_u16(object+288u,(uint16)((uint32)r_u8(record+6u)<<2));
        w_u16(object+2u,(uint16)game_800AE420((uint32)(sint32)(sint16)r_u16(record+2u)));
        w_u16(object,r_u16(record+2u));w_u16(object+4u,r_u16(object+2u));
        w_u8(object+299u,0u);w_u16(object+290u,r_u8(record+5u));
        fprintf(stderr,(const char *)psx_addr(0x8008BB4Cu,1u),(sint16)r_u16(object+2u),(sint16)r_u16(object));
        game_800AB380(object);
        w_u16(r_u32(object+120u)+8u,(uint16)r_u32(object+116u));
        {
            sint32 track=(sint16)r_u16(object+4u),length=(sint32)r_u32(0x800653A4u);
            sint32 remainder=scene_remainder(track,length);
            if(track<0) remainder=(sint32)((uint32)remainder+(uint32)length);
            w_u16(object+2u,(uint16)remainder);
        }
        game_800AB0D8(object);
        for(i=0;i<4;++i) sub_8002C188(object,object+12u);
        game_800AE264(object);
        w_u32(0x800C4268u,r_u32(0x800C4268u)+1u);
    }
}


extern uint32 dr_track_height(const uint8 *state,const sint32 *position,sint32 *height);
extern uint32 dr_track_normal(const uint8 *state,const sint32 *position,sint16 *normal);
extern sint32 draft_agent3_missing_800437EC(uint32 angle);
extern sint32 draft_agent3_missing_8004388C(uint32 angle);

void game_800AB380(uint32 object)
{
    uint32 model=r_u32(object+120u),vertices=model+r_u32(model+44u),i;
    sint32 count=(sint16)r_u16(model+14u),width=0,depth=0,top=0,bottom=0;
    for(i=0;i<(uint32)count && count>0;++i) {
        sint32 x=(sint16)r_u16(vertices+i*8u),y=(sint16)r_u16(vertices+i*8u+2u),z=(sint16)r_u16(vertices+i*8u+4u);
        if(width<x) width=x;if(width< -x) width=-x;
        if(depth<z) depth=z;if(depth< -z) depth=-z;
        if(top<y) top=y;if(y<bottom) bottom=y;
    }
    for(i=0;i<8;++i) {
        uint32 corner=object+220u+i*8u;
        w_u16(corner,(uint16)((i&1u)?width:-width));
        w_u16(corner+2u,(uint16)((i&4u)?top:bottom));
        w_u16(corner+4u,(uint16)((i&2u)?-depth:depth));
    }
    w_u32(object+116u,sub_8003B674((uint32)width*(uint32)width+(uint32)top*(uint32)top+(uint32)depth*(uint32)depth));
}

void game_800AB0D8(uint32 object)
{
    uint32 i,axis; sint32 factor=(sint32)r_u32(0x800651C4u);
    MATRIX *matrix=(MATRIX *)psx_addr(object+52u,32u);
    for(i=0;i<3;++i) {
        uint32 address=object+84u+i*4u;
        sint32 product=(sint32)(r_u32(object+104u+i*4u)*(uint32)factor);
        w_u32(address,r_u32(address)+(uint32)(product/256));
    }
    for(i=0;i<3;++i) {
        uint32 address=object+12u+i*4u;
        sint32 product=(sint32)(r_u32(object+24u+i*4u)*(uint32)factor);
        w_u32(address,r_u32(address)+(uint32)(product/256));
    }
    for(i=0;i<3;++i) {
        w_u16(object+96u+i*2u,(uint16)((sint32)r_u32(object+84u+i*4u)>>8));
        w_u32(object+72u+i*4u,(uint32)((sint32)r_u32(object+12u+i*4u)>>8));
    }
    sub_8002C188(object,object+12u);
    {
        sint32 progress=(sint16)r_u16(object+4u),length=(sint32)r_u32(0x800653A4u);
        sint32 remainder=scene_remainder(progress,length);
        if(progress<0) remainder=(sint32)((uint32)remainder+(uint32)length);
        w_u16(object+2u,(uint16)remainder);
    }
    sub_8003C960((sint16 *)matrix->m,(const sint16 *)psx_addr(object+96u,6u));
    SetRotMatrix(matrix);SetTransMatrix(matrix);
    for(i=0;i<8;++i) {
        uint32 corner=object+220u+i*8u,output=object+124u+i*12u;
        xport_gte_write_data(0u,r_u32(corner));xport_gte_write_data(1u,r_u32(corner+4u));
        xport_gte_execute(0x480012u);
        for(axis=0;axis<3;++axis) w_u32(output+axis*4u,xport_gte_read_data(25u+axis)<<8);
    }
}

void game_800AE264(uint32 object)
{
    sint32 sum=0,height,normal_x,normal_z,cosine,sine;uint32 i;
    sint16 normal[3];
    for(i=0;i<4;++i) {
        uint32 state=0x800C4218u+i*10u,position=object+124u+i*12u;
        w_u8(state+8u,r_u8(object+8u));w_u16(state,r_u16(object));
        sub_8002C188(state,position);
        dr_track_height((const uint8 *)psx_addr(state,10u),(const sint32 *)psx_addr(position,12u),(sint32 *)psx_addr(0x800C08D8u+i*4u,4u));
        height=(sint32)r_u32(0x800C08D8u+i*4u);
        sum=(sint32)((uint32)sum+(uint32)height);
    }
    w_u32(object+16u,(uint32)(sum/4)-((uint32)(sint32)(sint16)r_u16(object+222u)<<8));
    cosine=draft_agent3_missing_800437EC((uint32)(sint32)(sint16)r_u16(object+98u));
    sine=draft_agent3_missing_8004388C((uint32)(sint32)(sint16)r_u16(object+98u));
    if(!dr_track_normal((const uint8 *)psx_addr(object,10u),(const sint32 *)psx_addr(object+12u,12u),normal)) {
        /* Original normal output is undefined for unsupported track cell types */
        fprintf(stderr,"Undefined scene normal object=%08X\n",object);abort();
    }
    normal_x=(sint32)((uint32)(sint32)normal[0]*(uint32)cosine-(uint32)(sint32)normal[2]*(uint32)sine)/4096;
    normal_z=(sint32)((uint32)(sint32)normal[0]*(uint32)sine+(uint32)(sint32)normal[2]*(uint32)cosine)/4096;
    w_u32(object+84u,(uint32)(sint32)(sint16)normal_x<<8);
    w_u32(object+92u,(uint32)(sint32)(sint16)normal_z<<8);
}
