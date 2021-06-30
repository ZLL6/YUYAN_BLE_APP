#include "pbdata.h"

void SPI1_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_PinAFConfig(GPIOA,GPIO_PinSource5,GPIO_AF_0);  
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource6,GPIO_AF_0);
	GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_0); 

	/*!< Configure SPI_FLASH_SPI pins: SCK */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	/*!< Configure SPI_FLASH_SPI pins: MISO */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/*!< Configure SPI_FLASH_SPI pins: MOSI */
	GPIO_InitStructure.GPIO_Pin    = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Mode   = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType  = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed  = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_DOWN;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
}

void SPI1_Configation(void)//SPI通讯格式设置函数
{
	SPI_InitTypeDef  SPI_InitStructure;
	
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;	
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	
	SPI_Init(SPI1, &SPI_InitStructure);
	SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
	SPI_Cmd(SPI1, ENABLE);
}

uint8_t SPI1_SendByte(uint8_t byte)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);

	/* Send byte through the SPI2 peripheral */
	SPI_SendData8(SPI1, byte);

	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);

	/* Return the byte read from the SPI bus */
	return SPI_ReceiveData8(SPI1);
}
