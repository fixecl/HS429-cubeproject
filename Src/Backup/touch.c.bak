#include "touch.h"
//#include "usart.h"

TouchStructure touchInfo; //触摸信息结构体

/*---------------------- IIC相关函数 ------------------------*/

// 函数: 简单延时函数
//	说明：为了移植的简便性且对延时精度不高，所以不需要才有定时器延时
//
void IIC_Touch_Delay(u16 a)
{
	int i;
	while (a --)
	{
		for (i = 0; i < 10; i++);
	}
}

//	函数：配置IIC的数据脚为输出模式
//
void IIC_Touch_SDA_Out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;       
	GPIO_InitStructure.Pull = GPIO_PULLUP;		   	
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStructure.Pin = TOUCH_SDA_VD_Pin;  	
	HAL_GPIO_Init(IIC_Touch_SDA_PORT, &GPIO_InitStructure);		
}

//	函数：配置IIC的数据脚为输入模式
//
void IIC_Touch_SDA_In(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.Mode = GPIO_MODE_INPUT;   
	GPIO_InitStructure.Pull = GPIO_PULLUP;	
 	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	
	GPIO_InitStructure.Pin = TOUCH_SDA_VD_Pin ;
	HAL_GPIO_Init(IIC_Touch_SDA_PORT, &GPIO_InitStructure);
}

//	函数：IIC起始信号
//
void IIC_Touch_Start(void)
{
	IIC_Touch_SDA_Out();
	
	SDA(1);
	SCL(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	
	SDA(0);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(0);
	IIC_Touch_Delay( Touch_DelayVaule );
}

//	函数：IIC停止信号
//
void IIC_Touch_Stop(void)
{
    SCL(0);
    IIC_Touch_Delay( Touch_DelayVaule );
    SDA(0);
    IIC_Touch_Delay( Touch_DelayVaule );
    SCL(1);
    IIC_Touch_Delay( Touch_DelayVaule );
    SDA(1);
    IIC_Touch_Delay( Touch_DelayVaule );
}

//	函数：IIC应答信号
//
void IIC_Touch_Response(void)
{
	IIC_Touch_SDA_Out();

	SDA(0);
	IIC_Touch_Delay( Touch_DelayVaule );	
	SCL(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(0);
	IIC_Touch_Delay( Touch_DelayVaule );
}

//	函数：IIC非应答信号
//
void IIC_Touch_NoResponse(void)
{
	IIC_Touch_SDA_Out();
	
	SCL(0);	
	IIC_Touch_Delay( Touch_DelayVaule );
	SDA(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(0);
	IIC_Touch_Delay( Touch_DelayVaule );
}

//	函数：等待设备发出回应型号
//
u8 IIC_Touch_WaitResponse(void)
{

	SCL(0);
	IIC_Touch_Delay( Touch_DelayVaule );
	SDA(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(1);

	IIC_Touch_SDA_In();	//配置为输入模式
	IIC_Touch_Delay( Touch_DelayVaule );
	
	SCL(0);	
	if( HAL_GPIO_ReadPin(IIC_Touch_SDA_PORT,IIC_Touch_SDA_PIN) != 0) //判断设备是否有做出响应
	{		
		return (TOUCH_RES_ERR);
	}
	else
	{
		return (TOUCH_RES_OK);
	}

}

// 函数：IIC写字节
//	参数：IIC_Data - 要写入的8位数据
//	返回：设备有响应则返回 1，否则为0
//
u8 IIC_Touch_WriteByte(u8 IIC_Data)
{
	u8 i;

	IIC_Touch_SDA_Out(); //数据脚为输出模式
	
	for (i = 0; i < 8; i++)
	{
		SDA(IIC_Data & 0x80);
		
		IIC_Touch_Delay( Touch_DelayVaule );
		SCL(1);
		IIC_Touch_Delay( Touch_DelayVaule );
		SCL(0);		
		
		IIC_Data <<= 1;
	}

	return (IIC_Touch_WaitResponse()); //等待设备响应
}

// 函数：IIC读字节
//	参数：ResponseMode - 应答模式选择
//       ResponseMode = 1 时，CPU发出响应信号；为 0 时，CPU发出非应答信号
//	返回：读出的数据
//
u8 IIC_Touch_ReadByte(u8 ResponseMode)
{
	u8 IIC_Data;
	u8 i;
	
	SDA(1);
	SCL(0);

	IIC_Touch_SDA_In(); //输入模式
	
	//读一字节数据
	for (i = 0; i < 8; i++)
	{
		IIC_Data <<= 1;
		
		SCL(1);
		IIC_Touch_Delay( Touch_DelayVaule );

		IIC_Data |= (HAL_GPIO_ReadPin(IIC_Touch_SDA_PORT,IIC_Touch_SDA_PIN) & 0x01);
		
		SCL(0);
		IIC_Touch_Delay( Touch_DelayVaule );
	}

	//	做出相应信号
	if (ResponseMode)
	{
		IIC_Touch_Response();
	}
	else
	{
		IIC_Touch_NoResponse();
	}
	
	return (IIC_Data); 
}

//	函数：初始化IIC的GPIO口
//
void IIC_Touch_GPIO_Config (void)
{
}

/*---------------------- GT9XX相关函数 ------------------------*/

// 函数：GT9XX 写操作
//	参数：addr - 要操作的寄存器
//			
u8 GT9XX_WriteHandle (u16 addr)
{
	u8 status;

	IIC_Touch_Start();
	if( IIC_Touch_WriteByte(GT9XX_IIC_WADDR) == TOUCH_RES_OK ) //写数据指令
	{
		if( IIC_Touch_WriteByte((u8)(addr >> 8)) == TOUCH_RES_OK ) //写入16位地址
		{
			if( IIC_Touch_WriteByte((u8)(addr)) != TOUCH_RES_OK )
			{
				status = ERROR;
			}			
		}
	}
	status = SUCCESS;
	return status;	
}

// 函数：GT9XX 写数据
//	参数：addr - 要写数据的地址
//			value - 写入的数据
//
u8 GT9XX_WriteData (u16 addr,u8 value)
{
	u8 status;
	
	IIC_Touch_Start(); //启动IIC通讯

	if( GT9XX_WriteHandle(addr) == SUCCESS)	//写入要操作的寄存器
	{
		if (IIC_Touch_WriteByte(value) != TOUCH_RES_OK) //写数据
		{
			status = ERROR;						
		}
	}	
	IIC_Touch_Stop(); //停止通讯
	
	status = SUCCESS;
	return status;
}

// 函数：GT9XX 读数据
//	参数：addr - 要读数据的地址
//			num - 读出的字节数
//			*value - 用于获取存储数据的首地址
//
u8 GT9XX_ReadData (u16 addr, u8 cnt, u8 *value)
{
	u8 status;
	u8 i;

	status = ERROR;
	IIC_Touch_Start();

	if( GT9XX_WriteHandle(addr) == SUCCESS) //写入要操作的寄存器
	{
		IIC_Touch_Start(); //重新启动IIC通讯

		if (IIC_Touch_WriteByte(GT9XX_IIC_RADDR) == TOUCH_RES_OK)
		{	
			for(i = 0 ; i < cnt; i++)
			{
				if (i == (cnt - 1))
				{
					value[i] = IIC_Touch_ReadByte(0);//读到最后一个数据时发送 非应答信号
				}
				else
				{
					value[i] = IIC_Touch_ReadByte(1);
				}
			}					
			IIC_Touch_Stop();
			status = SUCCESS;
		}
	}
	IIC_Touch_Stop();
	return (status);	
}

// 函数: 触摸屏初始化
//	返回：1 - 初始化成功， 0 - 错误，未检测到触摸屏	
//
HAL_StatusTypeDef TOUCH_Init(void)
{
	u8 touchIC_ID[4];	

	IIC_Touch_GPIO_Config(); //初始化IIC引脚
	
	GT9XX_ReadData (GT9XX_ID_ADDR,4,touchIC_ID);	//读ID信息
	
	if( touchIC_ID[0] == '9' )	//判断第一个字符是否为 9
	{
		//printf("Touch ID: %s \r\n",touchIC_ID);	//打印触摸芯片的ID
        //UART1_PrintDataLn("TouchID:",touchIC_ID[0]);
		return HAL_OK;
	}
	else
	{
		//printf("Touch Error\r\n");	//错误，未检测到触摸屏
		return HAL_ERROR;
	}
}

// 函数：触摸扫描
//	说明：在程序里周期性的调用该函数，用以检测触摸操作
//
void TOUCH_Scan(void)
{
 	u8  touchData[2 + 8 * TOUCH_MAX ]; //用于存储触摸数据

	GT9XX_ReadData (GT9XX_READ_ADDR,2 + 8 * TOUCH_MAX ,touchData);	//读数据
	GT9XX_WriteData (GT9XX_READ_ADDR,0);	//	清除触摸芯片的寄存器标志位
	touchInfo.num = touchData[0] & 0x0f;	//取当前的触摸点数
	
	if ( (touchInfo.num >= 1) && (touchInfo.num <=5) ) //当触摸数在 1-5 之间时
	{
		// 取相应的触摸坐标
		switch(touchInfo.num)
		{
			case 5:
			{
				touchInfo.y[4] = ((touchData[5+32]<<8) | touchData[4+32])*0.80;
				touchInfo.x[4] = ((touchData[3+32]<<8) | touchData[2+32])*0.78;	
     		}
			case 4:
			{
				touchInfo.y[3] = ((touchData[5+24]<<8) | touchData[4+24])*0.80;
				touchInfo.x[3] = ((touchData[3+24]<<8) | touchData[2+24])*0.78;		
			}
			case 3:
			{
				touchInfo.y[2] = ((touchData[5+16]<<8) | touchData[4+16])*0.80;
				touchInfo.x[2] = ((touchData[3+16]<<8) | touchData[2+16])*0.78;				
			}
			case 2:
			{
				touchInfo.y[1] = ((touchData[5+8]<<8) | touchData[4+8])*0.80;
				touchInfo.x[1] = ((touchData[3+8]<<8) | touchData[2+8])*0.78;						
			}
			case 1:
			{
				touchInfo.y[0] = ((touchData[5]<<8) | touchData[4])*0.80;
				touchInfo.x[0] = ((touchData[3]<<8) | touchData[2])*0.78;					
			}
			default:break;
		}	
		touchInfo.flag = 1;		
	}
	else                       
	{
		touchInfo.flag = 0;
	}
}
