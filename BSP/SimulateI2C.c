
//=============================================================================
//				           深圳市格瑞普电池有限公司
//                         电子产品部			    
//文件名称: SimulateI2C.c
//文件标识: 
//摘要    : 模拟I2C驱动
//-----------------------------------------------------------------------------
//当前版本: V1.0
//作者    : dangxinkai
//完成日期: 2020.10.15
//=============================================================================
//取代版本: 
//原作者  : 
//完成日期:
//=============================================================================

//=============================================================================
//包含的头文件
//=============================================================================
#include "pbdata.h"
#include "SimulateI2C.h"
#include "stm32f0xx_gpio.h"
//读取IIC数据时故障标志
u8  sSimI2CErrFlag = 0;//错误标志
//CRC计算BUF
//static u8 sCrcCal = 0;

u8 crcCal = 0;
u8 crcRev = 0;

u8  gSimI2CErrFlag;//错误标志
//=============================================================================
//静态函数声明
//=============================================================================
//=============================================================================
//函数名称: static void SetSCL(u8 channel, BitAction level)
//输入参数: channel I2C通道	level 电平状态
//输出参数: void
//返回值  : void
//功能描述: 设置SCL输出电平
//注意事项: 
//=============================================================================
static void SetSCL( BitAction level);

//=============================================================================
//函数名称: static void SetSDA(BitAction level)
//输入参数: channel I2C通道	level 电平状态
//输出参数: void
//返回值  : void
//功能描述: 设置SDA输出电平
//注意事项: 
//=============================================================================
static void SetSDA(BitAction level);

//=============================================================================
//函数名称: static void SetSCLDirect(u8 channel, u8 direct)
//输入参数: channel I2C通道	direct 输入输出方向
//输出参数: void
//返回值  : void
//功能描述: 设置SCL输入输出方向
//注意事项: 
//=============================================================================
static void SetSCLDirect(u8 channel, u8 direct);

//=============================================================================
//函数名称: static void SetSDADirect(u8 channel, u8 direct)
//输入参数: channel I2C通道	direct 输入输出方向
//输出参数: void
//返回值  : void
//功能描述: 设置SDA输入输出方向
//注意事项: 
//=============================================================================
static void SetSDADirect(u8 channel, u8 direct);

//=============================================================================
//函数名称: static u8 GetSCLState(u8 channel)
//输入参数: channel I2C通道
//输出参数: void
//返回值  : void
//功能描述: 获得SCL电平状态
//注意事项: 
//=============================================================================
static u8 GetSCLState(u8 channel);

//=============================================================================
//函数名称: static u8 GetSDAState(u8 channel)
//输入参数: channel I2C通道
//输出参数: void
//返回值  : void
//功能描述: 获得SDA电平状态
//注意事项: 
//=============================================================================
static u8 GetSDAState(u8 channel);

//=============================================================================
//函数名称: static u8 SimI2CStart(u8 channel)
//输入参数: void
//输出参数: void
//返回值  : u8// 错误信号
//功能描述: IIC 起始信号
//注意事项: 
//=============================================================================
static u8 SimI2CStart(u8 channel);

//=============================================================================
//函数名称: static u8 SimI2CStop(u8 channel)
//输入参数: void
//输出参数: void
//返回值  : u8// 错误信号
//功能描述: IIC 停止信号
//注意事项: 
//=============================================================================
static u8 SimI2CStop(u8 channel);

//=============================================================================
//函数名称: static u8 SimI2C_WriteByte(u8 channel, u8 data)
//输入参数: u8// SimI2C_ReadBufferData  需要发送的数据
//输出参数: void
//返回值  :    u8// 错误信号
//功能描述: IIC 发送一BYTE 数据
//注意事项: 
//=============================================================================
static u8 SimI2C_WriteByte(u8 channel, u8 data);

//=============================================================================
//函数名称: static u8	SimI2C_ReadByte(u8 ack)	
//输入参数: u8// ack  需要发送ACK或者NOACK
//输出参数: void
//返回值  :    u8// 读取的数据
//功能描述: IIC 读取一BYTE 数据
//注意事项: 
//=============================================================================
static u8	SimI2C_ReadByte(u8 channel, u8 ack);

//==============================================================================
//函数名称: static u8 CalcByteCRC(u8 Data, u8 InitVal)
//输入参数: data 要计算的CRC数据 initVal 初始值
//输出参数: void
//返回值  : crc计算值
//功能描述:	计算字符CRC
//注意事项:
//==============================================================================
static u8 CalcByteCRC(u8 data, u8 initVal);

//=============================================================================
//接口函数定义
//=============================================================================

//=============================================================================
//函数名称: u8 SimI2C_ReadBuffer_NOCRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC读数据
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_ReadBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
{
		u8	i;
		u8 *pTempBuffer = NULL;
		
		pTempBuffer= pBuffer;

		if(SimI2CStart(channel))  //开始               
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(SimI2C_WriteByte(channel, (devAddr << 1)))   //写设备地址
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(1 == regLen)		//写寄存器地址
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
		
		if(SimI2CStart(channel))		//开始
		{
				SimI2CStop(channel);
				return FALSE;
		}//Restart

		if(SimI2C_WriteByte(channel, ((devAddr << 1)+1))) 		//写设备地址
		{
				SimI2CStop(channel);
				return FALSE;
		}//Restart
		
		if(0 == bufLen)          
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		for(i = 0;i < (bufLen - 1);i++)		//读数据
		{
				*(pTempBuffer++) = SimI2C_ReadByte(channel, SIM_IIC_HOST_ACK);	
				if(gSimI2CErrFlag)        
				{
						SimI2CStop(channel);
						return FALSE;
				}		
		}
		
		*(pTempBuffer) = SimI2C_ReadByte(channel, SIM_IIC_HOST_NOACK);		//读最后一个字节不响应
		if(gSimI2CErrFlag)            
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		SimI2CStop(channel);		//结束

		gSimI2CErrFlag = SIM_IIC_BUS_OK;
		return TRUE;
}

//=============================================================================
//函数名称:u8	SimI2C_WriteBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC写数据
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_WriteBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
{
		u8	i;
		u8 *pTempBuffer= NULL;

		pTempBuffer= pBuffer;

		if(SimI2CStart(channel))		//开始        
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(SimI2C_WriteByte(channel, (devAddr << 1)))		//写设备地址
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(1 == regLen)		//写寄存器地址
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

		for(i = 0;i < bufLen;i++)		//写数据
		{
				if(SimI2C_WriteByte(channel ,*(pTempBuffer++)))
				{
						SimI2CStop(channel);
						return FALSE;
				}
		}
								
		SimI2CStop(channel);		//结束
		return TRUE;
}

//=============================================================================
//函数名称: u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC读数据带CRC校验
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
{
		u8	i = 0;
		u8  j = 0;

		u8 *pTempBuffer = NULL;
		
		pTempBuffer= pBuffer;

		if(SimI2CStart(channel))  //开始               
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(SimI2C_WriteByte(channel, (devAddr << 1)))   //写设备地址
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(1 == regLen)		//写寄存器地址
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
		
		if(SimI2CStart(channel))		//开始
		{
				SimI2CStop(channel);
				return FALSE;
		}//Restart

		if(SimI2C_WriteByte(channel, ((devAddr << 1)+1))) 		//写设备地址
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
		SimI2CStop(channel);		//结束
		return TRUE;
}

//=============================================================================
//函数名称:u8	SimI2C_WriteBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC写数据带CRC校验
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_WriteBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
{
		u8	i;
	  u8 crcCal = 0;
		u8 *pTempBuffer= NULL;

		pTempBuffer= pBuffer;

		if(SimI2CStart(channel))		//开始        
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(SimI2C_WriteByte(channel, (devAddr << 1)))		//写设备地址
		{
				SimI2CStop(channel);
				return FALSE;
		}
		
		if(1 == regLen)		//写寄存器地址
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
								
		SimI2CStop(channel);		//结束
		return TRUE;
}

//=============================================================================
//静态函数定义
//=============================================================================

//==============================================================================
//函数名称: static u8 CalcByteCRC(u8 Data, u8 InitVal)
//输入参数: data 要计算的CRC数据 initVal 初始值
//输出参数: void
//返回值  : crc计算值
//功能描述:	计算字符CRC
//注意事项:
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
//函数名称: static void SetSCL(BitAction level)
//输入参数: channel I2C通道	level 电平状态
//输出参数: void
//返回值  : void
//功能描述: 设置SCL输出电平
//注意事项: 
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
//函数名称: static void SetSDA(u8 channel, BitAction level)
//输入参数: channel I2C通道	level 电平状态
//输出参数: void
//返回值  : void
//功能描述: 设置SDA输出电平
//注意事项: 
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
//函数名称: static void SetSCLDirect(u8 channel, u8 direct)
//输入参数: channel I2C通道	direct 输入输出方向
//输出参数: void
//返回值  : void
//功能描述: 设置SCL输入输出方向
//注意事项: 
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
				Pin_GPIO_Mode = GPIO_Mode_IN;   //这块待确认******************************************************************************************************
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
//函数名称: static void SetSDADirect(u8 channel, u8 direct)
//输入参数: channel I2C通道	direct 输入输出方向
//输出参数: void
//返回值  : void
//功能描述: 设置SDA输入输出方向
//注意事项: 
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
//函数名称: static u8 GetSCLDirect(u8 channel)
//输入参数: channel I2C通道
//输出参数: void
//返回值  : u8 SCL状态
//功能描述: 获得SCL电平状态
//注意事项: 
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
//函数名称: static u8 GetSDADirect(u8 channel)
//输入参数: channel I2C通道
//输出参数: void
//返回值  : u8 SDA状态
//功能描述: 获得SDA电平状态
//注意事项: 
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
//函数名称: static u8 SimI2CStart(u8 channel)
//输入参数: void
//输出参数: void
//返回值  : u8// 错误信号
//功能描述: IIC 起始信号
//注意事项: 
//=============================================================================
static u8 SimI2CStart(u8 channel)
{
		u16 count;
		
		SetSCL(Bit_SET);
		SetSDA(Bit_SET);
		SetSCLDirect(channel, IN);
		count=0;
		while(!GetSCLState(channel))	//拉高SCL
		{
				count++;
				if(count>=Time10MS)
				{
						return SIM_IIC_START_BUSY_SCL;
				}
		}
		SetSDADirect(channel, IN);
		count=0;
		while(!GetSDAState(channel))	//拉高SDA
		{
				count++;
				if(count>=Time10MS)
				{
						return SIM_IIC_START_BUSY_SDA;
				}
		}
			
		Delay_50us();
		SetSDA( Bit_RESET);
		SetSDADirect(channel, OUT);		//拉低SDA
		Delay_50us();
		SetSCL( Bit_RESET);
		SetSCLDirect(channel, OUT);		//拉低SCL
		Delay_50us();
		return SIM_IIC_BUS_OK;
}

//=============================================================================
//函数名称: static u8 SimI2CStop(u8 channel)
//输入参数: void
//输出参数: void
//返回值  : u8// 错误信号
//功能描述: IIC 停止信号
//注意事项: 
//=============================================================================
static u8 SimI2CStop(u8 channel)
{

		u16 count;
		
		SetSCL(Bit_RESET);		//拉低SCL
		SetSCLDirect(channel, OUT);
		SetSDA( Bit_RESET);		//拉低SDA
		SetSDADirect(channel, OUT);

		Delay_50us();
		SetSCL(Bit_SET);			//拉高SCL
		Delay_50us();
		SetSDA( Bit_SET);			//拉高SDA
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
//函数名称: static u8 SimI2C_WriteByte(u8 channel, u8 data)
//输入参数: u8// SimI2C_ReadBufferData  需要发送的数据
//输出参数: void
//返回值  :    u8// 错误信号
//功能描述: IIC 发送一BYTE 数据
//注意事项: 
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
//函数名称: static u8	SimI2C_ReadByte(u8 channel, u8 ack)	
//输入参数: u8// ack  需要发送ACK或者NOACK
//输出参数: void
//返回值  :    u8// 读取的数据
//功能描述: IIC 读取一BYTE 数据
//注意事项: 
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






