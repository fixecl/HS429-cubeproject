#include "touch.h"
//#include "usart.h"

TouchStructure touchInfo; //������Ϣ�ṹ��

/*---------------------- IIC��غ��� ------------------------*/

// ����: ����ʱ����
//	˵����Ϊ����ֲ�ļ�����Ҷ���ʱ���Ȳ��ߣ����Բ���Ҫ���ж�ʱ����ʱ
//
void IIC_Touch_Delay(u16 a)
{
	int i;
	while (a --)
	{
		for (i = 0; i < 10; i++);
	}
}

//	����������IIC�����ݽ�Ϊ���ģʽ
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

//	����������IIC�����ݽ�Ϊ����ģʽ
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

//	������IIC��ʼ�ź�
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

//	������IICֹͣ�ź�
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

//	������IICӦ���ź�
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

//	������IIC��Ӧ���ź�
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

//	�������ȴ��豸������Ӧ�ͺ�
//
u8 IIC_Touch_WaitResponse(void)
{

	SCL(0);
	IIC_Touch_Delay( Touch_DelayVaule );
	SDA(1);
	IIC_Touch_Delay( Touch_DelayVaule );
	SCL(1);

	IIC_Touch_SDA_In();	//����Ϊ����ģʽ
	IIC_Touch_Delay( Touch_DelayVaule );
	
	SCL(0);	
	if( HAL_GPIO_ReadPin(IIC_Touch_SDA_PORT,IIC_Touch_SDA_PIN) != 0) //�ж��豸�Ƿ���������Ӧ
	{		
		return (TOUCH_RES_ERR);
	}
	else
	{
		return (TOUCH_RES_OK);
	}

}

// ������IICд�ֽ�
//	������IIC_Data - Ҫд���8λ����
//	���أ��豸����Ӧ�򷵻� 1������Ϊ0
//
u8 IIC_Touch_WriteByte(u8 IIC_Data)
{
	u8 i;

	IIC_Touch_SDA_Out(); //���ݽ�Ϊ���ģʽ
	
	for (i = 0; i < 8; i++)
	{
		SDA(IIC_Data & 0x80);
		
		IIC_Touch_Delay( Touch_DelayVaule );
		SCL(1);
		IIC_Touch_Delay( Touch_DelayVaule );
		SCL(0);		
		
		IIC_Data <<= 1;
	}

	return (IIC_Touch_WaitResponse()); //�ȴ��豸��Ӧ
}

// ������IIC���ֽ�
//	������ResponseMode - Ӧ��ģʽѡ��
//       ResponseMode = 1 ʱ��CPU������Ӧ�źţ�Ϊ 0 ʱ��CPU������Ӧ���ź�
//	���أ�����������
//
u8 IIC_Touch_ReadByte(u8 ResponseMode)
{
	u8 IIC_Data;
	u8 i;
	
	SDA(1);
	SCL(0);

	IIC_Touch_SDA_In(); //����ģʽ
	
	//��һ�ֽ�����
	for (i = 0; i < 8; i++)
	{
		IIC_Data <<= 1;
		
		SCL(1);
		IIC_Touch_Delay( Touch_DelayVaule );

		IIC_Data |= (HAL_GPIO_ReadPin(IIC_Touch_SDA_PORT,IIC_Touch_SDA_PIN) & 0x01);
		
		SCL(0);
		IIC_Touch_Delay( Touch_DelayVaule );
	}

	//	������Ӧ�ź�
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

//	��������ʼ��IIC��GPIO��
//
void IIC_Touch_GPIO_Config (void)
{
}

/*---------------------- GT9XX��غ��� ------------------------*/

// ������GT9XX д����
//	������addr - Ҫ�����ļĴ���
//			
u8 GT9XX_WriteHandle (u16 addr)
{
	u8 status;

	IIC_Touch_Start();
	if( IIC_Touch_WriteByte(GT9XX_IIC_WADDR) == TOUCH_RES_OK ) //д����ָ��
	{
		if( IIC_Touch_WriteByte((u8)(addr >> 8)) == TOUCH_RES_OK ) //д��16λ��ַ
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

// ������GT9XX д����
//	������addr - Ҫд���ݵĵ�ַ
//			value - д�������
//
u8 GT9XX_WriteData (u16 addr,u8 value)
{
	u8 status;
	
	IIC_Touch_Start(); //����IICͨѶ

	if( GT9XX_WriteHandle(addr) == SUCCESS)	//д��Ҫ�����ļĴ���
	{
		if (IIC_Touch_WriteByte(value) != TOUCH_RES_OK) //д����
		{
			status = ERROR;						
		}
	}	
	IIC_Touch_Stop(); //ֹͣͨѶ
	
	status = SUCCESS;
	return status;
}

// ������GT9XX ������
//	������addr - Ҫ�����ݵĵ�ַ
//			num - �������ֽ���
//			*value - ���ڻ�ȡ�洢���ݵ��׵�ַ
//
u8 GT9XX_ReadData (u16 addr, u8 cnt, u8 *value)
{
	u8 status;
	u8 i;

	status = ERROR;
	IIC_Touch_Start();

	if( GT9XX_WriteHandle(addr) == SUCCESS) //д��Ҫ�����ļĴ���
	{
		IIC_Touch_Start(); //��������IICͨѶ

		if (IIC_Touch_WriteByte(GT9XX_IIC_RADDR) == TOUCH_RES_OK)
		{	
			for(i = 0 ; i < cnt; i++)
			{
				if (i == (cnt - 1))
				{
					value[i] = IIC_Touch_ReadByte(0);//�������һ������ʱ���� ��Ӧ���ź�
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

// ����: ��������ʼ��
//	���أ�1 - ��ʼ���ɹ��� 0 - ����δ��⵽������	
//
HAL_StatusTypeDef TOUCH_Init(void)
{
	u8 touchIC_ID[4];	

	IIC_Touch_GPIO_Config(); //��ʼ��IIC����
	
	GT9XX_ReadData (GT9XX_ID_ADDR,4,touchIC_ID);	//��ID��Ϣ
	
	if( touchIC_ID[0] == '9' )	//�жϵ�һ���ַ��Ƿ�Ϊ 9
	{
		//printf("Touch ID: %s \r\n",touchIC_ID);	//��ӡ����оƬ��ID
        //UART1_PrintDataLn("TouchID:",touchIC_ID[0]);
		return HAL_OK;
	}
	else
	{
		//printf("Touch Error\r\n");	//����δ��⵽������
		return HAL_ERROR;
	}
}

// ����������ɨ��
//	˵�����ڳ����������Եĵ��øú��������Լ�ⴥ������
//
void TOUCH_Scan(void)
{
 	u8  touchData[2 + 8 * TOUCH_MAX ]; //���ڴ洢��������

	GT9XX_ReadData (GT9XX_READ_ADDR,2 + 8 * TOUCH_MAX ,touchData);	//������
	GT9XX_WriteData (GT9XX_READ_ADDR,0);	//	�������оƬ�ļĴ�����־λ
	touchInfo.num = touchData[0] & 0x0f;	//ȡ��ǰ�Ĵ�������
	
	if ( (touchInfo.num >= 1) && (touchInfo.num <=5) ) //���������� 1-5 ֮��ʱ
	{
		// ȡ��Ӧ�Ĵ�������
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
