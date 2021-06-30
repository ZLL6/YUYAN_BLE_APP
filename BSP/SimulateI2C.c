
//=============================================================================
//				           �����и����յ�����޹�˾
//                         ���Ӳ�Ʒ��			    
//�ļ�����: SimulateI2C.c
//�ļ���ʶ: 
//ժҪ    : ģ��I2C����
//-----------------------------------------------------------------------------
//��ǰ�汾: V1.0
//����    : dangxinkai
//�������: 2020.10.15
//=============================================================================
//ȡ���汾: 
//ԭ����  : 
//�������:
//=============================================================================

//=============================================================================
//������ͷ�ļ�
//=============================================================================
#include "pbdata.h"
#include "SimulateI2C.h"
#include "stm32f0xx_gpio.h"
//��ȡIIC����ʱ���ϱ�־
u8  sSimI2CErrFlag = 0;//�����־
//CRC����BUF
//static u8 sCrcCal = 0;

u8 crcCal = 0;
u8 crcRev = 0;

u8  gSimI2CErrFlag;//�����־
//=============================================================================
//��̬��������
//=============================================================================
//=============================================================================
//��������: static void SetSCL(u8 channel, BitAction level)
//�������: channel I2Cͨ��	level ��ƽ״̬
//�������: void
//����ֵ  : void
//��������: ����SCL�����ƽ
//ע������: 
//=============================================================================
static void SetSCL( BitAction level);

//=============================================================================
//��������: static void SetSDA(BitAction level)
//�������: channel I2Cͨ��	level ��ƽ״̬
//�������: void
//����ֵ  : void
//��������: ����SDA�����ƽ
//ע������: 
//=============================================================================
static void SetSDA(BitAction level);

//=============================================================================
//��������: static void SetSCLDirect(u8 channel, u8 direct)
//�������: channel I2Cͨ��	direct �����������
//�������: void
//����ֵ  : void
//��������: ����SCL�����������
//ע������: 
//=============================================================================
static void SetSCLDirect(u8 channel, u8 direct);

//=============================================================================
//��������: static void SetSDADirect(u8 channel, u8 direct)
//�������: channel I2Cͨ��	direct �����������
//�������: void
//����ֵ  : void
//��������: ����SDA�����������
//ע������: 
//=============================================================================
static void SetSDADirect(u8 channel, u8 direct);

//=============================================================================
//��������: static u8 GetSCLState(u8 channel)
//�������: channel I2Cͨ��
//�������: void
//����ֵ  : void
//��������: ���SCL��ƽ״̬
//ע������: 
//=============================================================================
static u8 GetSCLState(u8 channel);

//=============================================================================
//��������: static u8 GetSDAState(u8 channel)
//�������: channel I2Cͨ��
//�������: void
//����ֵ  : void
//��������: ���SDA��ƽ״̬
//ע������: 
//=============================================================================
static u8 GetSDAState(u8 channel);

//=============================================================================
//��������: static u8 SimI2CStart(u8 channel)
//�������: void
//�������: void
//����ֵ  : u8// �����ź�
//��������: IIC ��ʼ�ź�
//ע������: 
//=============================================================================
static u8 SimI2CStart(u8 channel);

//=============================================================================
//��������: static u8 SimI2CStop(u8 channel)
//�������: void
//�������: void
//����ֵ  : u8// �����ź�
//��������: IIC ֹͣ�ź�
//ע������: 
//=============================================================================
static u8 SimI2CStop(u8 channel);

//=============================================================================
//��������: static u8 SimI2C_WriteByte(u8 channel, u8 data)
//�������: u8// SimI2C_ReadBufferData  ��Ҫ���͵�����
//�������: void
//����ֵ  :    u8// �����ź�
//��������: IIC ����һBYTE ����
//ע������: 
//=============================================================================
static u8 SimI2C_WriteByte(u8 channel, u8 data);

//=============================================================================
//��������: static u8	SimI2C_ReadByte(u8 ack)	
//�������: u8// ack  ��Ҫ����ACK����NOACK
//�������: void
//����ֵ  :    u8// ��ȡ������
//��������: IIC ��ȡһBYTE ����
//ע������: 
//=============================================================================
static u8	SimI2C_ReadByte(u8 channel, u8 ack);

//==============================================================================
//��������: static u8 CalcByteCRC(u8 Data, u8 InitVal)
//�������: data Ҫ�����CRC���� initVal ��ʼֵ
//�������: void
//����ֵ  : crc����ֵ
//��������:	�����ַ�CRC
//ע������:
//==============================================================================
static u8 CalcByteCRC(u8 data, u8 initVal);

//=============================================================================
//�ӿں�������
//=============================================================================

//=============================================================================
//��������: u8 SimI2C_ReadBuffer_NOCRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IIC������
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_ReadBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
{
		u8	i;
		u8 *pTempBuffer = NULL;
		
		pTempBuffer= pBuffer;

		if(SimI2CStart(channel))  //��ʼ               
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(SimI2C_WriteByte(channel, (devAddr << 1)))   //д�豸��ַ
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(1 == regLen)		//д�Ĵ�����ַ
		{
				if(SimI2C_WriteByte(channel, regAddr))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}
		else
		{
				if(SimI2C_WriteByte(channel, ((regAddr & 0xff00) >> 8)))
				{
						SimI2CStop(channel);
						return FALSE;
				}
				
				if(SimI2C_WriteByte(channel, (regAddr & 0x00ff)))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}
		
		if(SimI2CStart(channel))		//��ʼ
		{
				SimI2CStop(channel);
				return FALSE;
		}//Restart

		if(SimI2C_WriteByte(channel, ((devAddr << 1)+1))) 		//д�豸��ַ
		{
				SimI2CStop(channel);
				return FALSE;
		}//Restart
		
		if(0 == bufLen)          
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		for(i = 0;i < (bufLen - 1);i++)		//������
		{
				*(pTempBuffer++) = SimI2C_ReadByte(channel, SIM_IIC_HOST_ACK);	
				if(gSimI2CErrFlag)        
				{
						SimI2CStop(channel);
						return FALSE;
				}		
		}
		
		*(pTempBuffer) = SimI2C_ReadByte(channel, SIM_IIC_HOST_NOACK);		//�����һ���ֽڲ���Ӧ
		if(gSimI2CErrFlag)            
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		SimI2CStop(channel);		//����

		gSimI2CErrFlag = SIM_IIC_BUS_OK;
		return TRUE;
}

//=============================================================================
//��������:u8	SimI2C_WriteBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IICд����
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_WriteBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
{
		u8	i;
		u8 *pTempBuffer= NULL;

		pTempBuffer= pBuffer;

		if(SimI2CStart(channel))		//��ʼ        
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(SimI2C_WriteByte(channel, (devAddr << 1)))		//д�豸��ַ
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(1 == regLen)		//д�Ĵ�����ַ
		{
				if(SimI2C_WriteByte(channel, regAddr))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}
		else
		{
				if(SimI2C_WriteByte(channel, ((regAddr & 0xff00) >> 8)))
				{
						SimI2CStop(channel);
						return FALSE;
				}
				
				if(SimI2C_WriteByte(channel, (regAddr & 0x00ff)))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}

		for(i = 0;i < bufLen;i++)		//д����
		{
				if(SimI2C_WriteByte(channel ,*(pTempBuffer++)))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}
								
		SimI2CStop(channel);		//����
		return TRUE;
}

//=============================================================================
//��������: u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IIC�����ݴ�CRCУ��
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
{
		u8	i = 0;
		u8  j = 0;

		u8 *pTempBuffer = NULL;
		
		pTempBuffer= pBuffer;

		if(SimI2CStart(channel))  //��ʼ               
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(SimI2C_WriteByte(channel, (devAddr << 1)))   //д�豸��ַ
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(1 == regLen)		//д�Ĵ�����ַ
		{
				if(SimI2C_WriteByte(channel, regAddr))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}
		else
		{
				if(SimI2C_WriteByte(channel, ((regAddr & 0xff00) >> 8)))
				{
						SimI2CStop(channel);
						return FALSE;
				}
				
				if(SimI2C_WriteByte(channel, (regAddr & 0x00ff)))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}
		
		if(SimI2CStart(channel))		//��ʼ
		{
				SimI2CStop(channel);
				return FALSE;
		}//Restart

		if(SimI2C_WriteByte(channel, ((devAddr << 1)+1))) 		//д�豸��ַ
		{
				SimI2CStop(channel);
				return FALSE;
		}//Restart
		
		if(0 == bufLen)          
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		crcCal = CalcByteCRC(((devAddr << 1) | 1), 0);

		for(i = 0; i < bufLen * 2; i++)   
		{
				if(0 == (i % 2))
				{
						*(pTempBuffer + j) = SimI2C_ReadByte(channel, SIM_IIC_HOST_ACK);
						if(gSimI2CErrFlag)            
						{
								SimI2CStop(channel);
								return FALSE;
						}
						j++;
				}
				else
				{
						if(i != (bufLen * 2 - 1))
						{
								crcRev = SimI2C_ReadByte(channel, SIM_IIC_HOST_ACK);
								if(gSimI2CErrFlag)            
								{
										SimI2CStop(channel);
										return FALSE;
								}
						}
						else
						{
								crcRev = SimI2C_ReadByte(channel, SIM_IIC_HOST_NOACK);
								if(gSimI2CErrFlag)            
								{
										SimI2CStop(channel);
										return FALSE;
								}
						}

						crcCal = CalcByteCRC(pTempBuffer[j - 1], crcCal);
						if(crcCal != crcRev)
						{
								SimI2CStop(channel);
								return FALSE;
						}
						crcCal = 0;
				}
		}
		SimI2CStop(channel);		//����
		return TRUE;
}

//=============================================================================
//��������:u8	SimI2C_WriteBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IICд���ݴ�CRCУ��
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_WriteBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
{
		u8	i;
	  u8 crcCal = 0;
		u8 *pTempBuffer= NULL;

		pTempBuffer= pBuffer;

		if(SimI2CStart(channel))		//��ʼ        
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(SimI2C_WriteByte(channel, (devAddr << 1)))		//д�豸��ַ
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(1 == regLen)		//д�Ĵ�����ַ
		{
				if(SimI2C_WriteByte(channel, regAddr))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}
		else
		{
				if(SimI2C_WriteByte(channel, ((regAddr & 0xff00) >> 8)))
				{
						SimI2CStop(channel);
						return FALSE;
				}
				
				if(SimI2C_WriteByte(channel, (regAddr & 0x00ff)))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}

		if(SimI2C_WriteByte(channel, *pTempBuffer))
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		crcCal = CalcByteCRC((devAddr << 1), 0);
		crcCal = CalcByteCRC(regAddr, crcCal);
		crcCal = CalcByteCRC(pTempBuffer[0], crcCal);

		if(SimI2C_WriteByte(channel, crcCal))
		{
				SimI2CStop(channel);
				return FALSE;
		}

		for(i = 1; i < bufLen; i++)
		{	
				if(SimI2C_WriteByte(channel, *(pTempBuffer + i)))
				{
						SimI2CStop(channel);
						return FALSE;
				}

				crcCal = CalcByteCRC(*(pTempBuffer + i), 0);

				if(SimI2C_WriteByte(channel, crcCal))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}
								
		SimI2CStop(channel);		//����
		return TRUE;
}

//=============================================================================
//��̬��������
//=============================================================================

//==============================================================================
//��������: static u8 CalcByteCRC(u8 Data, u8 InitVal)
//�������: data Ҫ�����CRC���� initVal ��ʼֵ
//�������: void
//����ֵ  : crc����ֵ
//��������:	�����ַ�CRC
//ע������:
//==============================================================================
static u8 CalcByteCRC(u8 data, u8 initVal)
{
    u8 i = 0;
    u8 crc = initVal;
    
    crc = initVal;
    crc ^= data;

    for(i = 0; i < 8; i++)
    {
        if(crc & 0x80)
        {
            crc <<= 1;
            crc ^= 0x07;
        }
        else
        {
            crc <<= 1;
        }
    }
    return(crc);
}

//=============================================================================
//��������: static void SetSCL(BitAction level)
//�������: channel I2Cͨ��	level ��ƽ״̬
//�������: void
//����ֵ  : void
//��������: ����SCL�����ƽ
//ע������: 
//=============================================================================
static void SetSCL( BitAction level)
{
//		switch(channel)
//		{
//				case eSIMCHANNEL_1:
					//	HAL_GPIO_WritePin(Sim1_I2C_SCL_GPIO_PORT, Sim1_I2C_SCL_PIN, level);
				GPIO_WriteBit(GPIOB, Sim1_I2C_SCL_PIN, level);				
//						break;
//				case eSIMCHANNEL_2:
//						HAL_GPIO_WritePin(Sim1_I2C_SCL_GPIO_PORT, Sim1_I2C_SCL_PIN, level);
//						break;
//				case eSIMCHANNEL_3:
//						HAL_GPIO_WritePin(Sim1_I2C_SCL_GPIO_PORT, Sim1_I2C_SCL_PIN, level);
//						break;
//				default:
//						break;
//		}
}

//=============================================================================
//��������: static void SetSDA(u8 channel, BitAction level)
//�������: channel I2Cͨ��	level ��ƽ״̬
//�������: void
//����ֵ  : void
//��������: ����SDA�����ƽ
//ע������: 
//=============================================================================
static void SetSDA( BitAction level)
{
	GPIO_WriteBit(GPIOB, Sim1_I2C_SDA_PIN, level);
//		switch(channel)
//		{
//				case eSIMCHANNEL_1:
//						//HAL_GPIO_WritePin(Sim1_I2C_SDA_GPIO_PORT, Sim1_I2C_SDA_PIN, level);
//				GPIO_WriteBit(Sim1_I2C_SDA_GPIO_PORT, Sim1_I2C_SDA_PIN, level);
//						break;
//				case eSIMCHANNEL_2:
//						HAL_GPIO_WritePin(Sim1_I2C_SDA_GPIO_PORT, Sim1_I2C_SDA_PIN, level);
//						break;
//				case eSIMCHANNEL_3:
//						HAL_GPIO_WritePin(Sim1_I2C_SDA_GPIO_PORT, Sim1_I2C_SDA_PIN, level);
//						break;
//				default:
//						break;
//		}
}

//=============================================================================
//��������: static void SetSCLDirect(u8 channel, u8 direct)
//�������: channel I2Cͨ��	direct �����������
//�������: void
//����ֵ  : void
//��������: ����SCL�����������
//ע������: 
//=============================================================================
static void SetSCLDirect(u8 channel, u8 direct)
{
		GPIO_InitTypeDef  GPIO_InitStructure; 
		GPIOMode_TypeDef	Pin_GPIO_Mode;

	
		if(OUT == direct)
		{
				Pin_GPIO_Mode = GPIO_Mode_OUT;
		}
		else
		{
				Pin_GPIO_Mode = GPIO_Mode_IN;   //����ȷ��******************************************************************************************************
		}
		
		switch(channel)
		{
				case eSIMCHANNEL_1:
						GPIO_InitStructure.GPIO_Pin = Sim1_I2C_SCL_PIN;
						GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
						GPIO_InitStructure.GPIO_Mode = Pin_GPIO_Mode;
						GPIO_Init(Sim1_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
						break;
//				case eSIMCHANNEL_2:
//						GPIO_InitStructure.GPIO_Pin = Sim1_I2C_SCL_PIN;
//						GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
//						GPIO_InitStructure.GPIO_Mode = Pin_GPIO_Mode;
//						GPIO_Init(Sim1_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
//						break;
//				case eSIMCHANNEL_3:
//						GPIO_InitStructure.GPIO_Pin = Sim1_I2C_SCL_PIN;
//						GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
//						GPIO_InitStructure.GPIO_Mode = Pin_GPIO_Mode;
//						GPIO_Init(Sim1_I2C_SCL_GPIO_PORT, &GPIO_InitStructure);
//						break;
				default:
						break;
		}
}

//=============================================================================
//��������: static void SetSDADirect(u8 channel, u8 direct)
//�������: channel I2Cͨ��	direct �����������
//�������: void
//����ֵ  : void
//��������: ����SDA�����������
//ע������: 
//=============================================================================
static void SetSDADirect(u8 channel, u8 direct)
{
		GPIO_InitTypeDef  GPIO_InitStructure; 
		GPIOMode_TypeDef	Pin_GPIO_Mode;	
			if(OUT == direct)
		{
				Pin_GPIO_Mode = GPIO_Mode_OUT;
		}
		else
		{
				Pin_GPIO_Mode = GPIO_Mode_IN;   
		}		
		switch(channel)
		{
				case eSIMCHANNEL_1:
						GPIO_InitStructure.GPIO_Pin = Sim1_I2C_SDA_PIN;
						GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Level_3;
						GPIO_InitStructure.GPIO_Mode = Pin_GPIO_Mode;
						GPIO_Init(Sim1_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
						break;
//				case eSIMCHANNEL_2:
//						GPIO_InitStructure.Pin = Sim1_I2C_SDA_PIN;
//						GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
//						GPIO_InitStructure.Mode = Pin_GPIO_Mode;
//						HAL_GPIO_Init(Sim1_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
//						break;
//				case eSIMCHANNEL_3:
//						GPIO_InitStructure.Pin = Sim1_I2C_SDA_PIN;
//						GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
//						GPIO_InitStructure.Mode = Pin_GPIO_Mode;
//						HAL_GPIO_Init(Sim1_I2C_SDA_GPIO_PORT, &GPIO_InitStructure);
//						break;
				default:
						break;
		}
}

//=============================================================================
//��������: static u8 GetSCLDirect(u8 channel)
//�������: channel I2Cͨ��
//�������: void
//����ֵ  : u8 SCL״̬
//��������: ���SCL��ƽ״̬
//ע������: 
//=============================================================================
static u8 GetSCLState(u8 channel)
{
		u8 pinData = Bit_RESET;
	
		switch(channel)
		{
				case eSIMCHANNEL_1:
						pinData = GPIO_ReadInputDataBit(Sim1_I2C_SCL_GPIO_PORT, Sim1_I2C_SCL_PIN);
						break;
//				case eSIMCHANNEL_2:
//						pinData = HAL_GPIO_ReadPin(Sim1_I2C_SCL_GPIO_PORT, Sim1_I2C_SCL_PIN);
//						break;
//				case eSIMCHANNEL_3:
//						pinData = HAL_GPIO_ReadPin(Sim1_I2C_SCL_GPIO_PORT, Sim1_I2C_SCL_PIN);
//						break;
				default:
						break;
		}
		
		return pinData;
}

//=============================================================================
//��������: static u8 GetSDADirect(u8 channel)
//�������: channel I2Cͨ��
//�������: void
//����ֵ  : u8 SDA״̬
//��������: ���SDA��ƽ״̬
//ע������: 
//=============================================================================
static u8 GetSDAState(u8 channel)
{
		u8 pinData = Bit_RESET;
	
		switch(channel)
		{
				case eSIMCHANNEL_1:
						pinData = GPIO_ReadInputDataBit(Sim1_I2C_SDA_GPIO_PORT, Sim1_I2C_SDA_PIN);
						break;
//				case eSIMCHANNEL_2:
//						pinData = HAL_GPIO_ReadPin(Sim1_I2C_SDA_GPIO_PORT, Sim1_I2C_SDA_PIN);
//						break;
//				case eSIMCHANNEL_3:
//						pinData = HAL_GPIO_ReadPin(Sim1_I2C_SDA_GPIO_PORT, Sim1_I2C_SDA_PIN);
//						break;
				default:
						break;
		}
		
		return pinData;
}

//=============================================================================
//��������: static u8 SimI2CStart(u8 channel)
//�������: void
//�������: void
//����ֵ  : u8// �����ź�
//��������: IIC ��ʼ�ź�
//ע������: 
//=============================================================================
static u8 SimI2CStart(u8 channel)
{
		u16 count;
		
		SetSCL(Bit_SET);
		SetSDA(Bit_SET);
		SetSCLDirect(channel, IN);
		count=0;
		while(!GetSCLState(channel))	//����SCL
		{
				count++;
				if(count>=Time10MS)
				{
						return SIM_IIC_START_BUSY_SCL;
				}
		}
		SetSDADirect(channel, IN);
		count=0;
		while(!GetSDAState(channel))	//����SDA
		{
				count++;
				if(count>=Time10MS)
				{
						return SIM_IIC_START_BUSY_SDA;
				}
		}
			
		Delay_50us();
		SetSDA( Bit_RESET);
		SetSDADirect(channel, OUT);		//����SDA
		Delay_50us();
		SetSCL( Bit_RESET);
		SetSCLDirect(channel, OUT);		//����SCL
		Delay_50us();
		return SIM_IIC_BUS_OK;
}

//=============================================================================
//��������: static u8 SimI2CStop(u8 channel)
//�������: void
//�������: void
//����ֵ  : u8// �����ź�
//��������: IIC ֹͣ�ź�
//ע������: 
//=============================================================================
static u8 SimI2CStop(u8 channel)
{

		u16 count;
		
		SetSCL(Bit_RESET);		//����SCL
		SetSCLDirect(channel, OUT);
		SetSDA( Bit_RESET);		//����SDA
		SetSDADirect(channel, OUT);

		Delay_50us();
		SetSCL(Bit_SET);			//����SCL
		Delay_50us();
		SetSDA( Bit_SET);			//����SDA
		Delay_50us();
		SetSCLDirect(channel,IN);
		count = 0;
		while(!GetSCLState(channel))
		{
				count++;
				if(count >= Time10MS)
				{
						return SIM_IIC_STOP_BUSY_SDA;
				}
		}
		
		SetSDADirect(channel, IN);
		count=0;
		while(!GetSDAState(channel))
		{
				count++;
				if(count>=Time10MS)
				{
						return SIM_IIC_STOP_BUSY_SDA;
				}
		}
		
		return SIM_IIC_BUS_OK;
}

//=============================================================================
//��������: static u8 SimI2C_WriteByte(u8 channel, u8 data)
//�������: u8// SimI2C_ReadBufferData  ��Ҫ���͵�����
//�������: void
//����ֵ  :    u8// �����ź�
//��������: IIC ����һBYTE ����
//ע������: 
//=============================================================================
static u8 SimI2C_WriteByte(u8 channel, u8 data)
{

		u8	i;
		u16	count;
		
//		if(channel >= eSIMCH_NUM)
//		{
//				return SIM_IIC_BUS_ERR;
//		}
		
		sSimI2CErrFlag = SIM_IIC_BUS_OK;
		SetSDA(Bit_RESET);
		SetSDADirect(channel, OUT);
		SetSCL(Bit_RESET);
		SetSCLDirect(channel, OUT);

		for(i=0;i<8;i++)
		{
				if(data >= 0x80)	
				{
						SetSDA(Bit_SET);
				}
				else
				{
						SetSDA(Bit_RESET);
				}						
				Delay_8us();
				SetSCLDirect(channel,IN);
				count=0;
				while(!GetSCLState(channel))
				{
						count++;
						if(count >= Time10MS)
						{
								sSimI2CErrFlag = SIM_IIC_TX_BUSY_SCL;
								return SIM_IIC_TX_BUSY_SCL;
						}
				}
			
			//	Delay_10us();
				Delay_10us();

				SetSCL(Bit_RESET);
				SetSCLDirect(channel, OUT);
				data <<= 1;	
				Delay_8us();
		}
		SetSDADirect(channel, IN);
		Delay_10us();

		SetSCLDirect(channel,IN);
		count=0;
		while(!GetSCLState(channel))
		{
				count++;
				if(count >= Time10MS)
				{
						sSimI2CErrFlag = SIM_IIC_TX_BUSY_SCL;
						return SIM_IIC_TX_BUSY_SCL;
				}
		}
		count=0;
		if(eSIMCHANNEL_1 == channel )
		{
			while(GetSDAState(channel))
			{	
					count++;
					if(count>=Time10MS)
					{
							sSimI2CErrFlag = SIM_IIC_TX_NO_ACK;
							return SIM_IIC_TX_NO_ACK;
					}
			}
		}
		
		Delay_10us();
//		Delay_10us();
//		Delay_10us();
//		Delay_10us();
//		Delay_10us();
//		Delay_10us();
		SetSCL(Bit_RESET);
		SetSCLDirect(channel, OUT);
		Delay_50us();
		return SIM_IIC_BUS_OK;	
}

//=============================================================================
//��������: static u8	SimI2C_ReadByte(u8 channel, u8 ack)	
//�������: u8// ack  ��Ҫ����ACK����NOACK
//�������: void
//����ֵ  :    u8// ��ȡ������
//��������: IIC ��ȡһBYTE ����
//ע������: 
//=============================================================================
static u8	SimI2C_ReadByte(u8 channel, u8 ack)		
{
		u8	index;
		u8  rcv_iicdata;
		u16 count;
		
//		if(channel >= eSIMCH_NUM)
//		{
//				return SIM_IIC_BUS_ERR;
//		}
		
		SetSDADirect(channel, IN);
		SetSCL( Bit_RESET);
		SetSCLDirect(channel, OUT);
		Delay_10us();
		
		sSimI2CErrFlag = SIM_IIC_BUS_OK;
		for(index = 0;index < 8;index++)
		{
				rcv_iicdata <<= 1;
				SetSCLDirect(channel,IN);
				count = 0;
				while(!GetSCLState(channel))
				{
						count++;
						if(count >= Time10MS)
						{
								sSimI2CErrFlag = SIM_IIC_RX_BUSY_SCL;
								return SIM_IIC_RX_BUSY_SCL;
						}
				}
				count = 0;
				Delay_8us();
				if(GetSDAState(channel))
				{
						rcv_iicdata |= 0x01;
				}
				else
				{
						rcv_iicdata &= 0xfe;
				}
				Delay_8us();
				SetSCL(Bit_RESET);
				SetSCLDirect(channel, OUT);
				Delay_10us();
				Delay_10us();
			
		}
		if(ack==0)
		{
				SetSDA(Bit_RESET);
		}
		else
		{
				SetSDA(Bit_SET);
		}
		SetSDADirect(channel, OUT);
		Delay_10us();
		
		SetSCLDirect(channel,IN);
		while(!GetSCLState(channel))
		{
				count++;
				if(count>=Time10MS)
				{
						SetSCL(Bit_RESET);
						SetSCLDirect(channel, OUT);
						sSimI2CErrFlag = SIM_IIC_RX_BUSY_SCL;
						return SIM_IIC_RX_BUSY_SCL;
				}
		}
		count=0;
		Delay_10us();
		SetSCL(Bit_RESET);
		SetSCLDirect(channel, OUT);
		Delay_50us();	
		
		return rcv_iicdata;
}






