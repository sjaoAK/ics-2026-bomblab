#include "defuser.h"

#include <inttypes.h>
#include <stdio.h>

#define MAX_FIELDS 8

/*
 * 把 values 中的 count 个整数写成一行“空格分隔的无符号十进制整数”，存入 out。
 * 成功返回 0；count 为 0（还没有实现）、snprintf 出错或结果被截断时返回非零。
 */
static int write_answer(char *out, size_t cap, const uint32_t *values, size_t count) {
    size_t used = 0;
    for (size_t i = 0; i < count; ++i) {
        int size = snprintf(out + used, cap - used, "%s%" PRIu32, i ? " " : "", values[i]);
        if (size < 0 || (size_t)size >= cap - used) return 1;
        used += (size_t)size;
    }
    return count == 0;
}

int solve_phase_1(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_01 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t high32=(uint32_t)(challenge>>32);
    uint32_t low32=(uint32_t)challenge;
    uint32_t shr16=(uint32_t)(challenge>>16);
    uint32_t s1=(shr16+0xf6b7)^high32;
    uint32_t s0=(low32+2*high32)^0xe190;
    uint32_t s2=(s0*5+s1)^0xcebf;
    values[0]=s2;
    count=1;

    return write_answer(out, cap, values, count);
}

int solve_phase_2(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_02 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t t0=(uint32_t)((challenge>>13)&0x3F);
    uint32_t pre=3*t0;
    values[0]=pre;
    uint32_t shift=12;
    for (int i=1;i<=5;++i){
        uint32_t ti=(uint32_t)((challenge>>shift)&0x3F);
        uint32_t vi=(ti+pre)^(uint32_t)i;
        values[i]=vi;
        pre=vi;
        shift+=5;
    }
    count=6;
    
    return write_answer(out, cap, values, count);
}

int solve_phase_3(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_03 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t idx=(uint32_t)challenge&0x7u;
    values[0]=idx;
    uint32_t arg=(uint32_t)(challenge>>32)^(uint32_t)(challenge>>3);
    uint32_t ans;
    switch (idx){
        case 0: ans=3u*arg+0x40d9u;break;
        case 1: ans=(arg^0xf18du)+(arg>>2);break;
        case 2: ans=(arg^0xc319u)+(arg>>2);break;
        case 3: ans=5u*arg +0x45119u;break;
        case 4: ans=5u*arg +0x11ec2u;break;
        case 5: ans=(4u*arg)^0x4c30u;break;
        case 6: ans=(4u*arg)^0xb98fu;break;
        case 7: ans=5u*arg+0xcdf8u;break;
        default:    ans=0;
    }
    values[1]=ans;
    count=2;
    
    return write_answer(out, cap, values, count);
}

/*
 * 复刻 p4_walk（0x4022d0）：在区间 [b, c] 内向目标 a 二分下钻的递归函数。
 * 参数对应汇编寄存器：a=edi, b=esi, c=edx, d=ecx。
 * 返回 phase_04 要求输入的整数（与 sscanf 解析出的数比对）。
 */
static uint32_t p4_walk(uint32_t a,uint32_t b,uint32_t c,uint32_t d) {
    uint32_t mid=b+((c-b)>>1u);   
    if(a==mid){                     
        return (mid+0xeeu)^d;       
    }
    uint32_t v=mid+d;               
    if (a>mid){                      
        return (3u*p4_walk(a,mid+1u,c,d))^v^0x9u;
    }
    else{        
        return (3u*p4_walk(a,b,mid-1u,d))^v^0x10u;
    }
}

int solve_phase_4(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS]={0};
    size_t count=0;
    (void)challenge;
    // TODO: 根据 phase_04 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t a=(uint32_t)challenge&0x7fu;                             
    uint32_t b=0u;                                                      
    uint32_t c=0x7fu;                                                    
    uint32_t d=(uint32_t)(challenge>>7u)^(uint32_t)(challenge>>32u); 
    values[0]=p4_walk(a,b,c,d);
    count=1;         

    return write_answer(out, cap, values, count);
}

int solve_phase_5(uint64_t challenge,char *out,size_t cap) {
    uint32_t values[MAX_FIELDS]={0};
    size_t count=0;
    (void)challenge;
    // TODO: 根据 phase_05 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    static const uint32_t node_val[12] = {
        0x8e67, 0xd28b, 0x9fce, 0x4d61, 0xd0f1, 0x1af3,
        0x9807, 0x897a, 0xde59, 0x0e43, 0x7200, 0x0b39
    };
    static const uint32_t node_out[12] = {
        0x054c, 0x061e, 0x0122, 0x0937, 0x0012, 0x084d,
        0x030c, 0x0437, 0x0b5e, 0x0740, 0x0a5f, 0x0242
    };

    uint32_t low=(uint32_t)challenge;          
    uint32_t hi=(uint32_t)(challenge>>32);  
    int idx=low & 7;                            
    int k=(int)(((uint32_t)(challenge>>3))&3u); 
    uint32_t acc=(((uint32_t)(challenge>>6))^hi)^0x3046u;
    acc=(acc+node_val[idx])*5u;
    acc^=node_out[idx];
    uint32_t written = node_out[idx];
    for (int i=0;i<k;++i) {
        idx++;
        acc=(acc+node_val[idx])*5u;
        acc^=node_out[idx];
        written=node_out[idx];
    }
    values[0]=acc;      
    values[1]=written;  
    count=2;           

    return write_answer(out, cap, values, count);
}

int solve_phase_6(uint64_t challenge, char *out, size_t cap) {
    uint32_t values[MAX_FIELDS] = {0};
    size_t count = 0;
    (void)challenge;
    // TODO: 根据 phase_06 的汇编，用 challenge 算出口令中的各个整数，依次存入 values，并把 count 改为整数个数。
    uint32_t idx1 =(uint32_t)challenge&0x7u;
    uint32_t arg1 =((uint32_t)(challenge>>6))^((uint32_t)(challenge>>32));
    uint32_t res1;
    switch (idx1) {
        case 0: res1=(arg1^(arg1>>7))+0x8d84u;break;
        case 1: res1=((arg1<<2)|(arg1>>30))^0xe563u;break;
        case 2: res1=((arg1<<2)|(arg1>>30))^0xe563u;break;
        case 5: res1=((arg1<<2)|(arg1>>30))^0xe563u;break;
        case 3: res1=(arg1^0xb5d6u)+8u*arg1;break;
        case 4: res1=(arg1+0xc38du)^(arg1>>2);break;
        case 6: res1=(4u*arg1+0xd850u)^(7u*arg1);break;
        case 7: res1=(arg1>>7)|(arg1<<25);break;
        default:    res1=0;
    }
    uint32_t mid=res1^0x43d4u;
    uint32_t idx2=((uint32_t)challenge&0x38u)>>3;
    uint32_t res2;
    switch (idx2){
        case 0: res2=(mid^(mid>>7))+0x8d84u;break;
        case 1: res2=((mid<<2)|(mid>>30))^0xe563u;break;
        case 2: res2=((mid<<2)|(mid>>30))^0xe563u;break;
        case 5: res2=((mid<<2)|(mid>>30))^0xe563u;break;
        case 3: res2=(mid^0xb5d6u)+8u*mid;break;
        case 4: res2=(mid+0xc38du)^(mid>>2);break;
        case 6: res2=(4u*mid+0xd850u)^(7u*mid);break;
        case 7: res2=(mid>>7)|(mid<<25);break;
        default:    res2=0;
    }
    values[0]=res2+0xa7f0u;
    count=1;  
    
    return write_answer(out, cap, values, count);
}

int solve_secret(uint64_t challenge, uint64_t context, char *out, size_t cap) {
    (void)challenge;
    (void)context;
    (void)out;
    (void)cap;
    // TODO（选做）: 根据 secret_gate、secret_copy 和 secret_phase 的汇编构造完整的第七行，写入 out 后返回 0。
    // 第七行不一定只有整数，可以直接用 snprintf 写入；还没完成时保持返回 1，测试会显示 not attempted。
    uint32_t ch_low=(uint32_t)challenge;
    uint32_t ch_high=(uint32_t)(challenge>>32);
    uint32_t ctx_low=(uint32_t)context;
    uint32_t ctx_high=(uint32_t)(context>>32);
    uint32_t ch_xor=ch_high^ch_low;
    uint32_t ctx_xor=ctx_high^ctx_low;
    uint32_t x=ctx_low&0x3u;
    uint32_t mix_in=ch_xor^ctx_xor;
    uint32_t rol=(mix_in<<3)|(mix_in>>(32-3));
    rol+=0xbb45u;
    rol^=0xb3c5u;
    uint32_t r9=(x*0xbb45u)^(uint32_t)(context>>16);
    uint32_t y=rol^r9;
    uint32_t offset=ctx_xor+x+0xb3c5u+(ch_xor>>3);
    uint32_t z=y^offset;
    int ret=snprintf(out, cap,
        "                                OPEN "
        "%" PRIu32 " %" PRIu32 " %" PRIu32,
        x, y, z);
    if (ret<0||(size_t)ret>=cap) return 1;

    return 0;
}