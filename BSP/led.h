#ifndef _LED_H  //宏定义，定义文件名称
#define _LED_H

/*---------------------------头文件引用--------------------------------------*/
#include "stm32f0xx.h"//引用STM32F0头文件

/*---------------------------宏定义声明--------------------------------------*/
#define LED_1_PIN GPIO_Pin_6 //定义GPIO_Pin_6引脚为LED_1_PIN，相当于重新命名。
#define LED_B_PIN GPIO_Pin_7 //定义GPIO_Pin_7引脚为LED_B_PIN，相当于重新命名。
#define LED_G_PIN GPIO_Pin_8 //定义GPIO_Pin_8引脚为LED_G_PIN，相当于重新命名。
#define LED_R_PIN GPIO_Pin_9 //定义GPIO_Pin_9引脚为LED_R_PIN，相当于重新命名。

#define LED_PORT GPIOC  //定义GPIOC端口为LED_PORT，相当于重新命名。

//如果想用其他管脚控制LED,那么只需更改上面对应的端口与引脚编号即可

#define LED_1_L GPIO_ResetBits(LED_PORT,LED_1_PIN);    //定义LED_1_L,调用LED_1_L命令，PC6引脚输出低电平
#define LED_1_H GPIO_SetBits(LED_PORT,LED_1_PIN);      //定义LED_1_H,调用LED_1_H命令，PC6引脚输出低高平
#define LED_1_T GPIO_WriteBit(LED_PORT,LED_1_PIN,(BitAction)(1 - GPIO_ReadOutputDataBit(LED_PORT, LED_1_PIN)));//定义LED_1_T,调用LED_1_T命令，PC6引脚输出电平反转

#define LED_B_L GPIO_ResetBits(LED_PORT,LED_B_PIN);    //定义LED_B_L,调用LED_B_L命令，PC7引脚输出低电平
#define LED_B_H GPIO_SetBits(LED_PORT,LED_B_PIN);      //定义LED_B_H,调用LED_B_H命令，PC7引脚输出低高平
#define LED_B_T GPIO_WriteBit(LED_PORT,LED_B_PIN,(BitAction)(1 - GPIO_ReadOutputDataBit(LED_PORT, LED_B_PIN)));//定义LED_1_T,调用LED_1_T命令，PC7引脚输出电平反转

#define LED_G_L GPIO_ResetBits(LED_PORT,LED_G_PIN);    //定义LED_G_L,调用LED_G_L命令，PC8引脚输出低电平
#define LED_G_H GPIO_SetBits(LED_PORT,LED_G_PIN);      //定义LED_G_H,调用LED_G_H命令，PC8引脚输出低高平
#define LED_G_T GPIO_WriteBit(LED_PORT,LED_G_PIN,(BitAction)(1 - GPIO_ReadOutputDataBit(LED_PORT, LED_G_PIN)));//定义LED_1_T,调用LED_1_T命令，PC8引脚输出电平反转

#define LED_R_L GPIO_ResetBits(LED_PORT,LED_R_PIN);    //定义LED_R_L,调用LED_R_L命令，PC9引脚输出低电平
#define LED_R_H GPIO_SetBits(LED_PORT,LED_R_PIN);      //定义LED_R_H,调用LED_R_H命令，PC9引脚输出低高平
#define LED_R_T GPIO_WriteBit(LED_PORT,LED_R_PIN,(BitAction)(1 - GPIO_ReadOutputDataBit(LED_PORT, LED_R_PIN)));//定义LED_1_T,调用LED_1_T命令，PC9引脚输出电平反转

/*---------------------------函数声明--------------------------------------*/
void LED_GPIO(void);  //LED初始化函数
void LED_Demo(void);

#endif //定义文件名称结束


/************************ 论坛地址 www.zxkjmcu.com ******************************/
