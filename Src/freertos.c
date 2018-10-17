/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
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
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */     
#include "usart.h"
#include "fatfs.h"
#include "bsp_driver_flash.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
FIL fil;

u8 datas[1024];
/* USER CODE END Variables */
osThreadId defaultTaskHandle;
osThreadId debugTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);
void StartDebugTask(void const * argument);

extern void MX_FATFS_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
    /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
    /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
    /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* definition and creation of debugTask */
  osThreadDef(debugTask, StartDebugTask, osPriorityAboveNormal, 0, 128);
  debugTaskHandle = osThreadCreate(osThread(debugTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
    /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_QUEUES */
    /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for FATFS */
  MX_FATFS_Init();

  /* USER CODE BEGIN StartDefaultTask */
    UART1_PrintLn("Default Task Started");
    osDelay(500);
    //=======================fatfs====================
    
    //FRESULT fr;
    //UART1_PrintLn("FatFS mount...");
    /*
    fr=f_mount(&SDFatFS,"0:",1);
    if(fr != FR_OK)
    {
        if(fr==FR_NO_FILESYSTEM)
        {
            UART1_PrintLn("FatFS SD format...");
            fr=f_mkfs("0:",FM_ANY,0,datas,sizeof(datas));
            if(fr != FR_OK)
                UART1_PrintDataLn("FatFS SD format fail:",fr);
        }
        else
            UART1_PrintDataLn("FatFS SD mount Fail:",fr);
        
    }
    else
    {
        UART1_PrintLn("FatFS SD mount Success");
    }
    
    fr=f_mount(&USERFatFS,"1:",1);
    if(fr != FR_OK)
    {
        if(fr==FR_NO_FILESYSTEM)
        {
            UART1_PrintLn("FatFS FLASH format...");
            fr=f_mkfs("1:",FM_ANY,0,datas,sizeof(datas));
            if(fr != FR_OK)
                UART1_PrintDataLn("FatFS FLASH format fail:",fr);
        }
        else
            UART1_PrintDataLn("FatFS FLASH mount Fail:",fr);
        
    }
    else
    {
        UART1_PrintLn("FatFS FLASH mount Success");
    }
    
    //文件创建
    
    u32 bw;
    //char line[100];
    fr=f_open(&fil,"1:m.txt",FA_WRITE|FA_CREATE_ALWAYS);
    if(fr==FR_OK) 
    {
        
        UART1_PrintLn("Start write");
        fr=f_write(&fil,"ahhhhhh",5,&bw);
        if(fr!=FR_OK) UART1_PrintLn("write fail");
        f_close(&fil);
        
    }
    else UART1_PrintLn("create fail");
    
    //根目录读取
    DIR dir;
    FILINFO fno;
    fr=f_opendir(&dir,"1:");
    if(fr==FR_OK)
    {
        for(;;)
        {
            fr = f_readdir(&dir, &fno);     
                if (fr != FR_OK || fno.fname[0] == 0) 
                {
                    UART1_PrintLn("None");
                    break; 
                }
            UART1_Print("Get file:");
            UART1_PrintLn(fno.fname);
        }
        f_closedir(&dir);
    }
    else
    {
        UART1_PrintDataLn("Opendir fail:",fr);
    }
    */
    
    /* Infinite loop */
    for(;;)
    {
        HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
        osDelay(1000);
    }
  /* USER CODE END StartDefaultTask */
}

/* USER CODE BEGIN Header_StartDebugTask */
/**
* @brief Function implementing the debugTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDebugTask */
void StartDebugTask(void const * argument)
{
  /* USER CODE BEGIN StartDebugTask */
  UART1_PrintLn("Debug Task Started");
    osDelay(750);
    
    /* Infinite loop */
  for(;;)
  {
      HAL_GPIO_TogglePin(LED2_GPIO_Port,LED2_Pin);
    osDelay(200);
  }
  /* USER CODE END StartDebugTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
