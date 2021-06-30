//=============================================================================
//				           深圳市格瑞普电池有限公司
//                         电子产品部			     
//-----------------------------------------------------------------------------
//文件名称: SMBUS.h
//文件标识:
//摘要    : SMBUS协议头文件
//-----------------------------------------------------------------------------
//当前版本: V1.0
//作者    : dangxinkai
//完成日期: 2020.10.15
//=============================================================================
//取代版本:
//原作者:
//完成日期:
//==============================================================================
#ifndef __BSPMASTERIIC_H
#define __BSPMASTERIIC_H
#include "SYSTypeDef.h"
//=============================================================================
//数据结构定义声明
//=============================================================================
typedef enum          
{
		eSIMCHANNEL_1 = 0,
		eSIMCHANNEL_2 = 1,
		eSIMCHANNEL_3 = 2,
		eSIMCH_NUM
		
}e_SimChannel;
//=============================================================================
//宏定义
//=============================================================================
//校验
//#define SMB_USE_PEC

//端口

#define OUT  	1
#define IN   		0
// #define  Bit_SET 1
// #define  Bit_RESET 0

//死循环等待时间
#define  Time10MS  	30//10ms

//#define Delay(count)        {volatile u32 i = count; while(i--);}
#define Delay_10us() {volatile u16 i = 5; while(i--);}//软件延时
#define Delay_50us() {volatile u16 i= 50; while(i--);}//软件延时

#define Delay_8us() {volatile u16  i = 30; while(i--);}//软件延时

//
#define SIM_IIC_ADDR    				0x16
#define SIM_IIC_DATA_BUF_LEN   	36
#define SIM_IIC_DATAFLASH_ADDR  0x44
#define SIM_IIC_TYPE_WORD       0
#define SIM_IIC_TYPE_BLOCK      1

//错误标志
#define SIM_IIC_HOST_ACK   			0
#define SIM_IIC_HOST_NOACK    	1

#define SIM_IIC_ERROR_COUNT  		0xfff  

#define SIM_IIC_BUS_OK     				0x00
#define SIM_IIC_BUS_BUSY_SCL    0x01
#define SIM_IIC_BUS_BUSY_SDA    0x02
#define SIM_IIC_NO_ACK          			0x03
#define SIM_IIC_BUS_ERR 	 			0x04
#define SIM_IIC_BLOCK_LEN_ERR 	0x05

#define SIM_IIC_START_BUSY_SCL  0x06
#define SIM_IIC_START_BUSY_SDA  0x07
#define SIM_IIC_STOP_BUSY_SCL   0x08
#define SIM_IIC_STOP_BUSY_SDA   0x09
#define SIM_IIC_TX_BUSY_SCL     0x0A
#define SIM_IIC_TX_NO_ACK 			0X0B
#define SIM_IIC_RX_BUSY_SCL 		0X0C
#define SIM_IIC_CRC_ERROR 			0X0D

#define SIM_IIC_TYPE_WORD       0
#define SIM_IIC_TYPE_BLOCK      1

#define Sim1_I2C_SCL_PIN                  			GPIO_Pin_13                 //PB.13 
#define Sim1_I2C_SCL_GPIO_PORT            	GPIOB                       /* GPIOB */
//#define Sim1_I2C_SCL_GPIO_CLK             	RCC_APB2Periph_GPIOB
#define Sim1_I2C_SDA_PIN                  			GPIO_Pin_14                 /* PB.14 */
#define Sim1_I2C_SDA_GPIO_PORT             GPIOB                       /* GPIOB */
//#define Sim1_I2C_SDA_GPIO_CLK             	RCC_APB2Periph_GPIOB


//#define I2C_SCL GPIO_Pin_13
//#define I2C_SDA GPIO_Pin_14
//#define GPIO_I2C GPIOB

//#define I2C_SCL_H GPIO_SetBits(GPIO_I2C,I2C_SCL)
//#define I2C_SCL_L GPIO_ResetBits(GPIO_I2C,I2C_SCL)

//#define I2C_SDA_H GPIO_SetBits(GPIO_I2C,I2C_SDA)
//#define I2C_SDA_L GPIO_ResetBits(GPIO_I2C,I2C_SDA)
//====================================================================
//类型定义
//====================================================================


//=============================================================================
//全局变量声明
//=============================================================================

//=============================================================================
//接口函数声明
//=============================================================================
//=============================================================================
//函数名称: void SimI2C_Init(void)
//输入参数: void
//输出参数: void
//返回值  : void
//功能描述: 模拟IIC初始化
//注意事项: 
//=============================================================================
void SimI2C_Init(void);

//=============================================================================
//函数名称: u8 SimI2C_ReadBuffer_NOCRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC读数据
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_ReadBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen);

//=============================================================================
//函数名称:u8	SimI2C_WriteBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC写数据
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_WriteBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen);

//=============================================================================
//函数名称: u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC读数据带CRC校验
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen);

//=============================================================================
//函数名称: u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC读数据带CRC校验
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen);

//=============================================================================
//函数名称:u8	SimI2C_WriteBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC写数据带CRC校验
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_WriteBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen);

//=============================================================================
//函数名称: u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC读数据带CRC校验
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_ReadBuffer(u8 channel, u8 devAddr, u16 regAddr, u8 *pBuffer, u8 bufLen);

//=============================================================================
//函数名称:u8	SimI2C_WriteBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//输入参数: devAddr I2C设备地址	devlen 设备地址字节数 regAddr 内部寄存器地址 pBuffer 写的数据 bufLen 数据长度
//输出参数: void
//返回值  : u8  //错误信息
//功能描述: 模拟IIC写数据带CRC校验
//注意事项: 设备地址处理时会左移一位
//=============================================================================
u8 SimI2C_WriteBuffer(u8 channel, u8 devAddr, u16 regAddr, u8 *pBuffer, u8 bufLen);

#endif


