#include "mem.h"


#include "usart.h"

//memalloc
smemTable SRAM_MM_Table;
cmemTable CRAM_MM_Table;

__align(32) u8 ccmMemBase[60*1024] __attribute__((at(0X10000000)));
byte inMemBase[120*1024];



void mallocInit(void )
{
    
    int i;
    SRAM_MM_Table.memRate=0;
    SRAM_MM_Table.usedBlockNums=0;
    CRAM_MM_Table.memRate=0;
    CRAM_MM_Table.usedBlockNums=0;
    for(i=0;i<SMM_BLOCK_NUM;i++)
    {
        SRAM_MM_Table.memStateTable[i]=MM_MEMSTATE_AVAILABLE;
    }
    for(i=0;i<CMM_BLOCK_NUM;i++)
    {
        CRAM_MM_Table.memStateTable[i]=MM_MEMSTATE_AVAILABLE;
    }
    
    exmmallocInit();
}

u8 getSRAMRate()
{
    return SRAM_MM_Table.memRate;
}

u8 getCRAMRate()
{
    return CRAM_MM_Table.memRate;
}

void *srammalloc(u32 size)
{
    u8 *p;
    p= inMemBase;
    u32 sp=0,fp=0;   
    //�������������
    u32 blockNum;
    blockNum=(size+MM_BLOCK_SIZE-1)/MM_BLOCK_SIZE;
    
    //�����������򷵻�
    if(blockNum> SMM_BLOCK_NUM-SRAM_MM_Table.usedBlockNums )
    {
        return NULL;
    }
    
    for(;;)
    {
        if(SRAM_MM_Table.memStateTable[fp]== MM_MEMSTATE_AVAILABLE)
        {
            if(fp-sp+1 > 254)
                return NULL;
            
            if(fp-sp+1 == blockNum)
            {
                //����ɹ�
                SRAM_MM_Table.usedBlockNums+=blockNum;
                SRAM_MM_Table.memRate=SRAM_MM_Table.usedBlockNums *100/ SMM_BLOCK_NUM;
                for(int i=sp;i<=fp;i++)
                {
                    SRAM_MM_Table.memStateTable[i]=blockNum;
                }
                
                return p+sp*MM_BLOCK_SIZE;
            }
            fp++;  
        }
        else
        {
            fp+= SRAM_MM_Table.memStateTable[fp];
            sp=fp;
        }
        if(fp>=SMM_BLOCK_NUM) 
            return NULL;
    }
    
}

void sramfree(void *ptr)
{
    u32 sp;
    u32 blockNum;
    
    if(ptr == NULL) return;
    
    sp=((u32)ptr-(u32)inMemBase)/MM_BLOCK_SIZE;
    blockNum=SRAM_MM_Table.memStateTable[sp];
    
    //���±�
    SRAM_MM_Table.usedBlockNums -= blockNum;
    SRAM_MM_Table.memRate=SRAM_MM_Table.usedBlockNums *100 / SMM_BLOCK_NUM;
    for(int i=0;i<blockNum;i++)
    {
        SRAM_MM_Table.memStateTable[i+sp]=MM_MEMSTATE_AVAILABLE;
    }
    
    
}

void *crammalloc(u32 size)
{
    u8 *p;
    p= ccmMemBase;
    u32 sp=0,fp=0;   
    //�������������
    u32 blockNum;
    blockNum=(size+MM_BLOCK_SIZE-1)/MM_BLOCK_SIZE;
    
    //�����������򷵻�
    if(blockNum> CMM_BLOCK_NUM-CRAM_MM_Table.usedBlockNums )
    {
        return NULL;
    }
    
    for(;;)
    {
        if(CRAM_MM_Table.memStateTable[fp]== MM_MEMSTATE_AVAILABLE)
        {
            if(fp-sp+1 > 254)
                return NULL;
            
            if(fp-sp+1 == blockNum)
            {
                //����ɹ�
                CRAM_MM_Table.usedBlockNums+=blockNum;
                CRAM_MM_Table.memRate=CRAM_MM_Table.usedBlockNums *100 / CMM_BLOCK_NUM;
                for(int i=sp;i<=fp;i++)
                {
                    CRAM_MM_Table.memStateTable[i]=blockNum;
                }
                
                return p+sp*MM_BLOCK_SIZE;
            }
            fp++;  
        }
        else
        {
            fp+= SRAM_MM_Table.memStateTable[fp];
            sp=fp;
        }
        if(fp>=CMM_BLOCK_NUM) 
            return NULL;
    }
    
}

void cramfree(void *ptr)
{
    u32 sp;
    u32 blockNum;
    
    if(ptr == NULL) return;
    
    sp=((u32)ptr-(u32)ccmMemBase)/MM_BLOCK_SIZE;
    blockNum=CRAM_MM_Table.memStateTable[sp];
    
    //���±�
    CRAM_MM_Table.usedBlockNums -= blockNum;
    CRAM_MM_Table.memRate=CRAM_MM_Table.usedBlockNums *100 / CMM_BLOCK_NUM;
    for(int i=0;i<blockNum;i++)
    {
        CRAM_MM_Table.memStateTable[i+sp]=MM_MEMSTATE_AVAILABLE;
    }
    
    
}


//�����ڴ�
//*des:Ŀ�ĵ�ַ
//*src:Դ��ַ
//n:��Ҫ���Ƶ��ڴ泤��(�ֽ�Ϊ��λ)
void mmemcpy(void *des,void *src,u32 n)  
{  
    u8 *xdes=des;
	u8 *xsrc=src; 
    while(n--)*xdes++=*xsrc++;  
}  
//�����ڴ�
//*s:�ڴ��׵�ַ
//c :Ҫ���õ�ֵ
//count:��Ҫ���õ��ڴ��С(�ֽ�Ϊ��λ)
void mmemset(void *s,u8 c,u32 count)  
{  
    u8 *xs = s;  
    while(count--)*xs++=c;  
}	

