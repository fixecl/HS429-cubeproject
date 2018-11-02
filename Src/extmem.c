#include "extmem.h"
#include "fmc.h"
#include "usart.h"

//memalloc
exmemTable SDRAM_EXMM_Table;

//__align(32) u8 ccmMemBase[60*1024] __attribute__((at(0X10000000)));
//byte inMemBase[120*1024];

FMC_SDRAM_CommandTypeDef Command;
uint8_t isSequenceInited=0;
void SDRAM_Initialization_sequence(void)
{
    if(isSequenceInited) return ;
    
  __IO uint32_t tmpmrd = 0;
  
  /* Step 1: Configure a clock configuration enable command */
  Command.CommandMode            = FMC_SDRAM_CMD_CLK_ENABLE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);

  /* Step 2: Insert 100 us minimum delay */ 
  /* Inserted delay is equal to 1 ms due to systick time base unit (ms) */
  HAL_Delay(1);
    
  /* Step 3: Configure a PALL (precharge all) command */ 
  Command.CommandMode            = FMC_SDRAM_CMD_PALL;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);  
  
  /* Step 4: Configure an Auto Refresh command */ 
  Command.CommandMode            = FMC_SDRAM_CMD_AUTOREFRESH_MODE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber      = 8;
  Command.ModeRegisterDefinition = 0;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);
  
  /* Step 5: Program the external memory mode register */
  tmpmrd = (uint32_t)SDRAM_MODEREG_BURST_LENGTH_2          |\
                     SDRAM_MODEREG_BURST_TYPE_SEQUENTIAL   |\
                     SDRAM_MODEREG_CAS_LATENCY_3           |\
                     SDRAM_MODEREG_OPERATING_MODE_STANDARD |\
                     SDRAM_MODEREG_WRITEBURST_MODE_SINGLE;
  
  Command.CommandMode            = FMC_SDRAM_CMD_LOAD_MODE;
  Command.CommandTarget          = FMC_SDRAM_CMD_TARGET_BANK2;
  Command.AutoRefreshNumber      = 1;
  Command.ModeRegisterDefinition = tmpmrd;

  /* Send the command */
  HAL_SDRAM_SendCommand(&hsdram1, &Command, SDRAM_TIMEOUT);
  
  /* Step 6: Set the refresh rate counter */
  /* Set the device refresh rate */
  HAL_SDRAM_ProgramRefreshRate(&hsdram1, 1386); 
  isSequenceInited=1;
}

void SDRAM_Test(void)
{
    SDRAM_Initialization_sequence();
    UART1_PrintLn(">>>>>>ExtMem Test");
    HAL_SDRAM_StateTypeDef hsst;
    hsst=HAL_SDRAM_GetState(&hsdram1);
    if(hsst==HAL_SDRAM_STATE_READY)
        UART1_PrintDataLn("Mem State Ready:",hsst);
    
    
    volatile u32 *p;
    p=(uint32_t *)SDRAM_BANK_ADDR;
    UART1_Print("start p->");
    UART1_PrintNum((u32)p);
    UART1_PrintLn(" ");
    u32 ori=0x23EF4A00;
    volatile u32 i;
    for(i=0;i<SDRAM_SIZE;i+=4)
    {
        *p=ori+i;
        p++;
    }
    UART1_Print("end p->");
    p--;
    UART1_PrintNum((u32)p);
    UART1_PrintLn(" ");
    //Read
    u8 failFlg=0;
    p=(uint32_t *)SDRAM_BANK_ADDR;
    for(i=0;i<SDRAM_SIZE;i+=4)
    {
        if(*p!=ori+i)
        {
            failFlg=1;
            break;
        }
        p++;
    }
    
    UART1_Print(">>>>>>ExtMem Test End -> ");
    failFlg? UART1_PrintDataLn("Failed,p->",(u32)p) : UART1_PrintLn("Succeed");
    
}


void exmmallocInit(void )
{
    SDRAM_Initialization_sequence();
    
    int i;
    SDRAM_EXMM_Table.memRate=0;
    SDRAM_EXMM_Table.usedBlockNums=0;
    for(i=0;i<EXMM_BLOCK_NUM;i++)
    {
        SDRAM_EXMM_Table.memStateTable[i]=EXMM_MEMSTATE_AVAILABLE;
    }
}

void *exmmalloc(u32 size)
{
    u8 *p;
    p=(u8 *) EXMM_START_ARRD;
    u32 sp=0,fp=0;   
    //计算所需块数量
    u32 blockNum;
    blockNum=(size+EXMM_BLOCK_SIZE-1)/EXMM_BLOCK_SIZE;
    
    //块数量不够则返回
    if(blockNum> EXMM_BLOCK_NUM-SDRAM_EXMM_Table.usedBlockNums )
    {
        return NULL;
    }
    
    for(;;)
    {
        if(SDRAM_EXMM_Table.memStateTable[fp]==EXMM_MEMSTATE_AVAILABLE)
        {
            if(fp-sp+1 > 254)
                return NULL;
            
            if(fp-sp+1 == blockNum)
            {
                //分配成功
                SDRAM_EXMM_Table.usedBlockNums+=blockNum;
                SDRAM_EXMM_Table.memRate=SDRAM_EXMM_Table.usedBlockNums *100 / EXMM_BLOCK_NUM;
                for(int i=sp;i<=fp;i++)
                {
                    SDRAM_EXMM_Table.memStateTable[i]=blockNum;
                }
                
                return p+sp*EXMM_BLOCK_SIZE;
            }
            fp++;  
        }
        else
        {
            fp+= SDRAM_EXMM_Table.memStateTable[fp];
            sp=fp;
        }
        if(fp>=EXMM_BLOCK_NUM) 
            return NULL;
    }
    
}

void exmfree(void *ptr)
{
    u32 sp;
    u32 blockNum;
    
    if(ptr == NULL) return;
    
    sp=((u32)ptr-EXMM_START_ARRD)/EXMM_BLOCK_SIZE;
    blockNum=SDRAM_EXMM_Table.memStateTable[sp];
    
    //更新表
    SDRAM_EXMM_Table.usedBlockNums -= blockNum;
    SDRAM_EXMM_Table.memRate=SDRAM_EXMM_Table.usedBlockNums *100 / EXMM_BLOCK_NUM;
    for(int i=0;i<blockNum;i++)
    {
        SDRAM_EXMM_Table.memStateTable[i+sp]=EXMM_MEMSTATE_AVAILABLE;
    }
    
    
}

