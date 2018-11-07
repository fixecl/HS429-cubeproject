#ifndef __MEM_H_
#define __MEM_H_

#ifdef __cplusplus
 extern "C" {
#endif
     
#include "stm32f4xx_hal.h"
#include "main.h"
#include "extmem.h"
     
#define MM_BLOCK_SIZE  ((uint32_t)0x00000200U)  //512Byte
#define MM_MEMSTATE_AVAILABLE  0
     
//SRAM
#define SMM_SIZE        ((uint32_t)40*1024)  //40KB 
#define SMM_BLOCK_NUM   ((uint32_t)80)  //修改后需要计算 80个

typedef struct SMM_TABLE{
    u8 memRate;
    u8 memStateTable[SMM_BLOCK_NUM];
    u16 usedBlockNums;
} smemTable;

//CCMRAM
#define CMM_SIZE        ((uint32_t)60*1024)  //60KB
#define CMM_BLOCK_NUM   ((uint32_t)120)  //修改后需要计算 120个

typedef struct CMM_TABLE{
    u8 memRate;
    u8 memStateTable[CMM_BLOCK_NUM];
    u16 usedBlockNums;
} cmemTable;

void mallocInit(void );
void *srammalloc(u32 size);
void sramfree(void *ptr);
void *crammalloc(u32 size);
void cramfree(void *ptr);

void mmemcpy(void *des,void *src,u32 n) ;
void mmemset(void *s,u8 c,u32 count)  ;


u8 getSRAMRate(void );
u8 getCRAMRate(void);

#ifdef __cplusplus
}
#endif

#endif
