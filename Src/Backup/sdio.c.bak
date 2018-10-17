/**
  ******************************************************************************
  * File Name          : SDIO.c
  * Description        : This file provides code for the configuration
  *                      of the SDIO instances.
  ******************************************************************************
  * This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * Copyright (c) 2018 STMicroelectronics International N.V. 
  * All rights reserved.
  *
  * Redistribution and use in source and binary forms, with or without 
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice, 
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other 
  *    contributors to this software may be used to endorse or promote products 
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this 
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under 
  *    this license is void and will automatically terminate your rights under 
  *    this license. 
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "sdio.h"

#include "gpio.h"
#include "dma.h"

/* USER CODE BEGIN 0 */
#include "usart.h"
HAL_SD_CardCIDTypeDef SD_CardCID;

typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* 私有宏定义 ----------------------------------------------------------------*/
#define BLOCK_SIZE            512         // SD卡块大小     
#define NUMBER_OF_BLOCKS      8           // 测试块数量(小于15)
#define WRITE_READ_ADDRESS    0x00002000  // 测试读写地址

/* 私有变量 ------------------------------------------------------------------*/
uint32_t Buffer_Block_Tx[BLOCK_SIZE*NUMBER_OF_BLOCKS]; // 写数据缓存
uint32_t Buffer_Block_Rx[BLOCK_SIZE*NUMBER_OF_BLOCKS] = {1,2}; // 读数据缓存
HAL_StatusTypeDef sd_status;    // HAL库函数操作SD卡函数返回值：操作结果
TestStatus test_status;           // 数据测试结果

/* 扩展变量 ------------------------------------------------------------------*/
/* 私有函数原形 --------------------------------------------------------------*/
void Fill_Buffer(uint32_t *pBuffer, uint32_t BufferLength, uint32_t Offset);
TestStatus Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint32_t BufferLength);
TestStatus eBuffercmp(uint32_t* pBuffer, uint32_t BufferLength);

void SD_EraseTest(void);
void SD_Write_Read_Test(void);
/* USER CODE END 0 */

SD_HandleTypeDef hsd;
DMA_HandleTypeDef hdma_sdio_rx;
DMA_HandleTypeDef hdma_sdio_tx;

/* SDIO init function */

void MX_SDIO_SD_Init(void)
{

  hsd.Instance = SDIO;
  hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
  hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
  hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
  hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
  hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd.Init.ClockDiv = 4;

}

void HAL_SD_MspInit(SD_HandleTypeDef* sdHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspInit 0 */

  /* USER CODE END SDIO_MspInit 0 */
    /* SDIO clock enable */
    __HAL_RCC_SDIO_CLK_ENABLE();
  
    /**SDIO GPIO Configuration    
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF12_SDIO;
    HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

    /* SDIO DMA Init */
    /* SDIO_RX Init */
    hdma_sdio_rx.Instance = DMA2_Stream3;
    hdma_sdio_rx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_rx.Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_sdio_rx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_rx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_rx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_rx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_rx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_rx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sdio_rx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_rx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_rx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_rx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_rx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(sdHandle,hdmarx,hdma_sdio_rx);

    /* SDIO_TX Init */
    hdma_sdio_tx.Instance = DMA2_Stream6;
    hdma_sdio_tx.Init.Channel = DMA_CHANNEL_4;
    hdma_sdio_tx.Init.Direction = DMA_MEMORY_TO_PERIPH;
    hdma_sdio_tx.Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_sdio_tx.Init.MemInc = DMA_MINC_ENABLE;
    hdma_sdio_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
    hdma_sdio_tx.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
    hdma_sdio_tx.Init.Mode = DMA_PFCTRL;
    hdma_sdio_tx.Init.Priority = DMA_PRIORITY_LOW;
    hdma_sdio_tx.Init.FIFOMode = DMA_FIFOMODE_ENABLE;
    hdma_sdio_tx.Init.FIFOThreshold = DMA_FIFO_THRESHOLD_FULL;
    hdma_sdio_tx.Init.MemBurst = DMA_MBURST_INC4;
    hdma_sdio_tx.Init.PeriphBurst = DMA_PBURST_INC4;
    if (HAL_DMA_Init(&hdma_sdio_tx) != HAL_OK)
    {
      _Error_Handler(__FILE__, __LINE__);
    }

    __HAL_LINKDMA(sdHandle,hdmatx,hdma_sdio_tx);

    /* SDIO interrupt Init */
    HAL_NVIC_SetPriority(SDIO_IRQn, 5, 0);
    HAL_NVIC_EnableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspInit 1 */

  /* USER CODE END SDIO_MspInit 1 */
  }
}

void HAL_SD_MspDeInit(SD_HandleTypeDef* sdHandle)
{

  if(sdHandle->Instance==SDIO)
  {
  /* USER CODE BEGIN SDIO_MspDeInit 0 */

  /* USER CODE END SDIO_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_SDIO_CLK_DISABLE();
  
    /**SDIO GPIO Configuration    
    PC8     ------> SDIO_D0
    PC9     ------> SDIO_D1
    PC10     ------> SDIO_D2
    PC11     ------> SDIO_D3
    PC12     ------> SDIO_CK
    PD2     ------> SDIO_CMD 
    */
    HAL_GPIO_DeInit(GPIOC, GPIO_PIN_8|GPIO_PIN_9|GPIO_PIN_10|GPIO_PIN_11 
                          |GPIO_PIN_12);

    HAL_GPIO_DeInit(GPIOD, GPIO_PIN_2);

    /* SDIO DMA DeInit */
    HAL_DMA_DeInit(sdHandle->hdmarx);
    HAL_DMA_DeInit(sdHandle->hdmatx);

    /* SDIO interrupt Deinit */
    HAL_NVIC_DisableIRQ(SDIO_IRQn);
  /* USER CODE BEGIN SDIO_MspDeInit 1 */

  /* USER CODE END SDIO_MspDeInit 1 */
  }
} 

/* USER CODE BEGIN 1 */

void SD_InfoTest()
{
    HAL_SD_CardStateTypeDef sd_states;
    HAL_SD_CardCIDTypeDef cardCID;
    sd_states=HAL_SD_GetCardState(&hsd);
    if(sd_states==HAL_SD_CARD_TRANSFER)
    {
        HAL_SD_GetCardCID(&hsd,&cardCID);
        UART1_PrintLn("SD OK->");
        
        UART1_PrintDataLn("Capacity:",hsd.SdCard.BlockSize*hsd.SdCard.BlockNbr/1024);
        UART1_PrintDataLn("Block Size:",hsd.SdCard.BlockSize);
        UART1_PrintDataLn("MID:",cardCID.ManufacturerID);
        UART1_PrintDataLn("CID:",hsd.CID[0]);
        UART1_PrintDataLn("Card Type:",hsd.SdCard.CardType);
        UART1_PrintDataLn("Card Vision:",hsd.SdCard.CardVersion);
        UART1_PrintDataLn("Card RCA:",hsd.SdCard.RelCardAdd);
        
    }
    else
    {
        UART1_Print("SD Fail:");
        UART1_PrintNum(sd_states);
        UART1_PrintLn("");
    }
}


/**
  * 函数功能: SD卡擦除测试
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SD_EraseTest(void)
{
	/* 第1个参数为SD卡句柄，第2个参数为擦除起始地址，第3个参数为擦除结束地址 */
  sd_status=HAL_SD_Erase(&hsd,WRITE_READ_ADDRESS,WRITE_READ_ADDRESS+NUMBER_OF_BLOCKS*4);
   UART1_PrintDataLn("erase status:",sd_status);
	//osDelay(10);
    HAL_Delay(500);
  if (sd_status == HAL_OK)
  {	
    /* 读取刚刚擦除的区域 */
    sd_status = HAL_SD_ReadBlocks(&hsd,(uint8_t *)Buffer_Block_Rx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS,0xffff);
    UART1_PrintDataLn("erase read status:",sd_status);
    /* 把擦除区域读出来对比 */
    test_status = eBuffercmp(Buffer_Block_Rx,BLOCK_SIZE*NUMBER_OF_BLOCKS);

    if(test_status == PASSED)
      UART1_PrintLn("》擦除测试成功！" ); 
    else	  
      UART1_PrintLn("》擦除不成功，数据出错！" );      
  }
  else
  {
    UART1_PrintLn("》擦除测试失败！部分SD不支持擦除，只要读写测试通过即可" );
  }
}

/**
  * 函数功能: SD卡读写测试
  * 输入参数: 无
  * 返 回 值: 无
  * 说    明: 无
  */
void SD_Write_Read_Test(void)
{  
	int i,j = 0;
  /* 填充数据到写缓存 */
  Fill_Buffer(Buffer_Block_Tx,BLOCK_SIZE*NUMBER_OF_BLOCKS, 0x32F1);
  
  /* 往SD卡写入数据 */
  sd_status = HAL_SD_WriteBlocks(&hsd,(uint8_t *)Buffer_Block_Tx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS,0xffff);
  UART1_PrintDataLn("write status:",sd_status);
  //osDelay(500);
  HAL_Delay(500);
  /* 从SD卡读取数据 */
  sd_status = HAL_SD_ReadBlocks(&hsd,(uint8_t *)Buffer_Block_Rx,WRITE_READ_ADDRESS,NUMBER_OF_BLOCKS,0xffff);
  UART1_PrintDataLn("read status:",sd_status);
  
  /* 比较数据 */
  test_status = Buffercmp(Buffer_Block_Tx, Buffer_Block_Rx, BLOCK_SIZE*NUMBER_OF_BLOCKS/4);	//比较
  if(test_status == PASSED)
	{
    UART1_PrintLn("》读写测试成功！" );
		
		for(i=0;i<BLOCK_SIZE*NUMBER_OF_BLOCKS/4;i++)
		{
			if(j==8)
			{
				printf("\r\n");
				j=0;
			}
			
			UART1_PrintNum(Buffer_Block_Rx[i]);
			j++;
		}
		UART1_PrintLn("");
	}
  else  
  	UART1_PrintLn("》读写测试失败！ " );  
}

/**
  * 函数功能: 在缓冲区中填写数据
  * 输入参数: pBuffer：要填充的缓冲区
  *           BufferLength：要填充的大小
  *           Offset：填在缓冲区的第一个值 
  * 返 回 值: 无
  * 说    明: 无
  */
void Fill_Buffer(uint32_t *pBuffer, uint32_t BufferLength, uint32_t Offset)
{
  uint32_t index = 0;
  /* 填充数据 */
  for (index = 0; index < BufferLength; index++ )
  {
    pBuffer[index] = index + Offset;
  }
}

/**
  * 函数功能: 比较两个缓冲区中的数据是否相等
  * 输入参数: pBuffer1：要比较的缓冲区1的指针
  *           pBuffer2：要比较的缓冲区2的指针
  *           BufferLength：缓冲区长度
  * 返 回 值: PASSED：相等
  *           FAILED：不等
  * 说    明: 无
  */
TestStatus Buffercmp(uint32_t* pBuffer1, uint32_t* pBuffer2, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    if(BufferLength%50==0)
    {
      UART1_PrintNum(*pBuffer1);
        UART1_Print(" - ");
        UART1_PrintNum(*pBuffer2);
        UART1_PrintLn("");
    }
    if (*pBuffer1 != *pBuffer2)
    {
      return FAILED;
    }
    pBuffer1++;
    pBuffer2++;
  }
  return PASSED;
}

/**
  * 函数功能: 检查缓冲区的数据是否为0xff或0
  * 输入参数: pBuffer：要比较的缓冲区的指针
  *           BufferLength：缓冲区长度
  * 返 回 值: PASSED：缓冲区的数据全为0xff或0
  *           FAILED：缓冲区的数据至少有一个不为0xff或0 
  * 说    明: 无
  */
TestStatus eBuffercmp(uint32_t* pBuffer, uint32_t BufferLength)
{
  while (BufferLength--)
  {
    /* SD卡擦除后的可能值为0xff或0 */
    if ((*pBuffer != 0xFFFFFFFF) && (*pBuffer != 0))
    {
      return FAILED;
    }
    pBuffer++;
  }
  return PASSED;
}
/* USER CODE END 1 */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
