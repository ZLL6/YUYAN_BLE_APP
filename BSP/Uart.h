#ifndef __UART_H__
#define __UART_H__
#include "stm32f0xx_usart.h"
#include "Flash.h"
/******************************************************
Uart1Buffer1Status :
			BIT7  : ����������
			BIT6  : �������ڷ�����
			BIT5  : �������ڽ�����
			BIT4  : �������������
			BIT3  : �������������
			BIT2  : ��������
			BIT1  : �����������ݳ�ʱ
******************************************************/
#define  BufferFull        0x80
#define  BufferSending     0x40
#define  BufferReceing     0x20
#define  BufferReceOK      0X10
#define  BufferSendOK      0X08
#define  BufferEmpty       0x04
#define  BufferReTimeout   0x20
#define  USART_SUCCESS 0x00
#define  USART_CRC_ERR 0x5A
#define  USART_Operate_ERR 0xFF
#define APP_ADDR     (uint32_t)0x08004000



typedef struct  	//���ʹ��ڽ�����
{
	uint8_t Uart1Buffer[255]; //0~255
	volatile uint16_t Uart1Index;
	volatile uint8_t Uart1SendOKFlag;  //Uart1���ͱ�־
	volatile uint8_t Uart1ReceOKFlag;  //Uart1 ���ձ�־
	volatile uint8_t Uart1Receing;//���ڽ���
  volatile uint8_t Uart1RxTime;
	volatile uint8_t Uart1Command;
	



}Uart1BuffDef;

typedef struct  	//���ʹ��ڽ�����
{
	uint8_t Uart2Buffer[255]; //0~255
	volatile uint16_t Uart2Index;
	volatile uint8_t Uart2SendOKFlag;  //Uart1���ͱ�־
	volatile uint8_t Uart2ReceOKFlag;  //Uart1 ���ձ�־
	volatile uint8_t Uart2Receing;//���ڽ���
  volatile uint8_t Uart2RxTime;
	volatile uint8_t Uart2Command;



}Uart2BuffDef;





extern uint16_t CCITT_CRC16;
extern uint8_t Uart1Data[];
extern unsigned int FLASH_WR_ADDR;
extern unsigned char FLASH_Program_finish_flg;
extern unsigned char FLASH_Erase_finish_flg;





extern void LoadData(uint8_t* Data, uint16_t  LoadCount);

extern unsigned char UsartReturn_Buffer[224];  //����ת��CAN �����ݻ���
typedef void (*const Command_Func)(void);
extern const Command_Func Command_Func_Tabal[4];

extern void Uart_Init(uint32_t BaudRate);
extern void USART_Send_string(uint8_t *addr,uint8_t data_long);
extern void  USART_FRAME_PROCESS(void);
void USART_Send_status(uint8_t status);

extern void CCITT_CRC16Init(uint8_t const * bytes, uint16_t len); 
//extern void CANToUsart_Process(void);
#endif
