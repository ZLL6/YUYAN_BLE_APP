#ifndef _SPI_H  //宏定义，定义文件名称
#define _SPI_H

/*---------------------------头文件引用--------------------------------------*/
#include "stm32f0xx.h"//引用STM32F0头文件

/*---------------------------函数声明--------------------------------------*/

void SPI1_GPIO(void);
void SPI1_Configation(void);
uint8_t SPI1_SendByte(uint8_t byte);

void SPI2_GPIO(void);
void SPI2_Configation(void);
uint8_t SPI2_SendByte(uint8_t byte);

#endif //定义文件名称结束

/************************ 论坛地址 www.zxkjmcu.com ******************************/


