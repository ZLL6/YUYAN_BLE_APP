#include "pbdata.h"
/*********************************************************************************
*   函 数 名: LED_Init
*   功能说明: LED的GPIO管脚初始化
*   形    参：无
*   返 回 值: 无
*********************************************************************************/
void LED_GPIO(void)
{
	/*定义一个GPIO_InitTypeDef类型的结构体*/
	GPIO_InitTypeDef GPIO_InitStructure;

	/*选择要控制的GPIOB引脚*/															   
	GPIO_InitStructure.GPIO_Pin = LED_1_PIN|LED_B_PIN|LED_G_PIN|LED_R_PIN;	

	/*设置要控制的GPIOB引脚为输出模式*/	
	GPIO_InitStructure.GPIO_Mode=GPIO_Mode_OUT;

	/*设置引脚速率为50MHz */   
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 

	/*设置引脚模式为通用推挽输出*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; 

	/*设置引脚模式为上拉*/
	GPIO_InitStructure.GPIO_PuPd   = GPIO_PuPd_UP;  

	/*调用库函数，初始化GPIOB*/
	GPIO_Init(LED_PORT, &GPIO_InitStructure);	
	
	LED_1_H;
	LED_B_H;//拉高熄灭
	LED_G_H;//拉高熄灭
	LED_R_H;//拉高熄灭
}

/*********************************************************************************
*   函 数 名: LED_Demo2
*   功能说明: 闪烁例程，间隔时间为0.5秒。
*   形    参：无
*   返 回 值: 无
*********************************************************************************/
void LED_Demo(void)
{
	LED_1_L;
	delay_ms(500);
	LED_1_H;
	delay_ms(500);
	
	LED_R_L;
	delay_ms(500);
	LED_R_H;
	delay_ms(500);
	
	LED_G_L;
	delay_ms(500);
	LED_G_H;
	delay_ms(500);
	
	LED_B_L;
	delay_ms(500);
	LED_B_H;
	delay_ms(500);
}

/************************ 论坛地址 www.zxkjmcu.com ******************************/
