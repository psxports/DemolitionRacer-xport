#include "draft_signatures.h"
#include <stdint.h>
#include <stdlib.h>
#define AUDIO_GP 0x80064A10u
extern uint32 draft_agent3_missing_80049F84(uint32 enabled,uint32 voices);
extern uint32 draft_agent3_missing_8001FBE4(uint32 voice,sint32 frequency,uint32 pan,sint32 volume);
uint32 sub_8001ED2C(uint32 sound,uint32 voice,uint32 mode,sint32 frequency,uint32 position,uint32 velocity);
uint32 sub_8001FF14(uint32 depth)
{
    SpuReverbAttr attr = {0};
    attr.depth.left = (sint16)depth;
    attr.depth.right = (sint16)depth;
    SpuSetReverbDepth(&attr);
    w_u16(0x8005FF18u,(uint16)depth);
    w_u16(0x8005FF1Au,(uint16)depth);
    return 0u;
}
uint32 sub_8001E2E8(uint32 voice)
{
    return draft_agent3_missing_80049F84(0u,1u << (voice & 31u));
}
static uint32 audio_reverb_record(uint32 player,uint32 record)
{
    sint32 track=(sint16)r_u16(0x80066D6Cu+player*952u);
    uint32 i;
    if (track >= (sint16)r_u16(record) && track <= (sint16)r_u16(record+2u)) return record;
    record=r_u32(0x800651E4u)+2600u;
    for(i=0;i<30u;++i,record+=6u)
        if(track >= (sint16)r_u16(record) && track <= (sint16)r_u16(record+2u)) return record;
    return r_u32(0x800651E4u)+2600u;
}
uint32 sub_8001DEDC(uint32 player)
{
    uint32 record,depth;
    if(r_u32(0x80064C14u) || r_u32(0x800593A0u)) return sub_8001FF14(0u);
    record=audio_reverb_record(player,r_u32(0x800651E4u)+2600u);
    depth=(uint32)(sint32)(sint16)r_u16(record+4u);
    w_u32(AUDIO_GP+0x888u,record); w_u32(AUDIO_GP+0x88Cu,depth);
    return sub_8001FF14(depth);
}
uint32 sub_8001DFBC(uint32 player)
{
    uint32 record,current,target,delta;
    if(r_u32(0x80064C14u)) return sub_8001FF14(0u);
    if(r_u32(0x800593A0u)) return player<<4;
    record=audio_reverb_record(player,r_u32(AUDIO_GP+0x888u));
    w_u32(AUDIO_GP+0x888u,record);
    current=r_u32(AUDIO_GP+0x88Cu); target=(uint32)(sint32)(sint16)r_u16(record+4u);
    if(target!=current) {
        delta=r_u32(0x8006708Cu+player*952u);
        if((sint32)delta<0) delta=0u-delta;
        delta=(uint32)((sint32)delta>>6);
        if((sint32)delta<100) delta=100u;
        if((sint32)target<(sint32)current) { current-=delta; if((sint32)current<=(sint32)target) current=target; }
        else { current+=delta; if((sint32)current>=(sint32)target) current=target; }
        w_u32(AUDIO_GP+0x88Cu,current);
    }
    return sub_8001FF14(current);
}
static uint32 audio_abs(uint32 value) { return (sint32)value<0 ? 0u-value:value; }
static uint32 audio_distance(uint32 x,uint32 z,uint32 camera,uint32 strict)
{
    uint32 dx=audio_abs(r_u32(camera)-x),dz=audio_abs(r_u32(camera+8u)-z);
    if(strict ? ((sint32)dx<32767 && (sint32)dz<32767) : ((sint32)dx<=32767 && (sint32)dz<=32767)) return dx*dx+dz*dz;
    return 0x7FFFFFFFu;
}
static sint32 audio_div(uint32 numerator,sint32 denominator)
{
    if(!denominator || (denominator==-1 && numerator==0x80000000u)) abort();
    return (sint32)numerator/denominator;
}
uint32 sub_8001D1DC(void)
{
    uint32 distances[16],indices[16],player,slot=0u,mode=r_u32(0x80064C14u),pass,hazard,maximum=0u,soundmode;
    if(mode && !r_u32(AUDIO_GP+0x308u)) return 0u;
    for(player=mode+1u;(sint32)player<(sint32)r_u32(0x80065254u);++player,++slot) {
        uint32 object=0x80066D14u+player*952u;
        uint32 x=(uint32)((sint32)r_u32(object+100u)>>8),z=(uint32)((sint32)r_u32(object+108u)>>8);
        indices[slot]=player;
        distances[slot]=audio_distance(x,z,r_u32(0x80064C14u)?0x80073DC4u:0x80081BB8u,0u);
        if(r_u32(0x80064C14u)) {
            uint32 dx=audio_abs(r_u32(0x80073DE4u)-x),dz=audio_abs(r_u32(0x80073DECu)-z);
            if((sint32)dx<32767 && (sint32)dz<32767) { uint32 d=dx*dx+dz*dz; if((sint32)d<(sint32)distances[slot]) distances[slot]=d; }
        }
    }
    for(pass=0u;(sint32)pass<(sint32)r_u32(0x80065254u);++pass) {
        uint32 limit=r_u32(0x80065254u)-(mode+2u),j;
        if((sint32)limit>1) {
            limit=r_u32(0x80065254u)-(r_u32(0x80064C14u)+2u);
            for(j=1u;(sint32)j<(sint32)limit;++j) if((sint32)distances[j+1u]<(sint32)distances[j]) {
                uint32 t=distances[j]; distances[j]=distances[j+1u]; distances[j+1u]=t;
                t=indices[j]; indices[j]=indices[j+1u]; indices[j+1u]=t;
            }
        }
    }
    hazard=((r_u8(0x80066E8Eu)|r_u8(0x80067246u))&2u)?1u:0u;
    if(hazard!=r_u8(AUDIO_GP+0x313u)) {
        if(hazard) sub_8001ED2C(21u,11u,4096u,4096,0x80066D78u,0x80066D84u); else sub_8001E2E8(11u);
        w_u8(AUDIO_GP+0x313u,(uint8)hazard);
    }
    for(player=0u;(sint32)player<=(sint32)r_u32(0x80064C14u);++player) {
        uint32 offset=player*952u,voice=player*32u;
        sint32 speed=(sint16)r_u16(0x80066D20u+offset),state=(sint16)r_u16(0x80066D24u+offset);
        uint32 value=(uint32)speed-4000u-(uint32)((256-state)/4);
        sint32 pitch=audio_div((uint32)audio_div(value<<12,5000)*4u,5)+4096;
        sint32 amount=speed/2+2048;
        uint32 high=state>=193?1u:0u,ratio;
        w_u32(0x80073FB4u+voice,(uint32)pitch);
        if(amount>=3073) amount=3072;
        w_u32(0x80073FB0u+voice,(uint32)amount);
        if(high && !r_u8(0x80064D20u+player) && (sint32)r_u32(0x80067090u+offset)<7680)
            sub_8001ED2C(40u,0xFFFFFFFFu,2048u,pitch,0x80066D78u+offset,0u);
        w_u8(0x80064D20u+player,(uint8)high);
        ratio=(uint32)audio_div((uint32)(sint32)(sint16)r_u16(0x80066E54u+offset)<<12,(sint16)r_u16(0x80066E56u+offset));
        if((sint32)ratio>(sint32)maximum) maximum=ratio;
    }
    soundmode=(sint32)maximum<3073?0u:((sint32)maximum<3891?1u:2u);
    if(!soundmode) sub_8001E2E8(12u);
    else {
        uint32 amount=soundmode==2u?(uint32)audio_div(maximum-3890u,206):3072u;
        uint32 numerator=(r_u32(0x80073FB4u)+9000u)*2u;
        sint32 high=(sint32)(((int64_t)(sint32)numerator*(sint32)0x92492493u)>>32);
        uint32 frequency=(uint32)((sint32)((uint32)high+numerator)>>2)-(uint32)((sint32)numerator>>31);
        if(soundmode!=r_u8(AUDIO_GP+0x312u)) sub_8001ED2C(soundmode-1u,12u,amount,(sint32)frequency,0x80066D78u,0x80066D84u);
        w_u32(0x80074134u,frequency); w_u32(0x80074130u,amount);
    }
    w_u8(AUDIO_GP+0x312u,(uint8)soundmode);
    if((sint32)r_u32(0x800740F0u)>0) {
        uint32 volume=r_u32(0x800740F0u)-r_u32(AUDIO_GP+0x30Cu),alternate;
        sint32 scaled,frequency=(sint32)r_u32(0x800740F4u); uint32 pan=(uint32)(sint32)(sint16)r_u16(0x800740FCu);
        if((sint32)volume<0) volume=0u;
        alternate=r_u32(0x800652ACu); w_u32(0x800740F0u,volume);
        scaled=audio_div(volume*r_u32(0x80064BFCu),4096);
        if(alternate) draft_agent3_missing_8001FBE4(10u,frequency,pan,scaled);
        else sub_8001F8FC(10u,frequency,pan,scaled);
    }
    for(player=r_u32(0x80064C14u)+1u;;++player) {
        uint32 count=r_u32(0x80065254u),selected,offset,voice=player*32u,pitch;
        sint32 speed,amount;
        if((sint32)player>=((sint32)count>=5?4:(sint32)count)) return player<<2;
        selected=indices[player-1u];
        if((sint32)selected>=(sint32)count) abort();
        offset=selected*952u; speed=(sint16)r_u16(0x80066D20u+offset);
        pitch=(uint32)audio_div(((uint32)audio_div(((uint32)speed-4000u)<<12,5000)+4096u)*4u,5);
        w_u32(0x80073FB4u+voice,pitch);
        amount=speed/4+2048; if(amount>=3073) amount=3072;
        w_u32(0x80073FB0u+voice,(uint32)amount);
        w_u32(0x80073FA0u+voice,0x80066D78u+offset); w_u32(0x80073FA4u+voice,0x80066D84u+offset);
    }
}
static uint32 audio_leading(uint32 value)
{
    uint32 count=0u;
    if(value&0x80000000u) value=~value;
    if(!value) return 32u;
    while(!(value&0x80000000u)) { ++count; value<<=1; }
    return count;
}
static uint32 audio_square(const sint32 vector[3])
{
    uint32 leading=audio_leading((uint32)vector[0]),i,shift;
    SVECTOR input; VECTOR output;
    for(i=1u;i<3u;++i) { uint32 n=audio_leading((uint32)vector[i]); if(n<leading) leading=n; }
    shift=leading<17u?17u-leading:0u;
    input.vx=(sint16)(vector[0]>>shift); input.vy=(sint16)(vector[1]>>shift); input.vz=(sint16)(vector[2]>>shift); input.pad=0;
    /* Original reduced path overwrites the first sum */
    /* SQR with sf=1 produces the squared signed halfwords shifted by twelve */
    output.vx=((sint32)input.vx*input.vx)>>12;
    output.vy=((sint32)input.vy*input.vy)>>12;
    output.vz=((sint32)input.vz*input.vz)>>12;
    return (shift?0u:(uint32)output.vx)+(uint32)output.vy+(uint32)output.vz;
}
static void audio_scale(sint32 scale,sint32 vector[3])
{
    uint32 leading=audio_leading((uint32)vector[0]),i,shift=0u,scale_leading=audio_leading((uint32)scale);
    SVECTOR input; VECTOR output;
    for(i=1u;i<3u;++i) { uint32 n=audio_leading((uint32)vector[i]); if(n<leading) leading=n; }
    if(scale_leading<18u) { shift=18u-scale_leading; scale>>=shift; }
    i=leading<17u?17u-leading:0u;
    input.vx=(sint16)(vector[0]>>i); input.vy=(sint16)(vector[1]>>i); input.vz=(sint16)(vector[2]>>i); input.pad=0;
    shift+=i;
    gte_gpf12(&input,scale,&output);
    vector[0]=(sint32)((uint32)output.vx<<(shift&31u)); vector[1]=(sint32)((uint32)output.vy<<(shift&31u)); vector[2]=(sint32)((uint32)output.vz<<(shift&31u));
}
static uint32 audio_sqrt(uint32 value)
{
    uint32 leading=audio_leading(value),even,index,table; sint32 shift;
    if(leading==32u) return 0u;
    even=leading&~1u; shift=(19-(sint32)even)>>1;
    index=even>=24u ? value<<((even-24u)&31u):(uint32)((sint32)value>>((24u-even)&31u));
    table=(uint32)(sint32)(sint16)r_u16(0x800602C0u+(index-64u)*2u);
    return shift>=0 ? table<<((uint32)shift&31u):table>>((uint32)(-shift)&31u);
}
static uint32 audio_log(uint32 value)
{
    sint32 shift=20-(sint32)audio_leading(value); uint32 x,y=0u,i;
    x=shift>=0 ? (uint32)((sint32)value>>((uint32)shift&31u)):value<<((uint32)(-shift)&31u);
    for(i=1u;i<7u;++i) {
        uint32 next=x+(uint32)((sint32)x>>i);
        if((sint32)next<4096) { x=next; y-=r_u32(0x8005E82Cu+(i-1u)*4u); }
    }
    return y-4096u+x+(uint32)shift*r_u32(0x8005E828u);
}
extern void draft_agent3_missing_80049C84(const MATRIX *matrix);
uint32 sub_80043450(uint32 x,uint32 z);
uint32 sub_8001EE94(void)
{
    uint32 voice;
    draft_agent3_missing_80049C84((const MATRIX*)psx_addr(0x80081BA4u,32u));
    for(voice=0u;voice<24u;++voice) {
        uint32 slot=0x80073FA0u+voice*32u,position=r_u32(slot),frequency=r_u32(slot+20u),pan=0u,volume=0u;
        sint32 delta[3]; uint32 i;
        if(!position) continue;
        for(i=0u;i<3u;++i) {
            uint32 sum=r_u32(0x80081BB8u+i*4u)+(uint32)((sint32)r_u32(0x80066D78u+i*4u)>>8);
            delta[i]=(sint32)((uint32)((sint32)sum>>1)-(uint32)((sint32)r_u32(position+i*4u)>>8));
        }
        if((uint32)delta[0]+32767u<=65534u && (uint32)delta[2]+32767u<=65534u) {
            sint32 distance=(sint32)audio_sqrt(audio_square(delta)),amount;
            if(distance<1000) {
                if(distance<10) distance=10;
                audio_scale(audio_div(4096000u,distance),delta);
                amount=(sint32)r_u32(slot+16u);
            } else {
                sint32 attenuation=(sint32)(2798u-(uint32)audio_div(audio_log((uint32)distance<<12),16));
                amount=attenuation<0?0:attenuation<1025 ? audio_div((uint32)attenuation*r_u32(slot+16u),1024):(sint32)r_u32(slot+16u);
            }
            if(amount>0) {
                SVECTOR input; VECTOR rotated;
                sint32 angle;
                input.vx=(sint16)(0u-(uint32)delta[0]); input.vy=(sint16)(0u-(uint32)delta[1]); input.vz=(sint16)(0u-(uint32)delta[2]); input.pad=0;
                                xport_gte_write_data(0u,(uint16)input.vx | ((uint32)(uint16)input.vy<<16));
                xport_gte_write_data(1u,(uint16)input.vz);
                xport_gte_execute(0x486012u);
                rotated.vx=(sint32)xport_gte_read_data(9u);
                rotated.vy=(sint32)xport_gte_read_data(10u);
                rotated.vz=(sint32)xport_gte_read_data(11u);
                angle=(sint32)sub_80043450((uint32)(sint32)(sint16)rotated.vz<<4,(uint32)(sint32)(sint16)rotated.vx<<4);
                pan=(angle<2049 || r_u32(AUDIO_GP+0x89Cu)) ? (1024u-(uint32)angle)<<2:((uint32)angle-3072u)<<2;
                if((r_u8(slot+31u)&1u) && r_u32(slot+4u)!=0x80066D84u && distance>=11) {
                    uint32 velocity=r_u32(slot+4u),self_dot,other_dot,numerator,denominator;
                    if(!velocity) velocity=0x80059BF4u;
                    self_dot=(uint32)delta[0]*(uint32)((sint32)r_u32(0x80066D84u)>>12)+(uint32)delta[2]*(uint32)((sint32)r_u32(0x80066D8Cu)>>12);
                    other_dot=(uint32)delta[0]*(uint32)((sint32)r_u32(velocity)>>12)+(uint32)delta[2]*(uint32)((sint32)r_u32(velocity+8u)>>12);
                    numerator=frequency*(256u-(uint32)audio_div(self_dot,distance));
                    denominator=256u-(uint32)audio_div(other_dot,distance);
                    if(denominator) { sint32 adjusted=audio_div(numerator,(sint32)denominator); frequency=adjusted<2048?2048u:(uint32)adjusted; }
                    else frequency=r_u32(slot+20u);
                }
                volume=(uint32)audio_div((uint32)amount*r_u32(0x80064BFCu),4096);
                if(r_u32(AUDIO_GP+0x89Cu)) { draft_agent3_missing_8001FBE4(voice,(sint32)frequency,pan,(sint32)volume); continue; }
            }
        }
        sub_8001F8FC(voice,(sint32)frequency,(sint32)pan,(sint32)volume);
    }
    return 0u;
}

