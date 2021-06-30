#ifndef _W25QXX_H  //宏定义，定义文件名称
#define _W25QXX_H

/*---------------------------头文件引用--------------------------------------*/
#include "stm32f0xx.h"//引用STM32F0头文件

/*---------------------------宏定义声明--------------------------------------*/
#define W25Q_CS_PIN GPIO_Pin_4
#define W25Q_CS_PORT GPIOA

#define W25Q_CS_L GPIO_ResetBits(W25Q_CS_PORT,W25Q_CS_PIN);
#define W25Q_CS_H GPIO_SetBits(W25Q_CS_PORT,W25Q_CS_PIN);

//#define W25Q_CS_L GPIO_ResetBits(W25Q_CS_PORT,W25Q_CS_PIN);
//#define W25Q_CS_H GPIO_ResetBits(W25Q_CS_PORT,W25Q_CS_PIN);

//指令表
#define W25X_WriteEnable		0x06 
#define W25X_WriteDisable		0x04 
#define W25X_ReadStatusReg		0x05 
#define W25X_WriteStatusReg		0x01 
#define W25X_ReadData			0x03 
#define W25X_FastReadData		0x0B 
#define W25X_FastReadDual		0x3B 
#define W25X_PageProgram		0x02 
#define W25X_BlockErase			0xD8 
#define W25X_SectorErase		0x20 
#define W25X_ChipErase			0xC7 
#define W25X_PowerDown			0xB9 
#define W25X_ReleasePowerDown	0xAB 
#define W25X_DeviceID			0xAB 
#define W25X_ManufactDeviceID	0x90 
#define W25X_JedecDeviceID		0x9F 

void W25QXX_GPIO(void);
u8 W25QXX_Demo(void);
void W25QXX_Erase_Sector(uint32_t Dst_Addr);
void W25QXX_Erase_Chip(void);
void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite);
void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead);
void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);  
void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite);
#endif 
