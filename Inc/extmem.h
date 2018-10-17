#ifndef __EXTMEM_H_
#define __EXTMEM_H_

#ifdef __cplusplus
 extern "C" {
#endif
     
#include "stm32f4xx_hal.h"
#include "main.h"
     
#define SDRAM_TIMEOUT                    ((uint32_t)0x400)  
     
#define SDRAM_MODEREG_BURST_LENGTH_1             ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_LENGTH_2             ((uint16_t)0x0001)
#define SDRAM_MODEREG_BURST_LENGTH_4             ((uint16_t)0x0002)
#define SDRAM_MODEREG_BURST_LENGTH_8             ((uint16_t)0x0004)
#define SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL      ((uint16_t)0x0000)
#define SDRAM_MODEREG_BURST_TYPE_INTERLEAVED     ((uint16_t)0x0008)
#define SDRAM_MODEREG_CAS_LATENCY_2              ((uint16_t)0x0020)
#define SDRAM_MODEREG_CAS_LATENCY_3              ((uint16_t)0x0030)
#define SDRAM_MODEREG_OPERATING_MODE_STANDARD    ((uint16_t)0x0000)
#define SDRAM_MODEREG_WRITEBURST_MODE_PROGRAMMED ((uint16_t)0x0000) 
#define SDRAM_MODEREG_WRITEBURST_MODE_SINGLE     ((uint16_t)0x0200) 

#define SDRAM_BANK_ADDR    ((uint32_t)0xD0000000U) // FMC SDRAM 数据基地址
#define SDRAM_SIZE         ((uint32_t)0x01000000U) 
#define SDRAM_BANK_ENDARRD ((uint32_t)0xD1000000U)

#define MM_START_ARRD  ((uint32_t)0xD0000000U) 
#define MM_END_ARRD    ((uint32_t)0xD0D00000U) 
#define MM_SIZE        ((uint32_t)0x00D00000U) 
#define MM_BLOCK_SIZE  ((uint32_t)0x00001000U)  //4KB
#define MM_BLOCK_NUM   ((uint32_t)0x00000D00U)  //修改后需要计算 3328个

#define MM_MEMSTATE_AVAILABLE  0

typedef struct MM_TABLE{
    u8 memRate;
    u8 memStateTable[MM_BLOCK_NUM];
    u16 usedBlockNums;
} memTable;


void SDRAM_Initialization_sequence(void);
void SDRAM_Test(void);

void mmallocInit(void );
void *mmalloc(u16 size);
void mfree(void *ptr);

#ifdef __cplusplus
}
#endif

#endif
