#ifndef _RS232_H  //�궨�壬�����ļ�����
#define _RS232_H

/*---------------------------ͷ�ļ�����--------------------------------------*/
#include "stm32f0xx.h"//����STM32F0ͷ�ļ�

extern uint16_t CCITT_CRC16;
void RS232_GPIO(void);
void RS232_Configuration(void);
void CCITT_CRC16Init(uint8_t const * bytes, uint16_t len);  
void USART1_Recv_Control(void);
u8  Uart1_Task(void);
#endif //�����ļ����ƽ���
/************************ ��̳��ַ www.zxkjmcu.com ******************************/
