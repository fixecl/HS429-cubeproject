#ifndef __TOUCH_H
#define __TOUCH_H

#include "stm32f4xx_hal.h"
#include "main.h"
/*---------------------- IIC相关定义 ------------------------*/

#define TOUCH_RES_OK 1  //IIC响应
#define TOUCH_RES_ERR 0

#define Touch_DelayVaule 10  //通讯延时时间

// IO口相关宏
#define IIC_Touch_SCL_PORT    TOUCH_SCL_HD_GPIO_Port
#define IIC_Touch_SCL_PIN     TOUCH_SCL_HD_Pin

#define IIC_Touch_SDA_PORT    TOUCH_SDA_VD_GPIO_Port
#define IIC_Touch_SDA_PIN     TOUCH_SDA_VD_Pin 

// IO口操作
#define SCL(a)	if (a)	\
					HAL_GPIO_WritePin(TOUCH_SCL_HD_GPIO_Port,TOUCH_SCL_HD_Pin,GPIO_PIN_SET);\
					else		\
					HAL_GPIO_WritePin(TOUCH_SCL_HD_GPIO_Port,TOUCH_SCL_HD_Pin,GPIO_PIN_RESET)

#define SDA(a)	if (a)	\
					HAL_GPIO_WritePin(TOUCH_SDA_VD_GPIO_Port,TOUCH_SDA_VD_Pin,GPIO_PIN_SET);\
					else		\
					HAL_GPIO_WritePin(TOUCH_SDA_VD_GPIO_Port,TOUCH_SDA_VD_Pin,GPIO_PIN_RESET)
	
/*---------------------- GT9XX芯片相关定义 ------------------------*/
					
#define TOUCH_MAX   5	//最大触摸点数

typedef struct 
{
	u8  flag;	//触摸标志位，为1时表示有触摸操作
	u8  num;		//触摸点数
	u16 x[TOUCH_MAX];	//x坐标
	u16 y[TOUCH_MAX];	//y坐标
}TouchStructure;

extern TouchStructure touchInfo;	//结构体声明

#define GT9XX_IIC_RADDR 0xBB	//IIC初始化地址
#define GT9XX_IIC_WADDR 0xBA

#define GT9XX_READ_ADDR 0x814E	//触摸信息寄存器
#define GT9XX_ID_ADDR 0x8140		//触摸面板ID寄存器

HAL_StatusTypeDef TOUCH_Init(void);
void TOUCH_Scan(void);

#endif

