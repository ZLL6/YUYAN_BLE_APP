#ifndef _WKUP_H  //宏定义，定义文件名称
#define _WKUP_H

/*---------------------------头文件引用--------------------------------------*/
#include "stm32f0xx.h"//引用STM32F0头文件

/*---------------------------宏定义声明--------------------------------------*/
#define WKUP_PIN GPIO_Pin_5
#define WKUP_PORT GPIOB
#define COMWK_PIN GPIO_Pin_11
#define COMWK_PORT GPIOA

void WKUP_GPIO(void);
//void WKUP_Demo(void);
void Sleep_Bat(void);

#endif //定义文件名称结束


/************************ 论坛地址 www.zxkjmcu.com ******************************/
