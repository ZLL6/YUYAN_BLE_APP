#include "pbdata.h"
u16 W25QXX_ReadID(void);
/***************************************************************************
*   函 数 名: W25QXX_GPIO
*   功能说明: W25QXX_GPIO的GPIO管脚初始化
*   形    参：无
*   返 回 值: 无
***************************************************************************/
void W25QXX_GPIO(void)//flash控制管脚初始化函数
{ //   u16 temp=0;
	GPIO_InitTypeDef GPIO_InitStructure;//定义一个GPIO_InitTypeDef类型的结构体

	GPIO_InitStructure.GPIO_Pin = W25Q_CS_PIN;	//定义使用的管脚	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;	//模拟输出方式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_NOPULL;  
	GPIO_Init(W25Q_CS_PORT, &GPIO_InitStructure);	
	W25Q_CS_H;//片选置高
}

uint8_t W25QXX_ReadSR(void)//flash读入数据
{
    uint8_t byte=0;   
    W25Q_CS_L; 
    SPI1_SendByte(W25X_ReadStatusReg);
    byte=SPI1_SendByte(0);
    W25Q_CS_H; 
    return byte;
}

void W25QXX_Write_Enable(void)//flash写数据启动函数
{    
    W25Q_CS_L;  
    SPI1_SendByte(W25X_WriteEnable);
    W25Q_CS_H;  
}

void W25QXX_Write_Disable(void)//flash写数据关闭函数
{    
    W25Q_CS_L;   
    SPI1_SendByte(W25X_WriteDisable);
    W25Q_CS_H;   
}


//????ID
//?????:				   
//0XEF13,???????W25Q80  
//0XEF14,???????W25Q16    
//0XEF15,???????W25Q32  
//0XEF16,???????W25Q64 
//0XEF17,???????W25Q128 	  
u16 W25QXX_ReadID(void)
{
	u16 Temp = 0;	  
	 W25Q_CS_L;	
	SPI1_SendByte(0x90);//????ID??	    
	SPI1_SendByte(0x00); 	    
	SPI1_SendByte(0x00); 	    
	SPI1_SendByte(0x00); 	 			   
	Temp|=SPI1_SendByte(0xFF)<<8;  
	Temp|=SPI1_SendByte(0xFF);	 
	 W25Q_CS_H;
	return Temp;
}   

void W25QXX_Wait_Busy(void)//flash忙等待
{ //  static u16 Error=0;
    while((W25QXX_ReadSR()&0x01)==0x01);
//		{
//			Error++;
//			if(Error>10000)
//			{
//				Error = 0 ;
//			gBitStorageFailure = 1;	
//				break;
//			}
//			
//		};
}

void W25QXX_Erase_Chip(void)
{    
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();

    W25Q_CS_L; 
    SPI1_SendByte(W25X_ChipErase);
    W25Q_CS_H;  

    W25QXX_Wait_Busy();
}

void W25QXX_Erase_Sector(uint32_t Dst_Addr)
{    
    Dst_Addr*=4096;
    
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();

    W25Q_CS_L; 
    SPI1_SendByte(W25X_SectorErase);
    SPI1_SendByte((uint8_t)(Dst_Addr>>16));
    SPI1_SendByte((uint8_t)(Dst_Addr>>8));
    SPI1_SendByte((uint8_t)(Dst_Addr));
    W25Q_CS_H; 
    W25QXX_Wait_Busy();
}

void W25QXX_Read(uint8_t* pBuffer,uint32_t ReadAddr,uint16_t NumByteToRead)
{
    uint16_t i=0;
 //   W25QXX_Wait_Busy();  
    W25Q_CS_L;  
    SPI1_SendByte(W25X_ReadData);
    SPI1_SendByte((uint8_t)(ReadAddr>>16));
    SPI1_SendByte((uint8_t)(ReadAddr>>8));
    SPI1_SendByte((uint8_t)(ReadAddr));
    
    for(i=0;i<NumByteToRead;i++)
    {
        pBuffer[i]=SPI1_SendByte(0x00);
    }
    W25Q_CS_H;  
}

void W25QXX_Write_Page(uint8_t* pBuffer,uint32_t WriteAddr,uint16_t NumByteToWrite)//写页
{
    uint16_t i=0;
  
    W25QXX_Write_Enable();
    W25QXX_Wait_Busy();
    
    W25Q_CS_L;  
    SPI1_SendByte(W25X_PageProgram);
    SPI1_SendByte((uint8_t)(WriteAddr>>16));
    SPI1_SendByte((uint8_t)(WriteAddr>>8));
    SPI1_SendByte((uint8_t)(WriteAddr));
    
    for(i=0;i<NumByteToWrite;i++)
    {
        SPI1_SendByte(pBuffer[i]);
    }
    W25Q_CS_H;  
    W25QXX_Wait_Busy();
}

//????SPI FLASH 
//??????????????????0XFF,????0XFF?????????!
//???????? 
//????????????????,??????????!
//pBuffer:?????
//WriteAddr:???????(24bit)
//NumByteToWrite:???????(??65535)
//CHECK OK
//void W25QXX_Write_NoCheck(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
//{ 			 		 
//	u16 pageremain;	   
//	pageremain=256-WriteAddr%256; //????????		 	    
//	if(NumByteToWrite<=pageremain)pageremain=NumByteToWrite;//???256???
//	while(1)
//	{	   
//		W25QXX_Write_Page(pBuffer,WriteAddr,pageremain);
//		if(NumByteToWrite==pageremain)break;//?????
//	 	else //NumByteToWrite>pageremain
//		{
//			pBuffer+=pageremain;
//			WriteAddr+=pageremain;	

//			NumByteToWrite-=pageremain;			  //???????????
//			if(NumByteToWrite>256)pageremain=256; //??????256???
//			else pageremain=NumByteToWrite; 	  //??256????
//		}
//	};	    
//}
//?SPI FLASH  
//????????????????
//????????!
//pBuffer:?????
//WriteAddr:???????(24bit)						
//NumByteToWrite:???????(??65535)   
//static u8 W25QXX_BUFFER[4096];		 
//void W25QXX_Write(u8* pBuffer,u32 WriteAddr,u16 NumByteToWrite)   
//{ 
//	u32 secpos;
//	u16 secoff;
//	u16 secremain;	   
// 	u16 i;    
//	u8 * W25QXX_BUF;	 
//   	W25QXX_BUF=W25QXX_BUFFER;	     
// 	secpos=WriteAddr/4096;//????  
//	secoff=WriteAddr%4096;//???????
//	secremain=4096-secoff;//????????   
// 	if(NumByteToWrite<=secremain)secremain=NumByteToWrite;//???4096???
//	while(1) 
//	{	
//		W25QXX_Read(W25QXX_BUF,secpos*4096,4096);//?????????
//		for(i=0;i<secremain;i++)//????
//		{
//			if(W25QXX_BUF[secoff+i]!=0XFF)break;//????  	  
//		}
//		if(i<secremain)//????
//		{
//			W25QXX_Erase_Sector(secpos);//??????
//			for(i=0;i<secremain;i++)	   //??
//			{
//				W25QXX_BUF[i+secoff]=pBuffer[i];	  
//			}
//			W25QXX_Write_NoCheck(W25QXX_BUF,secpos*4096,4096);//??????  

//		}
//		else W25QXX_Write_NoCheck(pBuffer,WriteAddr,secremain);//???????,??????????. 				   
//		if(NumByteToWrite==secremain)break;//?????
//		else//?????
//		{
//			secpos++;//?????1
//			secoff=0;//?????0 	 

//		 	pBuffer+=secremain;  //????
//			WriteAddr+=secremain;//?????	   
//		  NumByteToWrite-=secremain;				//?????
//			if(NumByteToWrite>4096)secremain=4096;	//??????????
//			else secremain=NumByteToWrite;			//??????????
//		}	 
//	}	 
//}
u8 W25QXX_Demo(void)//例程
{
	
 static 	u8 tx[200];
 static u8 rx[200];
	memset(tx,0xAA,200);
	memset(rx,0x55,200);
	  
    W25QXX_Erase_Sector(0);//清空flash内部空间
	  delay_ms(50);
		W25QXX_Write_Page(tx,0,200);
    delay_ms(50);
	
    W25QXX_Read(rx,0,200);
    delay_ms(50);
	if(rx[0] !=tx[0])
	{
		return 1;
	}
	else
		return 0;
}













