#ifndef _RS232_H  //宏定义，定义文件名称
#define _RS232_H

/*---------------------------头文件引用--------------------------------------*/
#include "stm32f0xx.h"//引用STM32F0头文件

extern uint16_t CCITT_CRC16;
void RS232_GPIO(void);
void RS232_Configuration(void);
void CCITT_CRC16Init(uint8_t const * bytes, uint16_t len);  
void USART1_Recv_Control(void);
u8  Uart1_Task(void);
#endif //定义文件名称结束
/************************ 论坛地址 www.zxkjmcu.com ******************************/
