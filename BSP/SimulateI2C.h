//=============================================================================
//				           �����и����յ�����޹�˾
//                         ���Ӳ�Ʒ��			     
//-----------------------------------------------------------------------------
//�ļ�����: SMBUS.h
//�ļ���ʶ:
//ժҪ    : SMBUSЭ��ͷ�ļ�
//-----------------------------------------------------------------------------
//��ǰ�汾: V1.0
//����    : dangxinkai
//�������: 2020.10.15
//=============================================================================
//ȡ���汾:
//ԭ����:
//�������:
//==============================================================================
#ifndef __BSPMASTERIIC_H
#define __BSPMASTERIIC_H
#include "SYSTypeDef.h"
//=============================================================================
//���ݽṹ��������
//=============================================================================
typedef enum          
{
		eSIMCHANNEL_1 = 0,
		eSIMCHANNEL_2 = 1,
		eSIMCHANNEL_3 = 2,
		eSIMCH_NUM
		
}e_SimChannel;
//=============================================================================
//�궨��
//=============================================================================
//У��
//#define SMB_USE_PEC

//�˿�

#define OUT  	1
#define IN   		0
// #define  Bit_SET 1
// #define  Bit_RESET 0

//��ѭ���ȴ�ʱ��
#define  Time10MS  	30//10ms

//#define Delay(count)        {volatile u32 i = count; while(i--);}
#define Delay_10us() {volatile u16 i = 5; while(i--);}//�����ʱ
#define Delay_50us() {volatile u16 i= 50; while(i--);}//�����ʱ

#define Delay_8us() {volatile u16  i = 30; while(i--);}//�����ʱ

//
#define SIM_IIC_ADDR    				0x16
#define SIM_IIC_DATA_BUF_LEN   	36
#define SIM_IIC_DATAFLASH_ADDR  0x44
#define SIM_IIC_TYPE_WORD       0
#define SIM_IIC_TYPE_BLOCK      1

//�����־
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
//���Ͷ���
//====================================================================


//=============================================================================
//ȫ�ֱ�������
//=============================================================================

//=============================================================================
//�ӿں�������
//=============================================================================
//=============================================================================
//��������: void SimI2C_Init(void)
//�������: void
//�������: void
//����ֵ  : void
//��������: ģ��IIC��ʼ��
//ע������: 
//=============================================================================
void SimI2C_Init(void);

//=============================================================================
//��������: u8 SimI2C_ReadBuffer_NOCRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IIC������
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_ReadBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen);

//=============================================================================
//��������:u8	SimI2C_WriteBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IICд����
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_WriteBuffer_NOCRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen);

//=============================================================================
//��������: u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IIC�����ݴ�CRCУ��
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen);

//=============================================================================
//��������: u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IIC�����ݴ�CRCУ��
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen);

//=============================================================================
//��������:u8	SimI2C_WriteBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IICд���ݴ�CRCУ��
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_WriteBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen);

//=============================================================================
//��������: u8 SimI2C_ReadBuffer_CRC(u8 channel, u8 devAddr, u8 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IIC�����ݴ�CRCУ��
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_ReadBuffer(u8 channel, u8 devAddr, u16 regAddr, u8 *pBuffer, u8 bufLen);

//=============================================================================
//��������:u8	SimI2C_WriteBuffer_CRC(u8 channel, u8 devAddr, u16 regAddr, u8 regLen, u8 *pBuffer, u8 bufLen)
//�������: devAddr I2C�豸��ַ	devlen �豸��ַ�ֽ��� regAddr �ڲ��Ĵ�����ַ pBuffer д������ bufLen ���ݳ���
//�������: void
//����ֵ  : u8  //������Ϣ
//��������: ģ��IICд���ݴ�CRCУ��
//ע������: �豸��ַ����ʱ������һλ
//=============================================================================
u8 SimI2C_WriteBuffer(u8 channel, u8 devAddr, u16 regAddr, u8 *pBuffer, u8 bufLen);

#endif


